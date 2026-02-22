/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sandbox.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/22 01:23:30 by marvin            #+#    #+#             */
/*   Updated: 2026/02/22 01:23:30 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>

/* Use sigaction and alarm as authorized */
static void watchdog_handler(int sig)
{
	(void)sig;
}

static void setup_watchdog(unsigned int timeout)
{
	struct sigaction sa;

	/* Using only authorized sig functions */
	sigemptyset(&sa.sa_mask);
	sa.sa_handler = watchdog_handler;
	sa.sa_flags = 0;
	sigaction(SIGALRM, &sa, NULL);
	alarm(timeout);
}

static void log_res(int st, bool to, unsigned int t, bool v)
{
	if (!v)
		return ;
	if (to)
		printf("Bad function: timed out after %u seconds\n", t);
	else if (WIFEXITED(st) && WEXITSTATUS(st) == 0)
		printf("Nice function!\n");
	else if (WIFEXITED(st))
		printf("Bad function: exited code %d\n", WEXITSTATUS(st));
	else if (WIFSIGNALED(st))
		/* strsignal is authorized! */
		printf("Bad function: %s\n", strsignal(WTERMSIG(st)));
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
	pid_t pid;
	int   st;
	bool  to;

	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid == 0)
	{
		/* We cannot use setrlimit or setpgid here anymore */
		f();
		exit(0);
	}
	setup_watchdog(timeout);
	/* waitpid is authorized */
	to = (waitpid(pid, &st, 0) == -1 && errno == EINTR);
	if (to)
	{
		/* kill is authorized, but we can only target the direct child */
		kill(pid, SIGKILL);
		waitpid(pid, &st, 0);
	}
	alarm(0);
	log_res(st, to, timeout, verbose);
	return (!to && WIFEXITED(st) && WEXITSTATUS(st) == 0);
}