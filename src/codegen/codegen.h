#ifndef _CODEGEN_H_
#define _CODEGEN_H_

#include "types.h"

extern node* CODEGENglobdef(node *arg_node, info *arg_info);
extern node* CODEGENfundec(node *arg_node, info *arg_info);
extern node* CODEGENfundef(node *arg_node, info *arg_info);
extern node* CODEGENbody(node *arg_node, info *arg_info);

extern node* CODEGENfunstate(node *arg_node, info *arg_info);
extern node* CODEGENfuncall(node *arg_node, info *arg_info);

extern node* CODEGENhoare(node *arg_node, info *arg_info);
extern node* CODEGENbinop(node *arg_node, info *arg_info);
extern node* CODEGENmonop(node *arg_node, info *arg_info);
extern node* CODEGENint(node *arg_node, info *arg_info);
extern node* CODEGENfloat(node *arg_node, info *arg_info);
extern node* CODEGENbool(node *arg_node, info *arg_info);
extern node* CODEGENvarcall(node *arg_node, info *arg_info);
extern node* CODEGENcast(node *arg_node, info *arg_info);

extern node* CODEGENvarlet(node *arg_node, info *arg_info);
extern node* CODEGENassign(node *arg_node, info *arg_info);

extern node *CODEGENdoCodegen( node *syntaxtree);


extern node* CODEGENif(node *arg_node, info *arg_info);
extern node* CODEGENfor(node *arg_node, info *arg_info);
extern node* CODEGENwhile(node *arg_node, info *arg_info);

#endif /* _CODEGEN_H_ */
