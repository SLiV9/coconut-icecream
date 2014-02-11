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

  VAR_NAME( arg_node) = STRcat("__", VAR_NAME( arg_node));

  DBUG_RETURN (arg_node);
}

node* AS4IPvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("AS4IPvarlet");

  VARLET_NAME( arg_node) = STRcat("__", VARLET_NAME( arg_node));

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
