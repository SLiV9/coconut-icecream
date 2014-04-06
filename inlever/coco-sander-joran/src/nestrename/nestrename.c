#include <stdio.h>
#include <string.h>

#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include "globals.h"
#include "ctinfo.h"
#include "memory.h"
#include "print.h"

#include "nestrename.h"

/*
 * INFO structure
 */
#define BUFFSIZE (65535)

struct INFO {
		char buffer[BUFFSIZE];
		char* end;
};

#define INFO_END(n) ((n)->end)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  result->buffer[0] = '\0';
  result->end = result->buffer;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

static void append(info* arg_info, const char* name)
{
	if (strlen(arg_info->buffer) + strlen(name) >= BUFFSIZE)
	{
		CTIabort("function names too long or nesting too deep");
		return;
	}

	if (strlen(arg_info->buffer) > 0)
	{
		arg_info->end[0] = '.';
		arg_info->end += 1;
	}
	strcpy(arg_info->end, name);
	arg_info->end += strlen(name);
}

static void reset(info* arg_info, char* p)
{
	arg_info->end = p;
	arg_info->end[0] = '\0';
}

static char* obtain(info* arg_info)
{
	return STRcpy(arg_info->buffer);
}

node* NESTRENAMEheader(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("NESTRENAMEheader");

	append(arg_info, HEADER_NAME( arg_node));

	MEMfree( HEADER_NAME( arg_node));
	HEADER_NAME( arg_node) = obtain(arg_info);

	// no need to visit params

	DBUG_RETURN (arg_node);
}

node* NESTRENAMEfundec(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("NESTRENAMEfundec");

	char* old = INFO_END( arg_info);
	FUNDEC_HEAD( arg_node) = TRAVdo( FUNDEC_HEAD( arg_node), arg_info);
	reset(arg_info, old);

	DBUG_RETURN (arg_node);
}

node* NESTRENAMEfundef(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("NESTRENAMEfundef");

	char* old = INFO_END( arg_info);
	FUNDEF_HEAD( arg_node) = TRAVdo( FUNDEF_HEAD( arg_node), arg_info);
	FUNDEF_BODY( arg_node) = TRAVdo( FUNDEF_BODY( arg_node), arg_info);
	reset(arg_info, old);

	DBUG_RETURN (arg_node);
}

node* NESTRENAMEfuncall(node *arg_node, info *arg_info)
{
	DBUG_ENTER ("NESTRENAMEfuncall");

	// use declaration's name instead

	DBUG_RETURN (arg_node);
}

node *NESTRENAMEdoRename(node *syntaxtree)
{
	DBUG_ENTER("NESTRENAMEdoRename");

  info* arg_info = MakeInfo();

  TRAVpush( TR_nestrename);

  syntaxtree = TRAVdo( syntaxtree, arg_info);

  TRAVpop();

  arg_info = FreeInfo( arg_info);

  DBUG_RETURN( syntaxtree);
}
