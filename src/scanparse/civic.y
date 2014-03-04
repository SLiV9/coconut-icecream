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
%type <node> funhead funcall funstate globalfundef localfundef fundec
%type <node> instrs instr assign
%type <node> block
%type <node> vardecs fundefs retexp body
%type <node> globdef globdec declar declars program
%type <valuetype> basictype cast

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
	declars declar								{ $$ = TBmakeDeclars( $2, $1); }
|	declar												{ $$ = TBmakeDeclars( $1, NULL); }
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
	vardecs vardec 				{ $$ = TBmakeVardecs( $2, $1); }
| vardec 								{ $$ = TBmakeVardecs( $1, NULL); }
;

fundefs:
	fundefs localfundef					{ $$ = TBmakeFundefs( $1, $2); }
| localfundef 								{ $$ = TBmakeFundefs( $1, NULL); }
;

globalfundef:
	funhead BCL body BCR						{ $$ = TBmakeFundef( FALSE, $1, $3); }
| EXPORT funhead BCL body BCR			{ $$ = TBmakeFundef( TRUE, $2, $4); }
;

localfundef:
	funhead BCL body BCR						{ $$ = TBmakeFundef( FALSE, $1, $3); }
;

fundec:
	EXTERN funhead SEMICOLON	{ $$ = TBmakeFundec( $2); }
;

globdec:
	EXTERN basictype ID
			{ $$ = TBmakeGlobdec( TBmakeVarhead( STRcpy( $3), $2), NULL); }
| EXTERN basictype ID BSL dimdecs BSR
			{ $$ = TBmakeGlobdec( TBmakeVarhead( STRcpy( $3), $2), $5); }
;

block:
	instr									{ $$ = TBmakeInstrs( $1, NULL); }
| BCL instrs BCR				{ $$ = $2; }
;

instrs:
	instrs instr 					{ $$ = TBmakeInstrs( $2, $1); }
| instr 								{ $$ = TBmakeInstrs( $1, NULL); }
;

instr:
	assign 								{ $$ = $1; }
| funstate 							{ $$ = $1; }
| IF BRL expr BRR block 
			{ $$ = TBmakeIf($3,$5,NULL);}
| IF BRL expr BRR block ELSE block 
			{ $$ = TBmakeIf($3,$5,$7);}
| WHILE  BRL expr BRR block 
			{$$ = TBmakeWhile(FALSE,$3,$5);} 
| DO  block WHILE  BRL expr BRR SEMICOLON 
			{$$ = TBmakeWhile(TRUE,$5,$2);} 
| FOR  BRL INT ID LET expr COMMA expr BRR block 
			{$$ = TBmakeFor( TBmakeVarhead(STRcpy($4),INT),$6,$8,NULL,$10);} 
| FOR  BRL INT ID LET expr COMMA expr COMMA expr BRR block 
			{$$ = TBmakeFor( TBmakeVarhead(STRcpy($4),INT),$6,$8,$10,$12);} 
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
			{ $$ = TBmakeGlobdef( FALSE, TBmakeVarhead( STRcpy( $2), $1), NULL, NULL); }
| basictype ID LET expr SEMICOLON
			{ $$ = TBmakeGlobdef( FALSE, TBmakeVarhead( STRcpy( $2), $1), $4, NULL); }
| basictype ID BSL exprs BSR SEMICOLON
			{ $$ = TBmakeGlobdef( FALSE, TBmakeVarhead( STRcpy( $2), $1), NULL, $4); }
| basictype ID BSL exprs BSR LET expr SEMICOLON
			{ $$ = TBmakeGlobdef( FALSE, TBmakeVarhead( STRcpy( $2), $1), $7, $4); }
|	EXPORT basictype ID SEMICOLON
			{ $$ = TBmakeGlobdef( TRUE, TBmakeVarhead( STRcpy( $3), $2), NULL, NULL); }
| EXPORT basictype ID LET expr SEMICOLON
			{ $$ = TBmakeGlobdef( TRUE, TBmakeVarhead( STRcpy( $3), $2), $5, NULL); }
| EXPORT basictype ID BSL exprs BSR SEMICOLON
			{ $$ = TBmakeGlobdef( TRUE, TBmakeVarhead( STRcpy( $3), $2), NULL, $5); }
| EXPORT basictype ID BSL exprs BSR LET expr SEMICOLON
			{ $$ = TBmakeGlobdef( TRUE, TBmakeVarhead( STRcpy( $3), $2), $8, $5); }
;

vardec:
	basictype ID SEMICOLON
			{ $$ = TBmakeVardec( TBmakeVarhead( STRcpy( $2), $1), NULL, NULL); }
| basictype ID LET expr SEMICOLON
			{ $$ = TBmakeVardec( TBmakeVarhead( STRcpy( $2), $1), $4, NULL); }
| basictype ID BSL exprs BSR SEMICOLON
			{ $$ = TBmakeVardec( TBmakeVarhead( STRcpy( $2), $1), NULL, $4); }
| basictype ID BSL exprs BSR LET expr SEMICOLON
			{ $$ = TBmakeVardec( TBmakeVarhead( STRcpy( $2), $1), $7, $4); }
;

funhead:
	basictype ID BRL BRR
			{ $$ = TBmakeFunhead( STRcpy( $2), $1, NULL); }
| basictype ID BRL params BRR
			{ $$ = TBmakeFunhead( STRcpy( $2), $1, $4); }
| VOID ID BRL BRR
			{ $$ = TBmakeFunhead( STRcpy( $2), VT_void, NULL); }
| VOID ID BRL params BRR
			{ $$ = TBmakeFunhead( STRcpy( $2), VT_void, $4); }
;

params:
	param COMMA params		{ $$ = TBmakeParams( $1, $3); }
| param									{ $$ = TBmakeParams( $1, NULL); }
;

param:
	basictype ID
			{ $$ = TBmakeParam( TBmakeVarhead( STRcpy( $2), $1), NULL); }
| basictype ID BSL dimdecs BSR
			{ $$ = TBmakeParam( TBmakeVarhead( STRcpy( $2), $1), $4); }
;

dimdecs:
	ID COMMA dimdecs
			{ $$ = TBmakeDimdecs( TBmakeVarhead( STRcpy( $1), VT_int), $3); }
| ID
			{ $$ = TBmakeDimdecs( TBmakeVarhead( STRcpy( $1), VT_int), NULL); }
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

