/*
 * INFO structure
 */
struct INFO {
  node *stack;
  int depth;
  int level;
};

#define INFO_STACK(n) ((n)->stack)
#define INFO_DEPTH(n) ((n)->depth)
#define INFO_LEVEL(n) ((n)->level)

info *MakeInfo();

info *FreeInfo( info *info);

void putUndeclaredError(int line, const char* name, bool isFun);

node* findNameDec(node *arg_node, info *arg_info, const char* name);

int getNameDecScopeDiff();

void pushNameDec(node *arg_node, info *arg_info, const char* name);

void popUntilNameDec(info* arg_info, node* end);
