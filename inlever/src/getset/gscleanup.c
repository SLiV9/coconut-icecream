#include <string.h>

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "ctinfo.h"
#include "memory.h"
#include "str.h"
#include "myglobals.h"

#include "gscleanup.h"

node* GSCLEANUPdeclars(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GSCLEANUPdeclars");

  DECLARS_NEXT( arg_node) = TRAVopt( DECLARS_NEXT( arg_node), arg_info);

  node* declars = arg_node;

  if (NODE_TYPE( DECLARS_DECLAR( arg_node)) == N_globdec)
  {
    declars = DECLARS_NEXT( arg_node);
  }

  DBUG_RETURN (declars);
}
