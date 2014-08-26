This dir contains keys and certificates files produced by openssl.
Please refer to http://www.g-loaded.eu/2005/11/10/be-your-own-ca/ if you need to make your own CA.

file descriptions:

private/myca.key: my CA's private key

private/server.key: server's private key

certs/myca.crt: my CA's certificate, and can be publicly available

cert/server.crt: server's certificate, distributed publicly

openssl.cnf: copy from /etc/ssl/, then modify some values

Note: CA' certificate and server's certificates should be distributed to clients, like web
browsers.
