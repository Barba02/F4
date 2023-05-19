CFLAGS=-Wall -std=gnu99
INCLUDES=-I./inc

LIBS=src/errExit.c src/shared_memory.c src/game.c
SERVER_SRCS=$(LIBS) src/F4Server.c
CLIENT_SRCS=$(LIBS) src/F4Client.c
SERVER_OBJS=$(SERVER_SRCS:.c=.o)
CLIENT_OBJS=$(CLIENT_SRCS:.c=.o)
EXECUTABLES=F4Server F4Client

all: $(EXECUTABLES)

F4Server: $(SERVER_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@

F4Client: $(CLIENT_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@

.c.o:
	@echo "Compiling: "$<
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -f src/*.o $(EXECUTABLES)
	@echo "Removed object files and executables..."
