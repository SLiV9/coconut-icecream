#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include "types.h"

extern node* CODEGENfundef(node *arg_node, info *arg_info);
extern node* CODEGENhoare(node *arg_node, info *arg_info);
extern node* CODEGENbinop(node *arg_node, info *arg_info);
extern node* CODEGENmonop(node *arg_node, info *arg_info);
extern node* CODEGENint(node *arg_node, info *arg_info);
extern node* CODEGENfloat(node *arg_node, info *arg_info);
extern node* CODEGENbool(node *arg_node, info *arg_info);
extern node* CODEGENvarcall(node *arg_node, info *arg_info);


extern node *CODEGENdoCodegen( node *syntaxtree);

#endif /* _CODEGEN_H_ */
