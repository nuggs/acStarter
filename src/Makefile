CC = gcc
RM = rm
EXE = acStarter
PROF = #-O3 -ggdb
C89 = #-ansi -pedantic

C_FLAGS = $(PROF) $(INCC) $(C89) -Wall -g
L_FLAGS = $(PROF) $(INCL) -lm

C_FILES := core.c config.c io.c list.c stack.c callbacks.c event.c cars.c tracks.c parson.c random.c

O_DIR = obj
O_FILES := $(patsubst %.c,$(O_DIR)/%.o,$(C_FILES))

server: $(O_FILES)
	$(RM) -f $(EXE)
	$(CC) -o $(EXE) $(O_FILES) $(L_FLAGS)

$(O_DIR)/%.o: %.c
	$(CC) $(C_FLAGS) -c -o $@ $<

clean:
	$(RM) -f $(O_FILES) $(EXE)
