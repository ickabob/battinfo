NAME = battinfo 
VERSION = 0.1.1

#
# Customize below to fit your system
# 

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# includes and libs
INCS = -I. -I/usr/include 
LIBS = -L/usr/lib -lc 

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\"
#CFLAGS = -g -std=c11 -pedantic -Wall -O0 ${INCS} ${CPPFLAGS}
CFLAGS = -std=c11 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
#LDFLAGS = -g ${LIBS}
LDFLAGS = -s ${LIBS}

# compiler and linker
CC = cc

SRC = batstat.c smprintf.c  


