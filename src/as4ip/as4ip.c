#include "as4ip.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include <string.h>

node* AS4IPvar(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("AS4IPvar");

	char* oldname = VAR_NAME( arg_node);
  VAR_NAME( arg_node) = STRcat("__", oldname);
  free(oldname);

  DBUG_RETURN (arg_node);
}

node* AS4IPvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("AS4IPvarlet");

	char* oldname = VARLET_NAME( arg_node);
  VARLET_NAME( arg_node) = STRcat("__", oldname);
  free(oldname);

  DBUG_RETURN (arg_node);
}

node *AS4IPdoIdPrefix(node *syntaxtree)
{
	DBUG_ENTER("AS4IPdoIdPrefix");

  TRAVpush( TR_as4ip);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
