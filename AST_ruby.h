#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

extern FILE *yyout;

/* Enumeraci\xc3\xb3n de tipos de nodo para el AST */
#define NODO_VACIO             0
#define NODO_TERMINAL          1
#define SUMA                   2
#define RESTA                  3
#define NODO_PUTS              4
#define ASIGNACION             5
#define NODO_VARIABLE          6
#define NODO_SECUENCIA         7
#define NODO_IF                8
#define NODO_WHILE             9
#define IGUAL                 10
#define MAYOR_QUE             11
#define MENOR_QUE             12
#define NODO_ELSIF            13
#define NODO_ELSE             14
#define NODO_ELSIF_INDIVIDUAL 15
#define NODO_RETURN           16
#define MULTIPLICACION        17
#define DIVISION              18
#define AND_LOGICO            19
#define OR_LOGICO             20
#define NOT_LOGICO            21
#define NODO_PARAMETRO        22
#define CONCATENACION         23
#define NEGACION              24
#define NODO_FUNCION          25
#define NODO_LLAMADA_FUNCION  26
#define NODO_ASIG_COMPUESTA   27
#define NODO_CONDICIONAL      28
#define NODO_BUCLE            29

int contadorEtiqueta = 0;
int numMaxRegistros = 31;
int nombreVariable = 0;

bool registros[32] = {[0 ... 29] = true, [30 ... 31] = true};

struct variable
{
    float dato;
    int nombre;
    bool disponible;
};

struct variable variables[64];

/*\n * Estructura b\xc3\xa1sica para representar un nodo del \n * \xc3\xa1rbol de sintaxis abstracta.\n */
struct ast
{
    int tipoNodo;              /* Tipo de nodo (NODO_*)             */
    struct ast *izq;           /* Hijo izquierdo                     */
    struct ast *dcha;          /* Hijo derecho                       */
    struct ast *siguiente;     /* Para listas de parametros/args     */
    union {
        int intVal;
        float floatVal;
        char *stringVal;
        int boolVal;
    } valor;                   /* Valor asociado al nodo             */
    char *tipo;                /* Tipo sem\xc3\xa1ntico (integer, float...) */
    int resultado;             /* Registro/resultado para MIPS       */
    int nombreVar;             /* Identificador de variable temporal */
};

int crearNombreVariable()
{
    return nombreVariable++;
}

int encontrarReg()
{
    int i = 0;
    while (i < numMaxRegistros && !registros[i])
        i++;
    if (i >= numMaxRegistros)
    {
        printf("[ERROR] No quedan registros disponibles\n");
        exit(1);
    }
    registros[i] = false;
    return i;
}

void borrarReg(struct ast *izq, struct ast *dcha)
{
    registros[izq->resultado] = true;
    registros[dcha->resultado] = true;
}

struct ast *crearNodoTerminal(double valor)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = NULL;
    n->dcha = NULL;
    n->siguiente = NULL;
    n->tipoNodo = NODO_TERMINAL;
    n->valor.floatVal = (float)valor;
    n->resultado = encontrarReg();
    n->nombreVar = crearNombreVariable();
    variables[n->resultado].dato = valor;
    variables[n->resultado].nombre = n->nombreVar;
    variables[n->resultado].disponible = true;
    return n;
}

struct ast *crearVariableTerminal(double valor, int reg)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = NULL;
    n->dcha = NULL;
    n->siguiente = NULL;
    n->tipoNodo = NODO_VARIABLE;
    n->valor.floatVal = (float)valor;
    n->resultado = reg;
    return n;
}

struct ast *crearNodoNoTerminal(struct ast *izq, struct ast *dcha, int tipoNodo)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = izq;
    n->dcha = dcha;
    n->siguiente = NULL;
    n->tipoNodo = tipoNodo;

    if (tipoNodo == SUMA || tipoNodo == RESTA || tipoNodo == NODO_PUTS ||
        tipoNodo == IGUAL || tipoNodo == MAYOR_QUE || tipoNodo == MENOR_QUE ||
        tipoNodo == MULTIPLICACION || tipoNodo == DIVISION)
    {
        n->resultado = encontrarReg();
    }
    else
    {
        n->resultado = -1;
    }

    return n;
}
struct ast *crearNodoVacio()
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = NULL;
    n->dcha = NULL;
    n->siguiente = NULL;
    n->tipoNodo = NODO_VACIO;
    n->resultado = -1;
    return n;
}

/* Funciones auxiliares para construir distintos nodos del AST */

struct ast *crearNodoUnario(struct ast *hijo, int tipo)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = hijo;
    n->dcha = NULL;
    n->siguiente = NULL;
    n->tipoNodo = tipo;
    n->resultado = -1;
    return n;
}

struct ast *crearNodoAsignacion(const char *id, struct ast *expr)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = expr;
    n->dcha = NULL;
    n->siguiente = NULL;
    n->tipoNodo = ASIGNACION;
    n->valor.stringVal = strdup(id);
    n->resultado = -1;
    return n;
}

struct ast *crearNodoAsignacionCompuesta(const char *id, struct ast *expr, int op)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = expr;
    n->dcha = NULL;
    n->siguiente = NULL;
    n->tipoNodo = NODO_ASIG_COMPUESTA;
    n->valor.stringVal = strdup(id);
    n->resultado = -1;
    n->nombreVar = op;
    return n;
}

struct ast *crearNodoPuts(struct ast *expr)
{
    return crearNodoNoTerminal(expr, NULL, NODO_PUTS);
}

struct ast *crearNodoParametro(const char *nombre)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = NULL;
    n->dcha = NULL;
    n->siguiente = NULL;
    n->tipoNodo = NODO_PARAMETRO;
    n->valor.stringVal = strdup(nombre);
    n->resultado = -1;
    return n;
}

struct ast *crearNodoFuncion(const char *nombre, struct ast *params, struct ast *cuerpo)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = params;
    n->dcha = cuerpo;
    n->siguiente = NULL;
    n->tipoNodo = NODO_FUNCION;
    n->valor.stringVal = strdup(nombre);
    n->resultado = -1;
    return n;
}

struct ast *crearNodoLlamadaFuncion(const char *nombre, struct ast *args)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = args;
    n->dcha = NULL;
    n->siguiente = NULL;
    n->tipoNodo = NODO_LLAMADA_FUNCION;
    n->valor.stringVal = strdup(nombre);
    n->resultado = -1;
    return n;
}

struct ast *crearNodoCondicional(struct ast *cond, struct ast *cuerpo, struct ast *otro)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = cond;
    n->dcha = cuerpo;
    n->siguiente = otro;
    n->tipoNodo = NODO_CONDICIONAL;
    n->resultado = -1;
    return n;
}

struct ast *crearNodoBucle(struct ast *cond, struct ast *cuerpo)
{
    struct ast *n = malloc(sizeof(struct ast));
    n->izq = cond;
    n->dcha = cuerpo;
    n->siguiente = NULL;
    n->tipoNodo = NODO_BUCLE;
    n->resultado = -1;
    return n;
}

struct ast *crearNodoVariable(const char *nombre, int reg)
{
    struct ast *n = crearVariableTerminal(0.0, reg);
    n->valor.stringVal = strdup(nombre);
    return n;
}

struct ast *crearNodoTerminalFloat(float valor)
{
    return crearNodoTerminal(valor);
}

struct ast *crearNodoTerminalString(const char *valor)
{
    struct ast *n = crearNodoVacio();
    n->tipoNodo = NODO_TERMINAL;
    n->valor.stringVal = strdup(valor);
    return n;
}

struct ast *crearNodoTerminalBool(int val)
{
    struct ast *n = crearNodoVacio();
    n->tipoNodo = NODO_TERMINAL;
    n->valor.boolVal = val;
    return n;
}

void saltoLinea()
{
    fprintf(yyout, "li $v0, 4\n");
    fprintf(yyout, "la $a0, saltoLinea\n");
    fprintf(yyout, "syscall\n");
}

void funcionImprimir(struct ast *n)
{
    int reg = n->resultado;

    if (reg < 0 || reg > 31)
    {
        fprintf(stderr, "ERROR: registro inválido para funcionImprimir: %d\n", reg);
        exit(1);
    }

    fprintf(yyout, "li $v0, 2\n");
    fprintf(yyout, "mov.s $f12, $f%d\n", reg);
    fprintf(yyout, "syscall\n");

    saltoLinea();
}

void imprimirVariables()
{
    fprintf(yyout, "\n.data\n");
    fprintf(yyout, "saltoLinea: .asciiz \"\\n\"\n");
    fprintf(yyout, "zero: .float 0.0\n");
    for (int i = 0; i < 64; i++)
    {
        if (variables[i].disponible)
        {
            fprintf(yyout, "var_%d: .float %.2f\n", variables[i].nombre, variables[i].dato);
        }
    }
}

double comprobarValorNodo(struct ast *n, int etq, bool enCondicion)
{
    double dato = 0;
    switch (n->tipoNodo)
    {
    case 0:
        printf("DEBUG: case 0 (NODO VACIO)\n");
        break;

    case 1:
        printf("DEBUG: case 1 (HOJA / TERMINAL)\n");
        printf("DEBUG: case 1 → n->resultado = %d\n", n->resultado);

        dato = n->valor;

        if (n->resultado < 0 || n->resultado > 31)
        {
            fprintf(stderr, "ERROR: registro inválido en case 1: %d\n", n->resultado);
            exit(1);
        }

        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->resultado, n->nombreVar);
        break;

    case 2:
        dato = comprobarValorNodo(n->izq, etq, false) + comprobarValorNodo(n->dcha, etq, false);

        if (n->resultado < 0 || n->resultado > 31 ||
            n->izq->resultado < 0 || n->izq->resultado > 31 ||
            n->dcha->resultado < 0 || n->dcha->resultado > 31)
        {
            fprintf(stderr, "ERROR: registro inválido en case 2 (SUMA)\n");
            exit(1);
        }

        fprintf(yyout, "add.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        
        if (n->izq->tipoNodo == 6)
        { 
            fprintf(yyout, "mov.s $f%d, $f%d\n", n->izq->resultado, n->resultado);
        }
        borrarReg(n->izq, n->dcha);
        break;

    case 3:
        dato = comprobarValorNodo(n->izq, etq, false) - comprobarValorNodo(n->dcha, etq, false);

        if (n->resultado < 0 || n->resultado > 31 ||
            n->izq->resultado < 0 || n->izq->resultado > 31 ||
            n->dcha->resultado < 0 || n->dcha->resultado > 31)
        {
            fprintf(stderr, "ERROR: registro inválido en case 3 (RESTA)\n");
            exit(1);
        }

        fprintf(yyout, "sub.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);
        borrarReg(n->izq, n->dcha);
        break;

    case 4:
        comprobarValorNodo(n->izq, etq, false);
        funcionImprimir(n->izq);
        registros[n->izq->resultado] = true;
        break;

    case 5: // ASIGNACION
    {
        printf("DEBUG: case 5 (ASIGNACION)\n");

        if (n->izq != NULL && (n->izq->tipoNodo == 1 || n->izq->tipoNodo == 2 ||
                               n->izq->tipoNodo == 3 || n->izq->tipoNodo == 4 ||
                               n->izq->tipoNodo == 5 || n->izq->tipoNodo == 6 ||
                               n->izq->tipoNodo == 7 || n->izq->tipoNodo == 8 ||
                               n->izq->tipoNodo == 9 || n->izq->tipoNodo == 10 ||
                               n->izq->tipoNodo == 11 || n->izq->tipoNodo == 12 ||
                               n->izq->tipoNodo == 17 || n->izq->tipoNodo == 18))
        {
            printf("DEBUG: case 5 → entrando en izq (tipoNodo %d)\n", n->izq->tipoNodo);
            double dato = comprobarValorNodo(n->izq, etq, false);

           
            if (n->izq->tipoNodo == 6)
            {
                fprintf(yyout, "swc1 $f%d, var_%d\n", n->izq->resultado, n->izq->nombreVar);
            }

            if (n->izq->resultado != -1)
                registros[n->izq->resultado] = true;
        }
        else
        {
            printf("DEBUG: case 5 → NO se recorre izq (tipoNodo %d)\n", n->izq != NULL ? n->izq->tipoNodo : -1);
        }
    }
    break;

    case 6:
        printf("DEBUG: case 6 (VARIABLE)\n");

        dato = n->valor;
        printf("DEBUG: case 6 → n->resultado = %d\n", n->resultado);

        if (n->resultado == -1)
        {
            printf("ERROR: VARIABLE con resultado == -1 → revisar crearVariableTerminal!\n");
            exit(1);
        }
        break;

    case 7:
        printf("DEBUG: case 7 (SENTENCIAS)\n");

        if (n->izq != NULL && (n->izq->tipoNodo == 4 ||
                               n->izq->tipoNodo == 5 ||
                               n->izq->tipoNodo == 7 ||
                               n->izq->tipoNodo == 8 ||
                               n->izq->tipoNodo == 9 ||
                               n->izq->tipoNodo == 13 ||
                               n->izq->tipoNodo == 14 ||
                               n->izq->tipoNodo == 15 ||
                               n->izq->tipoNodo == 16))
        {
            printf("DEBUG: case 7 → entrando en izq (tipoNodo %d)\n", n->izq->tipoNodo);
            comprobarValorNodo(n->izq, etq, false);
            if (n->izq->resultado != -1)
                registros[n->izq->resultado] = true;
        }

        if (n->dcha != NULL && (n->dcha->tipoNodo == 4 ||
                                n->dcha->tipoNodo == 5 ||
                                n->dcha->tipoNodo == 7 ||
                                n->dcha->tipoNodo == 8 ||
                                n->dcha->tipoNodo == 9 ||
                                n->dcha->tipoNodo == 13 ||
                                n->dcha->tipoNodo == 14 ||
                                n->dcha->tipoNodo == 15 ||
                                n->dcha->tipoNodo == 16))
        {
            printf("DEBUG: case 7 → entrando en dcha (tipoNodo %d)\n", n->dcha->tipoNodo);
            comprobarValorNodo(n->dcha, etq, false);
            if (n->dcha->resultado != -1)
                registros[n->dcha->resultado] = true;
        }
        break;

    case 8: // IF
    {
        printf("DEBUG: case 8 (IF)\n");

        int etq_else_or_elsif = contadorEtiqueta++;
        int etq_end = contadorEtiqueta++;

        // Evaluar condición del IF
        double cond = comprobarValorNodo(n->izq, etq, true);

        fprintf(yyout, "lwc1 $f30, zero\n");

        // 🔥 Forzamos carga de la variable (esto es lo que te faltaba)
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->izq->resultado, n->izq->nombreVar);

        if (n->izq->resultado < 0 || n->izq->resultado > 31)
        {
            fprintf(stderr, "ERROR: resultado inválido en IF: %d\n", n->izq->resultado);
            exit(1);
        }

        fprintf(yyout, "c.eq.s $f%d, $f30\n", n->izq->resultado);
        fprintf(yyout, "bc1f etq_%d\n", etq_else_or_elsif);
        registros[n->izq->resultado] = true;

        
        if (n->dcha != NULL && n->dcha->izq != NULL)
        {
            comprobarValorNodo(n->dcha->izq, etq, false);

            if (n->dcha->izq->resultado != -1)
                registros[n->dcha->izq->resultado] = true;
        }

        fprintf(yyout, "j etq_%d\n", etq_end);

        // ELSE o ELSIF
        fprintf(yyout, "etq_%d:\n", etq_else_or_elsif);

        if (n->dcha != NULL && n->dcha->dcha != NULL)
        {
            comprobarValorNodo(n->dcha->dcha, etq_end, false);
        }

        // Fin del IF
        fprintf(yyout, "etq_%d:\n", etq_end);
    }
    break;

    case 9: // WHILE
    {
        printf("DEBUG: case 9 (WHILE)\n");

        int etq_start = contadorEtiqueta++;
        int etq_end = contadorEtiqueta++;

        
        fprintf(yyout, "etq_%d:\n", etq_start);

        
        comprobarValorNodo(n->izq, etq, true);

        
        fprintf(yyout, "bc1f etq_%d\n", etq_end);
        registros[n->izq->resultado] = true;

        // Cuerpo del WHILE
        if (n->dcha != NULL && n->dcha->tipoNodo != 0)
        {
            printf("DEBUG: case 9 → entrando en dcha (tipoNodo %d)\n", n->dcha->tipoNodo);
            comprobarValorNodo(n->dcha, etq, false);

            
            if (n->dcha->tipoNodo == 2 && n->dcha->resultado != -1)
            {
                fprintf(yyout, "mov.s $f%d, $f%d\n", n->izq->resultado, n->dcha->resultado);
            }
            registros[n->dcha->resultado] = true;
        }

        // Volver al inicio
        fprintf(yyout, "j etq_%d\n", etq_start);

        
        fprintf(yyout, "etq_%d:\n", etq_end);
    }
    break;

    case 10:
        printf("DEBUG: case 10 (IGUALDAD ==)\n");

        comprobarValorNodo(n->izq, etq, enCondicion);
        comprobarValorNodo(n->dcha, etq, enCondicion);

        if (!enCondicion)
        {
            if (n->izq->resultado < 0 || n->izq->resultado > 31 ||
                n->dcha->resultado < 0 || n->dcha->resultado > 31)
            {
                printf("ERROR: registro inválido en comparación ==\n");
                exit(1);
            }

            fprintf(yyout, "c.eq.s $f%d, $f%d\n", n->izq->resultado, n->dcha->resultado);
            registros[n->izq->resultado] = true;
            registros[n->dcha->resultado] = true;
        }
        break;

    case 11:
        printf("DEBUG: case 11 (MAYOR >)\n");

        comprobarValorNodo(n->izq, etq, enCondicion);
        comprobarValorNodo(n->dcha, etq, enCondicion);

        if (!enCondicion)
        {
            if (n->izq->resultado < 0 || n->izq->resultado > 31 ||
                n->dcha->resultado < 0 || n->dcha->resultado > 31)
            {
                printf("ERROR: registro inválido en comparación >\n");
                exit(1);
            }

            fprintf(yyout, "c.le.s $f%d, $f%d\n", n->dcha->resultado, n->izq->resultado);
            registros[n->izq->resultado] = true;
            registros[n->dcha->resultado] = true;
        }
        break;

    case 12:
        printf("DEBUG: case 12 (MENOR <)\n");

        comprobarValorNodo(n->izq, etq, enCondicion);
        comprobarValorNodo(n->dcha, etq, enCondicion);

        
        if (n->izq->resultado < 0 || n->izq->resultado > 31 ||
            n->dcha->resultado < 0 || n->dcha->resultado > 31)
        {
            printf("ERROR: registro inválido en comparación <\n");
            exit(1);
        }

        fprintf(yyout, "c.lt.s $f%d, $f%d\n", n->izq->resultado, n->dcha->resultado);
        registros[n->izq->resultado] = true;
        registros[n->dcha->resultado] = true;
        break;

    case 13: // ELSIF
    {
        printf("DEBUG: case 13 (ELSIF)\n");

        int etq_next = contadorEtiqueta++;

        
        double cond = comprobarValorNodo(n->izq, etq, true);

        fprintf(yyout, "lwc1 $f30, zero\n");

        if (n->izq->resultado < 0 || n->izq->resultado > 31)
        {
            fprintf(stderr, "ERROR: resultado inválido en ELSIF: %d\n", n->izq->resultado);
            exit(1);
        }

        fprintf(yyout, "c.eq.s $f%d, $f30\n", n->izq->resultado);
        fprintf(yyout, "bc1f etq_%d\n", etq_next);
        registros[n->izq->resultado] = true;

        
        if (n->dcha != NULL && n->dcha->izq != NULL)
        {
            comprobarValorNodo(n->dcha->izq, etq, false);

            if (n->dcha->izq->resultado != -1)
                registros[n->dcha->izq->resultado] = true;
        }

        fprintf(yyout, "j etq_%d\n", etq);

        // Siguiente rama (otro ELSIF o ELSE)
        fprintf(yyout, "etq_%d:\n", etq_next);

        if (n->dcha != NULL && n->dcha->dcha != NULL)
        {
            comprobarValorNodo(n->dcha->dcha, etq, false);
        }
    }
    break;

    case 14: // ELSE
    {
        printf("DEBUG: case 14 (ELSE)\n");

        
        if (n->izq != NULL && n->izq->tipoNodo != 0)
        {
            printf("DEBUG: case 14 → entrando en izq (tipoNodo %d)\n", n->izq->tipoNodo);

            comprobarValorNodo(n->izq, etq, false);

            if (n->izq->resultado != -1)
                registros[n->izq->resultado] = true;
        }
        else
        {
            printf("DEBUG: case 14 → izq es NULL o vacío\n");
        }
    }
    break;

    case 15:
    {
        printf("DEBUG: case 15 (ELSIF individual)\n");

        int etq_next = contadorEtiqueta++;

        // Condición del ELSIF individual
        if (n->izq != NULL && n->izq->tipoNodo != 0)
        {
            comprobarValorNodo(n->izq, etq, true);
            if (n->izq->resultado != -1)
                registros[n->izq->resultado] = true;
        }

        fprintf(yyout, "bc1f etq_%d\n", etq_next);

        
        if (n->dcha != NULL && n->dcha->izq != NULL &&
            (n->dcha->izq->tipoNodo == 2 ||
             n->dcha->izq->tipoNodo == 3 ||
             n->dcha->izq->tipoNodo == 6))
        {
            comprobarValorNodo(n->dcha->izq, etq, false);

            int regImp = n->dcha->izq->resultado;
            if (regImp < 0 || regImp > 31)
            {
                fprintf(stderr, "ERROR: registro inválido impresión ELSIF individual: %d\n", regImp);
                exit(1);
            }

            fprintf(yyout, "li $v0, 2\n");
            fprintf(yyout, "mov.s $f12, $f%d\n", regImp);
            fprintf(yyout, "syscall\n");
            saltoLinea();
        }
        else
        {
            printf("DEBUG: ELSIF individual → NO imprime (nodo vacío o no imprimible)\n");
        }

        fprintf(yyout, "j etq_%d\n", etq);

        fprintf(yyout, "etq_%d:\n", etq_next);

        // Ramas siguientes
        if (n->dcha != NULL)
        {
            struct ast *ramas = n->dcha;
            if (ramas->tipoNodo == 13)
            {
                comprobarValorNodo(ramas, etq, false);
            }
            else if (ramas->tipoNodo == 14)
            {
                comprobarValorNodo(ramas, etq, false);
            }
        }
    }
    break;

    case 16:
        comprobarValorNodo(n->izq, etq, false);
        break;
    case 17: // MULTIPLICACION
        printf("DEBUG: case 17 (MULTIPLICACION)\n");
        comprobarValorNodo(n->izq, etq, false);
        comprobarValorNodo(n->dcha, etq, false);

        if (n->resultado < 0 || n->resultado > 31 ||
            n->izq->resultado < 0 || n->izq->resultado > 31 ||
            n->dcha->resultado < 0 || n->dcha->resultado > 31)
        {
            fprintf(stderr, "ERROR: registro inválido en case 17 (MULTIPLICACION)\n");
            exit(1);
        }

        
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->izq->resultado, n->izq->nombreVar);
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->dcha->resultado, n->dcha->nombreVar);

        
        fprintf(yyout, "mul.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);

        
        fprintf(yyout, "swc1 $f%d, var_%d\n", n->resultado, n->nombreVar);

        borrarReg(n->izq, n->dcha);
        break;

    case 18: // DIVISION
        printf("DEBUG: case 18 (DIVISION)\n");
        comprobarValorNodo(n->izq, etq, false);
        comprobarValorNodo(n->dcha, etq, false);

        if (n->resultado < 0 || n->resultado > 31 ||
            n->izq->resultado < 0 || n->izq->resultado > 31 ||
            n->dcha->resultado < 0 || n->dcha->resultado > 31)
        {
            fprintf(stderr, "ERROR: registro inválido en case 18 (DIVISION)\n");
            exit(1);
        }

        
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->izq->resultado, n->izq->nombreVar);
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->dcha->resultado, n->dcha->nombreVar);

        // Realizar la división
        fprintf(yyout, "div.s $f%d, $f%d, $f%d\n", n->resultado, n->izq->resultado, n->dcha->resultado);

        // Guardar el resultado
        fprintf(yyout, "swc1 $f%d, var_%d\n", n->resultado, n->nombreVar);

        borrarReg(n->izq, n->dcha);
        break;

    case 19: // AND
        printf("DEBUG: case 19 (AND)\n");
        
        n->resultado = encontrarReg();
        
        n->nombreVar = 2; 
        variables[n->resultado].dato = 0.0;
        variables[n->resultado].nombre = n->nombreVar;
        variables[n->resultado].disponible = true;

        comprobarValorNodo(n->izq, etq, true);
        comprobarValorNodo(n->dcha, etq, true);

        if (n->resultado < 0 || n->resultado > 31 ||
            n->izq->resultado < 0 || n->izq->resultado > 31 ||
            n->dcha->resultado < 0 || n->dcha->resultado > 31)
        {
            fprintf(stderr, "ERROR: registro inválido en operación AND\n");
            exit(1);
        }

        // Cargar los valores en los registros
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->izq->resultado, n->izq->nombreVar);
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->dcha->resultado, n->dcha->nombreVar);

        
        fprintf(yyout, "lwc1 $f31, zero\n");                      
        fprintf(yyout, "c.lt.s $f31, $f%d\n", n->izq->resultado); 
        fprintf(yyout, "bc1f etq_and_false_%d\n", contadorEtiqueta);

        
        fprintf(yyout, "c.lt.s $f31, $f%d\n", n->dcha->resultado); 
        fprintf(yyout, "bc1f etq_and_false_%d\n", contadorEtiqueta);

        
        fprintf(yyout, "li $t0, 1\n");
        fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
        fprintf(yyout, "cvt.s.w $f%d, $f%d\n", n->resultado, n->resultado);
        fprintf(yyout, "j etq_and_end_%d\n", contadorEtiqueta);

        fprintf(yyout, "etq_and_false_%d:\n", contadorEtiqueta);
        
        fprintf(yyout, "lwc1 $f%d, zero\n", n->resultado);

        fprintf(yyout, "etq_and_end_%d:\n", contadorEtiqueta);

        // Guardar el resultado en var_2 (que ya existe)
        fprintf(yyout, "swc1 $f%d, var_%d\n", n->resultado, n->nombreVar);

	// Incrementar el contador de etiquetas
        contadorEtiqueta++;

        borrarReg(n->izq, n->dcha);
        break;
case 20: // OR
        printf("DEBUG: case 20 (OR)\n");
       
        n->resultado = encontrarReg();
        n->nombreVar = crearNombreVariable();
        variables[n->resultado].dato = 0.0;
        variables[n->resultado].nombre = n->nombreVar;
        variables[n->resultado].disponible = true;

        comprobarValorNodo(n->izq, etq, true);
        comprobarValorNodo(n->dcha, etq, true);

        if (n->resultado < 0 || n->resultado > 31 ||
            n->izq->resultado < 0 || n->izq->resultado > 31 ||
            n->dcha->resultado < 0 || n->dcha->resultado > 31)
        {
            fprintf(stderr, "ERROR: registro inválido en operación OR\n");
            exit(1);
        }

       
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->izq->resultado, n->izq->nombreVar);
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->dcha->resultado, n->dcha->nombreVar);

       
        fprintf(yyout, "c.eq.s $f%d, $f31\n", n->izq->resultado);
        fprintf(yyout, "bc1f etq_or_true_%d\n", contadorEtiqueta);
        fprintf(yyout, "c.eq.s $f%d, $f31\n", n->dcha->resultado);
        fprintf(yyout, "bc1f etq_or_true_%d\n", contadorEtiqueta);

       
        fprintf(yyout, "lwc1 $f%d, zero\n", n->resultado);
        fprintf(yyout, "j etq_or_end_%d\n", contadorEtiqueta);

        fprintf(yyout, "etq_or_true_%d:\n", contadorEtiqueta);
       
        fprintf(yyout, "li $t0, 1\n");
        fprintf(yyout, "mtc1 $t0, $f%d\n", n->resultado);
        fprintf(yyout, "cvt.s.w $f%d, $f%d\n", n->resultado, n->resultado);

        fprintf(yyout, "etq_or_end_%d:\n", contadorEtiqueta);
        fprintf(yyout, "swc1 $f%d, var_%d\n", n->resultado, n->nombreVar);

       
        contadorEtiqueta++;

        borrarReg

case 21: // NOT
        printf("DEBUG: case 21 (NOT)\n");
        comprobarValorNodo(n->izq, etq, true);

        if (n->izq->resultado < 0 || n->izq->resultado > 31)
        {
            fprintf(stderr, "ERROR: registro inválido en operación NOT\n");
            exit(1);
        }

       
        fprintf(yyout, "lwc1 $f%d, var_%d\n", n->izq->resultado, n->izq->nombreVar);

       
        fprintf(yyout, "c.eq.s $f%d, $f31\n", n->izq->resultado);
        fprintf(yyout, "bc1t etq_not_false\n");
        fprintf(yyout, "li.s $f%d, 0.0\n", n->resultado);
        fprintf(yyout, "j etq_not_end\n");
        fprintf(yyout, "etq_not_false:\n");
        fprintf(yyout, "li.s $f%d, 1.0\n", n->resultado);
        fprintf(yyout, "etq_not_end:\n");

        registros[n->izq->resultado] = true;
        break;
    }
    return dato;
}

void comprobarAST(struct ast *n)
{
    imprimirVariables();
    fprintf(yyout, "\n.text\n");
    fprintf(yyout, "lwc1 $f31, zero\n");
    comprobarValorNodo(n, contadorEtiqueta, false);
}
