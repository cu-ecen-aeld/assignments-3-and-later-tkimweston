CC = gcc
CROSS_COMPILE ?=
CFLAGS = -Wall

ifeq ($(CROSS_COMPILE),)
    COMPILER = $(CC)
else
    COMPILER = $(CROSS_COMPILE)gcc
endif

all: aesdsocket

default: aesdsocket

aesdsocket: aesdsocket.c
	$(COMPILER) $(CFLAGS) -o aesdsocket aesdsocket.c

clean:
	rm -f aesdsocket

