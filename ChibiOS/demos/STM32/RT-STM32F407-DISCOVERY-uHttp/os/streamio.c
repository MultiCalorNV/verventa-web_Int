/*
 * Copyright (C) 2012 by egnite GmbH
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holders nor the names of
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * For additional information see http://www.ethernut.de/
 */

/*!
 * $Id$
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "../../../ext/mbedtls-1.3.11/include/polarssl/config.h"

#include "../../../ext/mbedtls-1.3.11/include/polarssl/entropy.h"
#include "../../../ext/mbedtls-1.3.11/include/polarssl/ctr_drbg.h"
#include "../../../ext/mbedtls-1.3.11/include/polarssl/certs.h"
#include "../../../ext/mbedtls-1.3.11/include/polarssl/x509.h"
#include "../../../ext/mbedtls-1.3.11/include/polarssl/ssl.h"
#include "../../../ext/mbedtls-1.3.11/include/polarssl/net.h"
#include "../../../ext/mbedtls-1.3.11/include/polarssl/error.h"
//#include "../../../ext/mbedtls-1.3.11/include/polarssl/threading.h"

#if defined(POLARSSL_SSL_CACHE_C)
#include "../../../ext/mbedtls-1.3.11/include/polarssl/ssl_cache.h"
#endif

#include "os\streamio.h"

#include "ITM_trace.h"
#include "chprintf.h"

extern ITMStream itm_port;

#define INVALID_SOCKET  -1

ssl_context ssl;

static int num_threads;

int StreamInit(void)
{
   return 0;
}

typedef struct _CLIENT_THREAD_PARAM {
    HTTP_STREAM *ctp_stream;
    HTTP_CLIENT_HANDLER ctp_handler;
} CLIENT_THREAD_PARAM;

void StreamClientThread(void *param)
{
    CLIENT_THREAD_PARAM *ctp = (CLIENT_THREAD_PARAM *) param;

    (*ctp->ctp_handler)(ctp->ctp_stream);
    closesocket(ctp->ctp_stream->strm_csock);
    free(ctp->ctp_stream);
    free(ctp);

    num_threads--;
    
#if 0 /* @@MF */
    if (--num_threads == 0) {
        while(1) { __asm("nop"); }//_CrtDumpMemoryLeaks();
    }
#endif
}

int StreamClientAccept(HTTP_CLIENT_HANDLER handler, const char *params)
{
    int rc = -1;
    SOCKET sock;
    SOCKET csock;
    struct sockaddr_in addr;
    struct sockaddr_in caddr;
    unsigned short port = 80;

    HTTP_ASSERT(handler != NULL);
    if (params) {
        port = (unsigned short)atoi(params);
    }

    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (sock != INVALID_SOCKET) {
        memset((void*)&addr, 0, sizeof(addr));
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        addr.sin_port = htons(port);

        if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == 0) {
            if (listen(sock, 1) == 0) {
                socklen_t len = sizeof(caddr);

                for (;;) {
                    csock = accept(sock, (struct sockaddr*)&caddr, &len);
                    if (csock != INVALID_SOCKET) {
                        CLIENT_THREAD_PARAM *ctp;
                        unsigned int tmo = 1000;

                        setsockopt(csock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tmo, sizeof(tmo));

                        ctp = malloc(sizeof(CLIENT_THREAD_PARAM));
                        ctp->ctp_handler = handler;
                        ctp->ctp_stream = calloc(1, sizeof(HTTP_STREAM));
                        ctp->ctp_stream->strm_ssock = sock;
                        ctp->ctp_stream->strm_csock = csock;
                        memcpy(&ctp->ctp_stream->strm_saddr, &addr, sizeof(ctp->ctp_stream->strm_saddr));
                        memcpy(&ctp->ctp_stream->strm_caddr, &caddr, sizeof(ctp->ctp_stream->strm_caddr));

                        num_threads++;
                        while (num_threads > 2) {
                        	chThdSleepMilliseconds(100);
                        }

                        StreamClientThread(ctp);
                    }
                }
            }
        }
        closesocket(sock);
    }
    return rc;
}

int StreamClientAccept_SSL(HTTP_CLIENT_HANDLER handler, const char *params)
{
	int ret, len;
	SOCKET listen_fd;
	SOCKET client_fd = -1;
	struct sockaddr_in addr;
	struct sockaddr_in caddr;
	unsigned char buf[1024];
	unsigned short port = 443;
	const char *pers = "ssl_server";

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);

	entropy_context entropy;
	ctr_drbg_context ctr_drbg;
	x509_crt srvcert;
	rsa_context rsa;
#if defined(POLARSSL_SSL_CACHE_C)
	ssl_cache_context cache;
#endif

#if defined(POLARSSL_SSL_CACHE_C)
	ssl_cache_init( &cache );
#endif

	/*
	 * 1. Load the certificates and private RSA key
	 */
	chprintf((BaseSequentialStream *)&itm_port, "\n  . Loading the server cert. and key..." );

	memset( &srvcert, 0, sizeof( x509_crt ) );

	ret = x509_crt_parse( &srvcert, (const unsigned char *) test_srv_crt,
	                         strlen( test_srv_crt ) );
	if( ret != 0 )
	{
	   chprintf((BaseSequentialStream *)&itm_port, " [ PolarSSL ]failed\n  !  x509parse_crt returned %d\n\n", ret );
	}

	ret = x509_crt_parse( &srvcert, (const unsigned char *) test_ca_crt,
	                         strlen( test_ca_crt ) );
	if( ret != 0 )
	{
		chprintf((BaseSequentialStream *)&itm_port, " [ PolarSSL ]failed\n  !  x509parse_crt returned %d\n\n", ret );
	}

	rsa_init( &rsa, RSA_PKCS_V15, 0 );
	ret =  pk_parse_key( &rsa, (const unsigned char *) test_srv_key,
	                          strlen( test_srv_key ), NULL, 0 );
	if( ret != 0 )
	{
		chprintf((BaseSequentialStream *)&itm_port, " [ PolarSSL ]failed\n  !  x509parse_key returned %d\n\n", ret );
	}

	/*
	 * 2. Setup the listening TCP socket
	 */
	chprintf((BaseSequentialStream *)&itm_port, "  . Bind on https://192.168.2.10:443/ ..." );

	if( ( ret = net_bind( &listen_fd, "192.168.2.10", port ) ) != 0 )
	{
		chprintf((BaseSequentialStream *)&itm_port, " [ PolarSSL ]failed\n  ! net_bind returned %d\n\n", ret );
	}

	/*
	 * 3. Seed the RNG
	 */
	chprintf((BaseSequentialStream *)&itm_port, "  . Seeding the random number generator..." );

	entropy_init( &entropy );
	if( ( ret = ctr_drbg_init( &ctr_drbg, entropy_func, &entropy,
	                               (const unsigned char *) pers,
	                               strlen( pers ) ) ) != 0 )
	{
		chprintf((BaseSequentialStream *)&itm_port, " [ PolarSSL ]failed\n  ! ctr_drbg_init returned %d\n", ret );
	}

	/*
	 * 4. Setup stuff
	 */
	chprintf((BaseSequentialStream *)&itm_port, "  . Setting up the SSL data...." );

	if( ( ret = ssl_init( &ssl ) ) != 0 )
	{
		chprintf((BaseSequentialStream *)&itm_port, " [ PolarSSL ]failed\n  ! ssl_init returned %d\n\n", ret );
	}

	ssl_set_endpoint( &ssl, SSL_IS_SERVER );
	ssl_set_authmode( &ssl, SSL_VERIFY_NONE );

	ssl_set_rng( &ssl, ctr_drbg_random, &ctr_drbg );
	//ssl_set_dbg( &ssl, my_debug, stdout );

#if defined(POLARSSL_SSL_CACHE_C)
	ssl_set_session_cache( &ssl, ssl_cache_get, &cache,
	                                 ssl_cache_set, &cache );
#endif

	ssl_set_ca_chain( &ssl, srvcert.next, NULL, NULL );
	ssl_set_own_cert( &ssl, &srvcert, &rsa );

reset:
#ifdef POLARSSL_ERROR_C
	if( ret != 0 )
	{
	   char error_buf[100];
	   error_strerror( ret, error_buf, 100 );
	   chprintf((BaseSequentialStream *)&itm_port,"[ PolarSSL ]Last error was: %d - %s\n\n", ret, error_buf );
	}
#endif

	if( client_fd != -1 )
	      net_close( client_fd );

	ssl_session_reset( &ssl );

	/*
	 * 5. Wait until a client connects
	 */
#if defined(_WIN32_WCE)
	{
		SHELLEXECUTEINFO sei;

	    ZeroMemory( &sei, sizeof( SHELLEXECUTEINFO ) );

	    sei.cbSize = sizeof( SHELLEXECUTEINFO );
	    sei.fMask = 0;
	    sei.hwnd = 0;
	    sei.lpVerb = _T( "open" );
	    sei.lpFile = _T( "https://localhost:4433/" );
	    sei.lpParameters = NULL;
	    sei.lpDirectory = NULL;
	    sei.nShow = SW_SHOWNORMAL;

	    ShellExecuteEx( &sei );
	}
#elif defined(_WIN32)
	    ShellExecute( NULL, "open", "https://localhost:4433/",
	                  NULL, NULL, SW_SHOWNORMAL );
#endif

	 client_fd = -1;

	 chprintf((BaseSequentialStream *)&itm_port, "  . Waiting for a remote connection ..." );

	 if( ( ret = net_accept( listen_fd, &client_fd, NULL ) ) != 0 )
	 {
	  	chprintf((BaseSequentialStream *)&itm_port, " [ PolarSSL ]failed\n  ! net_accept returned %d\n\n", ret );
	 }

	 ssl_set_bio( &ssl, net_recv, &client_fd,
	                       net_send, &client_fd );

	 /*
	  * 6. Handshake
	  */
	 chprintf((BaseSequentialStream *)&itm_port, "  . Performing the SSL/TLS handshake..." );

	 while( ( ret = ssl_handshake( &ssl ) ) != 0 )
	 {
		 if( ret != POLARSSL_ERR_NET_WANT_READ && ret != POLARSSL_ERR_NET_WANT_WRITE )
	     {
	      	chprintf((BaseSequentialStream *)&itm_port, " [ PolarSSL ]failed\n  ! ssl_handshake returned %d\n\n", ret );
	        goto reset;
	     }
	 }

	 CLIENT_THREAD_PARAM *ctp;
	 unsigned int tmo = 1000;

	 setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (char *) &tmo, sizeof(tmo));

	 ctp = malloc(sizeof(CLIENT_THREAD_PARAM));
	 ctp->ctp_handler = handler;
	 ctp->ctp_stream = calloc(1, sizeof(HTTP_STREAM));
	 ctp->ctp_stream->strm_ssock = listen_fd;
	 ctp->ctp_stream->strm_csock = client_fd;
	 memcpy(&ctp->ctp_stream->strm_saddr, &addr, sizeof(ctp->ctp_stream->strm_saddr));
	 memcpy(&ctp->ctp_stream->strm_caddr, &caddr, sizeof(ctp->ctp_stream->strm_caddr));

	 StreamClientThread(ctp);

	 ret = 0;
	 goto reset;

exit:
#ifdef POLARSSL_ERROR_C
	if( ret != 0 )
	{
	     char error_buf[100];
	     error_strerror( ret, error_buf, 100 );
	     chprintf((BaseSequentialStream *)&itm_port,"Last error was: %d - %s\n\n", ret, error_buf );
	}
#endif

	    net_close( client_fd );
	    x509_crt_free( &srvcert );
	    rsa_free( &rsa );
	    ssl_free( &ssl );
#if defined(POLARSSL_SSL_CACHE_C)
	    ssl_cache_free( &cache );
#endif

#if defined(_WIN32)
	    chprintf((BaseSequentialStream *)&itm_port, "  Press Enter to exit this program.\n" );
#endif

	  return( ret );
}

int StreamReadUntilChars(HTTP_STREAM *sp, const char *delim, const char *ignore, char *buf, int siz)
{
	int len;
    int rc = 0;
    int skip = 0;
    char ch;

    HTTP_ASSERT(sp != NULL);

    /* Do not read more characters than requested. */
    while (rc < siz) {
        /* Check the current stream buffer. */
        if (sp->strm_ipos == sp->strm_ilen) {
            // No more buffered data, re-fill the buffer.
            int got = recv(sp->strm_csock, sp->strm_ibuf, 1460, 0);
            if (got <= 0) {
                 //Broken connection or timeout.
                if (got < 0) {
                    rc = -1;
                    skip = 0;
                }
                break;
            }
            sp->strm_ilen = got;
            sp->strm_ipos = 0;
        }
    	len = sizeof( buf ) - 1;
    	memset( buf, 0, sizeof( buf ) );
    	ssl_read( &ssl, buf, len );

        ch = sp->strm_ibuf[sp->strm_ipos];
        sp->strm_ipos++;
        if (rc == 0 && ch == ' ') {
            /* Skip leading spaces. */
            skip++;
        } else {
            rc++;
            if (delim && strchr(delim, ch)) {
                /* Delimiter found. */
                break;
            }
            if (buf && (ignore == NULL || strchr(ignore, ch) == NULL)) {
                /* Add valid character to application buffer. */
                *buf++ = ch;
            }
        }
    }
    if (buf) {
        *buf = '\0';
    }
    return rc + skip;
}

int StreamReadUntilString(HTTP_STREAM *sp, const char *delim, char *buf, int siz)
{
    int rc = 0;
    int n;
    int i;
    int delen = strlen(delim);

    HTTP_ASSERT(sp != NULL);

    /* Do not read more characters than requested. */
    while (rc < siz) {
        /* Check if the delimiter fits in the current stream buffer. */
        if (sp->strm_ipos >= sp->strm_ilen - delen) {
            int got;
            /* Not enough data to fit the delimiter, re-fill the buffer. */
            sp->strm_ilen -= sp->strm_ipos;
            memcpy(sp->strm_ibuf, sp->strm_ibuf + sp->strm_ipos, sp->strm_ilen);
            got = recv(sp->strm_csock, sp->strm_ibuf + sp->strm_ilen, sizeof(sp->strm_ibuf) - sp->strm_ilen, 0);
            if (got <= 0) {
                /* Broken connection or timeout. */
                if (got < 0) {
                    rc = -1;
                }
                break;
            }
            sp->strm_ilen += got;
            sp->strm_ipos = 0;
        }
        for (i = sp->strm_ipos, n = 0; i < sp->strm_ilen && rc + n < siz; i++, n++) {
            if (*delim == sp->strm_ibuf[i]) {
                if (i + delen >= sp->strm_ilen) {
                    break;
                }
                if (memcmp(&sp->strm_ibuf[i], delim, delen) == 0) {
                    break;
                }
            }
        }
        if (n) {
            memcpy(buf, sp->strm_ibuf + sp->strm_ipos, n);
            buf += n;
            rc += n;
            sp->strm_ipos += n;
        } else {
            break;
        }
    }
    return rc;
}

int s_write(const void *buf, size_t size, size_t count, HTTP_STREAM *sp)
{
    HTTP_ASSERT(sp != NULL);
    HTTP_ASSERT(buf != NULL);

    return send(sp->strm_csock, (const char *)buf, size * count, 0);
    //return ssl_write(&ssl, (const char *)buf, size * count);
}

int s_puts(const char *str, HTTP_STREAM *sp)
{
    HTTP_ASSERT(sp != NULL);
    HTTP_ASSERT(str != NULL);

    return send(sp->strm_csock, str, strlen(str), 0);
    //return ssl_write(&ssl, str, strlen(str));
}

int s_vputs(HTTP_STREAM *sp, ...)
{
    int rc = -1;
    int len;
    char *cp;
    char *buf;
    va_list ap;

    HTTP_ASSERT(sp != NULL);

    va_start(ap, sp);
    for (len = 0; (cp = va_arg(ap, char *)) != NULL; len += strlen(cp));
    va_end(ap);
    buf = malloc(len + 1);
    if (buf) {
        va_start(ap, sp);
        for (*buf = '\0'; (cp = va_arg(ap, char *)) != NULL; strcat(buf, cp));
        va_end(ap);
        rc = send(sp->strm_csock, buf, strlen(buf), 0);
        //rc = ssl_write(&ssl, buf, strlen(buf));
        free(buf);
    }
    return rc;
}

int s_printf(HTTP_STREAM *sp, const char *fmt, ...)
{
    int rc = -1;
    char *buf;
    va_list ap;

    HTTP_ASSERT(sp != NULL);
    HTTP_ASSERT(fmt != NULL);

    buf = malloc(1024);
    if (buf) {
        va_start(ap, fmt);
        rc = vsnprintf(buf, 1023, fmt, ap);
        va_end(ap);
        rc = send(sp->strm_csock, buf, rc, 0);
        //rc = ssl_write(&ssl, buf, rc);
        free(buf);
    }
    return rc;
}

int s_flush(HTTP_STREAM *sp)
{
    (void)sp;

    return 0;
}

const char *StreamInfo(HTTP_STREAM *sp, int item)
{
    static char *env_value;
    //char *vp = NULL;

    free(env_value);
    env_value = NULL;
    switch (item) {
    case SITEM_REMOTE_ADDR:
        env_value = strdup(inet_ntoa(sp->strm_caddr.sin_addr));
        break;
    case SITEM_REMOTE_PORT:
        env_value = malloc(16);
        sprintf(env_value, "%u", sp->strm_caddr.sin_port);
        break;
    case SITEM_SERVER_NAME:
    case SITEM_SERVER_ADDR:
        env_value = strdup(inet_ntoa(sp->strm_saddr.sin_addr));
        break;
    case SITEM_SERVER_PORT:
        env_value = malloc(16);
        sprintf(env_value, "%u", sp->strm_saddr.sin_port);
        break;
    }

    if (env_value == NULL) {
        env_value = strdup("");
    }
    return env_value;
}
