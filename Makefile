CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -Wpedantic -Wconversion -O2

all: studentmanager

studentmanager: src/main.c
	$(CC) $(CFLAGS) src/main.c -o studentmanager

.PHONY: all
