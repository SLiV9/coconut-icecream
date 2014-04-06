#ifndef _ITERSPLIT_H_
#define _ITERSPLIT_H_

#include "types.h"

extern node* ITERSPLITbody(node *arg_node, info *arg_info);
extern node* ITERSPLITfor(node *arg_node, info *arg_info);

extern node *ITERSPLITdoSplit( node *syntaxtree);

#endif /* _ITERSPLIT_H_ */
