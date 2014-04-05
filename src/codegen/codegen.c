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




extern node* CODEGENfundef(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENfundef");
  char * label;
  mallocf(label,"%s", HEADER_NAME( FUNDEF_HEAD( arg_node)));
  addlabel(arg_info,label);

  // no need to trav to header

  FUNDEF_BODY( arg_node) = TRAVdo( FUNDEF_BODY( arg_node), arg_info);

  DBUG_RETURN( arg_node);
}
extern node* CODEGENbody(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENbody");
  char * line;

  int nlocs = 0;
  node * vdecs = BODY_VARDECS( arg_node);
  while (vdecs != NULL)
    {
      nlocs++;
      vdecs = VARDECS_NEXT( vdecs);
    }
  mallocf(line,"esr %d", nlocs); addline(arg_info,line,NULL);

  BODY_INSTRS( arg_node) = TRAVopt( BODY_INSTRS( arg_node), arg_info);

  if (BODY_RETURN( arg_node) != NULL)
    {
      BODY_RETURN( arg_node) = TRAVopt( BODY_RETURN( arg_node), arg_info);
      switch (getType( BODY_RETURN( arg_node)))
        {
        case VT_int:
          mallocf(line,"ireturn");
          addline(arg_info,line,NULL);
          break;
        case VT_float:
          mallocf(line,"freturn");
          addline(arg_info,line,NULL);
          break;
        case VT_bool:
          mallocf(line,"breturn");
          addline(arg_info,line,NULL);
          break;
        default:
          DBUG_ASSERT( 0, "illegal return type detected!");
        }
    }
  else
    {
      mallocf(line,"return"); addline(arg_info,line,NULL);
    }

  mallocf(line," "); addline(arg_info,line,NULL); // empty line

  BODY_FUNDEFS( arg_node) = TRAVopt( BODY_FUNDEFS( arg_node), arg_info);

  DBUG_RETURN( arg_node);
}
extern node* CODEGENfunstate(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENfunstate");
  char * line;
  FUNSTATE_CALL( arg_node) = TRAVdo( FUNSTATE_CALL( arg_node), arg_info);
  switch (FUNCALL_TYPE( FUNSTATE_CALL( arg_node)))
    {
    case VT_int:
      mallocf(line,"ipop");
      addline(arg_info,line,NULL);
      break;
    case VT_float:
      mallocf(line,"fpop");
      addline(arg_info,line,NULL);
      break;
    case VT_bool:
      mallocf(line,"bpop");
      addline(arg_info,line,NULL);
      break;
    case VT_void:
      /* do nothing */;
      break;
    default:
      DBUG_ASSERT( 0, "illegal funcall type detected!");
    }
  DBUG_RETURN( arg_node);
}

extern node* CODEGENvarcall(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENvarCall");

  char c;
  if(VARCALL_TYPE(arg_node)==VT_int)
    c = 'i';
  else if(VARCALL_TYPE(arg_node)==VT_float)
    c = 'f';
  else
    c = 'b';

  int diff = -1-VARCALL_SCOPEDIFF( arg_node);
  int pos;
  char * line;
  if(NODE_TYPE(VARCALL_DEC( arg_node)) == N_vardec){
    pos = VARDEC_SCOPEPOS(VARCALL_DEC( arg_node))-1;
    if(diff == 0){
      mallocf(line,"%cload %i",c,pos);
    }else{
      mallocf(line,"%cloadn %i %i",c,diff,pos);
    }
    addline(arg_info,line,NULL);
  }else{
    pos = GLOBDEF_GLOBALPOS(VARCALL_DEC( arg_node))-1;
    mallocf(line,"%cloadg %i",c,pos);
    addline(arg_info,line,NULL);
  }

  DBUG_RETURN( arg_node);
}


extern node* CODEGENvarlet(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENvarLet");

  char c;
  if(VARLET_TYPE(arg_node)==VT_int)
    c = 'i';
  else if(VARLET_TYPE(arg_node)==VT_float)
    c = 'f';
  else
    c = 'b';

  int diff = -1-VARLET_SCOPEDIFF( arg_node);
  int pos;
  char * line;
  char * comm; 
  mallocf(comm,"%s",VARLET_NAME(arg_node));
  if(NODE_TYPE(VARLET_DEC( arg_node)) == N_vardec){
    pos = VARDEC_SCOPEPOS(VARLET_DEC( arg_node))-1;
    if(diff == 0){
      mallocf(line,"%cstore %i",c,pos);
    }else{
      mallocf(line,"%cstoren %i %i",c,diff,pos);
    }
    addline(arg_info,line,comm);
  }else{
    pos = GLOBDEF_GLOBALPOS(VARLET_DEC( arg_node))-1;
    mallocf(line,"%cstoreg %i",c,pos);
    addline(arg_info,line,comm);
  }

  DBUG_RETURN( arg_node);
}

extern node* CODEGENfuncall(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENfuncall");
  char * line;
  char * comment;
  FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);

  if (STReq("__alloc", FUNCALL_NAME( arg_node)))
    {
      mallocf(line,"alloc something");
      addline(arg_info,line,NULL);
      DBUG_RETURN( arg_node);
    }

  node * dec = FUNCALL_DEC( arg_node);
  char * name;
  node * args = FUNCALL_ARGS( arg_node);
  int i, nargs;
  switch (NODE_TYPE( dec))
    {
    case N_fundec:
      i = FUNDEC_IMPORTPOS( dec) - 1;
      mallocf(line,"jsre %d", i);
      break;
    case N_fundef:
      nargs = 0;
      while (args != NULL)
        {
          nargs++;
          args = EXPRS_NEXT( args);
        }
      name = HEADER_NAME( FUNDEF_HEAD( dec));
      mallocf(line,"jsr %d %s", nargs, name);
      break;
    default:
      DBUG_ASSERT( 0, "illegal funcall dec type detected!");
    }
  mallocf(comment,"%s", FUNCALL_NAME( arg_node));
  addline(arg_info,line,comment);
  DBUG_RETURN( arg_node);
}
extern node* CODEGENassign(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENassign");
  ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);
  ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
  DBUG_RETURN( arg_node);
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
      DBUG_ASSERT(0, "illegal binary operator detected!");
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
      DBUG_ASSERT(0, "illegal binary operator detected!");
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
      DBUG_ASSERT(0, "illegal binary operator detected!");
    }

    break;

  default: break;

  }
  DBUG_RETURN( arg_node);
}

extern node* CODEGENcast(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENcast");
  CAST_EXPR( arg_node) = TRAVopt( CAST_EXPR( arg_node), arg_info);
  char * line;
  char * label;
  int l_either, l_end;

  switch(getType(CAST_EXPR( arg_node))){
  case VT_int:
    switch( CAST_TYPE(arg_node)){
    case VT_int:
      //nothing to do
      break;
    case VT_float:
      //int -> float
      mallocf(line,"i2f");
      addline(arg_info,line,NULL);
      break;
    case VT_bool:
      //int -> bool
      l_either = arg_info->labelcount++;
      mallocf(line,"iloadc_0"); addline(arg_info,line,";-x-x-x");
      mallocf(line,"ieq"); addline(arg_info,line,NULL);
      mallocf(line,"branch_f %d", l_either); addline(arg_info,line,NULL);
      mallocf(line,"bloadc_f"); addline(arg_info,line,NULL);
      l_end = arg_info->labelcount++;
      mallocf(line,"jump %d", l_end); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_either); addlabel(arg_info,label);
      mallocf(line,"bloadc_t"); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_end); addlabel(arg_info,label);
      break;
    default:
      DBUG_ASSERT(0, "illegal cast detected!");
    }
    break;

  case VT_float:
    switch( CAST_TYPE(arg_node)){
    case VT_int:
      // float -> int
      mallocf(line,"f2i");
      addline(arg_info,line,NULL);
      break;
    case VT_float:
      //nothing to be done
      break;
    case VT_bool:
      //float -> bool
      l_either = arg_info->labelcount++;
      mallocf(line,"floadc_0"); addline(arg_info,line,NULL);
      mallocf(line,"feq"); addline(arg_info,line,NULL);
      mallocf(line,"branch_f %d", l_either); addline(arg_info,line,NULL);
      mallocf(line,"bloadc_f"); addline(arg_info,line,NULL);
      l_end = arg_info->labelcount++;
      mallocf(line,"jump %d", l_end); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_either); addlabel(arg_info,label);
      mallocf(line,"bloadc_t"); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_end); addlabel(arg_info,label);
      break;
    default:
      DBUG_ASSERT(0, "illegal cast detected!");
    }    break;

  case VT_bool:
    switch( CAST_TYPE(arg_node)){
    case VT_int:
      l_either = arg_info->labelcount++;
      mallocf(line,"branch_f %d", l_either); addline(arg_info,line,NULL);
      mallocf(line,"iloadc_1"); addline(arg_info,line,NULL);
      l_end = arg_info->labelcount++;
      mallocf(line,"jump %d", l_end); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_either); addlabel(arg_info,label);
      mallocf(line,"iloadc_0"); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_end); addlabel(arg_info,label);
      break;
    case VT_float:
      l_either = arg_info->labelcount++;
      mallocf(line,"branch_f %d", l_either); addline(arg_info,line,NULL);
      mallocf(line,"floadc_1"); addline(arg_info,line,NULL);
      l_end = arg_info->labelcount++;
      mallocf(line,"jump %d", l_end); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_either); addlabel(arg_info,label);
      mallocf(line,"floadc_0"); addline(arg_info,line,NULL);
      mallocf(label,"%d", l_end); addlabel(arg_info,label);
      break;
    case VT_bool:
      // nothing to do
      break;
    default:
      DBUG_ASSERT(0, "illegal cast detected!");
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
  switch(getType(MONOP_EXPR( arg_node))){
  case VT_int:
    switch(MONOP_OP(arg_node)){
    case MO_neg:
      mallocf(line,"ineg");\
      addline(arg_info,line,NULL);
      break;
    default:
      DBUG_ASSERT(0, "illegal operator detected!");
    }
    break;

  case VT_float:
    switch(MONOP_OP(arg_node)){
    case MO_neg:
      mallocf(line,"fneg");\
      addline(arg_info,line,NULL);
      break;
    default:
      DBUG_ASSERT(0, "illegal operator detected!");
    }

    break;

  case VT_bool:
    switch(MONOP_OP(arg_node)){
    case MO_not:
      mallocf(line,"bnot");\
      addline(arg_info,line,NULL);
      break;
    default:
      DBUG_ASSERT(0, "illegal operator detected!");
    }

    break;

  default: break;

  }
  DBUG_RETURN( arg_node);
}


extern node* CODEGENint(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENint");
  char * line;
  char * comment;
  int val = INT_VALUE(arg_node);
  if (val == 0)
    {
      mallocf(line,"iloadc_0");
      addline(arg_info,line,NULL);
    }
  else if (val == 1)
    {
      mallocf(line,"iloadc_1");
      addline(arg_info,line,NULL);
    }
  else if (val == -1)
    {
      mallocf(line,"iloadc_m1");
      addline(arg_info,line,NULL);
    }
  else
    {
      int cnum = addint(arg_info,INT_VALUE(arg_node));
      mallocf(line,"iloadc %i",cnum);
      mallocf(comment,"%d",INT_VALUE( arg_node));
      addline(arg_info,line,comment);
    }
  DBUG_RETURN( arg_node);
}
extern node* CODEGENfloat(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENfloat");
  char * line;
  char * comment;
  float val = FLOAT_VALUE(arg_node);
  if (val == 0.0)
    {
      mallocf(line,"floadc_0");
      addline(arg_info,line,NULL);
    }
  else if (val == 1.0)
    {
      mallocf(line,"floadc_1");
      addline(arg_info,line,NULL);
    }
  else
    {
      int cnum = addfloat(arg_info,FLOAT_VALUE(arg_node));
      mallocf(line,"floadc %i",cnum);
      mallocf(comment,"%f",FLOAT_VALUE( arg_node));
      addline(arg_info,line,comment);
    }
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
