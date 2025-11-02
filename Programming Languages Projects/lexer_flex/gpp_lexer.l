%{
#include <stdio.h>
%}


%option noyywrap



DIGIT          [0-9]
LETTER         [a-zA-Z]
NUMBER         {DIGIT}+
FRACTION       {DIGIT}+(:|f){DIGIT}+
IDENTIFIER     {LETTER}({LETTER}|{DIGIT}|_)*

%%


";;".*          { printf("COMMENT\n"); }


{DIGIT}+{LETTER}+    { printf("SYNTAX_ERROR: '%s' cannot be tokenized.\n", yytext); }



"and"             { printf("KW_AND\n"); }
"or"              { printf("KW_OR\n"); }
"not"             { printf("KW_NOT\n"); }
"equal"           { printf("KW_EQUAL\n"); }
"less"            { printf("KW_LESS\n"); }
"nil"             { printf("KW_NIL\n"); }
"list"            { printf("KW_LIST\n"); }
"append"          { printf("KW_APPEND\n"); }
"concat"          { printf("KW_CONCAT\n"); }
"set"             { printf("KW_SET\n"); }
"deffun"          { printf("KW_DEFFUN\n"); }  /* Sadece 'deffun' ile eşleşir */
"for"             { printf("KW_FOR\n"); }
"if"              { printf("KW_IF\n"); }
"exit"            { printf("KW_EXIT\n"); }
"load"            { printf("KW_LOAD\n"); }
"print"           { printf("KW_PRINT\n"); }
"true"            { printf("KW_TRUE\n"); }
"false"           { printf("KW_FALSE\n"); }


"+"               { printf("OP_PLUS\n"); }
"-"               { printf("OP_MINUS\n"); }
"*"               { printf("OP_MULT\n"); }
"/"               { printf("OP_DIV\n"); }
"("               { printf("OP_OP\n"); }
")"               { printf("OP_CP\n"); }
","               { printf("OP_COMMA\n"); }
"="               { printf("OP_ASSIGN\n"); }


{IDENTIFIER}      { printf("IDENTIFIER: %s\n", yytext); }


{FRACTION}        { printf("VAULEF: %s\n", yytext); }
{NUMBER}          { printf("VAULEI: %s\n", yytext); }


[ \t\n]+          { /* ignore whitespace */ }


.                 { printf("SYNTAX_ERROR: '%s' cannot be tokenized.\n", yytext); }

%%

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("fopen");
            return 1;
        }
    }
    yylex();
    return 0;
}


