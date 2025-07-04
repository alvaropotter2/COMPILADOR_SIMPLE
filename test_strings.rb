# Test para concatenación de cadenas
nombre = "Juan"
apellido = "Perez"
saludo = "Hola"

# Concatenación básica
nombre_completo = nombre + " " + apellido
mensaje = saludo + " " + nombre_completo

puts nombre_completo
puts mensaje

# Concatenación múltiple
prefijo = "Sr. "
titulo = prefijo + nombre + " " + apellido + " - Bienvenido"
puts titulo

# Test con strings literales
despedida = "Hasta luego, " + nombre + "!"
puts despedida

# Concatenación en expresiones
if nombre == "Juan"
    bienvenida = "Hola " + nombre + ", eres el usuario correcto"
    puts bienvenida
else
    error = "Usuario " + nombre + " no reconocido"
    puts error
end
