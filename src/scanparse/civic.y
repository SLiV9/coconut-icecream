%{


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <memory.h>

#include "types.h"
#include "tree_basic.h"
#include "str.h"
#include "dbug.h"
#include "ctinfo.h"
#include "free.h"
#include "globals.h"
#include "myglobals.h"

static node *parseresult = NULL;
extern int yylex();
static int yyerror( char *errname);

//#define YYDEBUG 1

%}

%union {
 nodetype            nodetype;
 char               *id;
 int                 cint;
 float               cflt;
 binop               cbinop;
 monop							 cmonop;
 node               *node;
}

%token BRL BRR BCL BCR BSL BSR COMMA SEMICOLON
%token MINUS PLUS STAR SLASH PERCENT LE LT GE GT EQ NE OR AND NOT
%token TRUEVAL FALSEVAL LET

%token EXPORT EXTERN
%token WHILE IF ELSE FOR DO RETURN
%token INT FLOAT BOOL VOID


%token <cint> INTVAL
%token <cflt> FLOATVAL
%token <id> ID

%type <node> intval floatval boolval constant expr
%type <node> instrs instr assign varlet program
%type <cbinop> binop
%type <cmonop> monop


%nonassoc OR
%nonassoc AND
%nonassoc EQ NE
%nonassoc LE LT GE GT 
%nonassoc MINUS PLUS
%nonassoc STAR SLASH PERCENT





%start program

%%

program: instrs 
         {
           parseresult = $1;
         }
         ;

instrs: instr instrs
        {
          $$ = TBmakeInstrs( $1, $2);
        }
      | instr
        {
          $$ = TBmakeInstrs( $1, NULL);
        }
        ;

instr: assign
       {
         $$ = $1;
       }
       ;

assign: varlet LET expr SEMICOLON
        {
          $$ = TBmakeAssign( $1, $3);
        }
        ;

varlet: ID
        {
          $$ = TBmakeVarlet( STRcpy( $1), NULL);
        }
        ;


expr: constant
      {
        $$ = $1;
      }
    | ID
      {
        $$ = TBmakeVarcall( STRcpy( $1), NULL);
      }
    | BRL expr BRR
      {
        $$ =  $2;
      }
    |  expr binop expr
      {
        $$ = TBmakeBinop( $2, $1, $3);
      }
    | monop expr
    	{
    		$$ = TBmakeMonop( $1, $2 );
    	}
    ;

constant: floatval
          {
            $$ = $1;
          }
        | intval
          {
            $$ = $1;
          }
        | boolval
          {
            $$ = $1;
          }
        ;

floatval: FLOATVAL
           {
             $$ = TBmakeFloat( $1);
           }
         ;

intval: INTVAL
        {
          $$ = TBmakeInt( $1);
        }
      ;

boolval: TRUEVAL
         {
           $$ = TBmakeBool( TRUE);
         }
       | FALSEVAL
         {
           $$ = TBmakeBool( FALSE);
         }
       ;

binop: PLUS      { $$ = BO_add; }
     | MINUS     { $$ = BO_sub; }
     | STAR      { $$ = BO_mul; }
     | SLASH     { $$ = BO_div; }
     | PERCENT   { $$ = BO_mod; }
     | LE        { $$ = BO_le; }
     | LT        { $$ = BO_lt; }
     | GE        { $$ = BO_ge; }
     | GT        { $$ = BO_gt; }
     | EQ        { $$ = BO_eq; }
     | OR        { $$ = BO_or; }
     | AND       { $$ = BO_and; }
     ;

monop: MINUS     { $$ = MO_neg; }
     | NOT       { $$ = MO_not; }
     ;
      
%%

static int yyerror( char *error)
{
  CTIabort( "file %s, line %d, col %d\nError parsing source code: %s\n", 
            myglobal.fn, global.line, global.col, error);

  return( 0);
}

node *YYparseTree( void)
{
  DBUG_ENTER("YYparseTree");

	//yydebug = 1;
  yyparse();

  DBUG_RETURN( parseresult);
}

