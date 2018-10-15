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
 * Copyright (c) 2018, Joyent, Inc.
 */

#include <sys/systm.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/priv.h>
#include <sys/brand.h>
#include <sys/cmn_err.h>
#include <sys/lx_brand.h>
#include <sys/lx_impl.h>
#include <sys/lx_misc.h>
#include <lx_signum.h>

#define	LX_PR_SET_PDEATHSIG		1
#define	LX_PR_GET_PDEATHSIG		2
#define	LX_PR_GET_DUMPABLE		3
#define	LX_PR_SET_DUMPABLE		4
#define	LX_PR_GET_UNALIGN		5
#define	LX_PR_SET_UNALIGN		6
#define	LX_PR_GET_KEEPCAPS		7
#define	LX_PR_SET_KEEPCAPS		8
#define	LX_PR_GET_FPEMU			9
#define	LX_PR_SET_FPEMU			10
#define	LX_PR_GET_FPEXC			11
#define	LX_PR_SET_FPEXC			12
#define	LX_PR_GET_TIMING		13
#define	LX_PR_SET_TIMING		14
#define	LX_PR_SET_NAME			15
#define	LX_PR_GET_NAME			16
#define	LX_PR_GET_ENDIAN		19
#define	LX_PR_SET_ENDIAN		20
#define	LX_PR_GET_SECCOMP		21
#define	LX_PR_SET_SECCOMP		22
#define	LX_PR_CAPBSET_READ		23
#define	LX_PR_CAPBSET_DROP		24
#define	LX_PR_GET_TSC			25
#define	LX_PR_SET_TSC			26
#define	LX_PR_GET_SECUREBITS		27
#define	LX_PR_SET_SECUREBITS		28
#define	LX_PR_SET_TIMERSLACK		29
#define	LX_PR_GET_TIMERSLACK		30
#define	LX_PR_TASK_PERF_EVENTS_DISABLE	31
#define	LX_PR_TASK_PERF_EVENTS_ENABLE	32
#define	LX_PR_MCE_KILL			33
#define	LX_PR_MCE_KILL_GET		34
#define	LX_PR_SET_MM			35
#define	LX_PR_SET_CHILD_SUBREAPER	36
#define	LX_PR_GET_CHILD_SUBREAPER	37
#define	LX_PR_SET_NO_NEW_PRIVS		38
#define	LX_PR_GET_NO_NEW_PRIVS		39
#define	LX_PR_GET_TID_ADDRESS		40
#define	LX_PR_SET_THP_DISABLE		41
#define	LX_PR_GET_THP_DISABLE		42

long
lx_prctl(int opt, uintptr_t data)
{
	long err;
	char ebuf[64];

	switch (opt) {
	case LX_PR_GET_DUMPABLE: {
		/* Only track in brand data - could hook into SNOCD later */
		lx_proc_data_t *lxpd;
		int val;

		mutex_enter(&curproc->p_lock);
		VERIFY((lxpd = ptolxproc(curproc)) != NULL);
		val = lxpd->l_flags & LX_PROC_NO_DUMP;
		mutex_exit(&curproc->p_lock);

		return (val == 0);
	}

	case LX_PR_SET_DUMPABLE: {
		lx_proc_data_t *lxpd;

		if (data != 0 && data != 1) {
			return (set_errno(EINVAL));
		}

		mutex_enter(&curproc->p_lock);
		VERIFY((lxpd = ptolxproc(curproc)) != NULL);
		if (data == 0) {
			lxpd->l_flags |= LX_PROC_NO_DUMP;
		} else {
			lxpd->l_flags &= ~LX_PROC_NO_DUMP;
		}
		mutex_exit(&curproc->p_lock);

		return (0);
	}

	case LX_PR_GET_SECUREBITS: {
		/* Our bits are always 0 */
		return (0);
	}

	case LX_PR_SET_SECUREBITS: {
		/* Ignore setting any bits from arg2 */
		return (0);
	}

	case LX_PR_SET_KEEPCAPS: {
		/*
		 * The closest illumos analog to SET_KEEPCAPS is the PRIV_AWARE
		 * flag.  There are probably some cases where it's not exactly
		 * the same, but this will do for a first try.
		 */
		if (data == 0) {
			err = setpflags(PRIV_AWARE_RESET, 1, NULL);
		} else {
			err = setpflags(PRIV_AWARE, 1, NULL);
		}

		if (err != 0) {
			return (set_errno(err));
		}
		return (0);
	}

	case LX_PR_GET_NAME: {
		/*
		 * We allow longer thread names than Linux for compatibility
		 * with other OSes (Solaris, NetBSD) that also allow larger
		 * names.  We just truncate (with NUL termination) if
		 * the name is longer.
		 */
		char name[LX_PR_SET_NAME_NAMELEN] = { 0 };
		kthread_t *t = curthread;

		mutex_enter(&ttoproc(t)->p_lock);
		if (t->t_name != NULL) {
			(void) strlcpy(name, t->t_name, sizeof (name));
		}
		mutex_exit(&ttoproc(t)->p_lock);

		/*
		 * FWIW, the prctl(2) manpage says that the user-supplied
		 * buffer should be at least 16 (LX_PR_SET_NAME_NAMELEN) bytes
		 * long.
		 */
		if (copyout(name, (void *)data, LX_PR_SET_NAME_NAMELEN) != 0) {
			return (set_errno(EFAULT));
		}
		return (0);
	}

	case LX_PR_SET_NAME: {
		char name[LX_PR_SET_NAME_NAMELEN] = { 0 };
		kthread_t *t = curthread;
		proc_t *p = ttoproc(t);
		int ret;

		ret = copyinstr((const char *)data, name, sizeof (name), NULL);
		/*
		 * prctl(2) explicitly states that over length strings are
		 * silently truncated
		 */
		if (ret != 0 && ret != ENAMETOOLONG) {
			return (set_errno(EFAULT));
		}
		name[LX_PR_SET_NAME_NAMELEN - 1] = '\0';

		if ((ret = thread_setname(t, name)) != 0) {
			return (set_errno(ret));
		}

		/*
		 * In Linux, PR_SET_NAME sets the name of the thread, not the
		 * process.  Due to the historical quirks of Linux's asinine
		 * thread model, this name is effectively the name of the
		 * process (as visible via ps(1)) if the thread is the first of
		 * its task group.  The first thread is therefore special, and
		 * to best mimic Linux semantics we set the thread name, and if
		 * we are setting LWP 1, we also update the name of the process.
		 */
		if (t->t_tid != 1) {
			return (0);
		}

		/*
		 * We are currently choosing to not allow an empty thread
		 * name to clear p->p_user.u_comm and p->p_user.u_psargs.
		 * This is a slight divergence from linux behavior (which
		 * allows this) so that we can preserve the original command.
		 */
		if (strlen(name) == 0) {
			return (0);
		}

		/*
		 * We explicitly use t->t_name here instead of name in case
		 * a thread has come in between the above thread_setname()
		 * call and the setting of u_comm/u_psargs below.  On Linux,
		 * one can also change the name of a thread (either itself or
		 * another thread in the same process) via writing to /proc, so
		 * while racy, this is no worse than what might happen on
		 * Linux.
		 */
		mutex_enter(&p->p_lock);
		(void) strncpy(p->p_user.u_comm, t->t_name, MAXCOMLEN + 1);
		(void) strncpy(p->p_user.u_psargs, t->t_name, PSARGSZ);
		mutex_exit(&p->p_lock);
		return (0);
	}

	case LX_PR_GET_PDEATHSIG: {
		int sig;
		lx_proc_data_t *lxpd;

		mutex_enter(&curproc->p_lock);
		VERIFY((lxpd = ptolxproc(curproc)) != NULL);
		sig = lxpd->l_parent_deathsig;
		mutex_exit(&curproc->p_lock);

		return (sig);
	}

	case LX_PR_SET_PDEATHSIG: {
		int sig = lx_ltos_signo((int)data, 0);
		proc_t *pp = NULL;
		lx_proc_data_t *lxpd;

		if (sig == 0 && data != 0) {
			return (set_errno(EINVAL));
		}

		mutex_enter(&pidlock);
		/* Set signal on our self */
		mutex_enter(&curproc->p_lock);
		VERIFY((lxpd = ptolxproc(curproc)) != NULL);
		lxpd->l_parent_deathsig = sig;
		pp = curproc->p_parent;
		mutex_exit(&curproc->p_lock);

		/* Configure parent to potentially signal children on death */
		mutex_enter(&pp->p_lock);
		if (PROC_IS_BRANDED(pp)) {
			VERIFY((lxpd = ptolxproc(pp)) != NULL);
			/*
			 * Mark the parent as having children which wish to be
			 * signaled on death of parent.
			 */
			lxpd->l_flags |= LX_PROC_CHILD_DEATHSIG;
		} else {
			/*
			 * If the parent is not a branded process, the needed
			 * hooks to facilitate this mechanism will not fire
			 * when it dies. We lie about success in this case.
			 */
			/* EMPTY */
		}
		mutex_exit(&pp->p_lock);
		mutex_exit(&pidlock);
		return (0);
	}

	case LX_PR_CAPBSET_DROP: {
		/*
		 * On recent versions of Linux the login svc drops capabilities
		 * and if that fails the svc dies and is restarted by systemd.
		 * For now we pretend dropping capabilities succeeded.
		 */
		return (0);
	}

	default:
		break;
	}

	(void) snprintf(ebuf, 64, "prctl option %d", opt);
	lx_unsupported(ebuf);
	return (set_errno(EINVAL));
}
