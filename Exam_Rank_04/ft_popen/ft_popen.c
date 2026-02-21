/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_popen.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 21:09:51 by marvin            #+#    #+#             */
/*   Updated: 2026/02/21 21:09:51 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define OPEN_MAX 1024

// a static array to store in data segment PIDs, Singleton pattern
// This function allow to resolve the problem created by the school subject
// it builds a bridge between `ft_popen` and `ft_pclose`, so they can safely share
// the PID wihtout forcing the user to pass it manually.
pid_t	*ft_pid_table(void)
{
	static pid_t	pids[OPEN_MAX];

	return (pids);
}

/**
 * This function pays the debt. it solves the problem of lost PIDs due to
 * bad signature of the code that the school ask us to do..
 */
int	ft_pclose(int fd)
{
	int		status;
	pid_t	pid;

	if (fd < 0 || fd >= OPEN_MAX)
		return (-1);
	pid = ft_pid_table()[fd];
	status = 0;
	close(fd);
	if (pid > 0)
	{
		waitpid(pid, &status, 0);
		ft_pid_table()[fd] = 0;
	}
	return (status);
}


/**
 * PARADIGM: The trick to avoid overheads of ifs is to use the logic of `boolean Coercion`
 * or the so called `Branchless programming`
 * As you might know some of you know me personally and know that the infinite ifs solution
 * is not my cup of tea at all. 
 * 
 * ALGORITHM: the strategy is do a Branchless Programming via Boolean indexing
 * DATASTRUCTURE: system file descriptor
 * TIME: O(1), because we eliminate brancing (if/else statements), the CPU does not have to
 * perform branch prediction. This prevents pipeline flushes at the hardware level, making 
 * the execution strictly linear and fast.
 * SPACE:O(1) --> No dynamic memory allocation is used.
 * CRIT: This version is full packed of protein storing the child's PID in `ft_pid_table()`
 * The verbose version compltely forgets to save teh PID, which guarantees zombie processes.
 * @param file
 * @param argv
 * @param type
 * @return 
 * @note 
 * Let's break down exactly how the concise code eliminates all the noisy if/else
 * blocks from the verbose code
 * 
 * FIRST: Calculating the Parent's Index
 * p_idx =(type == 'w');
 * we need to know which side of the pipe the parent gets.
 * - if `type == 'w'`, the expression is true (`1`). The parent wants to write, so it gets
 * `fd[1]` (the write end) --> p_idx = 1.
 * - if `type == 'r'`, the expression is false(0). The parent want to read, so it gets `fd[0]`
 * (the read end). `p_idx = 0`
 * 
 * SECOND: Calculating the Child's index
 * if the parent uses `p_idx`, the child must use the exact opposite. In C, the logical NOT
 * operator (!)flips 0 to 1 and 1 to 0. 
 * - Child's pipe index is simply: !p_idx.
 * 
 * THIRD: The `dup2` Routing Trick
 * This is where 10 lines of `if else` are crushed into a single line inside the child process
 * `dup2(fd[!p_idx], (type == 'r'))
 * The `dup2` funciton takes two arguments: `dup2(old_fd, target_fd)`.
 * EXAMPLE: let's see how the boolean logic solves both paths instantly
 * |Scenario		|	type	|	Childs		|	Target FD		|	Resulting code	|	Translation												|
 * |parent read		|	'r'		|	!0->fd[1]	|	r == r --> 1	|	dup2(fd[1], 1)	|	child overwrites STDOUT with pipe's write end (fd[1])	|
 * |parent writes	|	'w'		|	!1->fd[0]	|	w == r --> 0	|	dup2(fd[0], 0)	|	child overwrites STDIN with pipe's read end (fd[0])		|
 * 
 * FOURTH:cleanup and return
 * ```c
 * close(fd[p!p_idx]));
 * return (ft[p_idx]);
 * ```
 * instead of an `if/else` to decide which pipe and the parent closes, the parent simply closes
 * the child's end (!p_idx), and returns its own end (p_idx).
 * 
 * IMPORTANT: While the verbose is user friendly and might be easier for beginner
 * it has a fatal flow: it does nto save the pid anywhere. In the verbose version
 * once `ft_open` return the file descriptor, the caller has absolutely no way to know the child's PID
 * making it impossible to `waitpid()` later. Our brancheless version solves this
 * perfectly with `ft_pid_table()[fd[p_idx]] = pid`
 */
int	ft_popen(const char *file, char **const argv, char type)
{
	int		fd[2];
	int		p_idx;
	pid_t	pid;

	if (!file || !argv
		|| (type != 'r' && type != 'w') || pipe(fd) == -1)
		return (-1);
	p_idx = (type == 'w');
	pid = fork();
	if (pid == -1)
		return (close(fd[0]), close(fd[1]), -1);
	if (pid == 0)
	{
		if (dup2(fd[!p_idx], (type == 'r')) == -1)
			exit(127);
		(close(fd[0]), close(fd[1]), execvp(file, argv), exit(127));
	}
	close(fd[!p_idx]);
	ft_pid_table()[fd[p_idx]] = pid;
	return (fd[p_idx]);
}


// surely pass the exam. Not good example of paradigm. Bad taste in design.
__attribute__((weak))
int ft_popen(const char *file, char *const argv[], char type)
{
    int fd[2];
    pid_t pid; 	// [1] 'pid' is declared as a local variable on the stack, meaning that is only exist as long as the function is running.

    if(!file || !argv || (type != 'r' && type != 'w'))
        return(-1);
    
    if(pipe(fd)== -1)
        return(-1);

    pid = fork();	// [2] we successfully create teh child. In the parent process, 'pid' now holds the unique ID of that child
    if(pid == -1)
    {
        close(fd[0]);
        close(fd[1]);
        return(-1);
    }
    if(pid == 0)
    {
		// --- child process logic ---
		// The chidl doesn't care about the PID loss because it just runs and exits
        if(type == 'r')
        {
            if(dup2(fd[1], STDOUT_FILENO) == -1)
                exit(1);
        }
        else
        {
            if(dup2(fd[0], STDIN_FILENO) == -1)
                exit(1);
        }
        close(fd[0]);
        close(fd[1]);
        execvp(file, argv);
        exit(1);
    }
    else
    {
		// Parent process logic : THE FATAL FLOW HAPPENS HERE ---
        if(type == 'r')
        {
            close(fd[1]);
            return(fd[0]); // [3a] We return the fd. As soon as 'return' executes, the local 'pid' variable is destroyed
        }
        else
        {
            close(fd[0]);
            return(fd[1]); //[3b] same here. The FD is returned, and the PID is erased from Memory
        }
		/**
		 * RESULT: The function calling ft_popen gets an integer (The FD)
		 * but it has absolutely no idea what the child's PID was. Because the caller
		 * doesn't have the PID, it cannot call waitpid(pid, ...).
		 * because waitpid() is never called, the child becomes a zombie process when it dies
		 */
    }
}
