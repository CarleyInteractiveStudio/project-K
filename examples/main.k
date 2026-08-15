import lib/math.dk
import lib/system.dk

# Direct execution entry point
screen_write_char(0, 0, 75, 10) # Print 'K' in light green
screen_write_char(1, 0, 79, 10) # Print 'O' in light green

fnc main() {
    res = add(15, 25)
    if (res == 40) {
        screen_write_char(2, 0, 33, 12) # Print '!'
    }
    return 0
}
