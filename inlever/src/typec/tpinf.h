#ifndef _TPINF_H_
#define _TPINF_H_

#include "types.h"

extern node* TPINFvarlet(node *arg_node, info *arg_info);
extern node* TPINFvarcall(node *arg_node, info *arg_info);
extern node* TPINFfuncall(node *arg_node, info *arg_info);

extern node* TPINFarraylit(node *arg_node, info *arg_info);

extern node* TPINFvardec(node *arg_node, info *arg_info);
extern node* TPINFparam(node *arg_node, info *arg_info);

extern node* TPINFcast(node *arg_node, info *arg_info);
extern node* TPINFmonop(node *arg_node, info *arg_info);
extern node* TPINFbinop(node *arg_node, info *arg_info);

extern node* TPINFdoInference(node *syntax_tree);

bool checkDereferenced(node* x);

#endif /* _TPINF_H_ */
