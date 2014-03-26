#ifndef _VARCOUNT_H_
#define _VARCOUNT_H_

#include "types.h"

extern node* VARCOUNTfundef(node *arg_node, info *arg_info);
extern node* VARCOUNTfundec(node *arg_node, info *arg_info);
extern node* VARCOUNTbody(node *arg_node, info *arg_info);

extern node* VARCOUNTparam(node *arg_node, info *arg_info);
extern node* VARCOUNTvardec(node *arg_node, info *arg_info);
extern node* VARCOUNTiter(node *arg_node, info *arg_info);
extern node* VARCOUNTdim(node *arg_node, info *arg_info);

extern node* VARCOUNTglobdef(node *arg_node, info *arg_info);
extern node* VARCOUNTglobdec(node *arg_node, info *arg_info);

extern node *VARCOUNTdoCount( node *syntaxtree);

#endif /* _VARCOUNT_H_ */
