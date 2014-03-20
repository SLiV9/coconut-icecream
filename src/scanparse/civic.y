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
 vtype							 valuetype;
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

%type <node> intval floatval boolval constant expr exprs
%type <node> params param dimdecs
%type <node> varlet varcall vardec
%type <node> header funcall funstate globalfundef localfundef fundec
%type <node> instrs instr assign
%type <node> block
%type <node> vardecs fundefs retexp body
%type <node> globdef globdec declar declars program
%type <valuetype> basictype cast

%nonassoc THEN
%nonassoc ELSE

%left OR
%left AND
%left EQ NE
%left LE LT GE GT
%left MINUS PLUS
%left STAR SLASH PERCENT
%right NOT NEG CAST



%start program

%%

program:
	declars { parseresult = $1; }
;

declars:
	declars declar			
			{ 
				node* N = $1;
				while (DECLARS_NEXT( N) != NULL)
				{
					N = DECLARS_NEXT( N);
				}
				DECLARS_NEXT( N) = TBmakeDeclars( $2, NULL); 
				$$ = $1;
			}
|	declar								{ $$ = TBmakeDeclars( $1, NULL); }
;

declar:
	globdef								{ $$ = $1; }
| globdec								{ $$ = $1; }
| globalfundef					{ $$ = $1; }
| fundec								{ $$ = $1; }
;

body:
	vardecs fundefs instrs retexp 	{ $$ = TBmakeBody( $1, $2, $3, $4); }
| vardecs fundefs instrs 					{ $$ = TBmakeBody( $1, $2, $3, NULL); }
| vardecs fundefs retexp 					{	$$ = TBmakeBody( $1, $2, NULL, $3); }
| vardecs fundefs									{ $$ = TBmakeBody( $1, $2, NULL, NULL); }
| vardecs instrs retexp 					{ $$ = TBmakeBody( $1, NULL, $2, $3); }
| vardecs instrs 									{ $$ = TBmakeBody( $1, NULL, $2, NULL); }
| vardecs retexp 									{	$$ = TBmakeBody( $1, NULL, NULL, $2); }
| vardecs 												{ $$ = TBmakeBody( $1, NULL, NULL, NULL); }
| fundefs instrs retexp						{ $$ = TBmakeBody( NULL, $1, $2, $3); }
| fundefs instrs 									{ $$ = TBmakeBody( NULL, $1, $2, NULL); }
| fundefs retexp									{	$$ = TBmakeBody( NULL, $1, NULL, $2); }
| fundefs													{ $$ = TBmakeBody( NULL, $1, NULL, NULL); }
| instrs retexp										{ $$ = TBmakeBody( NULL, NULL, $1, $2); }
| instrs 													{ $$ = TBmakeBody( NULL, NULL, $1, NULL); }
| retexp													{	$$ = TBmakeBody( NULL, NULL, NULL, $1); }
| /* empty */ 										{ $$ = TBmakeBody( NULL, NULL, NULL, NULL); }
;

retexp:
	RETURN expr SEMICOLON 		{ $$ = $2; }
;

vardecs:
	vardecs vardec			
			{ 
				node* N = $1;
				while (VARDECS_NEXT( N) != NULL)
				{
					N = VARDECS_NEXT( N);
				}
				VARDECS_NEXT( N) = TBmakeVardecs( $2, NULL); 
				$$ = $1;
			}
| vardec 								{ $$ = TBmakeVardecs( $1, NULL); }
;

fundefs:
	fundefs localfundef			
			{ 
				node* N = $1;
				while (FUNDEFS_NEXT( N) != NULL)
				{
					N = FUNDEFS_NEXT( N);
				}
				FUNDEFS_NEXT( N) = TBmakeFundefs( $2, NULL); 
				$$ = $1;
			}
| localfundef 								{ $$ = TBmakeFundefs( $1, NULL); }
;

globalfundef:
	header BCL body BCR						{ $$ = TBmakeFundef( FALSE, $1, $3); }
| EXPORT header BCL body BCR			{ $$ = TBmakeFundef( TRUE, $2, $4); }
;

localfundef:
	header BCL body BCR						{ $$ = TBmakeFundef( FALSE, $1, $3); }
;

fundec:
	EXTERN header SEMICOLON	{ $$ = TBmakeFundec( $2); }
;

globdec:
	EXTERN basictype ID SEMICOLON
			{ $$ = TBmakeGlobdec( STRcpy( $3), $2, NULL); }
| EXTERN basictype BSL dimdecs BSR ID SEMICOLON
			{ $$ = TBmakeGlobdec( STRcpy( $6), $2, $4); }
;

block:
	instr									{ $$ = TBmakeInstrs( $1, NULL); }
| BCL instrs BCR				{ $$ = $2; }
| BCL BCR								{ $$ = NULL; }
;

instrs:
	instrs instr			
			{ 
				node* N = $1;
				while (INSTRS_NEXT( N) != NULL)
				{
					N = INSTRS_NEXT( N);
				}
				INSTRS_NEXT( N) = TBmakeInstrs( $2, NULL); 
				$$ = $1;
			}
| instr 								{ $$ = TBmakeInstrs( $1, NULL); }
;

instr:
	assign 								{ $$ = $1; }
| funstate 							{ $$ = $1; }
| IF BRL expr BRR block %prec THEN
			{ $$ = TBmakeIf($3,$5,NULL);}
| IF BRL expr BRR block ELSE block %prec ELSE
			{ $$ = TBmakeIf($3,$5,$7);}
| WHILE  BRL expr BRR block 
			{$$ = TBmakeWhile(FALSE,$3,$5);} 
| DO  block WHILE  BRL expr BRR SEMICOLON 
			{$$ = TBmakeWhile(TRUE,$5,$2);} 
| FOR  BRL INT ID LET expr COMMA expr BRR block 
			{$$ = TBmakeFor( TBmakeIter(STRcpy($4)),$6,$8,NULL,$10);}
| FOR  BRL INT ID LET expr COMMA expr COMMA expr BRR block 
			{$$ = TBmakeFor( TBmakeIter(STRcpy($4)),$6,$8,$10,$12);}
;

funstate:
	funcall SEMICOLON 						{ $$ = TBmakeFunstate( $1);	}
;

assign:
	varlet LET expr SEMICOLON 		{ $$ = TBmakeAssign( $1, $3); }
;

varlet:
	ID 										{ $$ = TBmakeVarlet( STRcpy( $1), NULL); }
| ID BSL exprs BSR 			{ $$ = TBmakeVarlet( STRcpy( $1), $3); }
;

varcall:
	ID 										{ $$ = TBmakeVarcall( STRcpy( $1), NULL); }
| ID BSL exprs BSR 			{ $$ = TBmakeVarcall( STRcpy( $1), $3); }
;
        
funcall:
	ID BRL BRR 						{ $$ = TBmakeFuncall( STRcpy( $1), NULL); }
| ID BRL exprs BRR 			{ $$ = TBmakeFuncall( STRcpy( $1), $3); }
;

globdef:
	basictype ID SEMICOLON
			{ $$ = TBmakeGlobdef( STRcpy( $2), $1, FALSE, NULL, NULL); }
| basictype ID LET expr SEMICOLON
			{ $$ = TBmakeGlobdef( STRcpy( $2), $1, FALSE, $4, NULL); }
| basictype BSL exprs BSR ID SEMICOLON
			{ $$ = TBmakeGlobdef( STRcpy( $5), $1, FALSE, NULL, $3); }
| basictype BSL exprs BSR ID LET expr SEMICOLON
			{ $$ = TBmakeGlobdef( STRcpy( $5), $1, FALSE, $7, $3); }
|	EXPORT basictype ID SEMICOLON
			{ $$ = TBmakeGlobdef( STRcpy( $3), $2, TRUE, NULL, NULL); }
| EXPORT basictype ID LET expr SEMICOLON
			{ $$ = TBmakeGlobdef( STRcpy( $3), $2, TRUE, $5, NULL); }
| EXPORT basictype BSL exprs BSR ID SEMICOLON
			{ $$ = TBmakeGlobdef( STRcpy( $6), $2, TRUE, NULL, $4); }
| EXPORT basictype BSL exprs BSR ID LET expr SEMICOLON
			{ $$ = TBmakeGlobdef( STRcpy( $6), $2, TRUE, $8, $4); }
;

vardec:
	basictype ID SEMICOLON
			{ $$ = TBmakeVardec( STRcpy( $2), $1, NULL, NULL); }
| basictype ID LET expr SEMICOLON
			{ $$ = TBmakeVardec( STRcpy( $2), $1, $4, NULL); }
| basictype BSL exprs BSR ID SEMICOLON
			{ $$ = TBmakeVardec( STRcpy( $5), $1, NULL, $3); }
| basictype BSL exprs BSR ID LET expr SEMICOLON
			{ $$ = TBmakeVardec( STRcpy( $5), $1, $7, $3); }
;

header:
	basictype ID BRL BRR
			{ $$ = TBmakeHeader( STRcpy( $2), $1, NULL); }
| basictype ID BRL params BRR
			{ $$ = TBmakeHeader( STRcpy( $2), $1, $4); }
| VOID ID BRL BRR
			{ $$ = TBmakeHeader( STRcpy( $2), VT_void, NULL); }
| VOID ID BRL params BRR
			{ $$ = TBmakeHeader( STRcpy( $2), VT_void, $4); }
;

params:
	param COMMA params		{ $$ = TBmakeParams( $1, $3); }
| param									{ $$ = TBmakeParams( $1, NULL); }
;

param:
	basictype ID
			{ $$ = TBmakeParam( STRcpy( $2), $1, NULL); }
| basictype BSL dimdecs BSR ID
			{ $$ = TBmakeParam( STRcpy( $5), $1, $3); }
;

dimdecs:
	ID COMMA dimdecs
			{ $$ = TBmakeDimdecs( TBmakeDim( STRcpy( $1)), $3); }
| ID
			{ $$ = TBmakeDimdecs( TBmakeDim( STRcpy( $1)), NULL); }
;

exprs:
	expr									{ $$ = TBmakeExprs( $1, NULL); }
| expr COMMA exprs 			{	$$ = TBmakeExprs( $1, $3); }
;

expr:
	constant 												{ $$ = $1; }
| varcall													{ $$ = $1; }
| funcall													{ $$ = $1; }
| cast expr %prec CAST 						{ $$ = TBmakeCast( $1, $2); }
| MINUS expr %prec NEG 						{	$$ = TBmakeMonop( MO_neg, $2); }
| NOT expr												{ $$ = TBmakeMonop( MO_not, $2); }
| BRL expr BRR									  { $$ =  $2; }
| expr PLUS expr								  { $$ = TBmakeBinop( BO_add, $1, $3); }
| expr MINUS expr									{ $$ = TBmakeBinop( BO_sub, $1, $3); }
| expr STAR expr									{ $$ = TBmakeBinop( BO_mul, $1, $3); }
| expr SLASH expr									{ $$ = TBmakeBinop( BO_div, $1, $3); }
| expr PERCENT expr								{ $$ = TBmakeBinop( BO_mod, $1, $3); }
| expr LE expr										{ $$ = TBmakeBinop( BO_le, $1, $3); }
| expr LT expr										{ $$ = TBmakeBinop( BO_lt, $1, $3); }
| expr GE expr										{ $$ = TBmakeBinop( BO_ge, $1, $3); }
| expr GT expr										{ $$ = TBmakeBinop( BO_gt, $1, $3); }
| expr EQ expr										{ $$ = TBmakeBinop( BO_eq, $1, $3); }
| expr NE expr										{ $$ = TBmakeBinop( BO_ne, $1, $3); }
| expr AND expr										{ $$ = TBmakeBinop( BO_and, $1, $3); }
| expr OR expr										{ $$ = TBmakeBinop( BO_or, $1, $3); }
| BSL exprs BSR										{ $$ = TBmakeArraylit( $2); }
;
    
cast:
	BRL basictype BRR 		{ $$ = $2; }
;
    
basictype:
	INT 									{ $$ = VT_int; }
|	FLOAT 								{ $$ = VT_float; }
|	BOOL 									{ $$ = VT_bool; }
;

constant:
	floatval							{ $$ = $1; }
| intval								{ $$ = $1; }
| boolval								{ $$ = $1; }
;

floatval:
	FLOATVAL							{ $$ = TBmakeFloat( $1); }
;

intval:
	INTVAL 								{ $$ = TBmakeInt( $1); }
;

boolval:
	TRUEVAL 							{ $$ = TBmakeBool( TRUE); }
| FALSEVAL 							{ $$ = TBmakeBool( FALSE); }
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

