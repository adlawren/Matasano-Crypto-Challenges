IDIR =inc
CC=g++
CFLAGS=-std=c++0x -Wall -I$(IDIR)

ODIR=obj
LDIR =../lib

LIBS=-lm -lssl -lcrypto

_DEPS = ByteSequence.hpp CharacterFrequencyScoreCalculator.hpp debug.hpp Decryptor.hpp Encryptor.hpp
DEPS = $(patsubst %, $(IDIR)/%, $(_DEPS))

$(ODIR)/%.o: src/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

_CHALLENGE_1_OBJ = challenge1.o
CHALLENGE_1_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_1_OBJ))

_CHALLENGE_2_OBJ = challenge2.o
CHALLENGE_2_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_2_OBJ))

_CHALLENGE_3_OBJ = challenge3.o
CHALLENGE_3_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_3_OBJ))

_CHALLENGE_4_OBJ = challenge4.o
CHALLENGE_4_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_4_OBJ))

_CHALLENGE_5_OBJ = challenge5.o
CHALLENGE_5_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_5_OBJ))

_CHALLENGE_6_OBJ = challenge6.o
CHALLENGE_6_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_6_OBJ))

_CHALLENGE_7_OBJ = challenge7.o
CHALLENGE_7_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_7_OBJ))

all: challenge1 challenge2 challenge3 challenge4 challenge5 challenge6 challenge7

challenge1: $(CHALLENGE_1_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge2: $(CHALLENGE_2_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge3: $(CHALLENGE_3_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge4: $(CHALLENGE_4_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge5: $(CHALLENGE_5_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge6: $(CHALLENGE_6_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge7: $(CHALLENGE_7_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	find $(ODIR) -type f -name '*.o' -exec rm {} + # Source: http://unix.stackexchange.com/questions/116389/recursively-delete-all-files-with-a-given-extension/116390#116390
	find . -maxdepth 1 -type f -executable -exec rm {} +
