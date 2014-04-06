#include <string.h>

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "ctinfo.h"
#include "memory.h"
#include "str.h"
#include "myglobals.h"
#include "copy.h"

#include "getset.h"

/*
 * INFO structure
 */
struct INFO {
    node* head;
    node* last;
};

#define INFO_HEAD(n) ((n)->head)
#define INFO_LAST(n) ((n)->last)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_HEAD(result) = NULL;
  INFO_LAST(result) = NULL;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

static void add(info* arg_info, node* declars)
{
  if (INFO_HEAD( arg_info) == NULL)
  {
    INFO_HEAD( arg_info) = declars;
    INFO_LAST( arg_info) = declars;
  }
  else
  {
    DECLARS_NEXT( INFO_LAST( arg_info)) = declars;
    INFO_LAST( arg_info) = declars;
  }
}

node* GETSETdeclars(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GETSETdeclars");

  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");
  DECLARS_NEXT( arg_node) = TRAVopt( DECLARS_NEXT( arg_node), arg_info);
  DBUG_ASSERT( INFO_HEAD( arg_info) == NULL, "nonempty info!");

  DECLARS_DECLAR( arg_node) = TRAVdo( DECLARS_DECLAR( arg_node), arg_info);

  node* declars = arg_node;

  if (INFO_HEAD( arg_info) != NULL)
  {
    DECLARS_NEXT( INFO_LAST( arg_info)) = arg_node;
    declars = INFO_HEAD( arg_info);
    INFO_HEAD( arg_info) = NULL;
    INFO_LAST( arg_info) = NULL;
  }

  DBUG_RETURN (declars);
}

node* GETSETglobdef(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GETSETglobdef");

  if (!GLOBDEF_EXPORT( arg_node))
  {
    DBUG_RETURN (arg_node);
  }

  node *getter, *setter;
  node *head, *param, *params;
  node *body, *call, *assign, *instrs;
  node *let, *indxp, *indxc, *indxs;

  vtype t = GLOBDEF_TYPE( arg_node);
  char* name = GLOBDEF_NAME( arg_node);

  // Getter:

  params = NULL;
  indxs = NULL; 
  if (GLOBDEF_DIMDECS( arg_node) > 0)
  {
    indxp = TBmakeParam( STRcpy("i"), VT_int, NULL);
    params = TBmakeParams( indxp, params);

    indxc = TBmakeVarcall( STRcpy( PARAM_NAME( indxp)), NULL);
    VARCALL_DEC( indxc) = indxp;
    VARCALL_SCOPEDIFF( indxc) = NDSD_LOCAL();
    VARCALL_TYPE( indxc) = VT_int;
    VARCALL_DEPTH( indxc) = 0;
    indxs = TBmakeExprs( indxc, NULL);
  }

  call = TBmakeVarcall( STRcpy( name), indxs);
  VARCALL_DEC( call) = arg_node;
  VARCALL_SCOPEDIFF( call) = NDSD_GLOBAL();
  VARCALL_TYPE( call) = t;
  VARCALL_DEPTH( call) = 0;

  head = TBmakeHeader( STRcat( "__get_", name), t, params);
  body = TBmakeBody( NULL, NULL, NULL, call);
  getter = TBmakeFundef( TRUE, head, body);

  // Setter:

  params = NULL;
  indxs = NULL;
  if (GLOBDEF_DIMDECS( arg_node) > 0)
  {
    indxp = TBmakeParam( STRcpy("i"), VT_int, NULL);
    params = TBmakeParams( indxp, params);

    indxc = TBmakeVarcall( STRcpy( PARAM_NAME( indxp)), NULL);
    VARCALL_DEC( indxc) = indxp;
    VARCALL_SCOPEDIFF( indxc) = NDSD_LOCAL();
    VARCALL_TYPE( indxc) = VT_int;
    VARCALL_DEPTH( indxc) = 0;
    indxs = TBmakeExprs( indxc, NULL);
  }

  let = TBmakeVarlet( STRcpy( name), indxs);
  VARLET_DEC( let) = arg_node;
  VARLET_SCOPEDIFF( let) = NDSD_GLOBAL();
  VARLET_TYPE( let) = t;
  VARLET_DEPTH( let) = 0;

  param = TBmakeParam( STRcpy("v"), t, NULL);
  params = TBmakeParams( param, params);

  call = TBmakeVarcall( STRcpy( PARAM_NAME( param)), NULL);
  VARCALL_DEC( call) = param;
  VARCALL_SCOPEDIFF( call) = NDSD_LOCAL();
  VARCALL_TYPE( call) = t;
  VARCALL_DEPTH( call) = 0;

  assign = TBmakeAssign( let, call);
  instrs = TBmakeInstrs( assign, NULL);

  head = TBmakeHeader( STRcat( "__set_", name), VT_void, params);
  body = TBmakeBody( NULL, NULL, instrs, NULL);
  setter = TBmakeFundef( TRUE, head, body);

  GLOBDEF_GETTER( arg_node) = getter;
  GLOBDEF_SETTER( arg_node) = setter;

  GLOBDEF_EXPORT( arg_node) = FALSE;

  add( arg_info, TBmakeDeclars( getter, NULL));
  add( arg_info, TBmakeDeclars( setter, NULL));

  DBUG_RETURN (arg_node);
}

node* GETSETglobdec(node *arg_node, info *arg_info)
{
  DBUG_ENTER ("GETSETglobdec");

  node *getter, *setter;
  node *head, *param, *params;
  node *indxp;

  vtype t = GLOBDEC_TYPE( arg_node);
  char* name = GLOBDEC_NAME( arg_node);

  // Getter:

  params = NULL;
  if (GLOBDEC_DIMDECS( arg_node) > 0)
  {
    indxp = TBmakeParam( STRcpy("i"), VT_int, NULL);
    params = TBmakeParams( indxp, params);
  }

  head = TBmakeHeader( STRcat( "__get_", name), t, params);
  getter = TBmakeFundec( head);

  // Setter:

  params = COPYdoCopy( params);
  param = TBmakeParam( STRcpy("v"), t, NULL);
  params = TBmakeParams( param, params);
  head = TBmakeHeader( STRcat( "__set_", name), VT_void, params);
  setter = TBmakeFundec( head);

  GLOBDEC_GETTER( arg_node) = getter;
  GLOBDEC_SETTER( arg_node) = setter;

  add( arg_info, TBmakeDeclars( getter, NULL));
  add( arg_info, TBmakeDeclars( setter, NULL));

  DBUG_RETURN (arg_node);
}

node *GETSETdoReplace(node *syntaxtree)
{
	DBUG_ENTER("GETSETdoReplace");

  info* arg_info = MakeInfo();

  TRAVpush( TR_getset);
  syntaxtree = TRAVdo( syntaxtree, arg_info);
  TRAVpop();

  arg_info = FreeInfo( arg_info);

  TRAVpush( TR_gsvarcall);
  syntaxtree = TRAVdo( syntaxtree, arg_info);
  TRAVpop();

  TRAVpush( TR_gscleanup);
  syntaxtree = TRAVdo( syntaxtree, arg_info);
  TRAVpop();

  DBUG_RETURN( syntaxtree);
}

