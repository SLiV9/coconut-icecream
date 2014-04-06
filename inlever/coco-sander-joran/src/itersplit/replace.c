#include <stdio.h>
#include <string.h>

#include "replace.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "globals.h"
#include "ctinfo.h"
#include "memory.h"
#include "print.h"
#include "copy.h"

node* REPLACEvarlet(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("REPLACEvarlet");

  node* dec = VARLET_DEC( arg_node);
  DBUG_ASSERT( dec != NULL, "no dec!");

  node* ndec;
  switch (NODE_TYPE( dec))
  {
  	case N_iter:
  		ndec = ITER_REPLACER( dec);
  		DBUG_ASSERT( ndec != NULL, "no ndec!");
  		VARLET_DEC( arg_node) = ndec;
  		break;
  	case N_dim:
  		ndec = DIM_REPLACER( dec);
  		DBUG_ASSERT( ndec != NULL, "no ndec!");
  		VARLET_DEC( arg_node) = ndec;
  		break;
  	default:
  		/*nothing*/;
  }

  VARLET_INDX( arg_node) = TRAVopt( VARLET_INDX( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node* REPLACEvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("REPLACEvarcall");

  node* dec = VARCALL_DEC( arg_node);
  DBUG_ASSERT( dec != NULL, "no dec!");

  node* ndec;
  switch (NODE_TYPE( dec))
  {
  	case N_iter:
  		ndec = ITER_REPLACER( dec);
  		DBUG_ASSERT( ndec != NULL, "no ndec!");
  		VARCALL_DEC( arg_node) = ndec;
  		break;
  	case N_dim:
  		ndec = DIM_REPLACER( dec);
  		DBUG_ASSERT( ndec != NULL, "no ndec!");
  		VARCALL_DEC( arg_node) = ndec;
  		break;
  	default:
  		/*nothing*/;
  }

  VARCALL_INDX( arg_node) = TRAVopt( VARCALL_INDX( arg_node), arg_info);

  DBUG_RETURN (arg_node);
}

node *REPLACEdoReplace(node *syntaxtree)
{
	DBUG_ENTER("REPLACEdoSplit");

  TRAVpush( TR_replace);
  syntaxtree = TRAVdo( syntaxtree, NULL);
  TRAVpop();

  DBUG_RETURN( syntaxtree);
}
