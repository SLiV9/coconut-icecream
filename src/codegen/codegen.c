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

#include "gettype.h"



#define mallocf(res,...)                        \
  res = malloc(sizeof(char)*128);               \
  sprintf(res,  __VA_ARGS__);

/*
 * INFO structure
 */

typedef struct asmline{
  char * line;
  char * comment;
  bool islabel;
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
  int labelcount;
};


char * mystrdup(char * str){
  int len = strlen(str);
  char * res = malloc(sizeof(char)*(len+1));
  strcpy(res,str);
  return res;
}


void addline(info * lines,char * str,char * com){

  asmline * ln = malloc(sizeof(asmline));
  ln->line = str;
  ln->comment = com;
  ln->islabel = FALSE;
  ln->next = NULL;

  if(lines->first ==NULL){
    lines->first = ln;
  }else{
    lines->last->next = ln;
  }

  lines->last = ln;
  lines->lines++;
}

void addlabel(info * lines, char* str){

  asmline * ln = malloc(sizeof(asmline));
  ln->line = str;
  ln->comment = NULL;
  ln->islabel = TRUE;
  ln->next = NULL;

  if(lines->first ==NULL){
    lines->first = ln;
  }else{
    lines->last->next = ln;
  }

  lines->last = ln;
  lines->lines++;
}

void printlines(info * lines){
  asmline* current = lines->first;
  while(current!=NULL){
    if (!current->islabel){
      printf("    %-40s",current->line);
      if (current->comment != NULL) {
        printf(";  %s",current->comment);
      }
      printf("\n");
    }
    else {
      printf("%s:\n",current->line);
    }
    current = current->next;
  }
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


void printconst(info * inf)
{
  constant * now = inf->consts;
  while(now!=NULL){
    if(now->isfloat){
      printf(".const float %f\n",now->val.f);
    }else{

      printf(".const int %i\n",now->val.i);
    }
    now = now->next;
  }
}





extern node* CODEGENhoare(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENhoare");
  char * line;
  char * label;
  int l_else, l_end;
  HOARE_COND( arg_node) = TRAVdo( HOARE_COND( arg_node), arg_info);
  l_else = arg_info->labelcount++;
  mallocf(line,"branch_f %d", l_else); addline(arg_info,line,NULL);
  HOARE_LEFT( arg_node) = TRAVdo( HOARE_LEFT( arg_node), arg_info);
  l_end = arg_info->labelcount++;
  mallocf(line,"jump %d", l_end); addline(arg_info,line,NULL);
  mallocf(label,"%d", l_else); addlabel(arg_info,label);
  HOARE_RIGHT( arg_node) = TRAVdo( HOARE_RIGHT( arg_node), arg_info);
  mallocf(label,"%d", l_end); addlabel(arg_info,label);
  DBUG_RETURN( arg_node);
}
extern node* CODEGENbinop(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENbinop");
  BINOP_LEFT( arg_node) = TRAVopt( BINOP_LEFT( arg_node), arg_info);
  BINOP_RIGHT( arg_node) = TRAVopt( BINOP_RIGHT( arg_node), arg_info);
  char * line;
  char * label;
  int l_either, l_end;

  switch(getType(BINOP_LEFT( arg_node))){
  case VT_int:
    switch(BINOP_OP(arg_node)){
    case BO_add:
      mallocf(line,"iadd");\

      addline(arg_info,line,NULL);
      break;
    case BO_sub:
      mallocf(line,"isub");
      addline(arg_info,line,NULL);

      break;
    case BO_mul:
      mallocf(line,"imul");
      addline(arg_info,line,NULL);

      break;
    case BO_div:
      mallocf(line,"idiv");
      addline(arg_info,line,NULL);
      break;
    case BO_mod:
      mallocf(line,"irem");
      addline(arg_info,line,NULL);
      break;
    case BO_lt:
      mallocf(line,"ilt");
      addline(arg_info,line,NULL);
      break;
    case BO_le:
      mallocf(line,"ile");
      addline(arg_info,line,NULL);
      break;
    case BO_gt:
      mallocf(line,"igt");
      addline(arg_info,line,NULL);
      break;
    case BO_ge:
      mallocf(line,"ige");
      addline(arg_info,line,NULL);
      break;
    case BO_eq:
      mallocf(line,"ieq");
      addline(arg_info,line,NULL);
      break;
    case BO_ne:
      mallocf(line,"ine");
      addline(arg_info,line,NULL);
      break;
    default:
      break;
    }
    break;

  case VT_float:
    switch(BINOP_OP(arg_node)){
    case BO_add:
      mallocf(line,"fadd");\
      addline(arg_info,line,NULL);
      break;
    case BO_sub:
      mallocf(line,"fsub");
      addline(arg_info,line,NULL);

      break;
    case BO_mul:
      mallocf(line,"fmul");
      addline(arg_info,line,NULL);

      break;
    case BO_div:
      mallocf(line,"fdiv");
      addline(arg_info,line,NULL);
      break;
    case BO_lt:
      mallocf(line,"flt");
      addline(arg_info,line,NULL);
      break;
    case BO_le:
      mallocf(line,"fle");
      addline(arg_info,line,NULL);
      break;
    case BO_gt:
      mallocf(line,"fgt");
      addline(arg_info,line,NULL);
      break;
    case BO_ge:
      mallocf(line,"fge");
      addline(arg_info,line,NULL);
      break;
    case BO_eq:
      mallocf(line,"feq");
      addline(arg_info,line,NULL);
      break;
    case BO_ne:
      mallocf(line,"fne");
      addline(arg_info,line,NULL);
      break;
    default:
      break;
    }

    break;

  case VT_bool:
    switch(BINOP_OP(arg_node)){
    case BO_add:
      l_either = arg_info->labelcount++;
      mallocf(line,"branch_t %d", l_either); addline(arg_info,line,NULL);
      mallocf(line,"branch_t %d", l_either); addline(arg_info,line,NULL);
      mallocf(line,"bloadc_f"); addline(arg_info,line,NULL);
      l_end = arg_info->labelcount++;
      mallocf(line,"jump %d", l_end); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_either); addlabel(arg_info,label);
      mallocf(line,"bloadc_t"); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_end); addlabel(arg_info,label);
      break;
    case BO_mul:
      l_either = arg_info->labelcount++;
      mallocf(line,"branch_f %d", l_either); addline(arg_info,line,NULL);
      mallocf(line,"branch_f %d", l_either); addline(arg_info,line,NULL);
      mallocf(line,"bloadc_t"); addline(arg_info,line,NULL);
      l_end = arg_info->labelcount++;
      mallocf(line,"jump %d", l_end); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_either); addlabel(arg_info,label);
      mallocf(line,"bloadc_f"); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_end); addlabel(arg_info,label);
      break;
    case BO_eq:
      mallocf(line,"beq");
      addline(arg_info,line,NULL);
      break;
    case BO_ne:
      mallocf(line,"bne");
      addline(arg_info,line,NULL);
      break;
    default:
      break;
    }

    break;

  default: break;

  }
  DBUG_RETURN( arg_node);
}
extern node* CODEGENmonop(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENmonop");
  MONOP_EXPR( arg_node) = TRAVopt( MONOP_EXPR( arg_node), arg_info);
  char * line;
  mallocf(line,"monop");
  addline(arg_info,line,NULL);
  DBUG_RETURN( arg_node);
}
extern node* CODEGENint(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENint");
  char * line;
  char * comment;
  int cnum = addint(arg_info,INT_VALUE(arg_node));
  mallocf(line,"iloadc %i",cnum);
  mallocf(comment,"%d",INT_VALUE( arg_node));
  addline(arg_info,line,comment);
  DBUG_RETURN( arg_node);
}
extern node* CODEGENfloat(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENfloat");
  char * line;
  char * comment;
  int cnum = addint(arg_info,FLOAT_VALUE(arg_node));
  mallocf(line,"floadc %i",cnum);
  mallocf(comment,"%f",FLOAT_VALUE( arg_node));
  addline(arg_info,line,comment);
  DBUG_RETURN( arg_node);
}
extern node* CODEGENbool(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENbool");
  char * line;
  if(BOOL_VALUE(arg_node)){
    mallocf(line,"bloadc_t");
  }
  else{
    mallocf(line,"bloadc_f");
  }
  addline(arg_info,line,NULL);

  DBUG_RETURN( arg_node);
}
extern node* CODEGENvarcall(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENvarcall");
  char * line;
  char * comment;
  mallocf(line,"varcal something");
  mallocf(comment,"%s",VARCALL_NAME( arg_node));
  addline(arg_info,line,comment);

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
  code->labelcount = 0;
  syntaxtree = TRAVdo( syntaxtree, code);

  TRAVpop();
  printf("+-+-+-+-+-+-+-+-+-+-+-+-\n");
  printlines(code);
  printf("+-+-+-+-+-+-+-+-+-+-+-+-\n");
  printconst(code);
  printf("+-+-+-+-+-+-+-+-+-+-+-+-\n");
  DBUG_RETURN( syntaxtree);
}
