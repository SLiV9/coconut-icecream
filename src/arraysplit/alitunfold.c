#include <stdio.h>
#include <string.h>

#include "alitunfold.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "globals.h"
#include "ctinfo.h"
#include "memory.h"
#include "print.h"
#include "copy.h"
#include "free.h"

#include "aunfold_info.h"

node* ALITUNFOLDbody(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ALITUNFOLDbody");
  
  if (BODY_INSTRS( arg_node) != NULL)
  {
    do
    {
      INFO_DIRTY( arg_info) = 0;
      BODY_INSTRS( arg_node) = TRAVdo( BODY_INSTRS( arg_node), arg_info);
    }
    while (INFO_DIRTY( arg_info) > 0);
  }

  // no need to trav others

  DBUG_RETURN( arg_node);
}

node* ALITUNFOLDinstrs(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ALITUNFOLDinstrs");
  
  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");
  INSTRS_NEXT( arg_node) = TRAVopt( INSTRS_NEXT( arg_node), arg_info);
  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");

  INSTRS_INSTR( arg_node) = TRAVdo( INSTRS_INSTR( arg_node), arg_info);

  node* instrs = arg_node;

  if (INFO_HEAD( arg_info) != NULL)
  {
    // skip self, it has been replaced by info_head-info_last
    INSTRS_NEXT( INFO_LAST( arg_info)) = INSTRS_NEXT( arg_node);
    instrs = INFO_HEAD( arg_info);
    INFO_HEAD( arg_info) = NULL;
    INFO_LAST( arg_info) = NULL;

    // free self, it has been replaced
    INSTRS_NEXT( arg_node) = NULL;
    INSTRS_INSTR( arg_node) = NULL;
    FREEdoFreeNode( arg_node);
  }

  DBUG_RETURN( instrs);
}

node* ALITUNFOLDassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ALITUNFOLDassign");
  
  node* letje = ASSIGN_LET( arg_node);
  node* alit = ASSIGN_EXPR( arg_node);
  
  if (NODE_TYPE( alit) == N_arraylit)
  {
    node* exprs = ARRAYLIT_EXPRS( alit);
    int i = 0;
    if (EXPRS_EXPR( exprs) != NULL)
    {
      if (NODE_TYPE( EXPRS_EXPR( exprs)) == N_arraylit)
      {
        INFO_DIRTY( arg_info)++;
      }
    }
    while (exprs != NULL)
    {
      node* instr = makeAssignInstrs(letje, TBmakeInt(i), EXPRS_EXPR( exprs));
      addInstr( arg_info, instr);

      i++;
      exprs = EXPRS_NEXT( exprs);
    }

    FREEdoFreeNode( letje);
    ASSIGN_LET( arg_node) = NULL;
    ASSIGN_EXPR( arg_node) = NULL;
    FREEdoFreeNode( arg_node);
  }

  DBUG_RETURN( arg_node);
}

node *ALITUNFOLDdoUnfold(node *syntaxtree)
{
  info* info;

	DBUG_ENTER("ALITUNFOLDdoUnfold");

	info = MakeInfo();

  TRAVpush( TR_alitunfold);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();

  info = FreeInfo(info);

  DBUG_RETURN( syntaxtree);
}
