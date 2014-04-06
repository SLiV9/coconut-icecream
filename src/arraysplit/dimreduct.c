#include <stdio.h>
#include <string.h>

#include "dimreduct.h"

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

node* DIMREDUCTfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("DIMREDUCTfuncall");

  FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);

  if (STReq("__alloc", FUNCALL_NAME( arg_node)))
  {
    node* args = FUNCALL_ARGS( arg_node);
    DBUG_ASSERT( args != NULL, "empty alloc args!");
    node* expr = EXPRS_EXPR( args);
    while (EXPRS_NEXT( args) != NULL)
    {
      args = EXPRS_NEXT( args);
      expr = TBmakeBinop( BO_mul, expr, EXPRS_EXPR( args));
      BINOP_TYPE( expr) = VT_int;
    }
    
    node* prev;
    args = FUNCALL_ARGS( arg_node);
    while (args != NULL)
    {
      prev = args;
      args = EXPRS_NEXT( prev);
      EXPRS_NEXT( prev) = NULL;
      EXPRS_EXPR( prev) = NULL;
      FREEdoFreeNode( prev);
    }

    FUNCALL_ARGS( arg_node) = TBmakeExprs( expr, NULL);
  }

  DBUG_RETURN (arg_node);
}

static node* indxsToIndx(node* arg_node, node* indxs, node* dec, int scopediff)
{
  if (indxs == NULL)
    return NULL;

  node* dimdecs;
  switch (NODE_TYPE( dec))
  {
    case N_globdef:
      dimdecs = GLOBDEF_DIMDECS( dec);
      break;
    case N_globdec:
      dimdecs = GLOBDEC_DIMDECS( dec);
      break;
    case N_vardec:
      dimdecs = VARDEC_DIMDECS( dec);
      break;
    case N_param:
      dimdecs = PARAM_DIMDECS( dec);
      break;
    case N_iter:
    case N_dim:
      dimdecs = NULL;
      break;
    default:
      DBUG_ASSERT(0, "illegal varcall dec detected!");
  }

  DBUG_ASSERT(dimdecs != NULL, "no dimdecs!");

  node* newindx = NULL;
  node* curindxs = indxs;
  node* dim;
  node* dcall;
  node* bop = EXPRS_EXPR( indxs);

  while (DIMDECS_NEXT( dimdecs) != NULL)
  {
    dim = DIMDECS_DIM( dimdecs);
    dcall = TBmakeVarcall( STRcpy( DIM_NAME( dim)), NULL);
    VARCALL_DEC( dcall) = dim;
    VARCALL_SCOPEDIFF( dcall) = scopediff;
    VARCALL_TYPE( dcall) = VT_int;

    bop = TBmakeBinop( BO_mul, bop, dcall);
    BINOP_TYPE( bop) = VT_int;

    dimdecs = DIMDECS_NEXT( dimdecs);
    curindxs = EXPRS_NEXT( curindxs);
    bop = TBmakeBinop( BO_add, bop, EXPRS_EXPR( curindxs));
    BINOP_TYPE( bop) = VT_int;
  }

  node* prev;
  curindxs = indxs;
  while (curindxs != NULL)
  {
    prev = curindxs;
    curindxs = EXPRS_NEXT( prev);
    EXPRS_NEXT( prev) = NULL;
    EXPRS_EXPR( prev) = NULL;
    FREEdoFreeNode( prev);
  }

  if (bop != NULL)
  {
    newindx = TBmakeExprs( bop, NULL);
  }

  return newindx;
}

node* DIMREDUCTvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("DIMREDUCTvarcall");

  // no need to travdo

  VARCALL_INDX( arg_node) = indxsToIndx( arg_node, VARCALL_INDX( arg_node), \
      VARCALL_DEC( arg_node), VARCALL_SCOPEDIFF( arg_node));

  DBUG_RETURN (arg_node);
}

node* DIMREDUCTvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("DIMREDUCTvarlet");

  // no need to travdo

  VARLET_INDX( arg_node) = indxsToIndx( arg_node, VARLET_INDX( arg_node), \
      VARLET_DEC( arg_node), VARLET_SCOPEDIFF( arg_node));

  DBUG_RETURN (arg_node);
}

node *DIMREDUCTdoReduct(node *syntaxtree)
{
	DBUG_ENTER("DIMREDUCTdoReduct");

  TRAVpush( TR_dimreduct);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
