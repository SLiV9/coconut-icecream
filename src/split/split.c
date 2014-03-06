#include "as4ip.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include <string.h>

struct INFO{

};

node* SPLITdeclars(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITdeclars");
 DBUG_RETURN (arg_node);
}
node* SPLITbody(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITbody");
 DBUG_RETURN (arg_node);
}
node* SPLITglobaldef(node *arg_node, info *arg_info){
 DBUG_ENTER ("SPLITglobaldef");
 DBUG_RETURN (arg_node);
}
node* SPLITvardec(node *arg_node, info *arg_info){
  DBUG_ENTER ("SPLITvardec");
  if (VARDEC_EXPR(arg_node) != NULL){
    node * assign = TBmakeAssign(TBmakeVarlet(VARDEC_NAME(arg_node),NULL),VARDEC_EXPR(arg_node))
  }
  DBUG_RETURN (arg_node);
}


/*
node* AS4IPvarcall(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("AS4IPvarcall");

	char* oldname = VARCALL_NAME( arg_node);
  VARCALL_NAME( arg_node) = STRcat("__", oldname);
  free(oldname);

  DBUG_RETURN (arg_node);
}
*/
