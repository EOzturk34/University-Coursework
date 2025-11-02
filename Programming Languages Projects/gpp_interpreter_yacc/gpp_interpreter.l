%{
#include "gpp_interpreter.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern int yylineno;
%}

%option noyywrap
%option yylineno

STRING \"[^\"]*\"

%%
"and"         { return KW_AND; }
"or"          { return KW_OR; }
"not"         { return KW_NOT; }
"equal"       { return KW_EQUAL; }
"less"        { return KW_LESS; }
"nil"         { return KW_NIL; }
"list"        { return KW_LIST; }
"append"      { return KW_APPEND; }
"concat"      { return KW_CONCAT; }
"set"         { return KW_SET; }
"defun"       { return KW_DEFUN; }
"for"         { return KW_FOR; }
"if"          { return KW_IF; }
"exit"        { return KW_EXIT; }
"load"        { return KW_LOAD; }
"true"        { return KW_TRUE; }
"false"       { return KW_FALSE; }

"+"           { return OP_PLUS; }
"-"           { return OP_MINUS; }
"\\*"         { return OP_MULT; }
"/"           { return OP_DIV; }
"("           { return OP_OP; }
")"           { return OP_CP; }
","           { return OP_COMMA; }
"<"           { return OP_LT; }

{STRING}      {
                 int len = strlen(yytext);
                 char* str = (char*)malloc(len-1);
                 memcpy(str, yytext+1, len-2);
                 str[len-2] = '\0';
                 yylval.sval = str;
                 return STRINGLIT;
              }

[0-9]+        {
                 yylval.ival = atoi(yytext);
                 return VALUEF; 
              }

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.sval = strdup(yytext);
    return IDENTIFIER;
}

";;".*         { }

[ \t]+         { }
[\n\r]+        { }

.              { fprintf(stderr, "Unknown character at line %d: %s\n", yylineno, yytext); }

%%

int main(int argc, char** argv) {
    if(argc > 1) {
        FILE* f = fopen(argv[1], "r");
        if(!f) {
            perror("File open error");
            return 1;
        }
        yyin = f;
    }
    yyparse();
    return 0;
}

