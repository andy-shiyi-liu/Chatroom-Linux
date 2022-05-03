all: client server

# client: client_v2.c
# gcc -o $@ $^
	
server: server_v2_1.c ./include/server_v2.h
	gcc -w -o server_v2_1 $^ -lpthread

clean:
	rm -f server_v2_1
