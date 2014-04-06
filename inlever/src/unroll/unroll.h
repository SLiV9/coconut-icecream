#ifndef _UNROLL_H_
#define _UNROLL_H_

#include "types.h"


extern node* UNROLLfor(node *arg_node, info *arg_info);
extern node* UNROLLvarcall(node *arg_node, info *arg_info);
extern node* UNROLLinstrs(node *arg_node, info *arg_info);
extern node *UNROLLdoUnroll( node *syntaxtree);

#endif /* _UNROLL_H_ */
