CC= gcc -g -Wall
exe1 = keygen 
keygen:
	$(CC) keygen.c -o $(exe1)
serv:
	$(CC) server.c -o serv
client:
	$(CC) client.c -o client
clean:
	rm -rf *.o *.out $(exe1) serv client
