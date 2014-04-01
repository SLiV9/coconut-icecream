#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <libgen.h>

#include "codegen.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "globals.h"
#include "ctinfo.h"
#include "memory.h"
#include "print.h"



#define mallocf(res,...)                        \
  res = malloc(sizeof(char)*128);               \
  sprintf(res,  __VA_ARGS__);

/*
 * INFO structure
 */

typedef struct asmline{
  char * line;
  struct asmline * next;
}asmline;


typedef union fi
{
  int i;
  float f;
}fi;

typedef struct constant{
  fi val;
  bool isfloat;
  struct constant * next;
}constant;


struct INFO {
  asmline * first;
  asmline * last;
  constant * consts;
  int lines;
};


char * mystrdup(char * str){
  int len = strlen(str);
  char * res = malloc(sizeof(char)*(len+1));
  strcpy(res,str);
  return res;
}


void addline(info * lines,char * str){

  asmline * ln = malloc(sizeof(asmline));
  ln->line = str;
  ln->next = NULL;

  if(lines->first ==NULL){
    lines->first = ln;
  }else{
    lines->last->next = ln;
  }

  lines->last = ln;
  lines->lines++;
}

int addint(info * inf,int i)
{
  constant * now = inf->consts;
  if(now == NULL){
    inf->consts = malloc(sizeof(constant));
    inf->consts->next = NULL;
    inf->consts->isfloat = 0;
    inf->consts->val.i = i;
    return 0;
  }
  int n = 0;
  while(now!=NULL){
    if(!now->isfloat && now->val.i == i){
      return n;
    }
    n++;
    if(now->next == NULL)
      break;
    now = now->next;
  }
  now->next = malloc(sizeof(constant));
  now->next->next = NULL;
  now->next->isfloat = 0;
  now->next->val.i = i;
  return n;
}

int addfloat(info * inf,float i)
{
  constant * now = inf->consts;
  if(now == NULL){
    inf->consts = malloc(sizeof(constant));
    inf->consts->next = NULL;
    inf->consts->isfloat = 1;
    inf->consts->val.f = i;
    return 0;
  }
  int n = 0;
  while(now!=NULL){
    if(now->isfloat && now->val.f == i){
      return n;
    }
    n++;
    if(now->next == NULL)
      break;
    now = now->next;
  }
  now->next = malloc(sizeof(constant));
  now->next->next = NULL;
  now->next->isfloat = 1;
  now->next->val.f = i;
  return n;
}





extern node* CODEGENbinop(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENbinop");

  DBUG_RETURN( arg_node);
}
extern node* CODEGENmonop(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENmonop");
  DBUG_RETURN( arg_node);
}
extern node* CODEGENint(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENint");
  DBUG_RETURN( arg_node);
}
extern node* CODEGENfloat(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENfloat");
  DBUG_RETURN( arg_node);
}
extern node* CODEGENbool(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENbool");
  DBUG_RETURN( arg_node);
}
extern node* CODEGENvarcall(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENvarcall");
  DBUG_RETURN( arg_node);
}


node *CODEGENdoCodegen(node *syntaxtree)
{

  DBUG_ENTER("CODEGENdoCodegen");

  TRAVpush( TR_codegen);
  info * code = malloc(sizeof(info));
  code->first = code->last = NULL;
  code->consts = NULL;
  code->lines = 0;
  syntaxtree = TRAVdo( syntaxtree, code);
  free(code);
  TRAVpop();



  DBUG_RETURN( syntaxtree);
}
