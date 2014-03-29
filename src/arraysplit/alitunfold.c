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
		node* indxhead;
    node* indxlast;
    node* alitstack;
    node* assignstack;
};

#define INFO_INDXHEAD(n) ((n)->indxhead)
#define INFO_INDXLAST(n) ((n)->indxlast)
#define INFO_ALITS(n) ((n)->alitstack)
#define INFO_ASSIGNS(n) ((n)->assignstack)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_INDXHEAD(result) = NULL;
  INFO_INDXLAST(result) = NULL;
  INFO_ALITS(result) = NULL;
  INFO_ASSIGNS(result) = NULL;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

static void pushIndx(info* arg_info, int x)
{
  node* intje = TBmakeInt(x);
  node* indxs = TBmakeExprs( intje, NULL);

  if (INFO_INDXHEAD( arg_info) == NULL)
  {
    INFO_INDXHEAD( arg_info) = indxs;
    INFO_INDXLAST( arg_info) = indxs;
  }
  else
  {
    EXPRS_NEXT( INFO_INDXLAST( arg_info)) = indxs;
    INFO_INDXLAST( arg_info) = indxs;
  }
}

static int popIndx(info* arg_info)
{
  DBUG_ASSERT(INFO_INDXHEAD( arg_info) != NULL, \
      "called pop on an empty stack!");

  node* intje = EXPRS_EXPR( INFO_INDXLAST( arg_info));
  return INT_VALUE( intje);
}

static node* cloneIndx(info* arg_info)
{
  DBUG_ASSERT(INFO_INDXHEAD( arg_info) != NULL, \
      "called clone on an empty stack!");

  return COPYdoCopy(INFO_INDXHEAD( arg_info));
}

node* ALITUNFOLDassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ALITUNFOLDassign");
  
  node* expr = ASSIGN_EXPR( arg_node);
  
  if (NODE_TYPE( expr) == N_arraylit)
  {
    /*int x = 0;
    do
    {
      if (NODE_TYPE( expr) == N_arraylit)
      {
        INFO_ALITS( arg_info) = TBmakeExprs( expr, INFO_ALITS( arg_info));
        pushIndx(arg_info, x);
        x = 0;
        expr = EXPRS_EXPR( expr);
      }
      else
      {
        // make assignment
        expr = INFO_ALITS( arg_info);
        if (expr != NULL)
        {
          INFO_ALITS( arg_info) = EXPRS_NEXT( INFO_ALITS( arg_info));
          x = popIndx(arg_info) + 1;
        }
        else
        {
        }
        if (
      }
    }
    while (expr != NULL);*/
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
