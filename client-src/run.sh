CFLAGS="-O2 -pedantic -g -Wall -Wextra"
CFILES="utils.c client.c ui.c"
OUTPUT="client"
LIBS="-lncurses"
DEBUG=$false

gcc -o ${OUTPUT} ${CFILES} ${CFLAGS} ${LIBS} && gdb ./${OUTPUT}

