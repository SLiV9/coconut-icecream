#include "split.h"
#include "types.h"
#include "tree_basic.h"
#include "traverse.h"
#include "dbug.h"
#include "str.h"
#include <string.h>

struct INFO{

};

node* CALCCONSTcast(node *arg_node, info *arg_info){
  DBUG_ENTER ("CALCCONSTcast");

   CAST_EXPR( arg_node) = TRAVopt( CAST_EXPR( arg_node), NULL);  

  switch(NODE_TYPE(CAST_EXPR( arg_node))){
    case N_bool:
      switch(CAST_TYPE(arg_node)){
         case VT_bool:
	   //no cast needed
             DBUG_RETURN(CAST_EXPR( arg_node));
         break;
         case VT_int: 
	   DBUG_RETURN(TBmakeInt((int)BOOL_VALUE(CAST_EXPR( arg_node))));
         break;
         case VT_float:
	   DBUG_RETURN(TBmakeFloat((float)BOOL_VALUE(CAST_EXPR( arg_node))));
         break;
      default:
             DBUG_RETURN(arg_node);

      }

      break;
    case N_int:
      switch(CAST_TYPE(arg_node)){
         case VT_bool:
	   DBUG_RETURN(TBmakeBool(INT_VALUE(CAST_EXPR( arg_node))));
         break;
         case VT_int: 
	   //no cast needed
            DBUG_RETURN(CAST_EXPR( arg_node));
         break;
         case VT_float:
	   DBUG_RETURN(TBmakeFloat((float)INT_VALUE(CAST_EXPR( arg_node))));
         break;
      default:
        DBUG_RETURN(arg_node);

      }

      break;
    case N_float:
      switch(CAST_TYPE(arg_node)){
         case VT_bool:
	   DBUG_RETURN(TBmakeBool(FLOAT_VALUE(CAST_EXPR( arg_node))));
         break;
         case VT_int: 
	   DBUG_RETURN(TBmakeInt(FLOAT_VALUE(CAST_EXPR( arg_node))));
         break;
         case VT_float:
	   //no cast needed
            DBUG_RETURN(CAST_EXPR( arg_node));

         break;
      default:
             DBUG_RETURN(arg_node);

      }

      break;
      default:
             DBUG_RETURN(arg_node);

    }


}


node* CALCCONSTmonop(node *arg_node, info *arg_info){
  DBUG_ENTER ("CALCCONSTmonop");

  MONOP_EXPR( arg_node) = TRAVopt( MONOP_EXPR( arg_node), NULL);  
  nodetype nt = NODE_TYPE(MONOP_EXPR( arg_node));
  if(MONOP_OP(arg_node) == MO_not && nt == N_bool){
     DBUG_RETURN(TBmakeBool(! BOOL_VALUE(MONOP_EXPR( arg_node))));
  }else if(MONOP_OP(arg_node) == MO_neg){
    if(nt == N_int){
      DBUG_RETURN(TBmakeInt(-INT_VALUE(MONOP_EXPR( arg_node))));
    }else if(nt == N_float){
      DBUG_RETURN(TBmakeFloat(-FLOAT_VALUE(MONOP_EXPR( arg_node))));
    }else{
      DBUG_RETURN (arg_node);
    }
  }else{
     DBUG_RETURN (arg_node);
  }

}


node* CALCCONSTbinop(node *arg_node, info *arg_info){
  DBUG_ENTER ("CALCCONSTbinop");

  BINOP_LEFT( arg_node) = TRAVopt( BINOP_LEFT( arg_node), NULL);  
  BINOP_RIGHT( arg_node) = TRAVopt( BINOP_RIGHT( arg_node), NULL);  
  nodetype ntr = NODE_TYPE(BINOP_RIGHT( arg_node));
  nodetype ntl = NODE_TYPE(BINOP_LEFT( arg_node));
  if(ntr!= ntl || !(ntl==N_int || ntl==N_bool || ntl==N_float ))
     DBUG_RETURN (arg_node);

  if(ntl == N_bool){
    bool l = BOOL_VALUE(BINOP_LEFT( arg_node));
    bool r = BOOL_VALUE(BINOP_RIGHT( arg_node));

    switch(BINOP_OP(arg_node)){
    case BO_and:
       DBUG_RETURN(TBmakeBool(r&&l));
       break;
    case BO_or:
       DBUG_RETURN(TBmakeBool(r||l));
       break;
    case BO_ne:
       DBUG_RETURN(TBmakeBool(r != l));
       break;
    case BO_eq:
       DBUG_RETURN(TBmakeBool(r==l));
       break;
    case BO_add:
       DBUG_RETURN(TBmakeBool(r | l));
       break;
    case BO_mul:
       DBUG_RETURN(TBmakeBool(r & l));
       break;
    default:
      DBUG_RETURN (arg_node);
      break;

    }
  }else if(ntl == N_int){
    int l = INT_VALUE(BINOP_LEFT( arg_node));
    int r = INT_VALUE(BINOP_RIGHT( arg_node));

    switch(BINOP_OP(arg_node)){
    case BO_add:
       DBUG_RETURN(TBmakeInt(r+l));
       break;
    case BO_sub:
       DBUG_RETURN(TBmakeInt(r-l));
       break;
    case BO_mul:
       DBUG_RETURN(TBmakeInt(r*l));
       break;
    case BO_div:
    	 if (r != 0)
       		DBUG_RETURN(TBmakeInt(l/r));
       break;
    case BO_mod:
       DBUG_RETURN(TBmakeInt(l % r));
       break;
    case BO_lt:
       DBUG_RETURN(TBmakeBool(l < r));
       break;
    case BO_le:
       DBUG_RETURN(TBmakeBool(l<=r));
       break;
    case BO_gt:
       DBUG_RETURN(TBmakeBool(l>r));
       break;
    case BO_ge:
       DBUG_RETURN(TBmakeBool(l>=r));
       break;
    case BO_eq:
       DBUG_RETURN(TBmakeBool(l==r));
       break;
    case BO_ne:
       DBUG_RETURN(TBmakeBool(l != r));
       break;
    default:
      DBUG_RETURN (arg_node);
      break;

    }
  }else if(ntl == N_float){
    float l = FLOAT_VALUE(BINOP_LEFT( arg_node));
    float r = FLOAT_VALUE(BINOP_RIGHT( arg_node));

    switch(BINOP_OP(arg_node)){
    case BO_add:
       DBUG_RETURN(TBmakeFloat(r+l));
       break;
    case BO_sub:
       DBUG_RETURN(TBmakeFloat(r-l));
       break;
    case BO_mul:
       DBUG_RETURN(TBmakeFloat(r*l));
       break;
    case BO_div:
    	 if (r != 0.0f)
       		DBUG_RETURN(TBmakeFloat(l/r));
       break;
    case BO_lt:
       DBUG_RETURN(TBmakeBool(l < r));
       break;
    case BO_le:
       DBUG_RETURN(TBmakeBool(l<=r));
       break;
    case BO_gt:
       DBUG_RETURN(TBmakeBool(l>r));
       break;
    case BO_ge:
       DBUG_RETURN(TBmakeBool(l>=r));
       break;
    case BO_eq:
       DBUG_RETURN(TBmakeBool(l==r));
       break;
    case BO_ne:
       DBUG_RETURN(TBmakeBool(l != r));
       break;
    default:
      DBUG_RETURN (arg_node);
      break;

    }
  }






  DBUG_RETURN (arg_node);


}






node 
*CALCCONSTdoConstCalc( node *syntaxtree)
{

  DBUG_ENTER("CALCCONSTdoConstCalc");

  
  DBUG_ASSERT( (syntaxtree!= NULL), "CALCCONSTdoConstCalc called with empty syntaxtree");

  TRAVpush( TR_calcconst);

  syntaxtree = TRAVdo( syntaxtree, NULL);

  TRAVpop();


  DBUG_RETURN( syntaxtree);



}
