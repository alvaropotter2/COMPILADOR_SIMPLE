# Test de operaciones lógicas
# Archivo: test_logicas.rb

# Variables booleanas
verdadero = true
falso = false

puts "=== PRUEBAS DE OPERACIONES LOGICAS ==="

# Test AND lógico
puts "--- Test AND ---"
resultado1 = verdadero and falso
puts resultado1

resultado2 = verdadero and verdadero
puts resultado2

resultado3 = falso and falso
puts resultado3

# Test OR lógico  
puts "--- Test OR ---"
resultado4 = verdadero or falso
puts resultado4

resultado5 = falso or falso
puts resultado5

resultado6 = verdadero or verdadero
puts resultado6

# Test NOT lógico
puts "--- Test NOT ---"
resultado7 = not verdadero
puts resultado7

resultado8 = not falso
puts resultado8

# Test con expresiones relacionales
puts "--- Test con comparaciones ---"
x = 10
y = 5

resultado9 = (x > y) and (y > 0)
puts resultado9

resultado10 = (x < y) or (y == 5)
puts resultado10

resultado11 = not (x == y)
puts resultado11

# Test combinado
puts "--- Test combinado ---"
resultado12 = (x > 5) and not (y > 10)
puts resultado12

puts "=== FIN DE PRUEBAS ==="
