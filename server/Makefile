all: client server

# client: client_v2.c
# gcc -o $@ $^
	
server: server_v2_1.c ./include/server_v2.h
	gcc -w -pthread -o $@ $^

clean:
	rm -f server client
