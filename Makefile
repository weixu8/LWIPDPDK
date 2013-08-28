
#Source Folders
RTE_SDK=/home/peter/dpdk-1.2.3r2/
RTE_TARGET=x86_64-default-linuxapp-gcc
CONTRIBDIR=/home/peter/LWIP/contrib-1.4.1
LWIPARCH=$(CONTRIBDIR)/ports/unix
LWIPDIR=$(CONTRIBDIR)/../lwip/src

#Compilers
CC = gcc
AR = ar
ARCH = linux

#Compiler Flags
ARFLAGS=rs
CFLAGS = -g -O0 -Wall -D$(ARCH) -DIPv4 -DLWIP_DEBUG -pedantic \
         -Wswitch-default -Wextra -Wundef -Wshadow -Wbad-function-cast \
         -Wc++-compat -Wmissing-prototypes -Wredundant-decls -Wnested-externs 
LDFLAGS = -lutil -pthread


#CFLAGS including -Is
CFLAGS := $(CFLAGS) \
          -I. -I$(CONTRIBDIR)/apps/httpserver_raw -I$(LWIPDIR)/include -I$(LWIPARCH)/include -I$(LWIPDIR)/include/ipv4 -I$(LWIPDIR) \
          -I${RTE_SDK}/${RTE_TARGET}/include -L$(RTE_SDK)/$(RTE_TARGET)/lib \
	  -lethdev -lrte_eal -lrte_hash -lrte_lpm -lrte_malloc -lrte_mbuf -lrte_mempool -lrte_pmd_igb -lrte_pmd_ixgbe -lrte_ring -lrte_timer 
 
#Core Files for LWIP
COREFILES = $(LWIPDIR)/core/mem.c $(LWIPDIR)/core/memp.c $(LWIPDIR)/core/netif.c \
            $(LWIPDIR)/core/pbuf.c $(LWIPDIR)/core/raw.c $(LWIPDIR)/core/stats.c \
            $(LWIPDIR)/core/sys.c $(LWIPDIR)/core/tcp.c $(LWIPDIR)/core/tcp_in.c \
            $(LWIPDIR)/core/tcp_out.c $(LWIPDIR)/core/udp.c $(LWIPDIR)/core/dhcp.c \
            $(LWIPDIR)/core/init.c $(LWIPDIR)/core/timers.c $(LWIPDIR)/core/def.c \
             
CORE4FILES = $(wildcard $(LWIPDIR)/core/ipv4/*.c) $(LWIPDIR)/core/ipv4/inet.c \
            $(LWIPDIR)/core/ipv4/inet_chksum.c

APIFILES = $(LWIPDIR)/api/api_lib.c $(LWIPDIR)/api/api_msg.c $(LWIPDIR)/api/tcpip.c \
           $(LWIPDIR)/api/err.c $(LWIPDIR)/api/sockets.c $(LWIPDIR)/api/netbuf.c \
           $(LWIPDIR)/api/netdb.c

NETIFFILES = $(LWIPDIR)/netif/etharp.c $(LWIPDIR)/netif/slipif.c

ARCHFILES = $(wildcard $(LWIPARCH)/*.c $(LWIPARCH)/netif/unixif.c $(LWIPARCH)/netif/list.c $(LWIPARCH)/netif/tcpdump.c)

APPFILES = $(CONTRIBDIR)/apps/httpserver_raw/fs.c $(CONTRIBDIR)/apps/httpserver_raw/httpd.c

#LWIP FILES
LWIPFILES = $(COREFILES) $(CORE4FILES) $(APIFILES) $(NETIFFILES) $(ARCHFILES)
LWIPFILESW = $(wildcard $(LWIPFILES))
LWIPOBJ = $(notdir $(LWIPFILESW:.c=.o))

LWIPLIB = liblwip4.a
APPLIB = liblwipapps.a
APPOBJS = $(notdir $(APPFILES:.c=.o))


.PHONY: clean

all: http_server

%.o: 
	$(CC) $(CFLAGS) -c $(<:.o=.c)

clean: 
	rm -f *.o $(LWIPLIB) $(APPLIB) *.s .depend* *.core core

depend dep: .depend

include .depend 

#Compile contributed apps into a library
$(APPLIB) : $(APPOBJS)
	$(AR) $(ARFLAGS) $(APPLIB) $?
#Compile LWIP into a library
$(LWIPLIB): $(LWIPOBJS)
	$(AR) $(ARFLAGS) $(LWIPLIB) $?

.depend: http_server.c $(LWIPFILES) $(APPFILES) 
	$(CC) $(CFLAGS) -MM $^ > .depend || rm -f .depend

http_server: .depend $(LWIPLIB) $(APPLIB) http_server.o $(APPFILES)
	$(CC) $(CFLAGS) $(LDFLAGS) -o http_server http_server.o $(APPLIB) $(LWIPLIB)	
