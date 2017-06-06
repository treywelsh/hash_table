CFLAGS ?= -Wall -Wextra
LDFLAGS ?=
CC ?= gcc
DEBUG ?= 0
STATIC ?= 0
INCLUDE= -I ../../tools/include/

BIN = hash_test
OBJ = $(SRC:.c=.o)
SRC = hash.c main.c

#Mandatory flags
CFLAGS += ${INCLUDE}

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0 -ggdb3
else
	CFLAGS += -DNDEBUG -O3
endif

ifeq ($(STATIC), 1)
	LDFLAGS += -static
endif

all: $(BIN)

$(BIN) : $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS) $(CFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean :
	@rm -f $(OBJ) $(BIN)
