#include "idpre.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include <string.h>

node* IDPREvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("IDPREvarcall");

	char* oldname = VARCALL_NAME( arg_node);
  VARCALL_NAME( arg_node) = STRcat("_", oldname);
  free(oldname);
  
  VARCALL_INDX( arg_node) = TRAVopt( VARCALL_INDX( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* IDPREvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("IDPREvarlet");

	char* oldname = VARLET_NAME( arg_node);
  VARLET_NAME( arg_node) = STRcat("_", oldname);
  free(oldname);
  
  VARLET_INDX( arg_node) = TRAVopt( VARLET_INDX( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* IDPREvardec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("IDPREvardec");

	char* oldname = VARDEC_NAME( arg_node);
  VARDEC_NAME( arg_node) = STRcat("_", oldname);
  free(oldname);
  
  VARDEC_EXPR( arg_node) = TRAVopt( VARDEC_EXPR( arg_node), arg_info);
  
  VARDEC_DIMDEFS( arg_node) = TRAVopt( VARDEC_DIMDEFS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* IDPREglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("IDPREglobdef");

	char* oldname = GLOBDEF_NAME( arg_node);
  GLOBDEF_NAME( arg_node) = STRcat("_", oldname);
  free(oldname);
  
  GLOBDEF_EXPR( arg_node) = TRAVopt( GLOBDEF_EXPR( arg_node), arg_info);
  
  GLOBDEF_DIMDEFS( arg_node) = TRAVopt( GLOBDEF_DIMDEFS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* IDPREglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("IDPREglobdec");

	char* oldname = GLOBDEC_NAME( arg_node);
  GLOBDEC_NAME( arg_node) = STRcat("_", oldname);
  free(oldname);
  
  GLOBDEC_DIMDECS( arg_node) = TRAVopt( GLOBDEC_DIMDECS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* IDPREparam(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("IDPREparam");

	char* oldname = PARAM_NAME( arg_node);
  PARAM_NAME( arg_node) = STRcat("_", oldname);
  free(oldname);
  
  PARAM_DIMDECS( arg_node) = TRAVopt( PARAM_DIMDECS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* IDPREiter(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("IDPREiter");

	char* oldname = ITER_NAME( arg_node);
  ITER_NAME( arg_node) = STRcat("_", oldname);
  free(oldname);

  DBUG_RETURN (arg_node);
}

node* IDPREdim(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("IDPREdim");

	char* oldname = DIM_NAME( arg_node);
  DIM_NAME( arg_node) = STRcat("_", oldname);
  free(oldname);

  DBUG_RETURN (arg_node);
}

node* IDPREheader(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("IDPREheader");

	char* oldname = HEADER_NAME( arg_node);
  HEADER_NAME( arg_node) = STRcat("_", oldname);
  free(oldname);
  
  HEADER_PARAMS( arg_node) = TRAVopt( HEADER_PARAMS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* IDPREfuncall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("IDPREfuncall");

	char* oldname = FUNCALL_NAME( arg_node);
  FUNCALL_NAME( arg_node) = STRcat("_", oldname);
  free(oldname);
  
  FUNCALL_ARGS( arg_node) = TRAVopt( FUNCALL_ARGS( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node *IDPREdoIdPrefix(node *syntaxtree)
{
	DBUG_ENTER("IDPREdoIdPrefix");

  TRAVpush( TR_idpre);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
