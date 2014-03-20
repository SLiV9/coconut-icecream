#ifndef _CIVCC_MYTYPES_H_
#define _CIVCC_MYTYPES_H_

/*
 * This enumeration defines all value and return types.
 */
typedef enum { VT_void, VT_int, VT_float, VT_bool, VT_unknown } vtype;

/*
 * This enumeration defines all monops
 */
typedef enum { MO_not, MO_neg, MO_unknown } monop;

/*
 * This enumeration defines all binops
 */
typedef enum { BO_add, BO_sub, BO_mul, BO_div, BO_mod, 
               BO_lt, BO_le, BO_gt, BO_ge, BO_eq, BO_ne, 
               BO_and, BO_or, BO_unknown } binop;

/*
 * NameDec Scope Depth
 */
#define NDSD_GLOBAL() (-1)
#define NDSD_LOCAL() (0)
#define NDSD_OUTER(n) (n)

#endif  /* _CIVCC_MYTYPES_H_ */
