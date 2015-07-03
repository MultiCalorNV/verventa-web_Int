# List of the required PolarSSL files.
POLARSSL = 	${CHIBIOS}/ext/mbedtls-1.3.11

SSLSRC = \
		 ${POLARSSL}/library/memory_buffer_alloc.c \
         ${POLARSSL}/library/net.c \
		 ${POLARSSL}/library/timing.c \
		 ${POLARSSL}/library/asn1parse.c \
		 ${POLARSSL}/library/asn1write.c \
		 ${POLARSSL}/library/certs.c \
		 ${POLARSSL}/library/rsa.c \
		 ${POLARSSL}/library/bignum.c \
		 ${POLARSSL}/library/pem.c \
		 ${POLARSSL}/library/md.c \
		 ${POLARSSL}/library/md_wrap.c \
		 ${POLARSSL}/library/md5.c \
		 ${POLARSSL}/library/des.c \
		 ${POLARSSL}/library/aes.c \
		 ${POLARSSL}/library/base64.c \
		 ${POLARSSL}/library/pkcs5.c \
		 ${POLARSSL}/library/pkcs12.c \
		 ${POLARSSL}/library/pk.c \
		 ${POLARSSL}/library/pk_wrap.c \
		 ${POLARSSL}/library/pkparse.c \
		 ${POLARSSL}/library/cipher.c \
		 ${POLARSSL}/library/arc4.c \
		 ${POLARSSL}/library/sha1.c \
		 ${POLARSSL}/library/sha256.c \
		 ${POLARSSL}/library/sha512.c \
		 ${POLARSSL}/library/camellia.c \
		 ${POLARSSL}/library/blowfish.c \
		 ${POLARSSL}/library/cipher_wrap.c \
		 ${POLARSSL}/library/entropy.c \
		 ${POLARSSL}/library/x509_crt.c \
		 ${POLARSSL}/library/x509.c \
		 ${POLARSSL}/library/ctr_drbg.c \
		 ${POLARSSL}/library/entropy_poll.c \
		 ${POLARSSL}/library/ssl_cli.c \
		 ${POLARSSL}/library/ssl_srv.c \
		 ${POLARSSL}/library/ssl_cache.c \
		 ${POLARSSL}/library/ssl_tls.c \
		 ${POLARSSL}/library/ssl_ciphersuites.c \
		 ${POLARSSL}/library/debug.c \
		 ${POLARSSL}/library/dhm.c \
		 ${POLARSSL}/library/error.c \
		 ${POLARSSL}/library/oid.c \
		 ${POLARSSL}/library/gcm.c \
		 ${POLARSSL}/library/ccm.c \
		 ${POLARSSL}/library/ecdh.c \
		 ${POLARSSL}/library/ecdsa.c \
		 ${POLARSSL}/library/ecp.c \
		 ${POLARSSL}/library/ecp_curves.c \
		 ${POLARSSL}/library/ripemd160.c \
		 ${POLARSSL}/library/hmac_drbg.c \
		 ${POLARSSL}/library/threading.c
		 
		 
SSLINC = \
		 ${POLARSSL}/include/polarssl
