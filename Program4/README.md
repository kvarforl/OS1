Code in this directory demonstrates socket-based interprocess communication in C, using one time pad encryption.

Usage:
```
./compileall
otp_enc_d <port> &                              #start servers
otp_dec_d <port> &
keygen <numcharacters>                          #outputs key 
otp_enc <plaintext_file> <keyfile> <enc_port>   #outputs ciphertext
otp_dec <ciphertext_file> <keyfile> <dec_port>  #outputs decrypted plaintext
```
