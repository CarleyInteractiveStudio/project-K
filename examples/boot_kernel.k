import lib/sistema.dk
import lib/mat.dk

# Autonomous Direct Boot Execution Sequence in K
pantalla_escribir(0, 0, 80, 11) # 'P'
pantalla_escribir(1, 0, 82, 11) # 'R'
pantalla_escribir(2, 0, 79, 11) # 'O'
pantalla_escribir(3, 0, 74, 11) # 'J'
pantalla_escribir(4, 0, 69, 11) # 'E'
pantalla_escribir(5, 0, 67, 11) # 'C'
pantalla_escribir(6, 0, 84, 11) # 'T'
pantalla_escribir(7, 0, 45, 11) # '-'
pantalla_escribir(8, 0, 75, 14) # 'K'

# Sound startup chime
sonido_emitir_tono(523) # C5 note

fnc main() {
    val1 = 500
    val2 = 500
    total = sumar(val1, val2)

    if (total == 1000) {
        pantalla_escribir(0, 1, 66, 10) # 'B'
        pantalla_escribir(1, 1, 79, 10) # 'O'
        pantalla_escribir(2, 1, 79, 10) # 'O'
        pantalla_escribir(3, 1, 84, 10) # 'T'
        pantalla_escribir(4, 1, 32, 10) # ' '
        pantalla_escribir(5, 1, 79, 10) # 'O'
        pantalla_escribir(6, 1, 75, 10) # 'K'
    }

    return 0
}
