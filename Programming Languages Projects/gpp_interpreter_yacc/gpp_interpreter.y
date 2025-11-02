%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YYERROR_VERBOSE 1

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
extern FILE *yyin;

static int env_count = 0;
typedef struct {
    char* name;
    int value;
} VarEntry;
static VarEntry env[100];

static int lookup_var(const char* name) {
    for (int i=0; i<env_count; i++) {
        if(strcmp(env[i].name, name)==0)
            return env[i].value;
    }
    return 0;
}

static void set_var(const char* name, int val) {
    for (int i=0; i<env_count; i++) {
        if(strcmp(env[i].name, name)==0) {
            env[i].value = val;
            return;
        }
    }
    env[env_count].name = strdup(name);
    env[env_count].value = val;
    env_count++;
}

static int eval_op_plus(int a, int b) { return a+b; }
static int eval_op_minus(int a, int b) { return a-b; }
static int eval_op_mult(int a, int b) { return a*b; }
static int eval_op_div(int a, int b) { return b==0?0:a/b; }
static int eval_op_lt(int a, int b) { return a<b?1:0; }
static int eval_op_equal(int a, int b) { return a==b?1:0; }
static int eval_op_and(int a, int b) { return (a!=0 && b!=0)?1:0; }
static int eval_op_or(int a, int b) { return (a!=0 || b!=0)?1:0; }
static int eval_op_not(int a) { return (a==0)?1:0; }

static int bool_val_true() { return 1; }
static int bool_val_false() { return 0; }
static int bool_val_nil() { return 0; }

static int eval_list() { return 0; }
static int eval_append() { return 0; }
static int eval_concat() { return 0; }

%}

%union {
    int ival;
    char *sval;
}

%locations

%token <sval> IDENTIFIER
%token <ival> VALUEF
%token <sval> STRINGLIT
%token KW_AND KW_OR KW_NOT KW_EQUAL KW_LESS KW_NIL KW_LIST KW_APPEND KW_CONCAT KW_SET KW_DEFUN KW_FOR KW_IF KW_EXIT KW_LOAD KW_TRUE KW_FALSE
%token OP_PLUS OP_MINUS OP_DIV OP_MULT OP_OP OP_CP OP_COMMA
%token OP_LT

%type <ival> START SEXPLIST SEXP SETEXPR IFEXPR PLUSEXPR MINUSEXPR MULTEXPR DIVEXPR LESSEXPR EXITEXPR LOADEXPR EQUALLEXPR ANDEXPR OREXPR NOTEXPR
%type <ival> TRUEEXPR FALSEEXPR NILEXPR LISTEXPR APPENDEXPR CONCATEXPR SEXPSEQ

%start START

%%

START 
    : SEXPLIST { }
;

SEXPLIST 
    : SEXP {
        printf("%d\n", $1);
        $$ = $1;
      }
    | SEXPLIST SEXP {
        printf("%d\n", $2);
        $$ = $2;
      }
;

SEXP
    : IDENTIFIER {
        int v = lookup_var($1);
        free($1);
        $$ = v;
      }
    | VALUEF {
        $$ = $1;
      }
    | SETEXPR {
        $$ = $1;
      }
    | IFEXPR {
        $$ = $1;
      }
    | PLUSEXPR {
        $$ = $1;
      }
    | MINUSEXPR {
        $$ = $1;
      }
    | MULTEXPR {
        $$ = $1;
      }
    | DIVEXPR {
        $$ = $1;
      }
    | LESSEXPR {
        $$ = $1;
      }
    | EXITEXPR {
        $$ = $1;
      }
    | LOADEXPR {
        $$ = $1;
      }
    | EQUALLEXPR {
        $$ = $1;
      }
    | ANDEXPR {
        $$ = $1;
      }
    | OREXPR {
        $$ = $1;
      }
    | NOTEXPR {
        $$ = $1;
      }
    | TRUEEXPR {
        $$ = $1;
      }
    | FALSEEXPR {
        $$ = $1;
      }
    | NILEXPR {
        $$ = $1;
      }
    | LISTEXPR {
        $$ = $1;
      }
    | APPENDEXPR {
        $$ = $1;
      }
    | CONCATEXPR {
        $$ = $1;
      }
;

SETEXPR
    : OP_OP KW_SET IDENTIFIER SEXP OP_CP {
        set_var($3, $4);
        free($3);
        $$ = 0; 
      }
;

IFEXPR
    : OP_OP KW_IF SEXP SEXP SEXP OP_CP {
        int cond = $3;
        if(cond) {
            $$ = $4; 
        } else {
            $$ = $5;
        }
      }
;

PLUSEXPR
    : OP_OP OP_PLUS SEXP SEXP OP_CP {
        int v1 = $3;
        int v2 = $4;
        $$ = eval_op_plus(v1,v2);
      }
;

MINUSEXPR
    : OP_OP OP_MINUS SEXP SEXP OP_CP {
        int v1 = $3;
        int v2 = $4;
        $$ = eval_op_minus(v1,v2);
      }
;

MULTEXPR
    : OP_OP OP_MULT SEXP SEXP OP_CP {
        int v1 = $3;
        int v2 = $4;
        $$ = eval_op_mult(v1,v2);
      }
;

DIVEXPR
    : OP_OP OP_DIV SEXP SEXP OP_CP {
        int v1 = $3;
        int v2 = $4;
        $$ = eval_op_div(v1,v2);
      }
;

LESSEXPR
    : OP_OP OP_LT SEXP SEXP OP_CP {
        int v1 = $3;
        int v2 = $4;
        $$ = eval_op_lt(v1,v2);
      }
;

EXITEXPR
    : OP_OP KW_EXIT OP_CP {
        printf("Exiting...\n");
        exit(0);
      }
;

LOADEXPR
    : OP_OP KW_LOAD STRINGLIT OP_CP {
        printf("Loading file: %s\n", $3);
        free($3);
        $$ = 0;
      }
;

EQUALLEXPR
    : OP_OP KW_EQUAL SEXP SEXP OP_CP {
        int v1 = $3;
        int v2 = $4;
        $$ = eval_op_equal(v1,v2);
      }
;

ANDEXPR
    : OP_OP KW_AND SEXP SEXP OP_CP {
        int v1 = $3;
        int v2 = $4;
        $$ = eval_op_and(v1,v2);
      }
;

OREXPR
    : OP_OP KW_OR SEXP SEXP OP_CP {
        int v1 = $3;
        int v2 = $4;
        $$ = eval_op_or(v1,v2);
      }
;

NOTEXPR
    : OP_OP KW_NOT SEXP OP_CP {
        int v1 = $3;
        $$ = eval_op_not(v1);
      }
;

TRUEEXPR
    : OP_OP KW_TRUE OP_CP {
        $$ = bool_val_true();
      }
;

FALSEEXPR
    : OP_OP KW_FALSE OP_CP {
        $$ = bool_val_false();
      }
;

NILEXPR
    : OP_OP KW_NIL OP_CP {
        $$ = bool_val_nil();
      }
;

LISTEXPR
    : OP_OP KW_LIST SEXPSEQ OP_CP {
        $$ = eval_list();
      }
;

APPENDEXPR
    : OP_OP KW_APPEND SEXP SEXP OP_CP {
        $$ = eval_append();
      }
;

CONCATEXPR
    : OP_OP KW_CONCAT SEXP SEXP OP_CP {
        $$ = eval_concat();
      }
;

SEXPSEQ
    : /* boş */
      { $$ = 0; }
    | SEXP SEXPSEQ {
        $$ = 0;
      }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, s);
}

