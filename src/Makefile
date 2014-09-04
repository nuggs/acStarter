CC = gcc
RM = rm
EXE = acStarter
PROF = -O -ggdb

C_FLAGS = $(PROF) -Wall -Wstrict-prototypes -Wpointer-arith -Wno-char-subscripts -Winline
L_FLAGS = $(PROF)

C_FILES := core.c config.c io.c parson.c

O_DIR = obj
O_FILES := $(patsubst %.c,$(O_DIR)/%.o,$(C_FILES))

server: $(O_FILES)
	$(RM) -f $(EXE)
	$(CC) -o $(EXE) $(O_FILES) $(L_FLAGS)

$(O_DIR)/%.o: %.c
	$(CC) $(C_FLAGS) -c -o $@ $<

clean:
	$(RM) -f $(O_FILES) $(EXE)
