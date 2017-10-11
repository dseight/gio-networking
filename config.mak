AR := ar
AS := gcc
CC := gcc

INCLUDES := `pkg-config --cflags glib-2.0 gio-2.0`
LIBS := `pkg-config --libs glib-2.0 gio-2.0`

CFLAGS := -O2 -g -std=c99 -Wall -Wextra -Wno-unused-parameter\
	-Wstrict-prototypes -Wredundant-decls -Wundef -Wwrite-strings \
	-Wmissing-prototypes -fno-strict-aliasing -fno-common -Wendif-labels \
	-Wmissing-include-dirs -Wempty-body -Wnested-externs -Wformat-security \
	-Wformat-y2k -Winit-self -Wignored-qualifiers -Wtype-limits \
	-Wno-missing-field-initializers
