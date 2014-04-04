/*
 * INFO structure
 */
struct INFO {
    node* head;
    node* last;
    int dirty;
};

#define INFO_HEAD(n) ((n)->head)
#define INFO_LAST(n) ((n)->last)
#define INFO_DIRTY(n) ((n)->dirty)

static info *MakeInfo()
{
  info *result;
  
  result = MEMmalloc(sizeof(info));

  INFO_HEAD(result) = NULL;
  INFO_LAST(result) = NULL;
  INFO_DIRTY(result) = 0;
  
  return result;
}

static info *FreeInfo( info *info)
{
  info = MEMfree( info);

  return info;
}

static void addInstr(info* arg_info, node* instr)
{
  if (INFO_HEAD( arg_info) == NULL)
  {
    INFO_HEAD( arg_info) = instr;
    INFO_LAST( arg_info) = instr;
  }
  else
  {
    INSTRS_NEXT( INFO_LAST( arg_info)) = instr;
    INFO_LAST( arg_info) = instr;
  }
}

static node* makeAssignInstrs(node* oldlet, node* indx, node* expr)
{
  node* letje = COPYdoCopy( oldlet);
  node* indxs = VARLET_INDX( letje);

  if (indxs != NULL)
  {
    node* ixs = indxs;
    while (EXPRS_NEXT( ixs) != NULL)
    {
      ixs = EXPRS_NEXT( ixs);
    }
    EXPRS_NEXT( ixs) = TBmakeExprs( indx, NULL);
  }
  else
  {
    VARLET_INDX( letje) = TBmakeExprs( indx, NULL);
  }

  node* ass = TBmakeAssign( letje, expr);
  return TBmakeInstrs( ass, NULL);
}