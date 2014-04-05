#ifndef _GSVARCALL_H_
#define _GSVARCALL_H_

#include "types.h"

extern node* GSVARCALLvarlet(node *arg_node, info *arg_info);
extern node* GSVARCALLvarcall(node *arg_node, info *arg_info);

extern node *GSVARCALLdoReplace( node *syntaxtree);

#endif /* _GSVARCALL_H_ */
