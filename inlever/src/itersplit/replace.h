#ifndef _REPLACE_H_
#define _REPLACE_H_

#include "types.h"

extern node* REPLACEvarcall(node *arg_node, info *arg_info);
extern node* REPLACEvarlet(node *arg_node, info *arg_info);

extern node *REPLACEdoReplace( node *syntaxtree);

#endif /* _REPLACE_H_ */
