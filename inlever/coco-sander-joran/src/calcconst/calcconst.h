#ifndef _CALCCONST_H_
#define _CALCCONST_H_

#include "types.h"


extern node* CALCCONSTcast(node *arg_node, info *arg_info);
extern node* CALCCONSTmonop(node *arg_node, info *arg_info);
extern node* CALCCONSTbinop(node *arg_node, info *arg_info);
extern node* CALCCONSTbool(node *arg_node, info *arg_info);
extern node* CALCCONSTfloat(node *arg_node, info *arg_info);
extern node* CALCCONSTint(node *arg_node, info *arg_info);
extern node *CALCCONSTdoConstCalc( node *syntaxtree);

#endif /* _CALCCONST_H_ */
