CC= gcc -g -Wall
exe1 = keygen 
keygen:
	$(CC) keygen.c -o $(exe1)
serv:
	$(CC) opt_enc_d.c -o opt_enc_d
client:
	$(CC) opt_enc.c -o opt_enc
clean:
	rm -rf *.o *.out opt_enc opt_enc_d $(exe1)
