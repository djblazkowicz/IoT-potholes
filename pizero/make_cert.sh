openssl req -x509 -nodes -newkey rsa:2048 -keyout rsa_private.pem \
    -out rsa_cert.pem -subj "/CN=unused"
