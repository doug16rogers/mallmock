
TARGET = read_file_test

# No malloc.h for MacOS's gcc?
CC = clang
CFLAGS = -Wall -Werror -g

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $<

OBJS = read_file_test.o read_file.o cut.o mallmock.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $(CFLAGS) $^

.PHONY: test
test: $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -f *~ *.o $(TARGET)
