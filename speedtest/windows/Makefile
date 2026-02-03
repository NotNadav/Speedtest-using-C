CC = gcc
CFLAGS = -Wall -O2 -mwindows
LIBS = -lwinhttp -lws2_32
TARGET = speedtest.exe
OBJS = main.o speedtest.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

main.o: main.c speedtest.h
	$(CC) $(CFLAGS) -c main.c

speedtest.o: speedtest.c speedtest.h
	$(CC) $(CFLAGS) -c speedtest.c

clean:
	del /Q *.o $(TARGET)

.PHONY: all clean
