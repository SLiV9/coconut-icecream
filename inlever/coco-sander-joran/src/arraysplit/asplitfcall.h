#ifndef _ASPLITFCALL_H_
#define _ASPLITFCALL_H_

#include "types.h"

extern node* ASPLITFCALLexprs(node *arg_node, info *arg_info);
extern node* ASPLITFCALLvarcall(node *arg_node, info *arg_info);

extern node *ASPLITFCALLdoSplit( node *syntaxtree);

#endif /* _ASPLITFCALL_H_ */
