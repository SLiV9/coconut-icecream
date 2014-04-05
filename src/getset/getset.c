#include <string.h>

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "ctinfo.h"
#include "memory.h"
#include "str.h"
#include "myglobals.h"

#include "getset.h"

/*
 * INFO structure
 */
struct INFO {
    node* head;
    node* last;
};

#define INFO_HEAD(n) ((n)->head)
#define INFO_LAST(n) ((n)->last)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_HEAD(result) = NULL;
  INFO_LAST(result) = NULL;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

node* GETSETdeclars(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GETSETdeclars");

  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");
  DECLARS_NEXT( arg_node) = TRAVopt( DECLARS_NEXT( arg_node), arg_info);
  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");

  DECLARS_DECLAR( arg_node) = TRAVdo( DECLARS_DECLAR( arg_node), arg_info);

  node* declars = arg_node;

  if (INFO_HEAD( arg_info) != NULL)
  {
    // skip self, since info_head-info_last replaces it
    DECLARS_NEXT( INFO_LAST( arg_info)) = DECLARS_NEXT( arg_node);
    declars = INFO_HEAD( arg_info);
    INFO_HEAD( arg_info) = NULL;
    INFO_LAST( arg_info) = NULL;
  }

  DBUG_RETURN (declars);
}

node* GETSETglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GETSETglobdef");

  DBUG_RETURN (arg_node);
}

node* GETSETglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GETSETglobdec");

  DBUG_RETURN (arg_node);
}

node* GETSETvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GETSETvarlet");

  DBUG_RETURN (arg_node);
}

node* GETSETvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GETSETvarcall");

  DBUG_RETURN (arg_node);
}

node *GETSETdoReplace(node *syntaxtree)
{
	DBUG_ENTER("GETSETdoReplace");

  info* arg_info = MakeInfo();

  TRAVpush( TR_getset);

  syntaxtree = TRAVdo( syntaxtree, arg_info);

  TRAVpop();

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}

