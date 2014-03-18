#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include "types.h"

extern node* CODEGENfundef(node *arg_node, info *arg_info);

extern node *CODEGENdoCodegen( node *syntaxtree);

#endif /* _CODEGEN_H_ */
