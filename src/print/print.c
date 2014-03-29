
/**
 * @file print.c
 *
 * Functions needed by print traversal.
 *
 */

/**
 * @defgroup print Print Functions.
 *
 * Functions needed by print traversal.
 *
 * @{
 */


#include "print.h"
#include "traverse.h"
#include "tree_basic.h"
#include "dbug.h"
#include "memory.h"
#include "globals.h"
#include "mytypes.h"

/* mytype_name's. */
const char* vtype_name[5] =   { "void", "int", "float", "bool", "unknown" };
const char* monop_name[3] =   { "!", "-", "unknown" };
const char* binop_name[14] =  { "+", "-", "*", "/", "%",
               									"<", "<=", ">", ">=", "==", "!=",
               									"&&", "||", "unknown" };
const char* scope_name[2] =   { "g", "l" };

/*
 * INFO structure
 */
struct INFO {
  bool firsterror;
  int indent;
};

#define INFO_FIRSTERROR(n) ((n)->firsterror)
#define INFO_INDENT(n) ((n)->indent)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_FIRSTERROR(result) = FALSE;
  INFO_INDENT(result) = 0;
  
  return result;
}


static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

void printIndent(info *info)
{
	int i;
	for (i = INFO_INDENT(info); i > 0; i--)
	{
		printf("\t");
	}
}

void printType(vtype t)
{
	const char *tmp;

  switch (t) {
  	case VT_void:
    case VT_int:
    case VT_float:
    case VT_bool:
   		tmp = vtype_name[t];
   		break;
    case VT_unknown:
      DBUG_ASSERT( 0, "unknown vtype detected!");
      break;
    default:
      DBUG_ASSERT( 0, "invalid vtype detected!");
  }

  printf( "%s", tmp);
}

void printLink(node* dec, int scopediff)
{
	if (dec == NULL)
	{
		printf("{null}");
	}
	
	switch (NODE_TYPE(dec))
	{
		case N_vardec:
			printf("{loc %s:%d", VARDEC_NAME(dec), NODE_LINE(dec));
			break;
		case N_globdef:
			printf("{glb %s:%d", GLOBDEF_NAME(dec), NODE_LINE(dec));
			break;
		case N_globdec:
			printf("{glb %s:%d", GLOBDEF_NAME(dec), NODE_LINE(dec));
			break;
		case N_fundef:
			printf("{fun %s:%d", HEADER_NAME(FUNDEF_HEAD(dec)), NODE_LINE(dec));
			break;
		case N_fundec:
			printf("{fun %s:%d", HEADER_NAME(FUNDEC_HEAD(dec)), NODE_LINE(dec));
			break;
		case N_param:
			printf("{prm %s:%d", PARAM_NAME(dec), NODE_LINE(dec));
			break;
		case N_iter:
			printf("{itr %s:%d", ITER_NAME(dec), NODE_LINE(dec));
			break;
		case N_dim:
			printf("{dim %s:%d", DIM_NAME(dec), NODE_LINE(dec));
			break;
		default:
			printf("{unknown");
	}

  if (scopediff >= -2 && scopediff < 0)
  {
    printf(" $%s", scope_name[2 + scopediff]);
    
  }
  if (scopediff > 0)
  {
    printf(" $%d", scopediff);
  }

  switch (NODE_TYPE(dec))
  {
    case N_vardec:
      if (VARDEC_SCOPEPOS(dec))
      {
        printf(" #%d", VARDEC_SCOPEPOS(dec));
      }
      break;
    case N_param:
      if (PARAM_SCOPEPOS(dec))
      {
        printf(" #%d", PARAM_SCOPEPOS(dec));
      }
      break;
    case N_iter:
      if (ITER_SCOPEPOS(dec))
      {
        printf(" #%d", ITER_SCOPEPOS(dec));
      }
      break;
    case N_dim:
      if (DIM_SCOPEPOS(dec))
      {
        printf(" #%d", DIM_SCOPEPOS(dec));
      }
      break;
    default:
      /* nothing */;
  }

  printf("}");
}

node *
PRTdeclars (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTdeclars");

  DECLARS_DECLAR( arg_node) = TRAVdo( DECLARS_DECLAR( arg_node), arg_info);
  
  DECLARS_NEXT( arg_node) = TRAVopt( DECLARS_NEXT( arg_node), arg_info);
  
  DBUG_RETURN (arg_node);
}

node *
PRTbody (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTbody");

  BODY_VARDECS( arg_node) = TRAVopt( BODY_VARDECS( arg_node), arg_info);
  
  BODY_FUNDEFS( arg_node) = TRAVopt( BODY_FUNDEFS( arg_node), arg_info);
  
  BODY_INSTRS( arg_node) = TRAVopt( BODY_INSTRS( arg_node), arg_info);
  
  if (BODY_RETURN( arg_node) != NULL)
  {
  	printIndent( arg_info);
  	printf("return ");
	  BODY_RETURN( arg_node) = TRAVdo( BODY_RETURN( arg_node), arg_info);
	  printf(";\n");
  }
  
  DBUG_RETURN (arg_node);
}

node *
PRTparams (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTparams");

  PARAMS_PARAM( arg_node) = TRAVdo( PARAMS_PARAM( arg_node), arg_info);
  
 	if (PARAMS_NEXT( arg_node) != NULL)
 	{
 		printf(", ");
  	PARAMS_NEXT( arg_node) = TRAVdo( PARAMS_NEXT( arg_node), arg_info);
  }
  
  DBUG_RETURN (arg_node);
}

node *
PRTvardecs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvardecs");

  VARDECS_DEC( arg_node) = TRAVdo( VARDECS_DEC( arg_node), arg_info);
  
  VARDECS_NEXT( arg_node) = TRAVopt( VARDECS_NEXT( arg_node), arg_info);
  
  DBUG_RETURN (arg_node);
}

node *
PRTfundefs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTfundefs");

  FUNDEFS_FUNDEF( arg_node) = TRAVdo( FUNDEFS_FUNDEF( arg_node), arg_info);
  
  FUNDEFS_NEXT( arg_node) = TRAVopt( FUNDEFS_NEXT( arg_node), arg_info);
  
  DBUG_RETURN (arg_node);
}

node *
PRTinstrs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTinstrs");

  INSTRS_INSTR( arg_node) = TRAVdo( INSTRS_INSTR( arg_node), arg_info);
  
  INSTRS_NEXT( arg_node) = TRAVopt( INSTRS_NEXT( arg_node), arg_info);
  
  DBUG_RETURN (arg_node);
}

node *
PRTexprs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTexprs");

  EXPRS_EXPR( arg_node) = TRAVdo( EXPRS_EXPR( arg_node), arg_info);
  
  if (EXPRS_NEXT( arg_node) != NULL)
  {
  	printf(", ");
  	EXPRS_NEXT( arg_node) = TRAVopt( EXPRS_NEXT( arg_node), arg_info);
  }
  
  DBUG_RETURN (arg_node);
}

node *
PRTnamedecs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTnamedecs");

  NAMEDECS_DEC( arg_node) = TRAVdo( NAMEDECS_DEC( arg_node), arg_info);
  
  if (NAMEDECS_NEXT( arg_node) != NULL)
  {
  	printf("; ");
  	NAMEDECS_NEXT( arg_node) = TRAVopt( NAMEDECS_NEXT( arg_node), arg_info);
  }
  
  DBUG_RETURN (arg_node);
}

node *
PRTdimdecs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTdimdecs");

  printf("%s", DIM_NAME( DIMDECS_DIM( arg_node)));
  
  if (DIMDECS_NEXT( arg_node) != NULL)
  {
  	printf(", ");
  	DIMDECS_NEXT( arg_node) = TRAVopt( DIMDECS_NEXT( arg_node), arg_info);
  }
  
  DBUG_RETURN (arg_node);
}

node *
PRTdim (node * arg_node, info * arg_info)
{	
  DBUG_ENTER ("PRTdim");

  printIndent( arg_info);
  printf( "dim");
  printf( " %s;", DIM_NAME( arg_node));

  if (DIM_SCOPEPOS( arg_node))
  {
    printf(" #%d", DIM_SCOPEPOS( arg_node));
  }

  DBUG_RETURN (arg_node);
}

node *
PRTarraylit (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTarraylit");

	printf("[");
	
  ARRAYLIT_EXPRS( arg_node) = TRAVdo( ARRAYLIT_EXPRS( arg_node), arg_info);
  
  printf("]");
  
  DBUG_RETURN (arg_node);
}

node *
PRTassign (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTassign");

 	printIndent( arg_info);

  ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
  
  printf( " = ");
  
  ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);
  
  printf( ";\n");
  
  DBUG_RETURN (arg_node);
}

node *
PRTmonop (node * arg_node, info * arg_info)
{
  const char *tmp;

  DBUG_ENTER ("PRTmonop");

	monop t = MONOP_OP( arg_node);

  switch (t) {
    case MO_neg:
    case MO_not:
      tmp = monop_name[t];
      break;
    case MO_unknown:
      DBUG_ASSERT( 0, "unknown monop detected!");
  }

  printf( "%s", tmp);

  MONOP_EXPR( arg_node) = TRAVdo( MONOP_EXPR( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node *
PRTbinop (node * arg_node, info * arg_info)
{
  const char *tmp;

  DBUG_ENTER ("PRTbinop");

  printf( "(");

  BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);
  
  binop t = BINOP_OP( arg_node);

  switch (t) {
    case BO_add:
    case BO_sub:
    case BO_mul:
    case BO_div:
    case BO_mod:
    case BO_lt:
    case BO_le:
    case BO_gt:
    case BO_ge:
    case BO_eq:
    case BO_ne:
    case BO_or:
    case BO_and:
      tmp = binop_name[t];
      break;
    case BO_unknown:
      DBUG_ASSERT( 0, "unknown binop detected!");
  }

  printf( " %s ", tmp);

  BINOP_RIGHT( arg_node) = TRAVdo( BINOP_RIGHT( arg_node), arg_info);

  printf( ")");

  DBUG_RETURN (arg_node);
}

node *
PRTfloat (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTfloat");

  printf( "%f", FLOAT_VALUE( arg_node));

  DBUG_RETURN (arg_node);
}

node *
PRTint (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTint");

  printf( "%i", INT_VALUE( arg_node));

  DBUG_RETURN (arg_node);
}

node *
PRTbool (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTbool");

  if (BOOL_VALUE( arg_node)) {
    printf( "true");
  }
  else {
    printf( "false");
  }
  
  DBUG_RETURN (arg_node);
}

node *
PRTcast (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTcast");

  printf( "(");

  printType(CAST_TYPE( arg_node));
  
  printf( ") ");

  CAST_EXPR( arg_node) = TRAVdo( CAST_EXPR( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node *
PRTfuncall (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTfuncall");

  if (FUNCALL_DEC( arg_node) != NULL)
	{
		printLink( FUNCALL_DEC( arg_node), FUNCALL_SCOPEDIFF( arg_node));
	}
	else
	{
  	printf( "%s", FUNCALL_NAME( arg_node));
  }
  
  printf("(");
 	
 	FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);
 	
  printf(")");

  DBUG_RETURN (arg_node);
}

node *
PRTvarcall (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvarcall");

  if (VARCALL_DEC( arg_node) != NULL)
	{
		printLink( VARCALL_DEC( arg_node), VARCALL_SCOPEDIFF( arg_node));
	}
	else
	{
  	printf( "%s", VARCALL_NAME( arg_node));
  }
  
  if (VARCALL_INDX( arg_node) != NULL)
  {
  	printf("[");
  	
  	VARCALL_INDX( arg_node) = TRAVdo( VARCALL_INDX( arg_node), arg_info);
  	
  	printf("]");
  }

  DBUG_RETURN (arg_node);
}

node *
PRTvarlet (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvarlet");

	if (VARLET_DEC( arg_node) != NULL)
	{
		printLink( VARLET_DEC( arg_node), VARLET_SCOPEDIFF( arg_node));
	}
	else
	{
  	printf( "%s", VARLET_NAME( arg_node));
  }
  
  if (VARLET_INDX( arg_node) != NULL)
  {
  	printf("[");
  	
  	VARLET_INDX( arg_node) = TRAVdo( VARLET_INDX( arg_node), arg_info);
  	
  	printf("]");
  }

  DBUG_RETURN (arg_node);
}

node *
PRTheader (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTheader");

  printType(HEADER_TYPE( arg_node));

  printf( " %s", HEADER_NAME( arg_node));
  
  printf("(");
  
  HEADER_PARAMS( arg_node) = TRAVopt( HEADER_PARAMS( arg_node), arg_info);
  
  printf(")");

  DBUG_RETURN (arg_node);
}

node *
PRTvardec (node * arg_node, info * arg_info)
{	
  DBUG_ENTER ("PRTvardec");
  
  printIndent( arg_info);

  printType(VARDEC_TYPE( arg_node));

  if (VARDEC_DIMDEFS( arg_node) != NULL)
  {
  	printf("[");
  	VARDEC_DIMDEFS( arg_node) = TRAVdo( VARDEC_DIMDEFS( arg_node), arg_info);
  	printf("]");
  }

  printf( " %s", VARDEC_NAME( arg_node));
  
  if (VARDEC_EXPR( arg_node) != NULL)
  {
  	printf(" = ");
  	VARDEC_EXPR( arg_node) = TRAVdo( VARDEC_EXPR( arg_node), arg_info);
  }
  
  printf(";");

  if (VARDEC_SCOPEPOS( arg_node))
  {
    printf(" #%d", VARDEC_SCOPEPOS( arg_node));
  }
  
  if (VARDEC_ESCAPING( arg_node))
  {
  	printf(" !esc");
  }
  
  printf("\n");

  DBUG_RETURN (arg_node);
}

node *
PRTglobdef (node * arg_node, info * arg_info)
{	
  DBUG_ENTER ("PRTglobdef");
  
	if (GLOBDEF_EXPORT( arg_node))
	{
		printf("export ");
	}
  printType(GLOBDEF_TYPE( arg_node));

  if (GLOBDEF_DIMDEFS( arg_node) != NULL)
  {
  	printf("[");
  	GLOBDEF_DIMDEFS( arg_node) = TRAVdo( GLOBDEF_DIMDEFS( arg_node), arg_info);
  	printf("]");
  }

  printf( " %s", GLOBDEF_NAME( arg_node));
  
  if (GLOBDEF_EXPR( arg_node) != NULL)
  {
  	printf(" = ");
  	GLOBDEF_EXPR( arg_node) = TRAVdo( GLOBDEF_EXPR( arg_node), arg_info);
  }
  
  printf(";\n");

  DBUG_RETURN (arg_node);
}

node *
PRTglobdec (node * arg_node, info * arg_info)
{	
  DBUG_ENTER ("PRTglobdec");

	printf("extern ");
  printType(GLOBDEC_TYPE( arg_node));

  if (GLOBDEC_DIMDECS( arg_node) != NULL)
  {
  	printf("[");
  	GLOBDEC_DIMDECS( arg_node) = TRAVdo( GLOBDEC_DIMDECS( arg_node), arg_info);
  	printf("]");
  }

  printf( " %s", GLOBDEC_NAME( arg_node));
  
  printf(";\n");

  DBUG_RETURN (arg_node);
}

node *
PRTparam (node * arg_node, info * arg_info)
{
	DBUG_ENTER ("PRTparam");

  printType(PARAM_TYPE( arg_node));

  if (PARAM_DIMDECS( arg_node) != NULL)
  {
  	printf("[");
  	PARAM_DIMDECS( arg_node) = TRAVdo( PARAM_DIMDECS( arg_node), arg_info);
  	printf("]");
  }

  printf( " %s", PARAM_NAME( arg_node));

  if (PARAM_SCOPEPOS( arg_node))
  {
    printf(" #%d", PARAM_SCOPEPOS( arg_node));
  }
  
  if (PARAM_ESCAPING( arg_node))
  {
  	printf(" !esc");
  }

  DBUG_RETURN (arg_node);
}

node *
PRTfundef (node * arg_node, info * arg_info)
{
	DBUG_ENTER ("PRTfundef");
	
	printIndent( arg_info);
	if (FUNDEF_EXPORT( arg_node))
	{
		printf("export ");
	}
  FUNDEF_HEAD( arg_node) = TRAVdo( FUNDEF_HEAD( arg_node), arg_info);
	printf("\n");
	
	printIndent( arg_info);
	printf("{\n");
	
	INFO_INDENT( arg_info) = INFO_INDENT( arg_info) + 1;
	FUNDEF_BODY( arg_node) = TRAVdo( FUNDEF_BODY( arg_node), arg_info);
	INFO_INDENT( arg_info) = INFO_INDENT( arg_info) - 1;
	
	printIndent( arg_info);
	printf("}\n");

  DBUG_RETURN (arg_node);
}

node *
PRTfundec (node * arg_node, info * arg_info)
{
	DBUG_ENTER ("PRTfundec");

	printf("extern ");

  FUNDEC_HEAD( arg_node) = TRAVdo( FUNDEC_HEAD( arg_node), arg_info);

  printf(";\n");

  DBUG_RETURN (arg_node);
}

node *
PRTfunstate (node * arg_node, info * arg_info)
{
	DBUG_ENTER ("PRTfunstate");
	
	printIndent( arg_info);

  FUNSTATE_CALL( arg_node) = TRAVdo( FUNSTATE_CALL( arg_node), arg_info);
  
  printf(";\n");

  DBUG_RETURN (arg_node);
}

node *
PRTif (node * arg_node, info * arg_info)
{
	DBUG_ENTER ("PRTif");
	
	printIndent( arg_info);
  printf("if (");
  
	IF_COND( arg_node) = TRAVdo( IF_COND( arg_node), arg_info);
	
	printf(")\n");
	
	printIndent( arg_info);
	printf("{\n");
	
	INFO_INDENT( arg_info) = INFO_INDENT( arg_info) + 1;
	IF_THEN( arg_node) = TRAVopt( IF_THEN( arg_node), arg_info);
	INFO_INDENT( arg_info) = INFO_INDENT( arg_info) - 1;
	
	printIndent( arg_info);
	printf("}\n");
	
	if (IF_ELSE( arg_node) != NULL)
	{
		printIndent( arg_info);
		printf("else\n");
		
		printIndent( arg_info);
		printf("{\n");
		
		INFO_INDENT( arg_info) = INFO_INDENT( arg_info) + 1;
		IF_ELSE( arg_node) = TRAVdo( IF_ELSE( arg_node), arg_info);
		INFO_INDENT( arg_info) = INFO_INDENT( arg_info) - 1;
		
		printIndent( arg_info);
		printf("}\n");
	}

  DBUG_RETURN (arg_node);
}

node *
PRTwhile (node * arg_node, info * arg_info)
{
	DBUG_ENTER ("PRTwhile");
  
  if (WHILE_DOFIRST( arg_node))
  {
  	printIndent( arg_info);
		printf("do\n");
		
		printIndent( arg_info);
		printf("{\n");
		
		INFO_INDENT( arg_info) = INFO_INDENT( arg_info) + 1;
		WHILE_DO( arg_node) = TRAVopt( WHILE_DO( arg_node), arg_info);
		INFO_INDENT( arg_info) = INFO_INDENT( arg_info) - 1;
		
		printIndent( arg_info);
		printf("}\n");
  }
  
	printIndent( arg_info);
  printf("while (");
  
	WHILE_COND( arg_node) = TRAVdo( WHILE_COND( arg_node), arg_info);
	
	printf(")\n");
	
	if (!WHILE_DOFIRST( arg_node))
	{
		printIndent( arg_info);
		printf("{\n");
		
		INFO_INDENT( arg_info) = INFO_INDENT( arg_info) + 1;
		WHILE_DO( arg_node) = TRAVopt( WHILE_DO( arg_node), arg_info);
		INFO_INDENT( arg_info) = INFO_INDENT( arg_info) - 1;
		
		printIndent( arg_info);
		printf("}\n");
	}

  DBUG_RETURN (arg_node);
}

node *
PRTfor (node * arg_node, info * arg_info)
{
	DBUG_ENTER ("PRTfor");
	
	printIndent( arg_info);
  printf("for (int ");
  
	printf("%s", ITER_NAME( FOR_ITER( arg_node)));
	printf(" = ");
	FOR_FROM( arg_node) = TRAVdo( FOR_FROM( arg_node), arg_info);
	printf(", ");
	FOR_TO( arg_node) = TRAVdo( FOR_TO( arg_node), arg_info);
	
	if (FOR_INCR( arg_node) != NULL)
	{
		printf(", ");
		FOR_INCR( arg_node) = TRAVdo( FOR_INCR( arg_node), arg_info);
	}
	
	printf(")\n");
	
	printIndent( arg_info);
	printf("{\n");
	
	INFO_INDENT( arg_info) = INFO_INDENT( arg_info) + 1;
	FOR_DO( arg_node) = TRAVopt( FOR_DO( arg_node), arg_info);
	INFO_INDENT( arg_info) = INFO_INDENT( arg_info) - 1;
	
	printIndent( arg_info);
	printf("}\n");

  DBUG_RETURN (arg_node);
}

node *
PRTiter (node * arg_node, info * arg_info)
{	
  DBUG_ENTER ("PRTiter");

  printIndent( arg_info);
  printf( "itr");
  if (ITER_FOR( arg_node) != NULL)
  {
    printf(":%d", NODE_LINE( ITER_FOR( arg_node)));
  }
  printf( " %s;", ITER_NAME( arg_node));

  if (ITER_SCOPEPOS( arg_node))
  {
    printf(" #%d", ITER_SCOPEPOS( arg_node));
  }

  printf("\n");

  DBUG_RETURN (arg_node);
}

node *
PRTerror (node * arg_node, info * arg_info)
{
  bool first_error;

  DBUG_ENTER ("PRTerror");

  if (NODE_ERROR (arg_node) != NULL) {
    NODE_ERROR (arg_node) = TRAVdo (NODE_ERROR (arg_node), arg_info);
  }

  first_error = INFO_FIRSTERROR( arg_info);

  if( (global.outfile != NULL)
      && (ERROR_ANYPHASE( arg_node) == global.compiler_anyphase)) {

    if ( first_error) {
      printf ( "\n/******* BEGIN TREE CORRUPTION ********\n");
      INFO_FIRSTERROR( arg_info) = FALSE;
    }

    printf ( "%s\n", ERROR_MESSAGE( arg_node));

    if (ERROR_NEXT (arg_node) != NULL) {
      TRAVopt (ERROR_NEXT (arg_node), arg_info);
    }

    if ( first_error) {
      printf ( "********  END TREE CORRUPTION  *******/\n");
      INFO_FIRSTERROR( arg_info) = TRUE;
    }
  }

  DBUG_RETURN (arg_node);
}



/** <!-- ****************************************************************** -->
 * @brief Prints the given syntaxtree
 * 
 * @param syntaxtree a node structure
 * 
 * @return the unchanged nodestructure
 ******************************************************************************/

node 
*PRTdoPrint( node *syntaxtree)
{
  info *info;
  
  DBUG_ENTER("PRTdoPrint");

  DBUG_ASSERT( (syntaxtree!= NULL), "PRTdoPrint called with empty syntaxtree");

  printf( "\n\n------------------------------\n\n");

  info = MakeInfo();
  
  TRAVpush( TR_prt);

  syntaxtree = TRAVdo( syntaxtree, info);

  TRAVpop();

  info = FreeInfo(info);

  printf( "\n\n------------------------------\n\n");

  DBUG_RETURN( syntaxtree);
}

/**
 * @}
 */
