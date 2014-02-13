
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
};

#define INFO_FIRSTERROR(n) ((n)->firsterror)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_FIRSTERROR(result) = FALSE;
  
  return result;
}


static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}


/** <!--******************************************************************-->
 *
 * @fn PRTbody
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Body node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTbody (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTbody");

  BODY_VARDECS( arg_node) = TRAVdo( BODY_VARDECS( arg_node), arg_info);
  
  BODY_INSTRS( arg_node) = TRAVdo( BODY_INSTRS( arg_node), arg_info);
  
  BODY_RETURN( arg_node) = TRAVopt( BODY_RETURN( arg_node), arg_info);
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTvardecs
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Vardecs node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvardecs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvardecs");

  VARDECS_VARDEC( arg_node) = TRAVdo( VARDECS_VARDEC( arg_node), arg_info);
  
  VARDECS_NEXT( arg_node) = TRAVopt( VARDECS_NEXT( arg_node), arg_info);
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTinstr
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Instrs node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTinstrs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTinstrs");

  INSTRS_INSTR( arg_node) = TRAVdo( INSTRS_INSTR( arg_node), arg_info);
  
  INSTRS_NEXT( arg_node) = TRAVopt( INSTRS_NEXT( arg_node), arg_info);
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTargs
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Args node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTargs (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTargs");

  ARGS_EXPR( arg_node) = TRAVdo( ARGS_EXPR( arg_node), arg_info);
  
  ARGS_NEXT( arg_node) = TRAVopt( ARGS_NEXT( arg_node), arg_info);
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTvardec
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node VarDec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvardec (node * arg_node, info * arg_info)
{
	char* tmp;

  DBUG_ENTER ("PRTvardec");
  
  switch (VARDEC_TYPE( arg_node)) {
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

  printf( " %s ", tmp);

  printf( "%s", VARDEC_NAME( arg_node));
  
  if (VARDEC_EXPR( arg_node) != NULL) {
    printf( " = ");
    VARDEC_EXPR( arg_node) = TRAVdo( VARDEC_EXPR( arg_node), arg_info);
  }
  
  printf( ";\n");
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTassign
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Assign node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTassign (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTassign");

  ASSIGN_LET( arg_node) = TRAVdo( ASSIGN_LET( arg_node), arg_info);
  
  printf( " = ");
  
  ASSIGN_EXPR( arg_node) = TRAVdo( ASSIGN_EXPR( arg_node), arg_info);
  
  printf( ";\n");
  
  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTmonop
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node MonOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTmonop (node * arg_node, info * arg_info)
{
  char *tmp;

  DBUG_ENTER ("PRTmonop");

  printf( "( ");

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

  printf( " %s ", tmp);

  MONOP_EXPR( arg_node) = TRAVdo( MONOP_EXPR( arg_node), arg_info);

  printf( ")");

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTbinop
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node BinOp node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTbinop (node * arg_node, info * arg_info)
{
  char *tmp;

  DBUG_ENTER ("PRTbinop");

  printf( "( ");

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


/** <!--******************************************************************-->
 *
 * @fn PRTfloat
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Float node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTfloat (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTfloat");

  printf( "%f", FLOAT_VALUE( arg_node));

  DBUG_RETURN (arg_node);
}



/** <!--******************************************************************-->
 *
 * @fn PRTnum
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Int node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTint (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTint");

  printf( "%i", INT_VALUE( arg_node));

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTboolean
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Boolean node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

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


/** <!--******************************************************************-->
 *
 * @fn PRTcast
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node Cast node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTcast (node * arg_node, info * arg_info)
{
  char *tmp;

  DBUG_ENTER ("PRTcast");

  printf( "( ");

  switch (CAST_TYPE( arg_node)) {
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

  printf( " %s ", tmp);
  
  printf( ")");

  CAST_EXPR( arg_node) = TRAVdo( CAST_EXPR( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTvar
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvar (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvar");

  printf( "%s", VAR_NAME( arg_node));

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTvarlet
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTvarlet (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTvarlet");

  printf( "%s", VARLET_NAME( arg_node));

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTcall
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node call node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

node *
PRTcall (node * arg_node, info * arg_info)
{
  DBUG_ENTER ("PRTcall");

  printf( "%s(", CALL_NAME( arg_node));
  
  CALL_ARGS( arg_node) = TRAVopt( CALL_ARGS( arg_node), arg_info);
  
  printf( ")");

  DBUG_RETURN (arg_node);
}


/** <!--******************************************************************-->
 *
 * @fn PRTerror
 *
 * @brief Prints the node and its sons/attributes
 *
 * @param arg_node letrec node to process
 * @param arg_info pointer to info structure
 *
 * @return processed node
 *
 ***************************************************************************/

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
