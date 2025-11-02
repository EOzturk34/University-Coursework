/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 1 "gpp_interpreter.y"

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


#line 130 "gpp_interpreter.tab.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "gpp_interpreter.tab.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFIER = 3,                 /* IDENTIFIER  */
  YYSYMBOL_VALUEF = 4,                     /* VALUEF  */
  YYSYMBOL_STRINGLIT = 5,                  /* STRINGLIT  */
  YYSYMBOL_KW_AND = 6,                     /* KW_AND  */
  YYSYMBOL_KW_OR = 7,                      /* KW_OR  */
  YYSYMBOL_KW_NOT = 8,                     /* KW_NOT  */
  YYSYMBOL_KW_EQUAL = 9,                   /* KW_EQUAL  */
  YYSYMBOL_KW_LESS = 10,                   /* KW_LESS  */
  YYSYMBOL_KW_NIL = 11,                    /* KW_NIL  */
  YYSYMBOL_KW_LIST = 12,                   /* KW_LIST  */
  YYSYMBOL_KW_APPEND = 13,                 /* KW_APPEND  */
  YYSYMBOL_KW_CONCAT = 14,                 /* KW_CONCAT  */
  YYSYMBOL_KW_SET = 15,                    /* KW_SET  */
  YYSYMBOL_KW_DEFUN = 16,                  /* KW_DEFUN  */
  YYSYMBOL_KW_FOR = 17,                    /* KW_FOR  */
  YYSYMBOL_KW_IF = 18,                     /* KW_IF  */
  YYSYMBOL_KW_EXIT = 19,                   /* KW_EXIT  */
  YYSYMBOL_KW_LOAD = 20,                   /* KW_LOAD  */
  YYSYMBOL_KW_TRUE = 21,                   /* KW_TRUE  */
  YYSYMBOL_KW_FALSE = 22,                  /* KW_FALSE  */
  YYSYMBOL_OP_PLUS = 23,                   /* OP_PLUS  */
  YYSYMBOL_OP_MINUS = 24,                  /* OP_MINUS  */
  YYSYMBOL_OP_DIV = 25,                    /* OP_DIV  */
  YYSYMBOL_OP_MULT = 26,                   /* OP_MULT  */
  YYSYMBOL_OP_OP = 27,                     /* OP_OP  */
  YYSYMBOL_OP_CP = 28,                     /* OP_CP  */
  YYSYMBOL_OP_COMMA = 29,                  /* OP_COMMA  */
  YYSYMBOL_OP_LT = 30,                     /* OP_LT  */
  YYSYMBOL_YYACCEPT = 31,                  /* $accept  */
  YYSYMBOL_START = 32,                     /* START  */
  YYSYMBOL_SEXPLIST = 33,                  /* SEXPLIST  */
  YYSYMBOL_SEXP = 34,                      /* SEXP  */
  YYSYMBOL_SETEXPR = 35,                   /* SETEXPR  */
  YYSYMBOL_IFEXPR = 36,                    /* IFEXPR  */
  YYSYMBOL_PLUSEXPR = 37,                  /* PLUSEXPR  */
  YYSYMBOL_MINUSEXPR = 38,                 /* MINUSEXPR  */
  YYSYMBOL_MULTEXPR = 39,                  /* MULTEXPR  */
  YYSYMBOL_DIVEXPR = 40,                   /* DIVEXPR  */
  YYSYMBOL_LESSEXPR = 41,                  /* LESSEXPR  */
  YYSYMBOL_EXITEXPR = 42,                  /* EXITEXPR  */
  YYSYMBOL_LOADEXPR = 43,                  /* LOADEXPR  */
  YYSYMBOL_EQUALLEXPR = 44,                /* EQUALLEXPR  */
  YYSYMBOL_ANDEXPR = 45,                   /* ANDEXPR  */
  YYSYMBOL_OREXPR = 46,                    /* OREXPR  */
  YYSYMBOL_NOTEXPR = 47,                   /* NOTEXPR  */
  YYSYMBOL_TRUEEXPR = 48,                  /* TRUEEXPR  */
  YYSYMBOL_FALSEEXPR = 49,                 /* FALSEEXPR  */
  YYSYMBOL_NILEXPR = 50,                   /* NILEXPR  */
  YYSYMBOL_LISTEXPR = 51,                  /* LISTEXPR  */
  YYSYMBOL_APPENDEXPR = 52,                /* APPENDEXPR  */
  YYSYMBOL_CONCATEXPR = 53,                /* CONCATEXPR  */
  YYSYMBOL_SEXPSEQ = 54                    /* SEXPSEQ  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
             && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
  YYLTYPE yyls_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE) \
             + YYSIZEOF (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  45
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   76

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  31
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  24
/* YYNRULES -- Number of rules.  */
#define YYNRULES  46
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  96

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   285


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    82,    82,    86,    90,    97,   102,   105,   108,   111,
     114,   117,   120,   123,   126,   129,   132,   135,   138,   141,
     144,   147,   150,   153,   156,   159,   165,   173,   184,   192,
     200,   208,   216,   224,   231,   239,   247,   255,   263,   270,
     276,   282,   288,   294,   300,   307,   308
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFIER", "VALUEF",
  "STRINGLIT", "KW_AND", "KW_OR", "KW_NOT", "KW_EQUAL", "KW_LESS",
  "KW_NIL", "KW_LIST", "KW_APPEND", "KW_CONCAT", "KW_SET", "KW_DEFUN",
  "KW_FOR", "KW_IF", "KW_EXIT", "KW_LOAD", "KW_TRUE", "KW_FALSE",
  "OP_PLUS", "OP_MINUS", "OP_DIV", "OP_MULT", "OP_OP", "OP_CP", "OP_COMMA",
  "OP_LT", "$accept", "START", "SEXPLIST", "SEXP", "SETEXPR", "IFEXPR",
  "PLUSEXPR", "MINUSEXPR", "MULTEXPR", "DIVEXPR", "LESSEXPR", "EXITEXPR",
  "LOADEXPR", "EQUALLEXPR", "ANDEXPR", "OREXPR", "NOTEXPR", "TRUEEXPR",
  "FALSEEXPR", "NILEXPR", "LISTEXPR", "APPENDEXPR", "CONCATEXPR",
  "SEXPSEQ", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-17)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-1)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      19,   -17,   -17,    -5,    11,    19,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,    19,    19,    19,    19,
     -16,    19,    19,    19,    21,    19,     2,    26,     6,     8,
      19,    19,    19,    19,    19,   -17,   -17,    19,    19,     9,
      19,   -17,    19,    10,    19,    19,    19,    19,   -17,    17,
     -17,   -17,    19,    19,    19,    19,    19,    23,    24,   -17,
      25,   -17,   -17,    30,    31,    32,    19,   -17,    33,    39,
      40,    41,    42,   -17,   -17,   -17,   -17,   -17,   -17,    43,
     -17,   -17,   -17,   -17,   -17,   -17
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     5,     6,     0,     0,     2,     3,     7,     8,     9,
      10,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,     0,     0,     0,     0,
       0,    45,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     1,     4,     0,     0,     0,
       0,    41,    45,     0,     0,     0,     0,     0,    33,     0,
      39,    40,     0,     0,     0,     0,     0,     0,     0,    38,
       0,    46,    42,     0,     0,     0,     0,    34,     0,     0,
       0,     0,     0,    36,    37,    35,    43,    44,    26,     0,
      28,    29,    31,    30,    32,    27
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -17,   -17,   -17,     0,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,   -17,
     -17,   -17,   -17,   -13
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     4,     5,    52,     7,     8,     9,    10,    11,    12,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    24,    25,    53
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
       6,    26,    27,    28,    29,    46,    30,    31,    32,    33,
      34,    45,    51,    35,    36,    37,    38,    39,    40,    41,
      42,    43,     1,     2,    56,    44,    47,    48,    49,    50,
      58,    59,    54,    55,    60,    57,    61,    69,    72,    71,
      62,    63,    64,    65,    66,    77,     3,    67,    68,     0,
      70,    83,    84,    85,    73,    74,    75,    76,    86,    87,
      88,    90,    78,    79,    80,    81,    82,    91,    92,    93,
      94,    95,     0,     0,     0,     0,    89
};

static const yytype_int8 yycheck[] =
{
       0,     6,     7,     8,     9,     5,    11,    12,    13,    14,
      15,     0,    28,    18,    19,    20,    21,    22,    23,    24,
      25,    26,     3,     4,     3,    30,    26,    27,    28,    29,
      28,     5,    32,    33,    28,    35,    28,    28,    28,    52,
      40,    41,    42,    43,    44,    28,    27,    47,    48,    -1,
      50,    28,    28,    28,    54,    55,    56,    57,    28,    28,
      28,    28,    62,    63,    64,    65,    66,    28,    28,    28,
      28,    28,    -1,    -1,    -1,    -1,    76
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     3,     4,    27,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,    50,    51,    52,    53,     6,     7,     8,     9,
      11,    12,    13,    14,    15,    18,    19,    20,    21,    22,
      23,    24,    25,    26,    30,     0,    34,    34,    34,    34,
      34,    28,    34,    54,    34,    34,     3,    34,    28,     5,
      28,    28,    34,    34,    34,    34,    34,    34,    34,    28,
      34,    54,    28,    34,    34,    34,    34,    28,    34,    34,
      34,    34,    34,    28,    28,    28,    28,    28,    28,    34,
      28,    28,    28,    28,    28,    28
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    31,    32,    33,    33,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    34,    34,    34,    34,
      34,    34,    34,    34,    34,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,    50,    51,    52,    53,    54,    54
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     1,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     5,     6,     5,     5,
       5,     5,     5,     3,     4,     5,     5,     5,     4,     3,
       3,     3,     4,     5,     5,     0,     2
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
  do                                                              \
    if (yychar == YYEMPTY)                                        \
      {                                                           \
        yychar = (Token);                                         \
        yylval = (Value);                                         \
        YYPOPSTACK (yylen);                                       \
        yystate = *yyssp;                                         \
        goto yybackup;                                            \
      }                                                           \
    else                                                          \
      {                                                           \
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF

/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)                                \
    do                                                                  \
      if (N)                                                            \
        {                                                               \
          (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;        \
          (Current).first_column = YYRHSLOC (Rhs, 1).first_column;      \
          (Current).last_line    = YYRHSLOC (Rhs, N).last_line;         \
          (Current).last_column  = YYRHSLOC (Rhs, N).last_column;       \
        }                                                               \
      else                                                              \
        {                                                               \
          (Current).first_line   = (Current).last_line   =              \
            YYRHSLOC (Rhs, 0).last_line;                                \
          (Current).first_column = (Current).last_column =              \
            YYRHSLOC (Rhs, 0).last_column;                              \
        }                                                               \
    while (0)
#endif

#define YYRHSLOC(Rhs, K) ((Rhs)[K])


/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)


/* YYLOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

# ifndef YYLOCATION_PRINT

#  if defined YY_LOCATION_PRINT

   /* Temporary convenience wrapper in case some people defined the
      undocumented and private YY_LOCATION_PRINT macros.  */
#   define YYLOCATION_PRINT(File, Loc)  YY_LOCATION_PRINT(File, *(Loc))

#  elif defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL

/* Print *YYLOCP on YYO.  Private, do not rely on its existence. */

YY_ATTRIBUTE_UNUSED
static int
yy_location_print_ (FILE *yyo, YYLTYPE const * const yylocp)
{
  int res = 0;
  int end_col = 0 != yylocp->last_column ? yylocp->last_column - 1 : 0;
  if (0 <= yylocp->first_line)
    {
      res += YYFPRINTF (yyo, "%d", yylocp->first_line);
      if (0 <= yylocp->first_column)
        res += YYFPRINTF (yyo, ".%d", yylocp->first_column);
    }
  if (0 <= yylocp->last_line)
    {
      if (yylocp->first_line < yylocp->last_line)
        {
          res += YYFPRINTF (yyo, "-%d", yylocp->last_line);
          if (0 <= end_col)
            res += YYFPRINTF (yyo, ".%d", end_col);
        }
      else if (0 <= end_col && yylocp->first_column < end_col)
        res += YYFPRINTF (yyo, "-%d", end_col);
    }
  return res;
}

#   define YYLOCATION_PRINT  yy_location_print_

    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT(File, Loc)  YYLOCATION_PRINT(File, &(Loc))

#  else

#   define YYLOCATION_PRINT(File, Loc) ((void) 0)
    /* Temporary convenience wrapper in case some people defined the
       undocumented and private YY_LOCATION_PRINT macros.  */
#   define YY_LOCATION_PRINT  YYLOCATION_PRINT

#  endif
# endif /* !defined YYLOCATION_PRINT */


# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value, Location); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  YY_USE (yylocationp);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  YYLOCATION_PRINT (yyo, yylocationp);
  YYFPRINTF (yyo, ": ");
  yy_symbol_value_print (yyo, yykind, yyvaluep, yylocationp);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp, YYLTYPE *yylsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)],
                       &(yylsp[(yyi + 1) - (yynrhs)]));
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, yylsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
{
  YY_USE (yyvaluep);
  YY_USE (yylocationp);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Location data for the lookahead symbol.  */
YYLTYPE yylloc
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  = { 1, 1, 1, 1 }
# endif
;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

    /* The location stack: array, bottom, top.  */
    YYLTYPE yylsa[YYINITDEPTH];
    YYLTYPE *yyls = yylsa;
    YYLTYPE *yylsp = yyls;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[3];



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  yylsp[0] = yylloc;
  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;
        YYLTYPE *yyls1 = yyls;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yyls1, yysize * YYSIZEOF (*yylsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
        yyls = yyls1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
        YYSTACK_RELOCATE (yyls_alloc, yyls);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;


/*-----------.
| yybackup.  |
`-----------*/
yybackup:
  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      yyerror_range[1] = yylloc;
      goto yyerrlab1;
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END
  *++yylsp = yylloc;

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];

  /* Default location. */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  yyerror_range[1] = yyloc;
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
  case 2: /* START: SEXPLIST  */
#line 82 "gpp_interpreter.y"
               { }
#line 1341 "gpp_interpreter.tab.c"
    break;

  case 3: /* SEXPLIST: SEXP  */
#line 86 "gpp_interpreter.y"
           {
        printf("%d\n", (yyvsp[0].ival));
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1350 "gpp_interpreter.tab.c"
    break;

  case 4: /* SEXPLIST: SEXPLIST SEXP  */
#line 90 "gpp_interpreter.y"
                    {
        printf("%d\n", (yyvsp[0].ival));
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1359 "gpp_interpreter.tab.c"
    break;

  case 5: /* SEXP: IDENTIFIER  */
#line 97 "gpp_interpreter.y"
                 {
        int v = lookup_var((yyvsp[0].sval));
        free((yyvsp[0].sval));
        (yyval.ival) = v;
      }
#line 1369 "gpp_interpreter.tab.c"
    break;

  case 6: /* SEXP: VALUEF  */
#line 102 "gpp_interpreter.y"
             {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1377 "gpp_interpreter.tab.c"
    break;

  case 7: /* SEXP: SETEXPR  */
#line 105 "gpp_interpreter.y"
              {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1385 "gpp_interpreter.tab.c"
    break;

  case 8: /* SEXP: IFEXPR  */
#line 108 "gpp_interpreter.y"
             {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1393 "gpp_interpreter.tab.c"
    break;

  case 9: /* SEXP: PLUSEXPR  */
#line 111 "gpp_interpreter.y"
               {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1401 "gpp_interpreter.tab.c"
    break;

  case 10: /* SEXP: MINUSEXPR  */
#line 114 "gpp_interpreter.y"
                {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1409 "gpp_interpreter.tab.c"
    break;

  case 11: /* SEXP: MULTEXPR  */
#line 117 "gpp_interpreter.y"
               {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1417 "gpp_interpreter.tab.c"
    break;

  case 12: /* SEXP: DIVEXPR  */
#line 120 "gpp_interpreter.y"
              {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1425 "gpp_interpreter.tab.c"
    break;

  case 13: /* SEXP: LESSEXPR  */
#line 123 "gpp_interpreter.y"
               {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1433 "gpp_interpreter.tab.c"
    break;

  case 14: /* SEXP: EXITEXPR  */
#line 126 "gpp_interpreter.y"
               {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1441 "gpp_interpreter.tab.c"
    break;

  case 15: /* SEXP: LOADEXPR  */
#line 129 "gpp_interpreter.y"
               {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1449 "gpp_interpreter.tab.c"
    break;

  case 16: /* SEXP: EQUALLEXPR  */
#line 132 "gpp_interpreter.y"
                 {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1457 "gpp_interpreter.tab.c"
    break;

  case 17: /* SEXP: ANDEXPR  */
#line 135 "gpp_interpreter.y"
              {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1465 "gpp_interpreter.tab.c"
    break;

  case 18: /* SEXP: OREXPR  */
#line 138 "gpp_interpreter.y"
             {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1473 "gpp_interpreter.tab.c"
    break;

  case 19: /* SEXP: NOTEXPR  */
#line 141 "gpp_interpreter.y"
              {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1481 "gpp_interpreter.tab.c"
    break;

  case 20: /* SEXP: TRUEEXPR  */
#line 144 "gpp_interpreter.y"
               {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1489 "gpp_interpreter.tab.c"
    break;

  case 21: /* SEXP: FALSEEXPR  */
#line 147 "gpp_interpreter.y"
                {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1497 "gpp_interpreter.tab.c"
    break;

  case 22: /* SEXP: NILEXPR  */
#line 150 "gpp_interpreter.y"
              {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1505 "gpp_interpreter.tab.c"
    break;

  case 23: /* SEXP: LISTEXPR  */
#line 153 "gpp_interpreter.y"
               {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1513 "gpp_interpreter.tab.c"
    break;

  case 24: /* SEXP: APPENDEXPR  */
#line 156 "gpp_interpreter.y"
                 {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1521 "gpp_interpreter.tab.c"
    break;

  case 25: /* SEXP: CONCATEXPR  */
#line 159 "gpp_interpreter.y"
                 {
        (yyval.ival) = (yyvsp[0].ival);
      }
#line 1529 "gpp_interpreter.tab.c"
    break;

  case 26: /* SETEXPR: OP_OP KW_SET IDENTIFIER SEXP OP_CP  */
#line 165 "gpp_interpreter.y"
                                         {
        set_var((yyvsp[-2].sval), (yyvsp[-1].ival));
        free((yyvsp[-2].sval));
        (yyval.ival) = 0; 
      }
#line 1539 "gpp_interpreter.tab.c"
    break;

  case 27: /* IFEXPR: OP_OP KW_IF SEXP SEXP SEXP OP_CP  */
#line 173 "gpp_interpreter.y"
                                       {
        int cond = (yyvsp[-3].ival);
        if(cond) {
            (yyval.ival) = (yyvsp[-2].ival); 
        } else {
            (yyval.ival) = (yyvsp[-1].ival);
        }
      }
#line 1552 "gpp_interpreter.tab.c"
    break;

  case 28: /* PLUSEXPR: OP_OP OP_PLUS SEXP SEXP OP_CP  */
#line 184 "gpp_interpreter.y"
                                    {
        int v1 = (yyvsp[-2].ival);
        int v2 = (yyvsp[-1].ival);
        (yyval.ival) = eval_op_plus(v1,v2);
      }
#line 1562 "gpp_interpreter.tab.c"
    break;

  case 29: /* MINUSEXPR: OP_OP OP_MINUS SEXP SEXP OP_CP  */
#line 192 "gpp_interpreter.y"
                                     {
        int v1 = (yyvsp[-2].ival);
        int v2 = (yyvsp[-1].ival);
        (yyval.ival) = eval_op_minus(v1,v2);
      }
#line 1572 "gpp_interpreter.tab.c"
    break;

  case 30: /* MULTEXPR: OP_OP OP_MULT SEXP SEXP OP_CP  */
#line 200 "gpp_interpreter.y"
                                    {
        int v1 = (yyvsp[-2].ival);
        int v2 = (yyvsp[-1].ival);
        (yyval.ival) = eval_op_mult(v1,v2);
      }
#line 1582 "gpp_interpreter.tab.c"
    break;

  case 31: /* DIVEXPR: OP_OP OP_DIV SEXP SEXP OP_CP  */
#line 208 "gpp_interpreter.y"
                                   {
        int v1 = (yyvsp[-2].ival);
        int v2 = (yyvsp[-1].ival);
        (yyval.ival) = eval_op_div(v1,v2);
      }
#line 1592 "gpp_interpreter.tab.c"
    break;

  case 32: /* LESSEXPR: OP_OP OP_LT SEXP SEXP OP_CP  */
#line 216 "gpp_interpreter.y"
                                  {
        int v1 = (yyvsp[-2].ival);
        int v2 = (yyvsp[-1].ival);
        (yyval.ival) = eval_op_lt(v1,v2);
      }
#line 1602 "gpp_interpreter.tab.c"
    break;

  case 33: /* EXITEXPR: OP_OP KW_EXIT OP_CP  */
#line 224 "gpp_interpreter.y"
                          {
        printf("Exiting...\n");
        exit(0);
      }
#line 1611 "gpp_interpreter.tab.c"
    break;

  case 34: /* LOADEXPR: OP_OP KW_LOAD STRINGLIT OP_CP  */
#line 231 "gpp_interpreter.y"
                                    {
        printf("Loading file: %s\n", (yyvsp[-1].sval));
        free((yyvsp[-1].sval));
        (yyval.ival) = 0;
      }
#line 1621 "gpp_interpreter.tab.c"
    break;

  case 35: /* EQUALLEXPR: OP_OP KW_EQUAL SEXP SEXP OP_CP  */
#line 239 "gpp_interpreter.y"
                                     {
        int v1 = (yyvsp[-2].ival);
        int v2 = (yyvsp[-1].ival);
        (yyval.ival) = eval_op_equal(v1,v2);
      }
#line 1631 "gpp_interpreter.tab.c"
    break;

  case 36: /* ANDEXPR: OP_OP KW_AND SEXP SEXP OP_CP  */
#line 247 "gpp_interpreter.y"
                                   {
        int v1 = (yyvsp[-2].ival);
        int v2 = (yyvsp[-1].ival);
        (yyval.ival) = eval_op_and(v1,v2);
      }
#line 1641 "gpp_interpreter.tab.c"
    break;

  case 37: /* OREXPR: OP_OP KW_OR SEXP SEXP OP_CP  */
#line 255 "gpp_interpreter.y"
                                  {
        int v1 = (yyvsp[-2].ival);
        int v2 = (yyvsp[-1].ival);
        (yyval.ival) = eval_op_or(v1,v2);
      }
#line 1651 "gpp_interpreter.tab.c"
    break;

  case 38: /* NOTEXPR: OP_OP KW_NOT SEXP OP_CP  */
#line 263 "gpp_interpreter.y"
                              {
        int v1 = (yyvsp[-1].ival);
        (yyval.ival) = eval_op_not(v1);
      }
#line 1660 "gpp_interpreter.tab.c"
    break;

  case 39: /* TRUEEXPR: OP_OP KW_TRUE OP_CP  */
#line 270 "gpp_interpreter.y"
                          {
        (yyval.ival) = bool_val_true();
      }
#line 1668 "gpp_interpreter.tab.c"
    break;

  case 40: /* FALSEEXPR: OP_OP KW_FALSE OP_CP  */
#line 276 "gpp_interpreter.y"
                           {
        (yyval.ival) = bool_val_false();
      }
#line 1676 "gpp_interpreter.tab.c"
    break;

  case 41: /* NILEXPR: OP_OP KW_NIL OP_CP  */
#line 282 "gpp_interpreter.y"
                         {
        (yyval.ival) = bool_val_nil();
      }
#line 1684 "gpp_interpreter.tab.c"
    break;

  case 42: /* LISTEXPR: OP_OP KW_LIST SEXPSEQ OP_CP  */
#line 288 "gpp_interpreter.y"
                                  {
        (yyval.ival) = eval_list();
      }
#line 1692 "gpp_interpreter.tab.c"
    break;

  case 43: /* APPENDEXPR: OP_OP KW_APPEND SEXP SEXP OP_CP  */
#line 294 "gpp_interpreter.y"
                                      {
        (yyval.ival) = eval_append();
      }
#line 1700 "gpp_interpreter.tab.c"
    break;

  case 44: /* CONCATEXPR: OP_OP KW_CONCAT SEXP SEXP OP_CP  */
#line 300 "gpp_interpreter.y"
                                      {
        (yyval.ival) = eval_concat();
      }
#line 1708 "gpp_interpreter.tab.c"
    break;

  case 45: /* SEXPSEQ: %empty  */
#line 307 "gpp_interpreter.y"
      { (yyval.ival) = 0; }
#line 1714 "gpp_interpreter.tab.c"
    break;

  case 46: /* SEXPSEQ: SEXP SEXPSEQ  */
#line 308 "gpp_interpreter.y"
                   {
        (yyval.ival) = 0;
      }
#line 1722 "gpp_interpreter.tab.c"
    break;


#line 1726 "gpp_interpreter.tab.c"

      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;
  *++yylsp = yyloc;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
    }

  yyerror_range[1] = yylloc;
  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, &yylloc);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;

      yyerror_range[1] = *yylsp;
      yydestruct ("Error: popping",
                  YY_ACCESSING_SYMBOL (yystate), yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  yyerror_range[2] = yylloc;
  ++yylsp;
  YYLLOC_DEFAULT (*yylsp, yyerror_range, 2);

  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, &yylloc);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp, yylsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 313 "gpp_interpreter.y"


void yyerror(const char *s) {
    fprintf(stderr, "Syntax error at line %d: %s\n", yylineno, s);
}

