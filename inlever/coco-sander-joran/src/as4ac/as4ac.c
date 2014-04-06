#include "as4ac.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"

/*
 * INFO structure
 */
struct INFO {
  int countAdd;
  int countSub;
  int countMul;
  int countDiv;
  int countMod;
};

#define INFO_COUNTADD(n) ((n)->countAdd)
#define INFO_COUNTSUB(n) ((n)->countSub)
#define INFO_COUNTMUL(n) ((n)->countMul)
#define INFO_COUNTDIV(n) ((n)->countDiv)
#define INFO_COUNTMOD(n) ((n)->countMod)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_COUNTADD(result) = 0;
  INFO_COUNTSUB(result) = 0;
  INFO_COUNTMUL(result) = 0;
  INFO_COUNTDIV(result) = 0;
  INFO_COUNTMOD(result) = 0;
  
  return result;
}


static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}



node* AS4ACbinop (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTbinop");

  BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);

  switch (BINOP_OP( arg_node)) {
    case BO_add:
      INFO_COUNTADD(arg_info) = INFO_COUNTADD(arg_info) + 1;
      break;
    case BO_sub:
      INFO_COUNTSUB(arg_info) = INFO_COUNTSUB(arg_info) + 1;
      break;
    case BO_mul:
      INFO_COUNTMUL(arg_info) = INFO_COUNTMUL(arg_info) + 1;
      break;
    case BO_div:
      INFO_COUNTDIV(arg_info) = INFO_COUNTDIV(arg_info) + 1;
      break;
    case BO_mod:
      INFO_COUNTMOD(arg_info) = INFO_COUNTMOD(arg_info) + 1;
      break;
   	default: /*nothing*/;
  }

  BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}




node *AS4ACdoCount(node *syntaxtree)
{
	info* info;
	
	DBUG_ENTER("AS4ACdoCount");
	
	info = MakeInfo();

  TRAVpush( TR_as4ac);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();
  
  printf("\t{{ Number of Addition Operators: %d }}\n", INFO_COUNTADD(info));
  printf("\t{{ Number of Minus Operators:    %d }}\n", INFO_COUNTSUB(info));
  printf("\t{{ Number of Multiply Operators: %d }}\n", INFO_COUNTMUL(info));
  printf("\t{{ Number of Divide Operators:   %d }}\n", INFO_COUNTDIV(info));
  printf("\t{{ Number of Modulo Operators:   %d }}\n", INFO_COUNTMOD(info));
  
  info = FreeInfo(info);

  DBUG_RETURN( syntaxtree);
}
