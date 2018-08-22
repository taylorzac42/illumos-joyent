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
 * Copyright 2018 Joyent, Inc.
 */

/*
 * Some basic pthread name API tests.
 */

#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <err.h>

/*ARGSUSED*/
static void *
thr(void *unused)
{
	(void) sleep(100);
	return (NULL);
}

/*ARGSUSED*/
int
main(int argc, char *argv[])
{
	char name[PTHREAD_MAX_NAMELEN_NP];
	pthread_attr_t attr;
	pthread_t tid;
	int test;
	int rc;

	/* Default thread name is empty string. */
	test = 1;

	rc = pthread_getname_np(pthread_self(), name, sizeof (name));

	if (rc != 0 || strcmp(name, "") != 0)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	/* Can set name. */
	test = 2;

	(void) strlcpy(name, "main", sizeof (name));
	rc = pthread_setname_np(pthread_self(), name);

	if (rc != 0)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	rc = pthread_getname_np(pthread_self(), name, sizeof (name));

	if (rc != 0 || strcmp(name, "main") != 0)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	/* ERANGE check. */
	test = 3;

	rc = pthread_getname_np(pthread_self(), name, 2);

	if (rc != ERANGE)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	/* EINVAL check. */
	test = 4;

	rc = pthread_getname_np(pthread_self(), NULL, sizeof (name));

	if (rc != EINVAL)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	/* can clear thread name. */
	test = 5;

	rc = pthread_setname_np(pthread_self(), NULL);

	if (rc != 0)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	rc = pthread_getname_np(pthread_self(), name, sizeof (name));

	if (rc != 0 || strcmp(name, "") != 0)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	/* non-existent thread check. */
	test = 6;

	rc = pthread_getname_np(808, name, sizeof (name));

	if (rc != ESRCH)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	rc = pthread_setname_np(808, "state");

	if (rc != ESRCH)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	/* too long a name. */
	test = 7;

	rc = pthread_setname_np(pthread_self(),
	    "12345678901234567890123456789012");

	if (rc != ERANGE)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	/* can name another thread. */
	test = 8;

	rc = pthread_create(&tid, NULL, thr, NULL);

	if (rc != 0)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	rc = pthread_setname_np(tid, "otherthread");

	if (rc != 0)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	/* attr tests. */
	test = 9;

	(void) pthread_attr_init(&attr);

	rc = pthread_attr_setname_np(&attr,
	    "12345678901234567890123456789012");

	if (rc != ERANGE)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	rc = pthread_attr_setname_np(&attr, "thread2");

	if (rc != 0)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	rc = pthread_attr_getname_np(&attr, NULL, sizeof (name));

	if (rc != EINVAL)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	rc = pthread_attr_getname_np(&attr, name, 2);

	if (rc != ERANGE)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	/* does the attr actually apply? */
	test = 10;

	rc = pthread_create(&tid, &attr, thr, NULL);

	if (rc != 0)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	rc = pthread_getname_np(tid, name, sizeof (name));

	if (rc != 0 || strcmp(name, "thread2") != 0)
		errx(EXIT_FAILURE, "test %d failed with %d", test, rc);

	return (EXIT_SUCCESS);
}
