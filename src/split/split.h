#ifndef _SPLIT_H_
#define _SPLIT_H_

#include "types.h"

extern node* SPLITdeclars(node *arg_node, info *arg_info);
extern node* SPLITbody(node *arg_node, info *arg_info);
extern node* SPLITglobaldef(node *arg_node, info *arg_info);
extern node* SPLITvardec(node *arg_node, info *arg_info);


#endif /* _SPLIT_H_ */
