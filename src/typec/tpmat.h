#ifndef _TPMAT_H_
#define _TPMAT_H_

#include "types.h"

extern node* TPMATassign(node* arg_node, info* arg_info);
extern node* TPMATvarlet(node* arg_node, info* arg_info);
extern node* TPMATfundef(node* arg_node, info* arg_info);
extern node* TPMATfuncall(node* arg_node, info* arg_info);
extern node* TPMATif(node* arg_node, info* arg_info);
extern node* TPMATwhile(node* arg_node, info* arg_info);
extern node* TPMATfor(node* arg_node, info* arg_info);
extern node* TPMATvarlet(node* arg_node, info* arg_info);
extern node* TPMATvarcall(node* arg_node, info* arg_info);

extern node* TPMATdoMatching(node *syntax_tree);

#endif /* _TPMAT_H_ */
