CC = gcc
CFLAGS += -std=c99 # Define which version of the C standard to use
CFLAGS += -Wall # Enable the 'all' set of warnings
CFLAGS += -Werror # Treat all warnings as error
CFLAGS += -Wshadow # Warn when shadowing variables
CFLAGS += -Wextra # Enable additional warnings
CFLAGS += -O2 -D_FORTIFY_SOURCE=2 # Add canary code, i.e. detect buffer overflows
CFLAGS += -fstack-protector-all # Add canary code to detect stack smashing

cracker : main.o
	gcc -o cracker main.o -lpthread

tests: test
	./test tests\01_4c_1k.bin

test: test.o
	gcc -o test test.o

main.o : src\main.c
	gcc -o main.o -c src\main.c 

test.o : tests\test.c
	gcc -o test.o -c tests\test.c

clean:
	del -rf *.exe *.o

all: cracker tests






