#include "as4ip.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"

node *AS4IPdoIdPrefix( node *syntaxtree)
{
	DBUG_ENTER("AS4IPdoIdPrefix");

  TRAVpush( TR_as4ip);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
  
}
