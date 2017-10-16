#
# This file and its contents are supplied under the terms of the
# Common Development and Distribution License ("CDDL"), version 1.0.
# You may only use this file in accordance with the terms of version
# 1.0 of the CDDL.
#
# A full copy of the text of the CDDL should have accompanied this
# source.  A copy of the CDDL is also available via the Internet at
# http://www.illumos.org/license/CDDL.
#

#
# Copyright 2017 Toomas Soome <tsoome@me.com>
#

PROG=		zstd
OBJS=		\
		bench.o \
		datagen.o \
		dibio.o \
		fileio.o \
		zstdcli.o
SRCS= $(OBJS:%.o=%.c)

include	../Makefile.cmd
include	../Makefile.ctf

SRCDIR=		$(SRC)/../../contrib/zstd
LDLIBS +=	-lzstd -lc
CSTD=		$(CSTD_GNU99)
CPPFLAGS +=	\
		-I$(SRCDIR)/programs \
		-I$(SRCDIR)/lib/common \
		-I$(SRCDIR)/lib/compress \
		-I$(SRCDIR)/lib/dictBuilder \
		-I$(SRCDIR)/lib \
		-DXXH_NAMESPACE=ZSTD_ \
		-DHAVE_THREAD=1 \
		-D_FILE_OFFSET_BITS=64

CLOBBERFILES += $(PROG)

.KEEP_STATE:

all: $(PROG)

$(PROG):	$(OBJS)
	$(LINK.c) -o $@ $(OBJS) $(LDLIBS)
	$(POST_PROCESS)

install:	all $(ROOTPROG) $(ROOTBIN)/zstdgrep
	$(RM) $(ROOTBIN)/unzstd $(ROOTBIN)/zstdcat
	$(LN) $(ROOTPROG) $(ROOTBIN)/unzstd
	$(LN) $(ROOTPROG) $(ROOTBIN)/zstdcat

clean:
	$(RM) $(OBJS)

$(ROOTBIN)/%: $(SRCDIR)/programs/%
	$(INS.file)

%.o:	$(SRCDIR)/programs/%.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

include ../Makefile.targ
