#ifndef _TPING_H_
#define _TPING_H_

#include "types.h"

extern node* TPINGglobdef(node *arg_node, info *arg_info);
extern node* TPINGglobdec(node *arg_node, info *arg_info);
extern node* TPINGfundef(node *arg_node, info *arg_info);
extern node* TPINGfundec(node *arg_node, info *arg_info);

extern node* TPINGdoInference(node *syntax_tree);

bool checkDereferenced(node* x);

#endif /* _TPING_H_ */
