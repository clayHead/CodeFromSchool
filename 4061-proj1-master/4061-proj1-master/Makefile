CC = gcc
CFLAGS = -Wall -g -pthread
TARGET = myBashProj

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
