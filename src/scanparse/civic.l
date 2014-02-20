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

#[ ][0-9]+[ ]\"[A-Za-z0-9\._-]+\".*\n     {
                            char* xxtxt = malloc(sizeof(char)*(strlen(yytext)+1));
                            char* txt = xxtxt + 2;
                            strcpy(xxtxt, yytext);
                            while(*txt!=' '){
                            	txt++;
                            }
														*txt = 0;
														int row = atoi(xxtxt+2);
														char * fn = txt+2;
                            txt+=2;
                            while(*txt!='\"')
                            {
                            	txt++;
                            }
                            *txt = 0;
                            char* filename = malloc(sizeof(char)*(txt-fn+1));
                            strcpy(filename,fn);
                            free(xxtxt);
                            
                            if(myglobal.fn!=NULL)
                            	free(myglobal.fn);
                            global.line = row;
                            myglobal.fn = filename;
                          }

(\r?\n)|(\n?\r)|(\n\r?)   { global.line += 1;
                             global.col = 0;
                             yyless(1);
                           }

[ \t]+                     { global.col += yyleng;
                           } 

%{

if (myglobal.fn != NULL)
{
	free(myglobal.fn);
}

%}
