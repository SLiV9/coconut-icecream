#include "split.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include <string.h>

struct INFO{
  node* head;
  node* last;
};

node* SPLITdeclars(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITdeclars");
 DBUG_RETURN (arg_node);
}
node* SPLITglobaldef(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITglobaldef");
 DBUG_RETURN (arg_node);
}

node* SPLITbody(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITbody");
 printf("hoi\n");
 arg_info->head = arg_info->last = NULL;
 BODY_VARDECS(arg_node) = TRAVopt(BODY_VARDECS(arg_node),arg_info); 
 INSTRS_NEXT(arg_info->last) = BODY_INSTRS(arg_node);
 BODY_INSTRS(arg_node) = arg_info->last;
 arg_info->head = arg_info->last = NULL;

 BODY_FUNDEFS(arg_node) = TRAVopt(BODY_FUNDEFS(arg_node),arg_info);
 BODY_INSTRS(arg_node) = TRAVopt(BODY_INSTRS(arg_node),arg_info);
 BODY_RETURN(arg_node) = TRAVopt(BODY_RETURN(arg_node),arg_info);

 DBUG_RETURN (arg_node);
}

node* SPLITvardec(node *arg_node, info *arg_info){
  DBUG_ENTER ("Splitvardec");
  printf("deccing\n");
  if (VARDEC_EXPR(arg_node) != NULL){
    node * assign = TBmakeAssign(TBmakeVarlet(VARDEC_NAME(arg_node),NULL),VARDEC_EXPR(arg_node));
    VARDEC_EXPR(arg_node) = NULL;
    node * instrs = TBmakeInstrs(assign,NULL);
    if(arg_info->head == NULL){
      arg_info->head = arg_info->last = instrs; 
    }else{
      arg_info->last = INSTRS_NEXT(arg_info->last) = instrs;
    }
  }
  DBUG_RETURN (arg_node);
}



node 
*SPLITdoSplit( node *syntaxtree)
{
  info info;

  DBUG_ENTER("SPLITdoSplit");

  DBUG_ASSERT( (syntaxtree!= NULL), "SPLITdoSplit called with empty syntaxtree");


  //  info = (info*) malloc(sizeof(info));
  TRAVpush( TR_prt);

  syntaxtree = TRAVdo( syntaxtree, &info);

  TRAVpop();

  //  info = FreeInfo(info);


  DBUG_RETURN( syntaxtree);
}

/*
node* AS4IPvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("AS4IPvarcall");

	char* oldname = VARCALL_NAME( arg_node);
  VARCALL_NAME( arg_node) = STRcat("__", oldname);
  free(oldname);

  DBUG_RETURN (arg_node);
}
*/
