#ifndef _TERNARY_H_
#define _TERNARY_H_

#include "types.h"

extern node* TERNARYcast(node *arg_node, info *arg_info);
extern node* TERNARYbinop(node *arg_node, info *arg_info);

extern node *TERNARYdoReplace( node *syntaxtree);

#endif /* _TERNARY_H_ */
