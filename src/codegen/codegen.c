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

typedef struct entry{
  char * line;
  struct entry * next;
}entry;


struct INFO {
  asmline * first;
  asmline * last;
  constant * consts;
  entry * globals;
  entry * imports;
  entry * exports;
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

void printlines(FILE* out, info * lines, bool verbose){
  asmline* current = lines->first;
  while(current!=NULL){
    if (!current->islabel){
      fprintf(out,"    %-40s",current->line);
      if (verbose && current->comment != NULL) {
        fprintf(out,";  %s",current->comment);
      }
      fprintf(out,"\n");
    }
    else {
      fprintf(out,"%s:\n",current->line);
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


void printconst(FILE* out, info * inf)
{
  constant * now = inf->consts;
  while(now!=NULL){
    if(now->isfloat){
      fprintf(out,".const float %f\n",now->val.f);
    }else{

      fprintf(out,".const int %i\n",now->val.i);
    }
    now = now->next;
  }
}

int addentry(info* inf, char* ln, entry** reg)
{
  entry* nw = malloc(sizeof(entry));
  nw->line = ln;
  nw->next = NULL;

  if (*reg == NULL)
  {
    *reg = nw;
    return 1;
  }

  int i = 2;

  entry* e = *reg;
  while (e->next != NULL)
  {
    e = e->next;
    i++;
  }
  e->next = nw;

  return i;
}

void addglobal(node* arg_node, info* inf, int pos)
{
  char* ln;
  vtype t = GLOBDEF_TYPE( arg_node);
  mallocf(ln,".global %s\n",vtype_name[t]);
  int i = addentry(inf, ln, &(inf->globals));
  DBUG_ASSERT(i == pos, "globals out of order!");
}

char* beheader(node* header)
{
  char* name = HEADER_NAME(header);
  vtype t = HEADER_TYPE(header);
  char* types = STRcpy( vtype_name[t]);
  node* params = HEADER_PARAMS(header);
  while (params != NULL)
  {
    t = PARAM_TYPE( PARAMS_PARAM( params));
    char* newtypes = STRcatn(3, types, " ", vtype_name[t]);
    MEMfree(types);
    types = newtypes;
    params = PARAMS_NEXT( params);
  }
  char* ln;
  mallocf(ln,"\"%s\" %s", name, types);
  MEMfree(types);
  return ln;
}

void addimport(node* arg_node, info* inf, int pos)
{
  char* ln;
  char* behead = beheader(FUNDEC_HEAD(arg_node));
  mallocf(ln,".import %s\n", behead);
  free(behead);
  int i = addentry(inf, ln, &(inf->imports));
  DBUG_ASSERT(i == pos, "imports out of order!");
}

void addexport(node* arg_node, info* inf)
{
  char* ln;
  char* behead = beheader(FUNDEF_HEAD(arg_node));
  mallocf(ln,".export %s %s\n", behead, HEADER_NAME(FUNDEF_HEAD(arg_node)));
  free(behead);
  addentry(inf, ln, &(inf->exports));
}


extern node* CODEGENif(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENif");

  int l_else,l_end;
  char * line;
  char * label;

  l_else = arg_info->labelcount++;


  IF_COND( arg_node) = TRAVdo( IF_COND( arg_node), arg_info);
  mallocf(line,"branch_f %d", l_else); addline(arg_info,line,NULL);
  IF_THEN( arg_node) = TRAVdo( IF_THEN( arg_node), arg_info);

  if(IF_ELSE( arg_node)!=NULL){
    l_end = arg_info->labelcount++;
    mallocf(line,"jump %d", l_end); addline(arg_info,line,NULL);
    mallocf(label,"%d", l_else); addlabel(arg_info,label);

    IF_ELSE( arg_node) = TRAVdo( IF_ELSE( arg_node), arg_info);
    mallocf(label,"%d", l_end); addlabel(arg_info,label);
  }else{
    mallocf(label,"%d", l_else); addlabel(arg_info,label);
  }
  DBUG_RETURN( arg_node);
}


extern node* CODEGENwhile(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENif");

  int l_begin,l_end;
  char * line;
  char * label;


  l_begin = arg_info->labelcount++;
  l_end = arg_info->labelcount++;

  mallocf(label,"%d", l_begin); addlabel(arg_info,label);

  WHILE_COND( arg_node) = TRAVdo( WHILE_COND( arg_node), arg_info);
  mallocf(line,"branch_f %d", l_end); addline(arg_info,line,NULL);

  WHILE_DO( arg_node) = TRAVdo( WHILE_DO( arg_node), arg_info);
  mallocf(line,"jump %d", l_begin); addline(arg_info,line,NULL);
  mallocf(label,"%d", l_end); addlabel(arg_info,label);

  DBUG_RETURN( arg_node);
}

extern node* CODEGENglobdef(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENglobdef");

  addglobal( arg_node, arg_info, GLOBDEF_GLOBALPOS( arg_node));

  DBUG_RETURN( arg_node);
}
extern node* CODEGENfundec(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENfundec");

  addimport( arg_node, arg_info, FUNDEC_IMPORTPOS( arg_node));

  DBUG_RETURN( arg_node);
}
extern node* CODEGENfundef(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENfundef");

  if (FUNDEF_EXPORT( arg_node))
  {
    addexport( arg_node, arg_info);
  }

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

static void var( info* arg_info, char c, const char* op, \
    const char* name, node* dec, int diff)
{
  int pos;
  char * line;
  char * comment;
  mallocf(comment,"%s",name);
  if(NODE_TYPE(dec) == N_vardec){
    pos = VARDEC_SCOPEPOS(dec)-1;
    if(diff == NDSD_LOCAL()){
      mallocf(line,"%c%s %i",c,op,pos);
    }else{
      mallocf(line,"%c%sn %i %i",c,op,diff,pos);
    }
    addline(arg_info,line,comment);
  }else if(NODE_TYPE(dec) == N_param){
    pos = PARAM_SCOPEPOS(dec)-1;
    if(diff == NDSD_LOCAL()){
      mallocf(line,"%c%s %i",c,op,pos);
    }else{
      mallocf(line,"%c%sn %i %i",c,op,diff,pos);
    }
    addline(arg_info,line,comment);
  }else{
    pos = GLOBDEF_GLOBALPOS(dec)-1;
    mallocf(line,"%c%sg %i",c,op,pos);
    addline(arg_info,line,comment);
  }
}

extern node* CODEGENvarcall(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENvarCall");

  VARCALL_INDX( arg_node) = TRAVopt( VARCALL_INDX( arg_node), arg_info);

  bool isArray = (getDepth( VARCALL_DEC( arg_node)) > 0);
  char c, ct;
  if(VARCALL_TYPE(arg_node)==VT_int)
    ct = 'i';
  else if(VARCALL_TYPE(arg_node)==VT_float)
    ct = 'f';
  else
    ct = 'b';

  if (isArray)
    c = 'a';
  else
    c = ct;

  var( arg_info, c, "load", VARCALL_NAME( arg_node), \
      VARCALL_DEC( arg_node), VARCALL_SCOPEDIFF( arg_node));

  if (isArray)
  {
    char* line;
    mallocf(line,"%cloada",ct);
    addline(arg_info,line,NULL);
  }

  DBUG_RETURN( arg_node);
}


extern node* CODEGENvarlet(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENvarLet");

  VARLET_INDX( arg_node) = TRAVopt( VARLET_INDX( arg_node), arg_info);

  bool isArray = (getDepth( VARLET_DEC( arg_node)) > 0);
  char c, ct;
  if(VARLET_TYPE(arg_node)==VT_int)
    ct = 'i';
  else if(VARLET_TYPE(arg_node)==VT_float)
    ct = 'f';
  else
    ct = 'b';

  if (isArray)
    c = 'a';
  else
    c = ct;

  char* op = "store";
  if (isArray)
    op = "load";

  var( arg_info, c, op, VARLET_NAME( arg_node), \
      VARLET_DEC( arg_node), VARLET_SCOPEDIFF( arg_node));

  if (isArray)
  {
    char* line;
    mallocf(line,"%cstorea",ct);
    addline(arg_info,line,NULL);
  }

  DBUG_RETURN( arg_node);
}

extern node* CODEGENfuncall(node *arg_node, info *arg_info){
  DBUG_ENTER("CODEGENfuncall");
  char * line;
  char * comment;

  bool isAlloc = (STReq("__alloc", FUNCALL_NAME( arg_node)));

  if (!isAlloc)
  {
    int diff = FUNCALL_SCOPEDIFF( arg_node);
    switch (diff)
    {
      case 1:
        mallocf(line,"isr");
        break;
      case NDSD_GLOBAL():
        mallocf(line,"isrg");
        break;
      case NDSD_LOCAL():
        mallocf(line,"isrl");
        break;
      default:
        DBUG_ASSERT(diff > 1, "illegal funcall scopediff!");
        mallocf(line,"isrn %d",(diff - 1));
    }
    mallocf(comment,"%s(",FUNCALL_NAME(arg_node));
    addline(arg_info,line,comment);
  }

  FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);

  if (isAlloc)
    {
      char c;
      switch (FUNCALL_TYPE( arg_node))
      {
        case VT_int:    c = 'i'; break;
        case VT_float:  c = 'f'; break;
        case VT_bool:   c = 'b'; break;
        default:
          DBUG_ASSERT(0, "illegal alloc type!");
      }
      mallocf(line,"%cnewa 1", c);
      mallocf(comment,"alloc");
      addline(arg_info,line,comment);
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
  mallocf(comment,")");
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


static void printcode(FILE* out, info* code, bool verbose)
{
  printlines(out,code,verbose);
  fprintf(out,"\n");
  fprintf(out,"\n");
  entry* e;
  e = code->globals; while (e != NULL) 
      { fprintf(out,"%s", e->line); e = e->next; }
  e = code->imports; while (e != NULL) 
      { fprintf(out,"%s", e->line); e = e->next; }
  e = code->exports; while (e != NULL) 
      { fprintf(out,"%s", e->line); e = e->next; }
  printconst(out,code);
}

node *CODEGENdoCodegen(node *syntaxtree)
{

  DBUG_ENTER("CODEGENdoCodegen");

  TRAVpush( TR_codegen);
  info * code = malloc(sizeof(info));
  code->first = code->last = NULL;
  code->consts = NULL;
  code->globals = NULL;
  code->imports = NULL;
  code->exports = NULL;
  code->lines = 0;
  code->labelcount = 0;
  syntaxtree = TRAVdo( syntaxtree, code);
  TRAVpop();

  printf("+-+-+-+-+-+-+-+-+-+-+-+-\n");
  printcode(stdout, code, TRUE);
  printf("+-+-+-+-+-+-+-+-+-+-+-+-\n");

  if (global.outfile == NULL)
  {
    global.outfile = "out.s";
  }
  
  FILE* out = fopen(global.outfile, "w+");
  if (out == NULL)
  {
    CTIabort("Cannot open file '%s'.", global.outfile);
  }

  printcode(out, code, FALSE);

  asmline *cur, *prv;
  cur = code->first;
  while (cur != NULL)
  {
    prv = cur;
    cur = cur->next;
    free(prv->line);
    free(prv->comment);
    free(prv);
  }
  constant *curc, *prvc;
  curc = code->consts;
  while (curc != NULL)
  {
    prvc = curc;
    curc = curc->next;
    free(prvc);
  }
  entry *cure, *prve;
  cure = code->globals;
  while (cure != NULL)
  {
    prve = cure;
    cure = cure->next;
    free(prve->line);
    free(prve);
  }
  cure = code->imports;
  while (cure != NULL)
  {
    prve = cure;
    cure = cure->next;
    free(prve->line);
    free(prve);
  }
  cure = code->exports;
  while (cure != NULL)
  {
    prve = cure;
    cure = cure->next;
    free(prve->line);
    free(prve);
  }

  free(code);

  DBUG_RETURN( syntaxtree);
}
