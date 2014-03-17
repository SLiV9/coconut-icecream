
/**
 * @file print.h
 *
 * Functions to print node structures
 * 
 */

#ifndef _SAC_PRT_NODE_H_
#define _SAC_PRT_NODE_H_

#include "types.h"

const char* vtype_name[5];
const char* monop_name[3];
const char* binop_name[14];

char* arrayDimDisplay(int n);

extern node *PRTdeclars (node * arg_node, info * arg_info);
extern node *PRTparams (node * arg_node, info * arg_info);
extern node *PRTvardecs (node * arg_node, info * arg_info);
extern node *PRTfundefs (node * arg_node, info * arg_info);
extern node *PRTinstrs (node * arg_node, info * arg_info);
extern node *PRTexprs (node * arg_node, info * arg_info);
extern node *PRTargs (node * arg_node, info * arg_info);
extern node *PRTnamedecs (node * arg_node, info * arg_info);
extern node *PRTdimdecs (node * arg_node, info * arg_info);
extern node *PRTdim (node * arg_node, info * arg_info);

extern node *PRTbody (node * arg_node, info * arg_info);
extern node *PRTarraylit (node * arg_node, info * arg_info);
extern node *PRTassign (node * arg_node, info * arg_info);
extern node *PRTvarlet (node * arg_node, info * arg_info);
extern node *PRTfunstate (node * arg_node, info * arg_info);
extern node *PRTif (node * arg_node, info * arg_info);
extern node *PRTwhile (node * arg_node, info * arg_info);
extern node *PRTfor (node * arg_node, info * arg_info);
extern node *PRTiter (node * arg_node, info * arg_info);

extern node *PRTvardec (node * arg_node, info * arg_info);
extern node *PRTglobdec (node * arg_node, info * arg_info);
extern node *PRTfundec (node * arg_node, info * arg_info);
extern node *PRTparam (node * arg_node, info * arg_info);
extern node *PRTglobdef (node * arg_node, info * arg_info);
extern node *PRTfundef (node * arg_node, info * arg_info);
extern node *PRTvarcall (node * arg_node, info * arg_info);
extern node *PRTfuncall (node * arg_node, info * arg_info);
extern node *PRTheader (node * arg_node, info * arg_info);

extern node *PRTmonop (node * arg_node, info * arg_info);
extern node *PRTbinop (node * arg_node, info * arg_info);
extern node *PRTfloat (node * arg_node, info * arg_info);
extern node *PRTint (node * arg_node, info * arg_info);
extern node *PRTbool (node * arg_node, info * arg_info);
extern node *PRTcast (node * arg_node, info * arg_info);
extern node *PRTerror (node * arg_node, info * arg_info);

extern node *PRTdoPrint( node *syntaxtree);

#endif /* _SAC_PRT_NODE_H_ */
