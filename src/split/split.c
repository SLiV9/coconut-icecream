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

static node* dimdefsToAlloc( node* dimdefs)
{
	DBUG_ASSERT( NODE_TYPE(dimdefs) == N_exprs, "dimdefsToAlloc called on "
			"something other than dimdefs!");
	return TBmakeFuncall( STRcpy("__alloc"), COPYdoCopy(dimdefs));
}

static node* dimdefsToInstrs( const char* name, node* dimdefs)
{
  if (dimdefs != NULL)
  {
  	return TBmakeInstrs( TBmakeAssign( TBmakeVarlet( STRcpy( name), NULL), \
  					dimdefsToAlloc( dimdefs)), NULL);
  }
  
  return NULL;
}

node* SPLITglobdef(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITglobaldef");
 
	node * instr_assign = NULL;
	node * instr_alloc = NULL;
	node * instrs;
	node * last;
 
  if (GLOBDEF_EXPR(arg_node) != NULL){
  	node * letje = TBmakeVarlet( STRcpy( GLOBDEF_NAME(arg_node)), NULL);
  	VARLET_DEC(letje) = arg_node;
    node * assign = TBmakeAssign( letje, GLOBDEF_EXPR(arg_node));
    GLOBDEF_EXPR(arg_node) = NULL;
    instr_assign = TBmakeInstrs(assign, NULL);
  }
  
  if (GLOBDEF_DIMDEFS( arg_node) != NULL)
  {
  	instr_alloc = dimdefsToInstrs( GLOBDEF_NAME( arg_node), \
  			GLOBDEF_DIMDEFS( arg_node));
  	INSTRS_NEXT( instr_alloc) = instr_assign;
  }
  
  instrs = instr_assign;
  if (instr_alloc != NULL)
  {
  	instrs = instr_alloc;
  }
  
  last = instr_alloc;
  if (instr_assign != NULL)
  {
  	last = instr_assign;
  }
  
  if (instrs != NULL)
  {
		if(arg_info->head == NULL){
		  arg_info->head = instrs;
		  arg_info->last = last; 
		}else{
			INSTRS_NEXT(arg_info->last) = instrs;
			arg_info->last = last;
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

node* SPLITvardec(node *arg_node, info *arg_info){
  DBUG_ENTER ("Splitvardec");
  
  node * instr_assign = NULL;
	node * instr_alloc = NULL;
	node * instrs;
	node * last;
 
  if (VARDEC_EXPR(arg_node) != NULL){
  	node * letje = TBmakeVarlet( STRcpy( VARDEC_NAME(arg_node)), NULL);
  	VARLET_DEC(letje) = arg_node;
    node * assign = TBmakeAssign( letje ,VARDEC_EXPR(arg_node));
    VARDEC_EXPR(arg_node) = NULL;
    instr_assign = TBmakeInstrs(assign, NULL);
  }
  
  if (VARDEC_DIMDEFS( arg_node) != NULL)
  {
  	instr_alloc = dimdefsToInstrs( VARDEC_NAME( arg_node), \
  			VARDEC_DIMDEFS( arg_node));
  	INSTRS_NEXT( instr_alloc) = instr_assign;
  }
  
  instrs = instr_assign;
  if (instr_alloc != NULL)
  {
  	instrs = instr_alloc;
  }
  
  last = instr_alloc;
  if (instr_assign != NULL)
  {
  	last = instr_assign;
  }
  
  if (instrs != NULL)
  {
		if(arg_info->head == NULL){
		  arg_info->head = instrs;
		  arg_info->last = last; 
		}else{
			INSTRS_NEXT(arg_info->last) = instrs;
			arg_info->last = last;
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

  node * decl = TBmakeDeclars(fun, syntaxtree);


  DBUG_RETURN( decl);



}
