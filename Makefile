# Use the GNU C/C++ compiler:
CC = gcc
CPP = g++

# COMPILER OPTIONS:
CFLAGS = -c #-g -Wall

#OBJECT FILES
OBJS = bankers.o

#TARGET FILES
TARGET = bankers

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -pthread -o $(TARGET) $(OBJS)

test.o: bankers.c bank.h customer.h
	$(CC) -pthread $(CFLAGS) test.c

clean:
	$(RM) *.o *~ $(TARGET)
