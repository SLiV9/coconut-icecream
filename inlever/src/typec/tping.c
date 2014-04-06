#include <string.h>

#include "tping.h"

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "memory.h"
#include "str.h"
#include "ctinfo.h"
#include "myglobals.h"

#include "gettype.h"
#include "print.h"


node* TPINGfundef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINGfundef");
  
  // do nothing

  DBUG_RETURN (arg_node);
}

node* TPINGfundec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINGfundec");
  
  // do nothing

  DBUG_RETURN (arg_node);
}

node* TPINGglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINGglobdef");
  
  // no need to travdo
  
  GLOBDEF_DEPTH( arg_node) = getDimDecsLen( GLOBDEF_DIMDECS( arg_node));

  DBUG_RETURN (arg_node);
}

node* TPINGglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("TPINGglobdec");
  
  // no need to travdo
  
  GLOBDEC_DEPTH( arg_node) = getDimDecsLen( GLOBDEC_DIMDECS( arg_node));

  DBUG_RETURN (arg_node);
}