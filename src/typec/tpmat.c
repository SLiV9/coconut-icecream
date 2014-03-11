#include <string.h>

#include "tpmat.h"
#include "tpinf.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "str.h"
#include "ctinfo.h"
#include "myglobals.h"

#include "gettype.h"

node *TPMATdoMatching(node *syntaxtree)
{	
	DBUG_ENTER("TPMATdoMatching");

  TRAVpush( TR_tpmat);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
