/*
 * This file and its contents are supplied under the terms of the
 * Common Development and Distribution License ("CDDL"), version 1.0.
 * You may only use this file in accordance with the terms of version
 * 1.0 of the CDDL.
 *
 * A full copy of the text of the CDDL should have accompanied this
 * source.  A copy of the CDDL is also available via the Internet at
 * http://www.illumos.org/license/CDDL.
 */

/*
 * Copyright 2018 Toomas Soome <tsoome@me.com>
 */

#ifndef _EFIRT_H
#define	_EFIRT_H

#ifdef __cplusplus
extern "C" {
#endif

#define	EFIRT_INIT_MINOR	0x00000001
#define	EFIRT_INIT_ARCH32	0x00000002
#define	EFIRT_INIT_ARCH64	0x00000004

#define	EFIRT_BUFLEN	1024
#define	EFIRT_MINOR_NAME "efirt"

struct efirt_state {
	dev_info_t	*efi_dip;	/* Opaque dev_info pointer */
	int		efi_init_state;	/* See EFIRT_INIT_* */
	caddr_t		efi_systab;
	kmutex_t	efi_mutex;	/* driver lock */
	char		*efi_buffer;	/* message buffer */
};

#ifdef __cplusplus
}
#endif

#endif /* _EFIRT_H */
