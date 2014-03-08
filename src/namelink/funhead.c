#include <string.h>

#include "funhead.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "str.h"

/*
 * INFO structure
 */
struct INFO {
  node *stack;
  int depth;
};

#define INFO_STACK(n) ((n)->stack)
#define INFO_DEPTH(n) ((n)->depth)

node* NAMELINKFUNHEADfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKFUNHEADfundef");

	INFO_STACK( arg_info) = TBmakeNamedecs( HEADER_NAME(FUNDEF_HEAD(arg_node)), \
			INFO_DEPTH( arg_info), arg_node, INFO_STACK( arg_info));

  DBUG_RETURN (arg_node);
}

node* NAMELINKFUNHEADfundec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKFUNHEADfundec");

	INFO_STACK( arg_info) = TBmakeNamedecs( HEADER_NAME(FUNDEC_HEAD(arg_node)), \
			INFO_DEPTH( arg_info), arg_node, INFO_STACK( arg_info));

  DBUG_RETURN (arg_node);
}
