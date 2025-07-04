# Makefile para el compilador Ruby -> MIPS (MinGW/Windows)
CC = gcc
CFLAGS = -Wall -Wextra -std=c99
BISON = bison
FLEX = flex

# Directorios
SRC_DIR = src
PRUEBAS_DIR = pruebas
EJECUTABLE_DIR = ejecutable

# Archivos fuente con rutas
SOURCES = ruby_parser.tab.c lex.yy.c $(SRC_DIR)/tabla_simbolos.c $(SRC_DIR)/tabla_simbolos_funciones.c $(SRC_DIR)/AST_ruby.c $(SRC_DIR)/generar_mips_completo.c
OBJECTS = $(SOURCES:.c=.o)
TARGET = $(EJECUTABLE_DIR)/ruby_compiler.exe

# Archivos de prueba
TEST_FILES = $(PRUEBAS_DIR)/test.rb $(PRUEBAS_DIR)/test_control.rb $(PRUEBAS_DIR)/test_funciones.rb

# Regla principal
all: directories $(TARGET)

# Crear directorios si no existen
directories:
	mkdir -p $(EJECUTABLE_DIR)
	mkdir -p $(PRUEBAS_DIR)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

# Generar el parser con Bison
ruby_parser.tab.c ruby_parser.tab.h: $(SRC_DIR)/ruby_parser.y
	$(BISON) -d $(SRC_DIR)/ruby_parser.y

# Generar el lexer con Flex  
lex.yy.c: $(SRC_DIR)/ruby_lexer.flex ruby_parser.tab.h
	$(FLEX) $(SRC_DIR)/ruby_lexer.flex

# Compilar archivos objeto
%.o: %.c
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Regla para compilar archivos del directorio src
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(SRC_DIR) -c $< -o $@

# Limpiar archivos generados
clean:
	-rm -f *.o
	-rm -f $(SRC_DIR)/*.o
	-rm -f $(TARGET)
	-rm -f ruby_parser.tab.c
	-rm -f ruby_parser.tab.h
	-rm -f lex.yy.c
	-rm -f *.asm

# Regla para probar todos los archivos de prueba
test: $(TARGET)
	@echo "=== EJECUTANDO BATERIA DE PRUEBAS ==="
	@echo ""
	@echo "[1/3] Compilando test.rb..."
	./$(TARGET) $(PRUEBAS_DIR)/test.rb
	@echo ""
	@echo "[2/3] Compilando test_control.rb..."
	./$(TARGET) $(PRUEBAS_DIR)/test_control.rb
	@echo ""
	@echo "[3/3] Compilando test_funciones.rb..."
	./$(TARGET) $(PRUEBAS_DIR)/test_funciones.rb
	@echo ""
	@echo "=== BATERIA DE PRUEBAS COMPLETADA ==="

# Regla para generar reporte de pruebas
report: test
	@echo "=== GENERANDO REPORTE DE PRUEBAS ===" > test_results.txt
	@date >> test_results.txt
	@echo "" >> test_results.txt
	@echo "ARCHIVOS PROCESADOS:" >> test_results.txt
	@echo "- test.rb" >> test_results.txt
	@echo "- test_control.rb" >> test_results.txt  
	@echo "- test_funciones.rb" >> test_results.txt
	@echo "" >> test_results.txt
	@echo "ARCHIVOS MIPS GENERADOS:" >> test_results.txt
	@echo "- Revisar archivos .asm generados" >> test_results.txt
	@echo "" >> test_results.txt
	@echo "=== REPORTE GENERADO EN test_results.txt ==="

# Reglas para dependencias con rutas actualizadas
$(SRC_DIR)/tabla_simbolos.o: $(SRC_DIR)/tabla_simbolos.c $(SRC_DIR)/tabla_simbolos.h
$(SRC_DIR)/tabla_simbolos_funciones.o: $(SRC_DIR)/tabla_simbolos_funciones.c $(SRC_DIR)/tabla_simbolos_funciones.h $(SRC_DIR)/AST_ruby.h
$(SRC_DIR)/AST_ruby.o: $(SRC_DIR)/AST_ruby.c $(SRC_DIR)/AST_ruby.h $(SRC_DIR)/tabla_simbolos.h
$(SRC_DIR)/generar_mips_completo.o: $(SRC_DIR)/generar_mips_completo.c $(SRC_DIR)/AST_ruby.h $(SRC_DIR)/tabla_simbolos.h
ruby_parser.tab.o: ruby_parser.tab.c $(SRC_DIR)/tabla_simbolos.h $(SRC_DIR)/tabla_simbolos_funciones.h $(SRC_DIR)/AST_ruby.h
lex.yy.o: lex.yy.c ruby_parser.tab.h

.PHONY: all clean test report directories

