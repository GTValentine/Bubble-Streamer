#include "SOP_BubbleStreamer.h"

#include <UT/UT_DSOVersion.h>
//#include <RE/RE_EGLServer.h>

#include <SYS/SYS_Math.h>
#include <UT/UT_Interrupt.h>
#include <GU/GU_Detail.h>
#include <GU/GU_PrimPoly.h>
#include <GU/GU_PrimSphere.h>
#include <CH/CH_LocalVariable.h>
#include <PRM/PRM_Include.h>
#include <PRM/PRM_SpareData.h>

#include <climits>
#include <cstdio>


static PRM_Name nm_gridres("gridres", "Grid Resolution");
static PRM_Name nm_simstep("simstep", "Simulation Step");

static PRM_Default df_gridres(12);
static PRM_Default df_simstep(0.1);


PRM_Template SOP_BubbleStreamer::myTemplateList[] = {
    PRM_Template(PRM_INT, PRM_Template::PRM_EXPORT_MIN, 1, &nm_gridres, &df_gridres, 0),
    PRM_Template(PRM_FLT, PRM_Template::PRM_EXPORT_MIN, 1, &nm_simstep, &df_simstep, 0),
    PRM_Template()
};

// Here's how we define local variables for the SOP.
//enum {
//    VAR_PT, // Point number of the star
//    VAR_NPT // Number of points in the star
//};

CH_LocalVariable SOP_BubbleStreamer::myVariables[] = {
    //{ "PT", VAR_PT, 0 }, // The table provides a mapping
    //{ "NPT", VAR_NPT, 0 }, // from text string to integer token
    { 0, 0, 0 },
};

bool SOP_BubbleStreamer::evalVariableValue(fpreal &val, int index, int thread)
{
    // myCurrPoint will be negative when we're not cooking so only try to
    // handle the local variables when we have a valid myCurrPoint index.
    if (myCurrPoint >= 0) {
        // Note that "gdp" may be null here, so we do the safe thing
        // and cache values we are interested in.
        switch (index) {
        //case VAR_PT:
        //    val = (fpreal)myCurrPoint;
        //    return true;
        //case VAR_NPT:
        //    val = (fpreal)myTotalPoints;
        //    return true;
        default:
            ; // do nothing
        }
    }
    // Not one of our variables, must delegate to the base class.
    return SOP_Node::evalVariableValue(val, index, thread);
}

OP_Node *SOP_BubbleStreamer::myConstructor(OP_Network *net, const char *name, OP_Operator *op)
{
    return new SOP_BubbleStreamer(net, name, op);
}

SOP_BubbleStreamer::SOP_BubbleStreamer(OP_Network *net, const char *name, OP_Operator *op)
        : SOP_Node(net, name, op)
        , solver(nullptr)
        , laststep(-1)
{
    myCurrPoint = -1; // To prevent garbage values from being returned
}

SOP_BubbleStreamer::~SOP_BubbleStreamer() {}

unsigned SOP_BubbleStreamer::disableParms()
{
    return 0;
}

OP_ERROR SOP_BubbleStreamer::cookMySop(OP_Context &context)
{
    printf("cookMySop\n");
    addMessage(SOP_MESSAGE, "cookMySop");  // TODO: Not happening?
    //fpreal now = context.getTime();

    int   gridres = get_gridres(0);
    float simstep = get_simstep(0);

    int currstep = (int) context.getTime();

    if (currstep < laststep || solver == nullptr) {
        if (solver) {
            delete solver;
        }
        addMessage(SOP_MESSAGE, "solver->new");
        solver = new BubbleSolver(gridres);
        laststep = -1;
    }

    for (; laststep < currstep; laststep++) {
        addMessage(SOP_MESSAGE, "solver->advance");
        solver->advance(simstep);
    }

    UT_Interrupt *boss;

    // Since we don't have inputs, we don't need to lock them.

    // Check to see that there hasn't been a critical error in cooking the SOP.
    if (error() < UT_ERROR_ABORT) {
        boss = UTgetInterrupt();
        //addWarning(SOP_MESSAGE, "Example warning");
        gdp->clearAndDestroy();

        // Start the interrupt server
        if (boss->opStart("Instantiating bubbles")) {
            const list<Bubble> &bubs = solver->get_bubbles();
            for (Bubble b : bubs) {
                GU_PrimSphereParms parms;
                parms.gdp = gdp;
                parms.rows = 8;
                parms.cols = 8;
                UT_Matrix4 mat(1.0f);
                mat.translate(b.position[0], b.position[1], b.position[2]);
                mat.scale(b.radius, b.radius, b.radius);
                parms.xform = mat;
                GU_PrimSphere::build(parms);
            }

            select(GU_SPrimitive);
        }

        // Tell the interrupt server that we've completed. Must do this
        // regardless of what opStart() returns.
        boss->opEnd();
    }

    // Notify the display cache that we have directly edited
    // points, this lets NURBs surfaces know that they need to rebuild,
    // for example.
    // In this case it isn't needed, but included in case this
    // code is reused for a more complicated example.
    //gdp->notifyCache(GU_CACHE_ALL);

    return error();
}
