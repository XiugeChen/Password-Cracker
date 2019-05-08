IDIR = include
ODIR = obj
SDIR = src
BIN_DIR = bin

CC = gcc
CFLAGS = -std=c99 -O3 -Wall -Wpedantic -I$(IDIR)

_DEPS = pwd_crack.h sha256.h
_OBJ = crack.o pwd_crack.o sha256.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

crack: $(OBJ)
	$(CC) -o $(BIN_DIR)/$@ $^ $(CFLAGS)

dh: $(SDIR)/dh.c
	$(CC) -o $(BIN_DIR)/dh $(SDIR)/dh.c $(CFLAGS)

all: mkdirs crack dh

.PHONY: clean

clean:
	rm -rf $(BIN_DIR) $(ODIR)

mkdirs:
	mkdir -p $(BIN_DIR) $(ODIR)
