#include "split.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "copy.h"
#include <string.h>

struct INFO{
  node* head;
  node* last;
};

node* SPLITglobdef(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITglobaldef");
 
  if (GLOBDEF_EXPR(arg_node) != NULL){
    node * assign = TBmakeAssign(TBmakeVarlet(STRcpy(GLOBDEF_NAME(arg_node)),NULL),GLOBDEF_EXPR(arg_node));
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
 if(this.head != NULL){
   INSTRS_NEXT(this.last) = BODY_INSTRS(arg_node);
   BODY_INSTRS(arg_node) = this.head;
 }

 BODY_FUNDEFS(arg_node) = TRAVopt(BODY_FUNDEFS(arg_node),arg_info);
 BODY_INSTRS(arg_node) = TRAVopt(BODY_INSTRS(arg_node),arg_info);
 BODY_RETURN(arg_node) = TRAVopt(BODY_RETURN(arg_node),arg_info);
 

 DBUG_RETURN (arg_node);
}

static node* dimdefsToAlloc( node* dimdefs)
{
	DBUG_ASSERT( NODE_TYPE(dimdefs) == N_exprs, "dimdefsToAlloc called on "
			"something other than dimdefs!");
	return TBmakeFuncall( STRcpy("__alloc"), COPYdoCopy(dimdefs));
}

node* SPLITvardec(node *arg_node, info *arg_info){
  DBUG_ENTER ("Splitvardec");
  if (VARDEC_EXPR(arg_node) != NULL){
    node * assign = TBmakeAssign( \
    		TBmakeVarlet( STRcpy(VARDEC_NAME(arg_node)), NULL), \
    		VARDEC_EXPR(arg_node));
    VARDEC_EXPR(arg_node) = NULL;
    node* arralloc = NULL;
    if (VARDEC_DIMDEFS( arg_node) != NULL)
    {
    	node* alloc = dimdefsToAlloc( VARDEC_DIMDEFS( arg_node));
    	arralloc = TBmakeInstrs(
    			TBmakeAssign( \
    					TBmakeVarlet( STRcpy( VARDEC_NAME( arg_node)), NULL), \
    					alloc), \
    			NULL);
    }
    node * instrs = TBmakeInstrs(assign, arralloc);
    node * last = instrs;
    if (arralloc != NULL)
    {
    	last = arralloc;
    }
    if(arg_info->head == NULL){
      arg_info->head = arg_info->last = last; 
    }else{
      arg_info->last = INSTRS_NEXT(arg_info->last) = last;
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

  node * header = TBmakeHeader(STRcpy("__init"),VT_void,NULL);
  node * body = TBmakeBody(NULL,NULL,info.head,NULL);
  node * fun = TBmakeFundef(TRUE,header,body);
  NODE_LINE(fun) = -1;
  
  node * headerA = TBmakeHeader(STRcpy("__alloc"), VT_void, NULL);
  node * funA = TBmakeFundec(headerA);
  NODE_LINE(funA) = 0;

  node * decl = TBmakeDeclars(fun, TBmakeDeclars(funA,   syntaxtree));


  DBUG_RETURN( decl);



}
