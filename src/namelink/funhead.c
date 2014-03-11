#include <string.h>

#include "funhead.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "ctinfo.h"
#include "memory.h"
#include "str.h"
#include "myglobals.h"

#include "namestackinfo.h"

node* NAMELINKFUNHEADfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKFUNHEADfundef");

	pushNameDec( arg_node, arg_info, HEADER_NAME(FUNDEF_HEAD(arg_node)));

  DBUG_RETURN (arg_node);
}

node* NAMELINKFUNHEADfundec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("NAMELINKFUNHEADfundec");

	pushNameDec( arg_node, arg_info, HEADER_NAME(FUNDEC_HEAD(arg_node)));

  DBUG_RETURN (arg_node);
}
