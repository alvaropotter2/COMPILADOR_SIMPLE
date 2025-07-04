/* A Bison parser, made by GNU Bison 2.4.2.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989-1990, 2000-2006, 2009-2010 Free Software
   Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     PLUS = 258,
     MINUS = 259,
     MULTIPLY = 260,
     DIVIDE = 261,
     ASSIGN = 262,
     PLUS_ASSIGN = 263,
     MINUS_ASSIGN = 264,
     EQUAL = 265,
     NOT_EQUAL = 266,
     LESS = 267,
     GREATER = 268,
     LESS_EQUAL = 269,
     GREATER_EQUAL = 270,
     LPAREN = 271,
     RPAREN = 272,
     LBRACE = 273,
     RBRACE = 274,
     LBRACKET = 275,
     COMMA = 276,
     IF = 277,
     ELSE = 278,
     ELSIF = 279,
     END = 280,
     WHILE = 281,
     FOR = 282,
     DO = 283,
     PUTS = 284,
     DEF = 285,
     RETURN = 286,
     TRUE = 287,
     FALSE = 288,
     AND = 289,
     OR = 290,
     NOT = 291,
     NEWLINE = 292,
     INTEGER = 293,
     FLOAT = 294,
     STRING = 295,
     IDENTIFIER = 296,
     COMMENT = 297,
     UMINUS = 298
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 42 "src/ruby_parser.y"

  int intVal;
  float floatVal;
  char* stringVal;
  int boolVal;
  struct atributos{
    int integer;
    float floatDecimal;
    char* string;
    int boolean;
    char* tipo;             //Define el tipo que se esta usando
    struct ast *n;          //Para almacenar los nodos del AST
  }tr;



/* Line 1685 of yacc.c  */
#line 111 "ruby_parser.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


