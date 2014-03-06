#ifndef _NAMELINK_H_
#define _NAMELINK_H_

#include "types.h"

extern node* NAMELINKvarcall(node *arg_node, info *arg_info);
extern node* NAMELINKvarlet(node *arg_node, info *arg_info);
extern node* NAMELINKfuncall(node *arg_node, info *arg_info);
extern node* NAMELINKvardec(node *arg_node, info *arg_info);
extern node* NAMELINKglobdef(node *arg_node, info *arg_info);
extern node* NAMELINKglobdec(node *arg_node, info *arg_info);
extern node* NAMELINKparam(node *arg_node, info *arg_info);
extern node* NAMELINKiter(node *arg_node, info *arg_info);
extern node* NAMELINKdim(node *arg_node, info *arg_info);
extern node* NAMELINKfundef(node *arg_node, info *arg_info);
extern node* NAMELINKfundec(node *arg_node, info *arg_info);

extern node* NAMELINKfor(node *arg_node, info *arg_info);

extern node *NAMELINKdoLinkNames( node *syntaxtree);

#endif /* _NAMELINK_H_ */
