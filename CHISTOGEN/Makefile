CC = gcc
CFLAGS = -Wall -Wextra -Wno-sign-compare -Wno-format -lcrypto
TARGET = chistogen
SRC = src/main.c src/cleaner.c

all:
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)

install:
	sudo cp $(TARGET) /usr/local/bin/
