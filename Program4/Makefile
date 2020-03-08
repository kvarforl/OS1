CC= gcc -g -Wall
exe1 = keygen 
keygen:
	$(CC) keygen.c -o $(exe1)
serv:
	$(CC) otp_enc_d.c -o otp_enc_d
client:
	$(CC) otp_enc.c -o otp_enc
clean:
	rm -rf *.o *.out otp_enc otp_enc_d $(exe1)
