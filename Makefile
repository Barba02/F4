CFLAGS=-Wall -std=gnu99
INCLUDES=-I./inc

LIBS=src/errExit.c
SERVER_SRCS=$(LIBS) src/server.c
CLIENT_SRCS=$(LIBS) src/client.c
SERVER_OBJS=$(SERVER_SRCS:.c=.o)
CLIENT_OBJS=$(CLIENT_SRCS:.c=.o)

all: server client

server: $(SERVER_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@

client: $(CLIENT_OBJS)
	@echo "Making executable: "$@
	@$(CC) $^ -o $@

.c.o:
	@echo "Compiling: "$<
	@$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -f src/*.o client server
	@echo "Removed object files and executables..."
