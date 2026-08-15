# Kernel Demonstration using Umbrella Drivers
import lib/mat.dk
import lib/sistema.dk

# Direct Kernel Statements
raton_inicializar()

# Calculate using imported math module from lib/mat.dk
calculo = sumar(100, 200)

# Write result notification on VGA screen using imported pantalla driver
pantalla_escribir(0, 0, 75, 10) # 'K'
pantalla_escribir(1, 0, 69, 10) # 'E'
pantalla_escribir(2, 0, 82, 10) # 'R'
pantalla_escribir(3, 0, 78, 10) # 'N'
pantalla_escribir(4, 0, 69, 10) # 'E'
pantalla_escribir(5, 0, 76, 10) # 'L'

if (calculo == 300) {
    # Send beep sound frequency using PC speaker
    sonido_emitir_tono(440)
}

fnc main() {
    return 0
}
