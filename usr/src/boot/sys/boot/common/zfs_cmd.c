/*
 * Copyright (c) 2018 Warner Losh <imp@freebd.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>

/*
 * MD bootstrap main() and assorted miscellaneous
 * commands.
 */

#include <stand.h>
#include <sys/stddef.h>
#include <sys/disk.h>
#include <sys/reboot.h>

#include "bootstrap.h"

#include "libzfs.h"

#ifndef BOOT2
COMMAND_SET(lszfs, "lszfs", "list child datasets of a zfs dataset",
	    command_lszfs);

static int
command_lszfs(int argc, char *argv[])
{
	int err;

	if (argc != 2) {
		command_errmsg = "a single dataset must be supplied";
		return (CMD_ERROR);
	}

	err = zfs_list(argv[1]);
	if (err != 0) {
		command_errmsg = strerror(err);
		return (CMD_ERROR);
	}
	return (CMD_OK);
}
#endif

uint64_t
ldi_get_size(void *priv)
{
	int fd = (uintptr_t) priv;
	uint64_t size;

	ioctl(fd, DIOCGMEDIASIZE, &size);
	return (size);
}
