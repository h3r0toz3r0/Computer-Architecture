#
# Makefile for project 1, load and switch
#
#

# build everything. note that 'make' with no args will build the first target
# in the file, so it's traditional to put a rule to build everything at the front
#
all: part-1 part-2 part-3 uprogs

# get rid of all compiler output
#
clean:
	rm -f *.o part-1 part-2 part-3 wait hello ugrep process1 process2

# general rule for compiling assembly language files
# $< is the first dependency - i.e. the .S file
# $@ is the target - i.e. the .o file
#
%.o : %.S
	gcc -g $< -c -o $@

# these flags work for all the C files:
#  -ffreestanding: don't include even implicit system include files
#  turn off various features that complicate the executable

CFLAGS = -Wall -ggdb3 -ffreestanding -fno-stack-protector -fstack-check=no -nostdlib
CC = gcc

# Main C programs will get compiled with the standard rule:
#  $(CC) $(CFLAGS) -c filename.c -o filename.o
#

# Main C programs - link
# $^ - *all* the prerequisite files (everything after ":")
#

part-1 part-2 part-3 : LDFLAGS = --entry=main -ffreestanding -nostdlib -Wl,--section-start=.vector=0x5000000 -static

part-1: part-1.o syscall.o

part-2: part-2.o syscall.o vector.o

part-3: part-3.o syscall.o vector.o stack.o switch.o

# Part 2, 3 micro-programs - compile and link
# 

# 'make uprogs' will compile them all
#
uprogs: wait hello ugrep process1 process2

# special compiler flags for them :
#  - "pie" = position-independent executable
#  - compiling with stack protection requires linking code from standard library, so don't
#  - skip the standard library entirely
#
wait.o hello.o ugrep.o process1.o process2.o : CFLAGS = -fpie -ffreestanding -fno-stack-protector -fstack-check=no -nostdlib

# special LDFLAGS only for these 5
#  - position-independent
#  - no standard libs
#  - start at 'main'
#  - linker adds an (empty) DYNAMIC section, but marks it LOAD and by default puts it at
#    a non-aligned address, so force it to be aligned
#
wait hello ugrep process1 process2 : LDFLAGS = --pie -nostdlib -e main -Wl,--section-start=.dynamic=0x3000

# and they all need to be linked with the system call vector, too
#
wait hello ugrep process1 process2 : call-vector.o




