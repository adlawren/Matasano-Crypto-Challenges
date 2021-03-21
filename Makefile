IDIR = inc
CC = g++
CFLAGS = -g -std=c++0x -Wall -I$(IDIR)

ODIR = obj
LDIR = ../lib

LIBS = -lm -lssl -lcrypto

_DEPS = BoundedBuffer.hpp ByteSequence.hpp Cracker.hpp debug.hpp Decryptor.hpp Encryptor.hpp EnglishBigramRelativeFrequencyProvider.hpp EnglishCharacterDistributionScoreCalculator.hpp EnglishMonogramRelativeFrequencyProvider.hpp EnglishTrigramRelativeFrequencyProvider.hpp LittleEndian8ByteCounterByteSequence.hpp MT19937Cloner.hpp MT19937RandomNumberGenerator.hpp Oracle.hpp
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

_CHALLENGE_8_OBJ = challenge8.o
CHALLENGE_8_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_8_OBJ))

_CHALLENGE_9_OBJ = challenge9.o
CHALLENGE_9_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_9_OBJ))

_CHALLENGE_10_OBJ = challenge10.o
CHALLENGE_10_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_10_OBJ))

_CHALLENGE_11_OBJ = challenge11.o
CHALLENGE_11_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_11_OBJ))

_CHALLENGE_12_OBJ = challenge12.o
CHALLENGE_12_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_12_OBJ))

_CHALLENGE_13_OBJ = challenge13.o
CHALLENGE_13_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_13_OBJ))

_CHALLENGE_14_OBJ = challenge14.o
CHALLENGE_14_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_14_OBJ))

_CHALLENGE_15_OBJ = challenge15.o
CHALLENGE_15_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_15_OBJ))

_CHALLENGE_16_OBJ = challenge16.o
CHALLENGE_16_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_16_OBJ))

_CHALLENGE_17_OBJ = challenge17.o
CHALLENGE_17_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_17_OBJ))

_CHALLENGE_18_OBJ = challenge18.o
CHALLENGE_18_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_18_OBJ))

_CHALLENGE_19_OBJ = challenge19.o
CHALLENGE_19_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_19_OBJ))

_CHALLENGE_20_OBJ = challenge20.o
CHALLENGE_20_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_20_OBJ))

_CHALLENGE_21_OBJ = challenge21.o
CHALLENGE_21_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_21_OBJ))

_CHALLENGE_22_OBJ = challenge22.o
CHALLENGE_22_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_22_OBJ))

_CHALLENGE_23_OBJ = challenge23.o
CHALLENGE_23_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_23_OBJ))

_CHALLENGE_24_OBJ = challenge24.o
CHALLENGE_24_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_24_OBJ))

_CHALLENGE_25_OBJ = challenge25.o
CHALLENGE_25_OBJ = $(patsubst %, $(ODIR)/%, $(_CHALLENGE_25_OBJ))

all: challenge1 challenge2 challenge3 challenge4 challenge5 challenge6 challenge7 challenge8 challenge9 challenge10 challenge11 challenge12 challenge13 challenge14 challenge15 challenge16 challenge17 challenge18 challenge19 challenge20 challenge21 challenge22 challenge23 challenge24 challenge25

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

challenge8: $(CHALLENGE_8_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge9: $(CHALLENGE_9_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge10: $(CHALLENGE_10_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge11: $(CHALLENGE_11_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge12: $(CHALLENGE_12_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge13: $(CHALLENGE_13_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge14: $(CHALLENGE_14_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge15: $(CHALLENGE_15_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge16: $(CHALLENGE_16_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge17: $(CHALLENGE_17_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge18: $(CHALLENGE_18_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge19: $(CHALLENGE_19_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge20: $(CHALLENGE_20_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge21: $(CHALLENGE_21_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge22: $(CHALLENGE_22_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge23: $(CHALLENGE_23_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge24: $(CHALLENGE_24_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

challenge25: $(CHALLENGE_25_OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	find $(ODIR) -type f -name '*.o' -exec rm {} + # Source: http://unix.stackexchange.com/questions/116389/recursively-delete-all-files-with-a-given-extension/116390#116390
	find . -maxdepth 1 -type f -executable -exec rm {} +
