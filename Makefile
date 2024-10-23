CC = clang
CFLAGS = -g
OBJFILES = test.o
TARGET = test

$(TARGET): $(OBJFILES)
	$(CC) $(OBJFILES) -o $(TARGET) $(CFLAGS)

main.o: main.c
	$(CC) -c $^ -o $@ $(CFLAGS)

clean:
	rm -f *.o $(TARGET)