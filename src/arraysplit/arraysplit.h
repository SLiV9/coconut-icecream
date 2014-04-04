#ifndef _ARRAYSPLIT_H_
#define _ARRAYSPLIT_H_

#include "types.h"

extern node* ARRAYSPLITdeclars(node *arg_node, info *arg_info);
extern node* ARRAYSPLITglobdec(node *arg_node, info *arg_info);
extern node* ARRAYSPLITglobdef(node *arg_node, info *arg_info);

extern node* ARRAYSPLITvardecs(node *arg_node, info *arg_info);
extern node* ARRAYSPLITvardec(node *arg_node, info *arg_info);

extern node* ARRAYSPLITparams(node *arg_node, info *arg_info);
extern node* ARRAYSPLITparam(node *arg_node, info *arg_info);

extern node *ARRAYSPLITdoSplit( node *syntaxtree);

#endif /* _ARRAYSPLIT_H_ */
