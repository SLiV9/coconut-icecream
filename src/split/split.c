#include "split.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "copy.h"
#include "memory.h"
#include "free.h"
#include <string.h>

struct INFO{
  node* head;
  node* last;
};

static node* dimdefsToDimdecs( node* dimdefs, const char* name)
{
  DBUG_ASSERT( NODE_TYPE(dimdefs) == N_exprs, "dimdefsToDimdecs called on "
      "something other than dimdefs!");

  node* defs = dimdefs;
  node* dimdecs = NULL;
  node* ddshead = NULL;
  node* ddslast = NULL;
  node* dim;
  int i = 0;

  while (defs != NULL)
  {
    char* stri = STRitoa(i);
    dim = TBmakeDim( STRcatn(4, "", stri, "_", name));
    dimdecs = TBmakeDimdecs( dim, NULL);
    MEMfree( stri);

    if (ddshead == NULL)
    {
      ddshead = dimdecs;
      ddslast = dimdecs;
    }
    else
    {
      DIMDECS_NEXT( ddslast) = dimdecs;
      ddslast = dimdecs;
    }

    defs = EXPRS_NEXT( defs);
    i++;
  }

  return ddshead;
}

static node* dimdecsToAlloc( node* dimdecs, int scopediff)
{
	DBUG_ASSERT( NODE_TYPE(dimdecs) == N_dimdecs, "dimdefsToAlloc called on "
			"something other than dimdecs!");

  node * decs = dimdecs;
  node * dim;
  node * call;

  node * argshead = NULL;
  node * argslast = NULL;
  node * args;

  while (decs != NULL)
  {
    dim = DIMDECS_DIM( decs);
    call = TBmakeVarcall( STRcpy( DIM_NAME( dim)), NULL);
    VARCALL_DEC( call) = dim;
    VARCALL_SCOPEDIFF( call) = scopediff;
    args = TBmakeExprs( call, NULL);

    if (argshead == NULL)
    {
      argshead = args;
      argslast = args;
    }
    else
    {
      EXPRS_NEXT( argslast) = args;
      argslast = args;
    }

    decs = DIMDECS_NEXT( decs);
  }

	return TBmakeFuncall( STRcpy("__alloc"), argshead);
}

static node* dimdefsToInstrs( node* arg_node, const char* name, \
    node* dimdefs, node* dimdecs, bool isglobal)
{
  DBUG_ASSERT( dimdefs != NULL, "dimdefsToAlloc called on null dimdefs!");
  DBUG_ASSERT( NODE_TYPE(dimdefs) == N_exprs, "dimdefsToAlloc called on "
      "something other than dimdefs!");
  DBUG_ASSERT( NODE_TYPE(dimdecs) == N_dimdecs, "dimdefsToAlloc called on "
      "something other than dimdecs!");

  int scopediff;
  if (isglobal)
    scopediff = NDSD_GLOBAL();
  else
    scopediff = NDSD_LOCAL();

  node * letje = TBmakeVarlet( STRcpy( name), NULL);
  VARLET_DEC( letje) = arg_node;
  VARLET_SCOPEDIFF( letje) = scopediff;
	node * instrs = TBmakeInstrs( TBmakeAssign( letje, \
					dimdecsToAlloc( dimdecs, scopediff)), NULL);

  node * decs = dimdecs;
  node * defs = dimdefs;
  node * dim;

  node * instrhead = NULL;
  node * instrlast = NULL;
  node * instr;

  while (decs != NULL && defs != NULL)
  {
    dim = DIMDECS_DIM( decs);
    letje = TBmakeVarlet( STRcpy( DIM_NAME( dim)), NULL);
    VARLET_DEC( letje) = dim;
    VARLET_SCOPEDIFF( letje) = scopediff;
    instr = TBmakeInstrs( TBmakeAssign( letje, \
        EXPRS_EXPR( defs)), NULL);

    if (instrhead == NULL)
    {
      instrhead = instr;
      instrlast = instr;
    }
    else
    {
      INSTRS_NEXT( instrlast) = instr;
      instrlast = instr;
    }

    EXPRS_EXPR( defs) = NULL;

    decs = DIMDECS_NEXT( decs);
    defs = EXPRS_NEXT( defs);
  }

  if (instrhead != NULL)
  {
    INSTRS_NEXT( instrlast) = instrs;
    instrs = instrhead;
  }

  return instrs;
}

node* SPLITglobdef(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITglobaldef");
 
	node * instr_assign = NULL;
	node * instr_alloc = NULL;
	node * instrs;
	node * last = NULL;
 
  if (GLOBDEF_EXPR(arg_node) != NULL){
  	node * letje = TBmakeVarlet( STRcpy( GLOBDEF_NAME(arg_node)), NULL);
  	VARLET_DEC(letje) = arg_node;
    VARLET_SCOPEDIFF(letje) = NDSD_GLOBAL();
    node * assign = TBmakeAssign( letje, GLOBDEF_EXPR(arg_node));
    GLOBDEF_EXPR(arg_node) = NULL;
    instr_assign = TBmakeInstrs(assign, NULL);
  }
  
  if (GLOBDEF_DIMDEFS( arg_node) != NULL)
  {
    GLOBDEF_DIMDECS( arg_node) = dimdefsToDimdecs( GLOBDEF_DIMDEFS( arg_node), \
        GLOBDEF_NAME( arg_node));
  	instr_alloc = dimdefsToInstrs( arg_node, GLOBDEF_NAME( arg_node), \
  			GLOBDEF_DIMDEFS( arg_node), GLOBDEF_DIMDECS( arg_node), TRUE);
    FREEdoFreeTree( GLOBDEF_DIMDEFS( arg_node));
    GLOBDEF_DIMDEFS( arg_node) = NULL;
    last = instr_alloc;
    while (INSTRS_NEXT( last) != NULL)
    {
        last = INSTRS_NEXT( last);
    }
  	INSTRS_NEXT( last) = instr_assign;
  }
  
  instrs = instr_assign;
  if (instr_alloc != NULL)
  {
  	instrs = instr_alloc;
  }
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
	node * last = NULL;
 
  if (VARDEC_EXPR(arg_node) != NULL){
  	node * letje = TBmakeVarlet( STRcpy( VARDEC_NAME(arg_node)), NULL);
  	VARLET_DEC(letje) = arg_node;
    VARLET_SCOPEDIFF(letje) = NDSD_LOCAL();
    node * assign = TBmakeAssign( letje ,VARDEC_EXPR(arg_node));
    VARDEC_EXPR(arg_node) = NULL;
    instr_assign = TBmakeInstrs(assign, NULL);
  }
  
  if (VARDEC_DIMDEFS( arg_node) != NULL)
  {
    VARDEC_DIMDECS( arg_node) = dimdefsToDimdecs( VARDEC_DIMDEFS( arg_node), \
        VARDEC_NAME( arg_node));
  	instr_alloc = dimdefsToInstrs( arg_node, VARDEC_NAME( arg_node), \
  			VARDEC_DIMDEFS( arg_node), VARDEC_DIMDECS( arg_node), FALSE);
    FREEdoFreeTree( VARDEC_DIMDEFS( arg_node));
    VARDEC_DIMDEFS( arg_node) = NULL;
    last = instr_alloc;
    while (INSTRS_NEXT( last) != NULL)
    {
        last = INSTRS_NEXT( last);
    }
    INSTRS_NEXT( last) = instr_assign;
  }
  
  instrs = instr_assign;
  if (instr_alloc != NULL)
  {
  	instrs = instr_alloc;
  }
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
