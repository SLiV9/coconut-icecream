#ifndef _DIMREDUCT_H_
#define _DIMREDUCT_H_

#include "types.h"

extern node* DIMREDUCTfuncall(node *arg_node, info *arg_info);
extern node* DIMREDUCTvarcall(node *arg_node, info *arg_info);
extern node* DIMREDUCTvarlet(node *arg_node, info *arg_info);

extern node *DIMREDUCTdoReduct( node *syntaxtree);

#endif /* _DIMREDUCT_H_ */
