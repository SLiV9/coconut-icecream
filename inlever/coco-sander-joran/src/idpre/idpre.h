#ifndef _IDPRE_H_
#define _IDPRE_H_

#include "types.h"

extern node* IDPREvarcall(node *arg_node, info *arg_info);
extern node* IDPREvarlet(node *arg_node, info *arg_info);
extern node* IDPREvardec(node *arg_node, info *arg_info);
extern node* IDPREglobdef(node *arg_node, info *arg_info);
extern node* IDPREglobdec(node *arg_node, info *arg_info);
extern node* IDPREparam(node *arg_node, info *arg_info);
extern node* IDPREiter(node *arg_node, info *arg_info);
extern node* IDPREdim(node *arg_node, info *arg_info);
extern node* IDPREheader(node *arg_node, info *arg_info);
extern node* IDPREfuncall(node *arg_node, info *arg_info);

extern node *IDPREdoIdPrefix( node *syntaxtree);

#endif /* _IDPRE_H_ */
