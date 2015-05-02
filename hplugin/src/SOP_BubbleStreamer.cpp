#include "SOP_BubbleStreamer.h"

#include <climits>
#include <cstdio>

#pragma warning(push, 0)
#include <UT/UT_DSOVersion.h>
//#include <RE/RE_EGLServer.h>
#include <SYS/SYS_Math.h>
#include <UT/UT_Interrupt.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPoly.h>
#include <GU/GU_PrimSphere.h>
#include <GU/GU_PrimPart.h>
#include <CH/CH_LocalVariable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>
#include <SOP/SOP_Guide.h>
#include <OP/OP_AutoLockInputs.h>
#include <GEO/GEO_AttributeHandle.h>
#pragma warning(pop)

static PRM_Name nm_dims("dims", "Volume Dimensions");
static PRM_Name nm_cellsize("cellsize", "Grid Cell Size");
static PRM_Name nm_simstep("simstep", "Simulation Step");
static PRM_Name nm_scfreq("scfreq", "Scattering Frequency");
static PRM_Name nm_sccoef("sccoef", "Scattering Coefficient");
static PRM_Name nm_scimpc("scimpc", "Scattering Impact");
static PRM_Name nm_brfreq("brfreq", "Breakup Frequency");
static PRM_Name nm_bubscale("bubscale", "Sim-View Bubble Scale Ratio");

static PRM_Default df_dims[] = { PRM_Default(1.0)
                               , PRM_Default(1.0)
                               , PRM_Default(1.0) };
static PRM_Default df_cellsize(0.1);
static PRM_Default df_simstep(0.1);
static PRM_Default df_scfreq(10.0);
static PRM_Default df_sccoef(0.9);
static PRM_Default df_brfreq(0.001);
static PRM_Default df_scimpc(1.0);
static PRM_Default df_bubscale(1.0);

PRM_Template SOP_BubbleStreamer::myTemplateList[] = {
  PRM_Template(PRM_STRING, 1, &PRMgroupName, 0, &SOP_Node::pointGroupMenu,
    0, 0, SOP_Node::getGroupSelectButton(
    GA_GROUP_POINT)),
  PRM_Template(PRM_FLT, 3, &nm_dims, df_dims),
  PRM_Template(PRM_FLT, PRM_Template::PRM_EXPORT_MIN, 1, &nm_cellsize, &df_cellsize, 0),
  PRM_Template(PRM_FLT, PRM_Template::PRM_EXPORT_MIN, 1, &nm_simstep, &df_simstep, 0),
  PRM_Template(PRM_FLT, PRM_Template::PRM_EXPORT_MIN, 1, &nm_scfreq, &df_scfreq, 0),
  PRM_Template(PRM_FLT, PRM_Template::PRM_EXPORT_MIN, 1, &nm_sccoef, &df_sccoef, 0),
  PRM_Template(PRM_FLT, PRM_Template::PRM_EXPORT_MIN, 1, &nm_scimpc, &df_scimpc, 0),
  PRM_Template(PRM_FLT, PRM_Template::PRM_EXPORT_MIN, 1, &nm_brfreq, &df_brfreq, 0),
  PRM_Template(PRM_FLT, PRM_Template::PRM_EXPORT_MIN, 1, &nm_bubscale, &df_bubscale, 0),
  PRM_Template()
};

CH_LocalVariable SOP_BubbleStreamer::myVariables[] = {
  //{ "PT", VAR_PT, 0 }, // The table provides a mapping
  //{ "NPT", VAR_NPT, 0 }, // from text string to integer token
  { 0, 0, 0 },
};

bool SOP_BubbleStreamer::evalVariableValue(fpreal &val, int index, int thread) {
  // myCurrPoint will be negative when we're not cooking so only try to
  // handle the local variables when we have a valid myCurrPoint index.

  // Note that "gdp" may be null here, so we do the safe thing
  // and cache values we are interested in.

  //switch (index) {
  //case VAR_PT:
  //    val = (fpreal)myCurrPoint;
  //    return true;
  //case VAR_NPT:
  //    val = (fpreal)myTotalPoints;
  //    return true;
  //default:
  //  ; // do nothing
  //}

  // Not one of our variables, must delegate to the base class.
  return SOP_Node::evalVariableValue(val, index, thread);
}

OP_Node *SOP_BubbleStreamer::myConstructor(OP_Network *net, const char *name, OP_Operator *op) {
  return new SOP_BubbleStreamer(net, name, op);
}

SOP_BubbleStreamer::SOP_BubbleStreamer(OP_Network *net, const char *name, OP_Operator *op)
    : SOP_Node(net, name, op)
    , myGroup(nullptr)
    , solver(nullptr)
    , laststep(-1) {
  //gdp = nullptr;  // Under some build circumstances, Houdini will not initialize this for us.
  mySopFlags.setNeedGuide1(true);
  mySopFlags.setManagesDataIDs(false);
}

SOP_BubbleStreamer::~SOP_BubbleStreamer() {
}

OP_ERROR SOP_BubbleStreamer::cookInputGroups(OP_Context &context, int alone)
{
  // The SOP_Node::cookInputPointGroups() provides a good default
  // implementation for just handling a point selection.
  return cookInputPointGroups(
        context, // This is needed for cooking the group parameter, and cooking the input if alone.
        myGroup, // The group (or NULL) is written to myGroup if not alone.
        alone,   // This is true iff called outside of cookMySop to update handles.
                 //   true means the group will be for the input geometry.
                 //   false means the group will be for gdp (the working/output geometry).
        true,    // (default) true means to set the selection to the group if not alone and the highlight flag is on.
        0,       // (default) Parameter index of the group field
        -1,      // (default) Parameter index of the group type field (-1 since there isn't one)
        true,    // (default) true means that a pointer to an existing group is okay; false means group is always new.
        false,   // (default) false means new groups should be unordered; true means new groups should be ordered.
        true,    // (default) true means that all new groups should be detached, so not owned by the detail;
                 //   false means that new point and primitive groups on gdp will be owned by gdp.
        0        // (default) Index of the input whose geometry the group will be made for if alone.
        );
}

OP_ERROR SOP_BubbleStreamer::cookMySop(OP_Context &context) {
  flags().timeDep = 1;

  OP_AutoLockInputs inputs(this);
  if (inputs.lock(context) >= UT_ERROR_ABORT) return error();
  duplicatePointSource(0, context);

  fpreal now = context.getTime();
  int currstep = context.getFrame();

  double simstep = get_simstep(now);
  double scfreq = get_scfreq(now);
  double sccoef = get_sccoef(now);
  double scimpc = get_scimpc(now);
  double brfreq = get_brfreq(now);
  double bubscale = get_bubscale(now);

  if (error() >= UT_ERROR_ABORT) return error();

  if (currstep < laststep || solver == nullptr) {
    if (solver) {
      delete solver;
    }
    double dimX = get_dimX(0);
    double dimY = get_dimY(0);
    double dimZ = get_dimZ(0);
    double cellsize = get_cellsize(0);
    solver = new BubbleSolver(dimX, dimY, dimZ, cellsize);

    laststep = -1;
  }

  solver->scattering_coef(sccoef);
  solver->scattering_freq(scfreq);
  solver->scattering_impact(scimpc);
  solver->breakup_freq(brfreq);

  GA_RWHandleF pscalein = GA_RWHandleF(gdp->findFloatTuple(GA_ATTRIB_POINT, "pscale"));
  if (!pscalein.isValid()) {
      pscalein = GA_RWHandleF(gdp->addFloatTuple(GA_ATTRIB_POINT, "pscale", 1, GA_Defaults(0.01)));
  }

  GA_Offset ptoff;
  GA_FOR_ALL_GROUP_PTOFF(gdp, myGroup, ptoff) {
    UT_Vector3 p = gdp->getPos3(ptoff);
    float ps = pscalein.get(ptoff);
    solver->add_bubble(glm::vec3(p[0], p[1], p[2]), ps * bubscale);
  }

  for (; laststep < currstep; laststep++) {
    solver->advance(simstep);
  }

  UT_Interrupt *boss;

  // Check to see that there hasn't been a critical error in cooking the SOP.
  if (error() >= UT_ERROR_ABORT) return error();

  clearInstance();
  boss = UTgetInterrupt();
  //addWarning(SOP_MESSAGE, "Example warning");
  gdp->clearAndDestroy();

  // Start the interrupt server
  if (boss->opStart("Instantiating bubbles")) {
    auto p = GU_PrimParticle::build(gdp, 0);

    GA_RWHandleF pscaleout = GA_RWHandleF(gdp->findFloatTuple(GA_ATTRIB_POINT, "pscale"));
    if (!pscaleout.isValid()) {
        pscaleout = GA_RWHandleF(gdp->addFloatTuple(GA_ATTRIB_POINT, "pscale", 1, GA_Defaults(0.01)));
    }

    const std::list<Bubble> &bubs = solver->get_bubbles();
    for (Bubble b : bubs) {
      GA_Offset offset = gdp->appendPointOffset();
      gdp->setPos3(offset, (fpreal32) b.position[0],
                           (fpreal32) b.position[1],
                           (fpreal32) b.position[2]);
      pscaleout.set(offset, b.radius / bubscale);
      p->appendParticle(offset);
      // TODO: add parameter for bubble size and output that
      // TODO: Also somehow instantiate spheres in Houdini using size parameter
      //GU_PrimSphereParms parms;
      //parms.gdp = gdp;
      //parms.rows = 8;
      //parms.cols = 8;
      //UT_Matrix4 mat(1.0f);
      //mat.scale(b.radius, b.radius, b.radius);
      //parms.xform = mat;
      //GU_PrimSphere::build(parms);
    }

    select(GU_SPrimitive);
  }

  // Tell the interrupt server that we've completed. Must do this
  // regardless of what opStart() returns.
  boss->opEnd();

  // Notify the display cache that we have directly edited
  // points, this lets NURBs surfaces know that they need to rebuild,
  // for example.
  // In this case it isn't needed, but included in case this
  // code is reused for a more complicated example.
  //gdp->notifyCache(GU_CACHE_ALL);

  return error();
}

OP_ERROR SOP_BubbleStreamer::cookMyGuide1(OP_Context &context)
{
  //float now = context.getTime();

  myGuide1->clearAndDestroy();

  if (error() >= UT_ERROR_ABORT) return error();

  myGuide1->cube(0, get_dimX(0),
                 0, get_dimY(0),
                 0, get_dimZ(0));

  return error();
}
