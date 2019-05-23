IDIR = include
ODIR = obj
SDIR = src

CC = gcc
CFLAGS = -std=c99 -O3 -Wall -Wpedantic -I $(IDIR)

_DEPS = pwd_crack.h hash_manage.h sha256.h common_sub.h error_check.h array_manage.h
_OBJ = crack.o pwd_crack.o hash_manage.o sha256.o common_sub.o error_check.o array_manage.o

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

all: mkdirs crack dh

crack: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

dh: $(SDIR)/dh.c
	$(CC) -o dh $(SDIR)/dh.c $(CFLAGS)

.PHONY: clean

clean:
	rm -rf $(ODIR) crack dh

mkdirs:
	mkdir -p $(ODIR)
