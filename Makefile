IDIR = include
ODIR = obj
SDIR = src
BIN_DIR = bin

CC = gcc
CFLAGS = -std=c99 -O3 -Wall -Wpedantic -I$(IDIR)

_DEPS =
_OBJ = crack.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

crack: $(OBJ)
	$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS)

all: mkdirs crack

.PHONY: clean

clean:
	rm -rf $(BIN_DIR) $(ODIR)

mkdirs:
	mkdir -p $(BIN_DIR) $(ODIR)
