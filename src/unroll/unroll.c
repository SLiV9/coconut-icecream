#include "split.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "free.h"
#include "copy.h"
#include <string.h>

typedef struct replace{
  node * iter;
  int val;
  struct replace * next;
}replace;

struct INFO{
  replace * first;
};


node* UNROLLfor(node *arg_node, info *arg_info){
  DBUG_ENTER ("UNROLLfor");

  FOR_FROM( arg_node) = TRAVdo( FOR_FROM( arg_node), arg_info);
  int incr;
  FOR_TO( arg_node) = TRAVdo( FOR_TO( arg_node), arg_info);

  if(FOR_INCR( arg_node) != NULL){
    FOR_INCR( arg_node) = TRAVdo( FOR_INCR( arg_node), arg_info);
    incr = INT_VALUE(FOR_INCR( arg_node));
  }else{
    incr  = 1;
  }
  if( NODE_TYPE(FOR_FROM( arg_node)) == N_int &&
      NODE_TYPE(FOR_TO( arg_node)) == N_int &&
      (FOR_INCR( arg_node)==NULL ||NODE_TYPE(FOR_INCR( arg_node)) == N_int ) &&
      incr !=0 &&
      (INT_VALUE(FOR_TO( arg_node))-INT_VALUE(FOR_FROM( arg_node)))/incr <=5){
    // unroll the dham loop


    node * first = NULL;
    int from = INT_VALUE(FOR_FROM( arg_node));
    int to = INT_VALUE(FOR_TO( arg_node));


    replace * rep = malloc(sizeof(replace));

    rep->next = arg_info->first;


    rep->iter = FOR_ITERDEC( arg_node);
    arg_info->first = rep;
    node * loopbody;
    printf("incr: %i\n",incr);
    for(int i  = from;(incr < 0 && i > to)||(incr>0 && i < to);i+=incr){
      printf("Now: %i\n",i);

      if((incr < 0 && i+incr > to)||(incr>0 && i+incr < to)){
        loopbody = COPYdoCopy(FOR_DO( arg_node));
      }else{
        printf("last: %i\n",i);
        loopbody = FOR_DO( arg_node);
      }

      arg_info->first->val = i;

      loopbody = TRAVdo(loopbody, arg_info);

      if(first == NULL){
        first = loopbody;
      }else{
        node * current = first;
        while(INSTRS_NEXT(current)!=NULL)
          current = INSTRS_NEXT(current);
        INSTRS_NEXT(current) = loopbody;
      }
    }
    // pop of iter
    replace * tmp = arg_info->first->next;
    free(arg_info->first);
    arg_info->first = tmp;
    FOR_DO(arg_node) = NULL;
    FREEdoFreeTree(arg_node);
    DBUG_RETURN( first);


  }
  DBUG_RETURN( arg_node);
}

node* UNROLLinstrs(node *arg_node, info *arg_info){
  DBUG_ENTER ("UNROLLinstrs");
  node * next = NULL;
  node * ins = TRAVdo(INSTRS_INSTR(arg_node), arg_info);
  if(INSTRS_NEXT(arg_node)!=NULL)
    next = TRAVdo(INSTRS_NEXT(arg_node), arg_info);

  if(NODE_TYPE(ins) == N_instrs){
    node * current = ins;
    while(INSTRS_NEXT(current)!=NULL){
      current = INSTRS_NEXT(current);
    }
    INSTRS_NEXT(current) = next;
    DBUG_RETURN( ins);
  }else{
    INSTRS_NEXT(arg_node) = next;
    INSTRS_INSTR(arg_node) = ins;
    DBUG_RETURN( arg_node);
  }
  DBUG_RETURN( arg_node);
}

node* UNROLLvarcall(node *arg_node, info *arg_info){
  DBUG_ENTER ("UNROLLvarcall");

  node * orign = VARCALL_DEC(arg_node);
  replace * cur = arg_info->first;
  while(cur != NULL && cur->iter != orign){
    cur  = cur->next;
  }
  if(cur == NULL){
    DBUG_RETURN( arg_node);
  }else{
    FREEdoFreeNode(arg_node);
    DBUG_RETURN( TBmakeInt(cur->val));
  }

}


node
*UNROLLdoUnroll( node *syntaxtree)
{

  DBUG_ENTER("UNROLLdoUnroll");


  DBUG_ASSERT( (syntaxtree!= NULL), "UNROLL called with empty syntaxtree");

  TRAVpush( TR_unroll);
  info vars;
  vars.first = NULL;
  syntaxtree = TRAVdo( syntaxtree, &vars);

  TRAVpop();


  DBUG_RETURN( syntaxtree);



}
