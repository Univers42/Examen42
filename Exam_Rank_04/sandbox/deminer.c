#define _GNU_SOURCE

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>

/* --- SANDBOX ENGINE --- */

static void watchdog_handler(int sig)
{
    (void)sig;
}

static void setup_watchdog(unsigned int timeout)
{
    struct sigaction sa;

    sigemptyset(&sa.sa_mask);
    sa.sa_handler = watchdog_handler;
    sa.sa_flags = 0; // Crucial: ensure waitpid is interrupted
    sigaction(SIGALRM, &sa, NULL);
    alarm(timeout);
}

static void log_res(int st, bool to, unsigned int t, bool v)
{
    if (!v) return;
    if (to)
        printf("Bad function: timed out after %u seconds\n", t);
    else if (WIFEXITED(st) && WEXITSTATUS(st) == 0)
        printf("Nice function!\n");
    else if (WIFEXITED(st))
        printf("Bad function: exited code %d\n", WEXITSTATUS(st));
    else if (WIFSIGNALED(st))
        printf("Bad function: %s\n", strsignal(WTERMSIG(st)));
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose)
{
    pid_t pid;
    int st;
    bool to;

    pid = fork();
    if (pid == -1) return -1;
    if (pid == 0)
    {
        /* 1. LIMIT PROCESSES: Child can only have 10 processes total */
        struct rlimit rl;
        rl.rlim_cur = 100000;
        rl.rlim_max = 100000;
        setrlimit(RLIMIT_NPROC, &rl);

        /* 2. ISOLATE GROUP: Child becomes leader of its own process group */
        setpgid(0, 0); 

        f();
        exit(0);
    }
    setup_watchdog(timeout);
    to = (waitpid(pid, &st, 0) == -1 && errno == EINTR);
    if (to)
    {
        /* 3. TACTICAL NUKE: Kill the entire process group (-pid) */
        kill(-pid, SIGKILL);
        waitpid(pid, &st, 0);
    }
    alarm(0);
    log_res(st, to, timeout, verbose);
    return (!to && WIFEXITED(st) && WEXITSTATUS(st) == 0);
}

/* --- THE EVIL FUNCTIONS --- */

void evil_ignore_alarm(void)
{
    printf("    [CHILD] I am ignoring SIGALRM and hanging!\n");
    signal(SIGALRM, SIG_IGN);
    while (1);
}

void forkbomb_lite(void)
{
    printf("    [CHILD] I am hiding a grandchild in the background!\n");
    if (fork() == 0)
        while(1);
    while(1);
}

/**
 * DANGEROUS: Exponential growth
 * 1 process becomes 2m4,8,16,32..
 * PID exhaustion: within a fraction of a second, we hit the system's MAX_PID (usually 32,768)
 * OS paralysis: because the OS needs to spawn a process just to run the `ls` or `kill` command,
 * and there are no PIDs, left, we can't even type in our terminal
 * kernel panic/freeze: The CPU spends 100% of its time context-switching between thousands
 * of identical processes
 */
void real_forkbomb(void)
{
    printf("    [CHILD] INITIATING REAL FORK BOMB!\n");
    while (1)
        fork();
}

void nightmare_forkbomb(void)
{
	pid_t			pid;
	volatile int	i;

	printf("	[CHILD] INITATING HEAVY FORK BOMB...");
	while (1)
	{
		pid = fork();
		if (pid == 0)
		{
			while (1)
			{
				i = 0;
				while (i < 1000000)
					i++;
			}
		}
	}
}
/* --- MAIN --- */

int main(void)
{
    printf("=== Test 1: Evil Ignore Alarm ===\n");
    sandbox(evil_ignore_alarm, 2, true);

    printf("\n=== Test 2: Forkbomb Lite (Checking for orphans...) ===\n");
    sandbox(forkbomb_lite, 2, true);

    printf("\n=== Test 3: THE REAL FORK BOMB ===\n");
    sandbox(real_forkbomb, 10, true);

	printf("\n=== Test 3: Final Boss ===\n");
	sandbox(nightmare_forkbomb, 5, true);
    printf("\n=== All Malicious Tests Neutralized! ===\n");
    return (0);
}