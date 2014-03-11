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

node* SPLITglobdef(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITglobaldef");
  if (GLOBDEF_EXPR(arg_node) != NULL){
    node * assign = TBmakeAssign(TBmakeVarlet(GLOBDEF_NAME(arg_node),NULL),GLOBDEF_EXPR(arg_node));
    GLOBDEF_EXPR(arg_node) = NULL;
    node * instrs = TBmakeInstrs(assign,NULL);
    if(arg_info->head == NULL){
      arg_info->head = arg_info->last = instrs; 
    }else{
      arg_info->last = INSTRS_NEXT(arg_info->last) = instrs;
    }
  }
  // dont have to trav down nothitg to change there
 DBUG_RETURN (arg_node);
}

node* SPLITbody(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITbody");
 info this;
 this.head = NULL;
 this.last = NULL;
 BODY_VARDECS(arg_node) = TRAVopt(BODY_VARDECS(arg_node),&this); 
 INSTRS_NEXT(this.last) = BODY_INSTRS(arg_node);
 BODY_INSTRS(arg_node) = this.head;


 BODY_FUNDEFS(arg_node) = TRAVopt(BODY_FUNDEFS(arg_node),arg_info);
 BODY_INSTRS(arg_node) = TRAVopt(BODY_INSTRS(arg_node),arg_info);
 BODY_RETURN(arg_node) = TRAVopt(BODY_RETURN(arg_node),arg_info);

 DBUG_RETURN (arg_node);
}

node* SPLITvardec(node *arg_node, info *arg_info){
  DBUG_ENTER ("Splitvardec");
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
  info.head = info.last = NULL;

  DBUG_ENTER("SPLITdoSplit");

  DBUG_ASSERT( (syntaxtree!= NULL), "SPLITdoSplit called with empty syntaxtree");

  TRAVpush( TR_split);

  syntaxtree = TRAVdo( syntaxtree, &info);

  TRAVpop();
  node * header = TBmakeHeader("__init",VT_void,NULL);

  node * body = TBmakeBody(NULL,NULL,info.head,NULL);

  node * fun = TBmakeFundef(TRUE,header,body);
  node * decl = TBmakeDeclars(fun,syntaxtree);
  DBUG_RETURN( decl);
}
