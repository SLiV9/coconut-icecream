#ifndef _ALITUNFOLD_H_
#define _ALITUNFOLD_H_

#include "types.h"

extern node* ALITUNFOLDbody(node *arg_node, info *arg_info);
extern node* ALITUNFOLDinstrs(node *arg_node, info *arg_info);
extern node* ALITUNFOLDassign(node *arg_node, info *arg_info);

extern node *ALITUNFOLDdoUnfold( node *syntaxtree);

#endif /* _ALITUNFOLD_H_ */
