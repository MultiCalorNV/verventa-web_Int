# List of the required PolarSSL files.
POLARSSL = 	${CHIBIOS}/os/ext/PolarSSL

SSLSRC = \
		 ${POLARSSL}/library/memory_buffer_alloc.c \
         ${POLARSSL}/library/net.c \
		 ${POLARSSL}/library/timing.c
		 
SSLINC = \
		 ${POLARSSL}/include/polarssl
