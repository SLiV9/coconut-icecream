#ifndef _NESTRENAME_H_
#define _NESTRENAME_H_

#include "types.h"

extern node* NESTRENAMEfundef(node *arg_node, info *arg_info);
extern node* NESTRENAMEfundec(node *arg_node, info *arg_info);
extern node* NESTRENAMEheader(node *arg_node, info *arg_info);
extern node* NESTRENAMEfuncall(node *arg_node, info *arg_info);

extern node *NESTRENAMEdoRename( node *syntaxtree);

#endif /* _NESTRENAME_H_ */
