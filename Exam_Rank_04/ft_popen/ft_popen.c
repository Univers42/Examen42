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
pid_t	*ft_pid_table(void)
{
	static pid_t	pids[OPEN_MAX];

	return (pids);
}

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
 * ft_popen 
 * @param file
 * @param argv
 * @param type
 * @return 
 * @note ft_pid_table()[fd[p_idx]] = pid <-- the Magic happens here
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
