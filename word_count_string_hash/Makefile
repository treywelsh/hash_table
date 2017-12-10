CC ?= gcc
DEBUG ?= 0
STATIC ?= 0
CACHELINE_LEN ?= 64

#NAME = 
TARGET = wordcount 
SRC = main.c wc_file.c strhash.c
OBJ = $(SRC:.c=.o)

INCLUDE = -Iinclude

#CFLAGS ?= -Wall -Wextra -std=c99
CFLAGS ?= 
LDFLAGS ?=

#_POSIX_C_SOURCE due to getline
override CFLAGS += -DCACHELINE_LEN=$(CACHELINE_LEN) -Iinclude -D_POSIX_C_SOURCE=200809L

ifeq ($(DEBUG), 1)
	override CFLAGS += -DDEBUG -O0 -g3
else
	override CFLAGS += -DNDEBUG -O3
endif

ifeq ($(STATIC), 1)
	override LDFLAGS += -static
endif

all: $(TARGET)

re: clean all

$(TARGET) : $(OBJ)
	$(CC) -o $(TARGET) $(OBJ) $(LDFLAGS) $(CFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

test: re
	./wordcount testfiles/dictfile.txt testfiles/lorem_ipsum_clean.txt testfiles/dictfile.txt

.PHONY: clean

clean :
	@rm -f $(OBJ) $(TARGET)
