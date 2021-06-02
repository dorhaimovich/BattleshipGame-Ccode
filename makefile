CC = gcc
OBJS = main.o source.o colors.o
COMP_FLAG = -c
EXEC = Battleship
OUT_FLAG = -o

$(EXEC): $(OBJS)
		$(CC) $(OBJS) $(OUT_FLAG) $@

main.o: main.c header.h colors.h
		$(CC) $(COMP_FLAG) $*.c 
source.o: source.c header.h colors.h
		$(CC) $(COMP_FLAG) $*.c
colors.o: colors.c colors.h
		$(CC) $(COMP_FLAG) $*.c