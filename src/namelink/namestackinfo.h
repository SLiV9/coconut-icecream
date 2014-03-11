/*
 * INFO structure
 */
struct INFO {
  node *stack;
  int depth;
};

#define INFO_STACK(n) ((n)->stack)
#define INFO_DEPTH(n) ((n)->depth)

info *MakeInfo();

info *FreeInfo( info *info);

void putUndeclaredError(int line, const char* name);

node* findNameDec(node *arg_node, info *arg_info, const char* name);

void pushNameDec(node *arg_node, info *arg_info, const char* name);
