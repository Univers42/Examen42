/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   picoshell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 22:42:50 by marvin            #+#    #+#             */
/*   Updated: 2026/02/21 22:42:50 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

static void child_node(char **cmd, int in, int *fd, int state)
{
	if (!(state & 2) && in != -1)
		(dup2(in, 0), close(in));
	if (!(state & 1))
		(dup2(fd[1], 1), close(fd[1]), close(fd[0]));
	(execvp(cmd[0], cmd), exit(1));
}

static int	pipe_sync(int in, int *fd, int state)
{
	if (in != 1)
		close(in);
	if (!(state & 1))
		return (close(fd[1]), fd[0]);
	return (-1);
}

static int wait_all(void)
{
	int	status;
	int	err;

	err = 0;
	while (wait(&status) != -1)
		if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
			err = 1;
	return (err);
}

/**
 * This implementation discards the messy "spaghetti code" of typical pipelines
 * where students write endless
 * ```pseudo-code
 * if (first_cmd)
 * else if (middle_cmd)
 * else if (last_cmd)
 * ```
 * in favor of an `Iterative Pipeline Automaton` driven  by bitwise logic.
 * DSA: implicit Linked List via OS pipes
 * We are not building a struct-based linked list in mem. Instead, we are treating the 
 * OS kernel's fd as the `pointers` linking our preocesses together.
 * - The `in` variables acts as a `prev` pointer (carrying the read-end of the prev pipe)
 * - The `fd[2]` array acts as the `next` pointer (generating  the connection to the nbext process)
 * `pipe_sync()` advance teh pointer` by closing the write-end and returning the read-end
 * for the next loop iteration
 * 
 * ALGORITHM: Bitwise state machine
 * The magice trick of this implementation lies in thi single line
 * ```
 * state = ((i == 0) << 1) | (cmds[i + 1] == NULL);
 * ```
 * Instead of branching logic, we compress the pipeline position into a 2-bit state
 * 
 * |	position	|	(i == 0)	|	(cmds[i + 1] == NULL)	|	bitwise MATH	|	Result	|	binary |
 * |	Middle cmd	|	false(0)	|	false(0)				|	(0 << 1) | 0	|	0		|	00
 * |	last cmd	|	false(0)	|	true(1)					|	(0 << 1) | 1	|	1		|	01
 * |	First cmd	|	true(1)		|	false(0)				|	(1 << 1) | 0	|	2		|	10
 * |	Only cmd	|	true(1)		|	true(1)					|	(1 << 1) | 1	|	3		|	11
 * 
 * EXECUTION:
 * The logic is by checking the bits, the helper function instantly know what to do without
 * needing to know the exact state:
 * - Checking bit 1 (!(state & 2)): "Am I not the first command?" if true, the child must
 * replace `STDIN` with the `in` fd from the previous pipe
 * - Checking Bit 0 (!(state & 1)): "Am I not the last command ?"
 * 		if true, the parent msut create a `pipe(fd)`, and hte child must replace `STDOUT`
 * 		with the pipe's write-end `fd[1]`
 * MEMORY:
 * O(1) there are absolutely no `malloc` calls. Everything operates on the stack and directly
 * in kernel space. Memory leaks are impossible.
 * PERFORMANCE: This function ony manage the loop. Child node manages `dup2`. pipe_sync
 * manages parent cleanup. this makes debugging incredibly easy.
 * @param cmds 
 * @note
 * 
 */
int	picoshell(char **cmds[])
{
	int		fd[2];
	int		i;
	int		in;
	int 	state;
	pid_t	pid;

	if (!cmds)
		return (0);
	i = 0;
	in = -1;
	while (cmds[i])
	{
		state = ((i == 0) << 1) | (cmds[i + 1] == NULL);
		if (!(state & 1) && pipe(fd) == -1)
			return (1);
		pid = fork();
		if (pid == -1)
			return (1);
		if (pid == 0)
			child_node(cmds[i], in, fd, state);
		in = pipe_sync(in, fd, state);
		i++;
	}
	return (wait_all());
}

/* --- 3. PARSING TOOLS --- */
static char **ft_split(char const *s, char c)
{
	char    **r;
	int     i = 0, j = 0, k;

	r = malloc(1024 * sizeof(char*));
	if (!s || !r)
		return (NULL);
	while (s[i])
	{
		while (s[i] == c)
			i++;
		if (!s[i])
			break;
		
		/* FIXED: Use a while loop to safely count the word length */
		k = 0;
		while (s[i + k] && s[i + k] != c)
			k++;
		
		r[j] = malloc(k + 1);
		for (k = 0; s[i] && s[i] != c; k++)
			r[j][k] = s[i++];
		r[j++][k] = '\0';
	}
	r[j] = NULL;
	return (r);
}


/* --- 4. HARDCODED TESTER --- */
void prog_picoshell(void)
{
	int status;
	char *cmd1[] = {"ls", "-la", NULL};
	char *cmd2[] = {"grep", "c", NULL};
	char *cmd3[] = {"wc", "-l", NULL};
	char **cmds[] = {cmd1, cmd2, cmd3, NULL};
	
	printf("=== Testing Picoshell Pipeline ===\n");
	printf("Simulating command: ls -la | grep \"c\" | wc -l\n\n");
	
	/* FIXED: Cast cmds to (char ***) to prevent compiler warnings */
	status = picoshell((char ***)cmds);
	
	/* FIXED: Spelling of 'with' */
	printf("\nPipeline finished with exit status: %d\n", status);
}

static char	***build_cmds(int argc, char **argv)
{
	char	***cmds;
	int		i;
	int		num_cmds;

	num_cmds = argc - 3; // Total arguments minus (program, infile, outfile)
	cmds = malloc(sizeof(char **) * (num_cmds + 1));
	if (!cmds)
		return (NULL);
	i = 0;
	while (i < num_cmds)
	{
		// Commands start at argv[2]
		cmds[i] = ft_split(argv[i + 2], ' ');
		i++;
	}
	cmds[i] = NULL;
	return (cmds);
}

__attribute__((weak))
int	main(int argc, char **argv)
{
	int		fd_in;
	int		fd_out;
	char	***cmds;

	if (argc < 5)
		return (write(2, "Usage: ./pipex inf cmd1 ... cmdN out\n", 37), 1);
	
	// Open files
	fd_in = open(argv[1], O_RDONLY);
	fd_out = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);
	
	// Handle /dev/null or file errors
	if (fd_in < 0 || fd_out < 0)
		return (perror("File Error"), 1);

	// Wire up the pipeline
	dup2(fd_in, STDIN_FILENO);
	dup2(fd_out, STDOUT_FILENO);
	close(fd_in);
	close(fd_out);

	cmds = build_cmds(argc, argv);
	return (picoshell(cmds));
}