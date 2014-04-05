#ifndef _GETSET_H_
#define _GETSET_H_

#include "types.h"

extern node* GETSETdeclars(node *arg_node, info *arg_info);
extern node* GETSETglobdef(node *arg_node, info *arg_info);
extern node* GETSETglobdec(node *arg_node, info *arg_info);

extern node *GETSETdoReplace( node *syntaxtree);

#endif /* _GETSET_H_ */
