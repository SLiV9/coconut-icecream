#include <string.h>

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "ctinfo.h"
#include "memory.h"
#include "str.h"
#include "myglobals.h"
#include "free.h"
#include "print.h"

#include "gettype.h"
#include "ternary.h"

node* TERNARYcast(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TERNARYcast");

  CAST_EXPR( arg_node) = TRAVdo( CAST_EXPR( arg_node), arg_info);

  node *newn = NULL;
  node *cond, *left, *right, *zero;

  vtype ct, et;
  ct = CAST_TYPE( arg_node);
  et = getType(CAST_EXPR( arg_node));

  if (ct == VT_bool && et != VT_bool)
  {
    if (et == VT_int)
    {
      zero = TBmakeInt( 0);
    }
    else if (et == VT_float)
    {
      zero = TBmakeFloat( 0.0);
    }
    else
    {
      DBUG_ASSERT(0, "illegal cast expr type detected!");
    }

    newn = TBmakeBinop( BO_ne, CAST_EXPR( arg_node), zero);
    BINOP_TYPE( newn) = ct;
    CAST_EXPR( arg_node) = NULL;
  }
  else if (et == VT_bool && ct != VT_bool)
  {
    cond = CAST_EXPR( arg_node);
    CAST_EXPR( arg_node) = NULL;

    if (ct == VT_int)
    {
      left = TBmakeInt( 1);
      right = TBmakeInt( 0);
    }
    else if (ct == VT_float)
    {
      left = TBmakeFloat( 1.0);
      right = TBmakeFloat( 0.0);
    }
    else
    {
      DBUG_ASSERT(0, "illegal cast type detected!");
    }

    newn = TBmakeHoare(cond, left, right);
    HOARE_TYPE( newn) = ct;
  }

  if (newn == NULL)
  {
    newn = arg_node;
  }
  else
  {
    FREEdoFreeNode( arg_node);
  }

  DBUG_RETURN (newn);
}

node* TERNARYbinop(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TERNARYbinop");

  BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);
  BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);

  node *newn = NULL;
  node *cond, *left, *right;

  if (BINOP_OP( arg_node) == BO_and)
  {
    cond = BINOP_LEFT( arg_node);
    left = BINOP_RIGHT( arg_node);
    right = TBmakeBool( FALSE);
    newn = TBmakeHoare(cond, left, right);
    HOARE_TYPE( newn) = VT_bool;
  }
  else if (BINOP_OP( arg_node) == BO_or)
  {
    cond = BINOP_LEFT( arg_node);
    left = TBmakeBool( TRUE);
    right = BINOP_RIGHT( arg_node);
    newn = TBmakeHoare(cond, left, right);
    HOARE_TYPE( newn) = VT_bool;
  }

  if (newn == NULL)
  {
    newn = arg_node;
  }
  else
  {
    BINOP_LEFT( arg_node) = NULL;
    BINOP_RIGHT( arg_node) = NULL;
    FREEdoFreeNode( arg_node);
  }

  DBUG_RETURN (newn);
}

node *TERNARYdoReplace(node *syntaxtree)
{
	DBUG_ENTER("TERNARYdoReplace");

  TRAVpush( TR_ternary);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
}

