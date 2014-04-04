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

/*
 * INFO structure
 */
struct INFO {
    node* head;
    node* last;
    int dirty;
};

#define INFO_HEAD(n) ((n)->head)
#define INFO_LAST(n) ((n)->last)
#define INFO_DIRTY(n) ((n)->dirty)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_HEAD(result) = NULL;
  INFO_LAST(result) = NULL;
  INFO_DIRTY(result) = 0;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

static void add(node* oldlet, info* arg_info, int x, node* expr)
{
  node* letje = COPYdoCopy( oldlet);
  node* indxs = VARLET_INDX( letje);

  if (indxs != NULL)
  {
    node* ixs = indxs;
    while (EXPRS_NEXT( ixs) != NULL)
    {
      ixs = EXPRS_NEXT( ixs);
    }
    EXPRS_NEXT( ixs) = TBmakeExprs( TBmakeInt(x), NULL);
  }
  else
  {
    VARLET_INDX( letje) = TBmakeExprs( TBmakeInt(x), NULL);
  }

  node* ass = TBmakeAssign( letje, expr);
  node* instrs = TBmakeInstrs( ass, NULL);

  if (INFO_HEAD( arg_info) == NULL)
  {
    INFO_HEAD( arg_info) = instrs;
    INFO_LAST( arg_info) = instrs;
  }
  else
  {
    INSTRS_NEXT( INFO_LAST( arg_info)) = instrs;
    INFO_LAST( arg_info) = instrs;
  }
}



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
    // skip instrs, it has been replaced by info_head-info_last
    INSTRS_NEXT( INFO_LAST( arg_info)) = INSTRS_NEXT( arg_node);
    instrs = INFO_HEAD( arg_info);
    INFO_HEAD( arg_info) = NULL;
    INFO_LAST( arg_info) = NULL;
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
      add( letje, arg_info, i, EXPRS_EXPR( exprs));

      i++;
      exprs = EXPRS_NEXT( exprs);
    }
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
