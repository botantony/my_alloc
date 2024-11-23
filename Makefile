CC=gcc
# do not use `-Wall -Werror` yet becuase we use some unused variables for demonstration
FLAGS=-g
BIN_DIR=bin

main:
	mkdir -p $(BIN_DIR)
	$(CC) my_alloc.c -o $(BIN_DIR)/test.o $(FLAGS)

clean:
	rm -fr $(BIN_DIR)
