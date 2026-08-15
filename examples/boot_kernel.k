import lib/system.dk
import lib/math.dk

# Direct Boot Execution Sequence in K (No main() function required)
mouse_init()

screen_write_char(0, 0, 80, 11) # 'P'
screen_write_char(1, 0, 82, 11) # 'R'
screen_write_char(2, 0, 79, 11) # 'O'
screen_write_char(3, 0, 74, 11) # 'J'
screen_write_char(4, 0, 69, 11) # 'E'
screen_write_char(5, 0, 67, 11) # 'C'
screen_write_char(6, 0, 84, 11) # 'T'
screen_write_char(7, 0, 45, 11) # '-'
screen_write_char(8, 0, 75, 14) # 'K'

speaker_emit_tone(523) # Play C5 note

v1 = 250
v2 = 250
total = add(v1, v2)

if (total == 500) {
    screen_write_char(0, 1, 66, 10) # 'B'
    screen_write_char(1, 1, 79, 10) # 'O'
    screen_write_char(2, 1, 79, 10) # 'O'
    screen_write_char(3, 1, 84, 10) # 'T'
    screen_write_char(4, 1, 32, 10) # ' '
    screen_write_char(5, 1, 79, 10) # 'O'
    screen_write_char(6, 1, 75, 10) # 'K'
}
