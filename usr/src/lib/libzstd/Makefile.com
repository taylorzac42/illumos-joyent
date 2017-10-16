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

LIBRARY=	libzstd.a
VERS=		.1

OBJECTS=	\
		entropy_common.o \
		error_private.o \
		fse_decompress.o \
		pool.o \
		threading.o \
		xxhash.o \
		zstd_common.o \
		fse_compress.o \
		huf_compress.o \
		zstd_compress.o \
		zstdmt_compress.o \
		huf_decompress.o \
		zstd_decompress.o \
		zbuff_common.o \
		zbuff_compress.o \
		zbuff_decompress.o \
		cover.o \
		divsufsort.o \
		zdict.o	\
		zstd_fast.o \
		zstd_lazy.o	\
		zstd_ldm.o	\
		zstd_opt.o	\
		zstd_double_fast.o

include	../../Makefile.lib

MAPFILEDIR=	..
LIBS=		$(DYNLIB)
SRCDIR=		$(SRC)/../../contrib/zstd
LDLIBS +=	-lc
CSTD=		$(CSTD_GNU99)
CPPFLAGS =	-I$(SRCDIR)/lib -I$(SRCDIR)/lib/common -DXXH_NAMESPACE=ZSTD_ \
                -DZSTD_MULTITHREAD=1

# gcc 4.4.4 thinks the zstd_decompress.c has
# error: 'seq.match' is used uninitialized in this function [-Wuninitialized]
# gcc 6 and 7 does not complain
CERRWARN +=	-_gcc=-Wno-uninitialized

CLOBBERFILES += $(LIBRARY)

all: $(LIBS)

pics/%.o:	$(SRCDIR)/lib/common/%.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

pics/%.o:	$(SRCDIR)/lib/compress/%.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

pics/%.o:	$(SRCDIR)/lib/decompress/%.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

pics/%.o:	$(SRCDIR)/lib/deprecated/%.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

pics/%.o:	$(SRCDIR)/lib/dictBuilder/%.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

pics/%.o:	$(SRCDIR)/lib/%.c
	$(COMPILE.c) -o $@ $<
	$(POST_PROCESS_O)

include ../../Makefile.targ
