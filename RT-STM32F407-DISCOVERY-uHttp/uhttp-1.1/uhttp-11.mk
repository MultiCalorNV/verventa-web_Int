UHTTP = ${CHIBIOS}/demos/STM32/RT-STM32F407-DISCOVERY-uHttp

# List of all the uHttp files.
UHTTPSRC = 	$(UHTTP)/uhttp-1.1/mediatypes.c	\
			$(UHTTP)/uhttp-1.1/uhttpd.c		\
			$(UHTTP)/uhttp-1.1/utils.c		\
			$(UHTTP)/uhttp-1.1/mtinit.c		\
			$(UHTTP)/uhttp-1.1/responses.c	\
			$(UHTTP)/uhttp-1.1/io.c	\
			$(UHTTP)/uhttp-1.1/htdocs.c	\
			$(UHTTP)/uhttp-1.1/pro/mtreg.c \
			$(UHTTP)/uhttp-1.1/pro/modules/mod_cgi_func.c	\
			$(UHTTP)/uhttp-1.1/pro/modules/mod_redir.c

# Required include directories
UHTTPINC =	$(UHTTP)/uhttp-1.1 \
			$(UHTTP)/uhttp-1.1/pro \
			$(UHTTP)/uhttp-1.1/cfg