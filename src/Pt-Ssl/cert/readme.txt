Generate a self-signed CA
-------------------------
openssl req -config ./openssl.cnf -newkey rsa:2048 -nodes -keyform PEM -keyout ca.key -x509 -days 3650 -extensions certauth -outform PEM -out ca.cer
openssl x509 -in ca.cer -outform PEM -out ca.pem


Generate and sign a server certificate using the previous CA
------------------------------------------------------------
openssl genrsa -des3 -out server.key 2048
openssl req -config ./openssl.cnf -new -key server.key -out server.req
openssl x509 -req -in server.req -CA ca.cer -CAkey ca.key -set_serial 100 -extfile openssl.cnf -extensions server -days 365 -outform PEM -out server.cer
openssl pkcs12 -export -inkey server.key -in server.cer -out server.p12
openssl x509 -in server.cer -outform PEM -out server.pem
rm -f server.req

NOTE: In the above example, we encrypted the key using Tripple-DES (we use abc123 as the password)


Generate and sign a client certificate using the previous CA
------------------------------------------------------------
openssl genrsa -out client.key 2048
openssl req -config ./openssl.cnf -new -key client.key -out client.req
openssl x509 -req -in client.req -CA ca.cer -CAkey ca.key -set_serial 101 -extfile openssl.cnf -extensions client -days 365 -outform PEM -out client.cer
openssl pkcs12 -export -inkey client.key -in client.cer -out client.p12
openssl x509 -in client.cer -outform PEM -out client.pem
rm -f client.req


Generate a DSA key
------------------
openssl dsaparam -out dsaparam.pem 2048
openssl gendsa -out dsakey1.pem dsaparam.pem
openssl gendsa -out dsakey2.pem dsaparam.pem
openssl dsa -in dsakey1.pem -pubout -out dsapubkey1.pem
openssl dsa -in dsakey2.pem -pubout -out dsapubkey2.pem
