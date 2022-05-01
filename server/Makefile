all: client server

# client: client_v2.c
# gcc -o $@ $^
	
server: server_v2_1.c ./include/server_v2.h
	gcc -Wall -o $@ $^ -lpthread

clean:
	rm -f server client
