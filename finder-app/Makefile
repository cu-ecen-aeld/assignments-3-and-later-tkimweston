CC = gcc
CROSS_COMPILE ?=
CFLAGS = -Wall -Wextra -std=c99

ifeq ($(CROSS_COMPILE),)
    COMPILER = $(CC)
else
    COMPILER = $(CROSS_COMPILE)gcc
endif

all: writer

writer: writer.c
	$(COMPILER) $(CFLAGS) -o writer writer.c

clean:
	rm -f writer

