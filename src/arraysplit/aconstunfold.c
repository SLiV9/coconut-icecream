#include <stdio.h>
#include <string.h>

#include "aconstunfold.h"

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

#include "aunfold_info.h"


node* ACONSTUNFOLDinstrs(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ACONSTUNFOLDinstrs");
  
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

node* ACONSTUNFOLDassign(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("ACONSTUNFOLDassign");

  node* letje = ASSIGN_LET( arg_node);
  node* dec = VARLET_DEC( letje);
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

  node* expr = ASSIGN_EXPR( arg_node);
  bool isAlloc = ((NODE_TYPE( expr) == N_funcall) && \
      STReq("__alloc", FUNCALL_NAME( expr)));

  if (dimdecs != NULL && !isAlloc && VARLET_INDX( letje) == NULL)
  {
    node* itr = TBmakeIter(STRcpy("j"));
    node* itrcall = TBmakeVarcall(STRcpy(ITER_NAME(itr)), NULL);
    VARCALL_DEC( itrcall) = itr;
    VARCALL_SCOPEDIFF( itrcall) = NDSD_LOCAL();
    VARCALL_TYPE( itrcall) = VT_int;
    VARCALL_DEPTH( itrcall) = 0;
  
    node* instr = makeAssignInstrs( letje, itrcall, expr);

    node* max = NULL;
    node* dim;
    node* dcall;

    do
    {
      dim = DIMDECS_DIM( dimdecs);
      dcall = TBmakeVarcall( STRcpy( DIM_NAME( dim)), NULL);
      VARCALL_DEC( dcall) = dim;
      VARCALL_SCOPEDIFF( dcall) = VARLET_SCOPEDIFF( letje);
      VARCALL_TYPE( dcall) = VT_int;

      if (max != NULL)
      {
        max = TBmakeBinop( BO_mul, max, dcall);
        BINOP_TYPE( max) = VT_int;
      }
      else
      {
        max = dcall;
      }

      dimdecs = DIMDECS_NEXT( dimdecs);
    }
    while (dimdecs != NULL);

    node* to = max;

    node* fortje = TBmakeFor( itr, TBmakeInt(0), to, NULL, instr);
    node* instrs = TBmakeInstrs( fortje, NULL);
    addInstr( arg_info, instrs);
  }

  DBUG_RETURN( arg_node);
}

node *ACONSTUNFOLDdoUnfold(node *syntaxtree)
{
  info* info;

	DBUG_ENTER("ACONSTUNFOLDdoUnfold");

	info = MakeInfo();

  TRAVpush( TR_aconstunfold);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();

  info = FreeInfo(info);

  DBUG_RETURN( syntaxtree);
}
