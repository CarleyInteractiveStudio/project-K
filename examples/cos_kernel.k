import lib/system.dk

# Carley Operating System (COS) - CK Kernel Entry Point written in K Language

# 1. Fill Desktop background (Teal / Cyan blue background: attr 0x3F)
screen_fill_color(63)

# 2. Draw Top macOS-style Menu Bar (Row 0)
i = 0
while (i < 80) {
    screen_write_char(i, 0, 32, 112) # Gray bar
    i = i + 1
}

# Apple / COS Icon (Row 0, Col 1)
screen_write_char(1, 0, 240, 112) # ≡ / Logo
screen_write_char(3, 0, 67, 112)  # C
screen_write_char(4, 0, 79, 112)  # O
screen_write_char(5, 0, 83, 112)  # S

# Menu options: "File", "Edit", "View"
screen_write_char(10, 0, 70, 112) # F
screen_write_char(11, 0, 105, 112) # i
screen_write_char(12, 0, 108, 112) # l
screen_write_char(13, 0, 101, 112) # e

screen_write_char(16, 0, 69, 112) # E
screen_write_char(17, 0, 100, 112) # d
screen_write_char(18, 0, 105, 112) # i
screen_write_char(19, 0, 116, 112) # t

# Right side Clock / Status: "CK 1.0"
screen_write_char(73, 0, 67, 112) # C
screen_write_char(74, 0, 75, 112) # K
screen_write_char(75, 0, 32, 112) #
screen_write_char(76, 0, 49, 112) # 1
screen_write_char(77, 0, 46, 112) # .
screen_write_char(78, 0, 48, 112) # 0

# 3. Draw Main Desktop Window Container (x=10, y=3, w=60, h=16)
ui_draw_window(10, 3, 60, 16, 31, 112, 120)

# Window Title: " Carley Operating System - CK Kernel "
screen_write_char(22, 4, 67, 31)  # C
screen_write_char(23, 4, 97, 31)  # a
screen_write_char(24, 4, 114, 31) # r
screen_write_char(25, 4, 108, 31) # l
screen_write_char(26, 4, 101, 31) # e
screen_write_char(27, 4, 121, 31) # y
screen_write_char(28, 4, 32, 31)  #
screen_write_char(29, 4, 79, 31)  # O
screen_write_char(30, 4, 83, 31)  # S

# Window Inner Text
screen_write_char(14, 7, 87, 112)  # W
screen_write_char(15, 7, 101, 112) # e
screen_write_char(16, 7, 108, 112) # l
screen_write_char(17, 7, 99, 112)  # c
screen_write_char(18, 7, 111, 112) # o
screen_write_char(19, 7, 109, 112) # m
screen_write_char(20, 7, 101, 112) # e
screen_write_char(21, 7, 32, 112)  #
screen_write_char(22, 7, 116, 112) # t
screen_write_char(23, 7, 111, 112) # o
screen_write_char(24, 7, 32, 112)  #
screen_write_char(25, 7, 67, 112)  # C
screen_write_char(26, 7, 79, 112)  # O
screen_write_char(27, 7, 83, 112)  # S

# 4. Draw Bottom macOS Dock Container (Row 22 - 24, Col 20 - 60)
d = 20
while (d < 60) {
    screen_write_char(d, 22, 32, 120)
    screen_write_char(d, 23, 32, 120)
    d = d + 1
}

# Dock App Icons: [Finder] [Terminal] [Settings]
screen_write_char(24, 23, 254, 113) # Icon 1
screen_write_char(34, 23, 227, 114) # Icon 2 (Terminal)
screen_write_char(44, 23, 15, 115)  # Icon 3 (Settings)
screen_write_char(54, 23, 14, 116)  # Icon 4 (Music)

# Play macOS Startup Audio Sound Sequence
speaker_emit_tone(523) # C5
