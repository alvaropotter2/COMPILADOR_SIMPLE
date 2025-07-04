# Archivo de prueba para funciones

def suma(a, b)
    return a + b
end

def factorial(n)
    if n <= 1
        return 1
    else
        return n * factorial(n - 1)
    end
end

# Llamadas a funciones
resultado1 = suma(5, 3)
puts resultado1

resultado2 = factorial(4)
puts resultado2
