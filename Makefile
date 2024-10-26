CC = clang
CFLAGS = -g
OBJFILES = simple_server.o
TARGET = simple_server

$(TARGET): $(OBJFILES)
	$(CC) $(OBJFILES) -o $(TARGET) $(CFLAGS)

main.o: main.c
	$(CC) -c $^ -o $@ $(CFLAGS)

clean:
	rm -f *.o $(TARGET)