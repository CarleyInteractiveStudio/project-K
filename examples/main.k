import matematica.dk
import vga.dk

# Main Kernel Entry Point in .k
fnc main() {
    # Test imported math function from matematica.dk
    res = sumar(10, 20)

    # Write character 'K' (ASCII 75, 0x4B) with light green color (0x0A) to VGA screen at top-left
    vga_print_char(0, 0, 75, 10)

    # Write character 'O' (ASCII 79) next to it
    vga_print_char(1, 0, 79, 10)

    # Output character 'K' to serial UART port 0x3F8
    uart_putc(75)

    if (res == 30) {
        # '!' symbol if sum calculation is correct
        vga_print_char(2, 0, 33, 12)
    }

    return 0
}
