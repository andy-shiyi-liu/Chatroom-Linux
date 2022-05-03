all: client server

client: client_v2.c
	gcc -w -pthread -o client_v2 $^
	
# server: server_v2_1.c server_v2.h
# 	gcc -w -pthread -o $@ $^

clean:
	rm -f client_v2
