#include <OP/OP_Operator.h>
#include <OP/OP_OperatorTable.h>

#include "SOP_BubbleStreamer.h"


/// newSopOperator is the hook that Houdini grabs from this dll
/// and invokes to register the SOP. In this case we add ourselves
/// to the specified operator table.
void newSopOperator(OP_OperatorTable *table)
{
  table->addOperator(
    new OP_Operator(
      "BubbleStreamer", // Internal name
      "BubbleStreamer", // UI name
      SOP_BubbleStreamer::myConstructor, // How to build the SOP
      SOP_BubbleStreamer::myTemplateList, // My parameters
      0, // Min # of sources
      0, // Max # of sources
      SOP_BubbleStreamer::myVariables, // Local variables
      OP_FLAG_GENERATOR // Flag it as generator
    )
  );
}
