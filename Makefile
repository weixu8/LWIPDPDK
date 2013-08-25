
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
         -Wswtich-default -Wextra -Wundef -Wshadow -Wbad-function-cast \
         -Wc++-compat -Wmissing-prototypes -Wredundant-decls -Wnested-externs 
LDFLAGS = -lutil -pthread


#CFLAGS including -Is
CFLAGS := $(CFLAGS) \
          -I. -I$(CONTRIBDIR)/apps/httpserver_raw -I$(LWIPDIR)/include -I$(LWIPARCH)/include -I$(LWIPDIR)/include/ipv4 -I$(LWIPDIR) \
          -I${RTE_SDK}/${RTE_TARGET}/include 

all: http_server

