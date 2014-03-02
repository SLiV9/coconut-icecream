
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
  }
  
  DBUG_RETURN (arg_node);
}

node *
PRTparams (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTparams");

  PARAMS_PARAM( arg_node) = TRAVdo( PARAMS_PARAM( arg_node), arg_info);
  
  PARAMS_NEXT( arg_node) = TRAVopt( PARAMS_NEXT( arg_node), arg_info);
  
  DBUG_RETURN (arg_node);
}

node *
PRTvardecs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvardecs");

  VARDECS_VARDEC( arg_node) = TRAVdo( VARDECS_VARDEC( arg_node), arg_info);
  
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
PRTdimdefs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTdimdefs");

  DIMDEFS_EXPR( arg_node) = TRAVdo( DIMDEFS_EXPR( arg_node), arg_info);
  
  if (DIMDEFS_NEXT( arg_node) != NULL)
  {
  	printf(", ");
  	DIMDEFS_NEXT( arg_node) = TRAVopt( DIMDEFS_NEXT( arg_node), arg_info);
  }
  
  DBUG_RETURN (arg_node);
}

node *
PRTdimdecs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTdimdecs");

  DIMDECS_DEC( arg_node) = TRAVdo( DIMDECS_DEC( arg_node), arg_info);
  
  if (DIMDECS_NEXT( arg_node) != NULL)
  {
  	printf(" , ");
  	DIMDECS_NEXT( arg_node) = TRAVopt( DIMDECS_NEXT( arg_node), arg_info);
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
  char *tmp;

  DBUG_ENTER ("PRTmonop");

  switch (MONOP_OP( arg_node)) {
    case MO_neg:
      tmp = "-";
      break;
    case MO_not:
      tmp = "!";
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
  char *tmp;

  DBUG_ENTER ("PRTbinop");

  printf( "(");

  BINOP_LEFT( arg_node) = TRAVdo( BINOP_LEFT( arg_node), arg_info);

  switch (BINOP_OP( arg_node)) {
    case BO_add:
      tmp = "+";
      break;
    case BO_sub:
      tmp = "-";
      break;
    case BO_mul:
      tmp = "*";
      break;
    case BO_div:
      tmp = "/";
      break;
    case BO_mod:
      tmp = "%";
      break;
    case BO_lt:
      tmp = "<";
      break;
    case BO_le:
      tmp = "<=";
      break;
    case BO_gt:
      tmp = ">";
      break;
    case BO_ge:
      tmp = ">=";
      break;
    case BO_eq:
      tmp = "==";
      break;
    case BO_ne:
      tmp = "!=";
      break;
    case BO_or:
      tmp = "||";
      break;
    case BO_and:
      tmp = "&&";
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
  char *tmp;

  DBUG_ENTER ("PRTcast");

  printf( "(");

  switch (CAST_TYPE( arg_node)) {
  	case VT_void:
  		tmp = "void";
  		break;
    case VT_int:
    	tmp = "int";
    	break;
    case VT_float:
    	tmp = "float";
    	break;
    case VT_bool:
    	tmp = "bool";
    	break;
    case VT_unknown:
      DBUG_ASSERT( 0, "unknown vtype detected!");
  }

  printf( "%s", tmp);
  
  printf( ") ");

  CAST_EXPR( arg_node) = TRAVdo( CAST_EXPR( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node *
PRTfuncall (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTfuncall");

  printf( "%s", FUNCALL_NAME( arg_node));
  
  printf("(");
 	
 	FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);
 	
  printf(")");

  DBUG_RETURN (arg_node);
}

node *
PRTvarcall (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvarcall");

  printf( "%s", VARCALL_NAME( arg_node));

  DBUG_RETURN (arg_node);
}

node *
PRTvarlet (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvarlet");

  printf( "%s", VARLET_NAME( arg_node));

  DBUG_RETURN (arg_node);
}

node *
PRTvarhead (node * arg_node, info * arg_info)
{
	char *tmp;
	
  DBUG_ENTER ("PRTvarhead");

  switch (CAST_TYPE( arg_node)) {
  	case VT_void:
  		tmp = "void";
  		break;
    case VT_int:
    	tmp = "int";
    	break;
    case VT_float:
    	tmp = "float";
    	break;
    case VT_bool:
    	tmp = "bool";
    	break;
    case VT_unknown:
      DBUG_ASSERT( 0, "unknown vtype detected!");
  }

  printf( "%s ", tmp);

  printf( "%s", VARLET_NAME( arg_node));

  DBUG_RETURN (arg_node);
}

node *
PRTfunhead (node * arg_node, info * arg_info)
{
	char *tmp;
	
  DBUG_ENTER ("PRTfunhead");

  switch (CAST_TYPE( arg_node)) {
  	case VT_void:
  		tmp = "void";
  		break;
    case VT_int:
    	tmp = "int";
    	break;
    case VT_float:
    	tmp = "float";
    	break;
    case VT_bool:
    	tmp = "bool";
    	break;
    case VT_unknown:
      DBUG_ASSERT( 0, "unknown vtype detected!");
  }

  printf( "%s ", tmp);

  printf( "%s", VARLET_NAME( arg_node));
  
  printf("(");
  
  FUNHEAD_PARAMS( arg_node) = TRAVdo( FUNHEAD_PARAMS( arg_node), arg_info);
  
  printf(")");

  DBUG_RETURN (arg_node);
}

node *
PRTvardec (node * arg_node, info * arg_info)
{	
  DBUG_ENTER ("PRTvardec");
  
  printIndent( arg_info);

  VARDEC_DEC( arg_node) = TRAVdo( VARDEC_DEC( arg_node), arg_info);

  if (VARDEC_DIMDEFS( arg_node) != NULL)
  {
  	printf("[");
  	VARDEC_DIMDEFS( arg_node) = TRAVdo( VARDEC_DIMDEFS( arg_node), arg_info);
  	printf("]");
  }
  
  if (VARDEC_EXPR( arg_node) != NULL)
  {
  	printf(" = ");
  	VARDEC_EXPR( arg_node) = TRAVdo( VARDEC_EXPR( arg_node), arg_info);
  }
  
  printf(";\n");

  DBUG_RETURN (arg_node);
}

node *
PRTglobdef (node * arg_node, info * arg_info)
{	
  DBUG_ENTER ("PRTglobdef");

  GLOBDEF_DEC( arg_node) = TRAVdo( GLOBDEF_DEC( arg_node), arg_info);

  if (GLOBDEF_DIMDEFS( arg_node) != NULL)
  {
  	printf("[ ");
  	GLOBDEF_DIMDEFS( arg_node) = TRAVdo( GLOBDEF_DIMDEFS( arg_node), arg_info);
  	printf(" ]");
  }
  
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

  GLOBDEC_DEC( arg_node) = TRAVdo( GLOBDEC_DEC( arg_node), arg_info);

  if (GLOBDEC_DIMDECS( arg_node) != NULL)
  {
  	printf("[");
  	GLOBDEC_DIMDECS( arg_node) = TRAVdo( GLOBDEC_DIMDECS( arg_node), arg_info);
  	printf("]");
  }
  
  printf(";\n");

  DBUG_RETURN (arg_node);
}

node *
PRTparam (node * arg_node, info * arg_info)
{
	DBUG_ENTER ("PRTparam");

  PARAM_DEC( arg_node) = TRAVdo( PARAM_DEC( arg_node), arg_info);

  if (PARAM_DIMDECS( arg_node) != NULL)
  {
  	printf("[ ");
  	PARAM_DIMDECS( arg_node) = TRAVdo( PARAM_DIMDECS( arg_node), arg_info);
  	printf(" ]");
  }

  DBUG_RETURN (arg_node);
}

node *
PRTfundef (node * arg_node, info * arg_info)
{
	DBUG_ENTER ("PRTfundef");
	
	printIndent( arg_info);
  FUNDEF_DEC( arg_node) = TRAVdo( FUNDEF_DEC( arg_node), arg_info);
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

  FUNDEC_DEC( arg_node) = TRAVdo( FUNDEC_DEC( arg_node), arg_info);

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
	IF_THEN( arg_node) = TRAVdo( IF_THEN( arg_node), arg_info);
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
		WHILE_DO( arg_node) = TRAVdo( WHILE_DO( arg_node), arg_info);
		INFO_INDENT( arg_info) = INFO_INDENT( arg_info) - 1;
		
		printIndent( arg_info);
		printf("}\n");
  }
  
	printIndent( arg_info);
  printf("while (");
  
	IF_COND( arg_node) = TRAVdo( IF_COND( arg_node), arg_info);
	
	printf(")\n");
	
	if (!WHILE_DOFIRST( arg_node))
	{
		printIndent( arg_info);
		printf("{\n");
		
		INFO_INDENT( arg_info) = INFO_INDENT( arg_info) + 1;
		WHILE_DO( arg_node) = TRAVdo( WHILE_DO( arg_node), arg_info);
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
  printf("for (");
  
	FOR_ITER( arg_node) = TRAVdo( FOR_ITER( arg_node), arg_info);
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
	FOR_DO( arg_node) = TRAVdo( FOR_DO( arg_node), arg_info);
	INFO_INDENT( arg_info) = INFO_INDENT( arg_info) - 1;
	
	printIndent( arg_info);
	printf("}\n");

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
