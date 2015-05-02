#pragma once

#pragma warning(push, 0)
//#include <GEO/GEO_Point.h>
#include <SOP/SOP_Node.h>
#include <GA/GA_ElementGroup.h>
#pragma warning(pop)

#include <la.h>
#include <bubble_solver.h>

//!
//! Houdini node BubbleStreamer
//!
//! This class generates a dispersed bubble flow and
//! imports bubbles to Houdini as SOP points,
//! allowing user to specify the parameters of the flow.
//!
//! The node's input is used to insert bubbles into the simulation.
//!
//! The node outputs a point for each bubble, with a pscale attribute
//! for the bubble size.
class SOP_BubbleStreamer : public SOP_Node {

 public:
  //!
  //! Calls the constructor for our SOP node and returns a reference.
  //! This function is linked by hooks.cpp.
  static OP_Node *myConstructor(OP_Network*, const char *, OP_Operator *);

  //!
  //! List of templates which define our SOP node's interface (node parameters).
  //!
  //! This is exported via hooks.cpp.
  static PRM_Template myTemplateList[];

  //!
  //! (Optional) list of local expression variables which are exported by our SOP node.
  //! We aren't using this right now, so it is empty.
  //!
  //! This is exported via hooks.cpp.
  static CH_LocalVariable myVariables[];

protected:
  //!
  //! Initialize the bubble streamer, including all fields and some options.
  SOP_BubbleStreamer(OP_Network *net, const char *name, OP_Operator *op);

  virtual ~SOP_BubbleStreamer();

  //!
  //! Configures/loads the input point groups. Modifies the ::myGroup field.
  OP_ERROR cookInputGroups(OP_Context &context, int alone);

  //!
  //! In this function, we read the inputs, update our simulation (which is
  //! stored in ::solver), and then produce the appropriate outputs.
  //!
  //! This function also sets the node to be time dependent so that it is
  //! re-cooked every frame.
  virtual OP_ERROR cookMySop(OP_Context &context);

  //!
  //! Output the geometry used as the "guide" in Houdini - extra visualization
  //! which appears in the editor but is never rendered.
  virtual OP_ERROR cookMyGuide1(OP_Context &context);

  //!
  //! When one of our node's local variables (as exported by ::myVariables)
  //! is used, this function is called to evaluate its value.
  virtual bool evalVariableValue(fpreal &val, int index, int thread);

  //!
  //! Add virtual overload that delegates to the super class to avoid
  //! shadow warnings.
  virtual bool evalVariableValue(UT_String &v, int i, int thread) {
    return SOP_Node::evalVariableValue(v, i, thread);
  }

 private:
  fpreal get_dimX(fpreal t) { return evalFloat("dims", 0, t); }
  fpreal get_dimY(fpreal t) { return evalFloat("dims", 1, t); }
  fpreal get_dimZ(fpreal t) { return evalFloat("dims", 2, t); }

  fpreal get_cellsize(fpreal t) {
    return evalFloat("cellsize", 0, t);
  }

  fpreal get_simstep(fpreal t) {
    return evalFloat("simstep", 0, t);
  }

  fpreal get_scfreq(fpreal t) {
    return evalFloat("scfreq", 0, t);
  }

  fpreal get_sccoef(fpreal t) {
    return evalFloat("sccoef", 0, t);
  }

  fpreal get_scimpc(fpreal t) {
    return evalFloat("scimpc", 0, t);
  }

  fpreal get_brfreq(fpreal t) {
    return evalFloat("brfreq", 0, t);
  }

  fpreal get_radexp(fpreal t) {
    return evalFloat("radexp", 0, t);
  }

  fpreal get_radsdv(fpreal t) {
    return evalFloat("radsdv", 0, t);
  }

  fpreal get_bubscale(fpreal t) {
    return evalFloat("bubscale", 0, t);
  }

  //!
  //! Groups are stored here by ::cookInputGroups.
  const GA_PointGroup *myGroup;

  //!
  //! Our bubble solver instance is stored here. This is re-created and
  //! replaced each time the simulator needs to restart the simulation from the
  //! beginning, because we do not perform any caching.
  BubbleSolver *solver;

  //!
  //! The last simulation step number (frame number) which was simulated. Since
  //! we do not have any caching, if the current step is before this, then the
  //! simulation is reset and rerun up to the current step.
  int laststep;
};
