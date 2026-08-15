import lib/system.dk

# Direct Boot Execution Sequence in K
# Display message: "el kernel arranco con el proyecto k"

# e (101), l (108)
screen_write_char(0, 0, 101, 14) # 'e'
screen_write_char(1, 0, 108, 14) # 'l'
screen_write_char(2, 0, 32, 14)  # ' '

# k (107), e (101), r (114), n (110), e (101), l (108)
screen_write_char(3, 0, 107, 10) # 'k'
screen_write_char(4, 0, 101, 10) # 'e'
screen_write_char(5, 0, 114, 10) # 'r'
screen_write_char(6, 0, 110, 10) # 'n'
screen_write_char(7, 0, 101, 10) # 'e'
screen_write_char(8, 0, 108, 10) # 'l'
screen_write_char(9, 0, 32, 14)  # ' '

# a (97), r (114), r (114), a (97), n (110), c (99), o (111)
screen_write_char(10, 0, 97, 14)  # 'a'
screen_write_char(11, 0, 114, 14) # 'r'
screen_write_char(12, 0, 114, 14) # 'r'
screen_write_char(13, 0, 97, 14)  # 'a'
screen_write_char(14, 0, 110, 14) # 'n'
screen_write_char(15, 0, 99, 14)  # 'c'
screen_write_char(16, 0, 111, 14) # 'o'
screen_write_char(17, 0, 32, 14)  # ' '

# c (99), o (111), n (110)
screen_write_char(18, 0, 99, 14)  # 'c'
screen_write_char(19, 0, 111, 14) # 'o'
screen_write_char(20, 0, 110, 14) # 'n'
screen_write_char(21, 0, 32, 14)  # ' '

# e (101), l (108)
screen_write_char(22, 0, 101, 14) # 'e'
screen_write_char(23, 0, 108, 14) # 'l'
screen_write_char(24, 0, 32, 14)  # ' '

# p (112), r (114), o (111), y (121), e (101), c (99), t (116), o (111)
screen_write_char(25, 0, 112, 11) # 'p'
screen_write_char(26, 0, 114, 11) # 'r'
screen_write_char(27, 0, 111, 11) # 'o'
screen_write_char(28, 0, 121, 11) # 'y'
screen_write_char(29, 0, 101, 11) # 'e'
screen_write_char(30, 0, 99, 11) # 'c'
screen_write_char(31, 0, 116, 11) # 't'
screen_write_char(32, 0, 111, 11) # 'o'
screen_write_char(33, 0, 32, 14)  # ' '

# k (107)
screen_write_char(34, 0, 107, 14) # 'k'

# Sound beep notification on boot
speaker_emit_tone(440)
