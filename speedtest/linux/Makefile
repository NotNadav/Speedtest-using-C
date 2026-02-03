CC = gcc
CFLAGS = -Wall -O2 `pkg-config --cflags gtk+-3.0`
LIBS = `pkg-config --libs gtk+-3.0` -lcurl -lpthread
TARGET = speedtest
OBJS = main.o speedtest.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBS)

main.o: main.c speedtest.h
	$(CC) $(CFLAGS) -c main.c

speedtest.o: speedtest.c speedtest.h
	$(CC) $(CFLAGS) -c speedtest.c

clean:
	rm -f *.o $(TARGET)

.PHONY: all clean
