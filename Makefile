CC = g++ -std=c++20
CFLAGS = -Wall -Werror -Wextra

all: clean server client

server:
	g++ server.cpp -o server -w

client:
	g++ client.cpp -o client -w

clean:
	@rm -rf *.o *.a server client
