import lib/system.dk

# 1. Fill background with green (0x2F = green background, bright white text)
screen_fill_color(47)

# 2. Draw UI window at position (x=15, y=5) with width=50, height=12
# title_color = 31 (blue background), body_color = 112 (gray background), border_color = 120
ui_draw_window(15, 5, 50, 12, 31, 112, 120)

# Window Title: " K-OS WINDOW SYSTEM "
screen_write_char(28, 6, 75, 31)  # K
screen_write_char(29, 6, 45, 31)  # -
screen_write_char(30, 6, 79, 31)  # O
screen_write_char(31, 6, 83, 31)  # S
screen_write_char(32, 6, 32, 31)  #
screen_write_char(33, 6, 87, 31)  # W
screen_write_char(34, 6, 73, 31)  # I
screen_write_char(35, 6, 78, 31)  # N
screen_write_char(36, 6, 68, 31)  # D
screen_write_char(37, 6, 79, 31)  # O
screen_write_char(38, 6, 87, 31)  # W

# Window Body Text: "Kernel arranco con exito!"
screen_write_char(20, 9, 75, 112)  # K
screen_write_char(21, 9, 101, 112) # e
screen_write_char(22, 9, 114, 112) # r
screen_write_char(23, 9, 110, 112) # n
screen_write_char(24, 9, 101, 112) # e
screen_write_char(25, 9, 108, 112) # l
screen_write_char(26, 9, 32, 112)  #
screen_write_char(27, 9, 97, 112)  # a
screen_write_char(28, 9, 114, 112) # r
screen_write_char(29, 9, 114, 112) # r
screen_write_char(30, 9, 97, 112)  # a
screen_write_char(31, 9, 110, 112) # n
screen_write_char(32, 9, 99, 112)  # c
screen_write_char(33, 9, 111, 112) # o

# Prompt Text: "Input: [ System Ready ]"
screen_write_char(20, 12, 73, 112)  # I
screen_write_char(21, 12, 110, 112) # n
screen_write_char(22, 12, 112, 112) # p
screen_write_char(23, 12, 117, 112) # u
screen_write_char(24, 12, 116, 112) # t
screen_write_char(25, 12, 58, 112)  # :
screen_write_char(26, 12, 32, 112)  #
screen_write_char(27, 12, 91, 112)  # [
screen_write_char(28, 12, 32, 112)  #
screen_write_char(29, 12, 82, 112)  # R
screen_write_char(30, 12, 101, 112) # e
screen_write_char(31, 12, 97, 112)  # a
screen_write_char(32, 12, 100, 112) # d
screen_write_char(33, 12, 121, 112) # y
screen_write_char(34, 12, 32, 112)  #
screen_write_char(35, 12, 93, 112)  # ]

# Emit startup audio chime
speaker_emit_tone(523)
