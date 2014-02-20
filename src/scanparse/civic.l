%{

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "node_basic.h"
#include "str.h"
#include "free.h"
#include "civic.tab.h"
#include "globals.h"
#include "myglobals.h"
#include "ctinfo.h"


#define FILTER(token) \
  global.col += yyleng;  \
  return( token);

#define LINEBUF_SIZE 256
#define YY_NO_UNPUT
#define YY_NO_INPUT

%}

%p 6000 /* set positions to 6000 */
%e 2000 /* set nodes to 1100 */
%option noyywrap
%option nounput


%%

"("                        { FILTER( BRL); }
")"                        { FILTER( BRR); }
"{"                        { FILTER( BCL); }
"}"                        { FILTER( BCR); }

","                        { FILTER( COMMA); }
";"                        { FILTER( SEMICOLON); }

"-"                        { FILTER( MINUS); }
"+"                        { FILTER( PLUS);  }
"*"                        { FILTER( STAR);  } 
"/"                        { FILTER( SLASH);  } 
"%"                        { FILTER( PERCENT);  } 


"<="                       { FILTER( LE);    }
"<"                        { FILTER( LT);    } 
">="                       { FILTER( GE);    }
">"                        { FILTER( GT);    }
"=="                       { FILTER( EQ);    }
"!="                       { FILTER( NE);    }
"&&"                       { FILTER( AND);    }
"||"                       { FILTER( OR);    }

"="                        { FILTER( LET);    }
"!"                        { FILTER( NOT);  } 

"true"                     { FILTER( TRUEVAL); }
"false"                    { FILTER( FALSEVAL); }

"export"									 { FILTER( EXPORT); }
"extern"									 { FILTER( EXTERN); }

"while"										 { FILTER( WHILE); }
"do"											 { FILTER( DO); }
"for"											 { FILTER( FOR); }
"if"											 { FILTER( IF); }
"else"										 { FILTER( ELSE); }
"return"									 { FILTER( RETURN); }

"int"											 { FILTER( INT); }
"float"										 { FILTER( FLOAT); }
"bool"										 { FILTER( BOOL); }
"void"										 { FILTER( VOID); }


[A-Za-z_][A-Za-z0-9_]*     { yylval.id = STRcpy(yytext);
                             FILTER( ID);
                           }

[0-9]+\.[0-9]*             { yylval.cflt=atof(yytext);
                             FILTER( FLOATVAL);
                           }

[0-9]*\.[0-9]+             { yylval.cflt=atof(yytext);
                             FILTER( FLOATVAL);
                           }


[0-9]+                     { yylval.cint=atoi(yytext);
                             FILTER( INTVAL);
                           }

#[ ][0-9]+[ ]\"[^\"]+\".*\n     {
									/* Interpret the Preprocessors hints.
									 * They are formatted:
									 * # row "filename" flags\n
									 * The row is the number of the next line,
									 * if it were a line in file filename. */
									int row = 0;
									char* filename;
									
									// xxtext is a local copy of yytext.
                  char* xxtext = malloc(sizeof(char)*(strlen(yytext)+1));
                  char* txt = xxtext + 2;
                  
									// The row can be read from rowstart to txt.
                  char* rowstart = txt;
                  strcpy(xxtext, yytext);
                  while (*txt != ' ')
                  {
                  	txt++;
                  }
									*txt = 0;
									row = atoi(rowstart);
									
                  // The filename can be read from fnstart to txt.
                  txt += 2;
									char* fnstart = txt;
                  while (*txt != '\"')
                  {
                  	txt++;
                  }
                  *txt = 0;
                  filename = malloc(sizeof(char)*(txt - fnstart + 1));
                  strcpy(filename,fn);
                  
                  // Done.
                  free(xxtext);
                  
                  if (myglobal.fn!=NULL)
                  {
                  	free(myglobal.fn);
                  }
                  myglobal.fn = filename;
                  global.line = row;
                  global.col = 0;
                }

(\r?\n)|(\n?\r)|(\n\r?)    { global.line += 1;
                             global.col = 0;
                           }

[ \t]+                     { global.col += yyleng;
                           } 

%{

// If necessary, free filenames malloc'd when interpreting [# row filename]'s.
if (myglobal.fn != NULL)
{
	free(myglobal.fn);
}

%}
