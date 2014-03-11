#ifndef _TPINF_H_
#define _TPINF_H_

#include "types.h"

extern node* TPINFvarlet(node *arg_node, info *arg_info);
extern node* TPINFvarcall(node *arg_node, info *arg_info);
extern node* TPINFfuncall(node *arg_node, info *arg_info);

extern node* TPINFmonop(node *arg_node, info *arg_info);
extern node* TPINFbinop(node *arg_node, info *arg_info);

extern node* TPINFdoInference(node *syntax_tree);

#endif /* _TPINF_H_ */
