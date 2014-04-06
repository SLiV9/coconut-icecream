#ifndef _ACONSTUNFOLD_H_
#define _ACONSTUNFOLD_H_

#include "types.h"

extern node* ACONSTUNFOLDinstrs(node *arg_node, info *arg_info);
extern node* ACONSTUNFOLDassign(node *arg_node, info *arg_info);

extern node *ACONSTUNFOLDdoUnfold( node *syntaxtree);

#endif /* _ACONSTUNFOLD_H_ */
