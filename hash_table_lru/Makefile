CC ?= gcc
CFLAGS ?= -Wall -Wextra
LDFLAGS ?=
INCLUDE= -I ../../tools/include/

DEBUG ?= 0
STATIC ?= 0

TARGET = hash_test
#SRC = $(wildcard *.c)
SRC = hash.c test.c
OBJ = $(SRC:.c=.o)

ifeq ($(CC), clang)
	CFLAGS = -Weverything
	DEBUG = 1
endif

#Mandatory flags
CFLAGS += ${INCLUDE}
LDFLAGS +=

ifeq ($(DEBUG), 1)
	CFLAGS += -DDEBUG -O0 -g -fno-inline -fno-strict-aliasing
else
	CFLAGS += -DNDEBUG -O2 -fstrict-aliasing
endif

ifeq ($(STATIC), 1)
	LDFLAGS += -static
endif

all: $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) -o $@ $(OBJ) $(LDFLAGS)

%.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean :
	@rm -f $(OBJ) $(TARGET)
