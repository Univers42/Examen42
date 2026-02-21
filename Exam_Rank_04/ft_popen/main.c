/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/21 21:23:00 by marvin            #+#    #+#             */
/*   Updated: 2026/02/21 21:23:00 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Forward declarations of the functions of the file.
pid_t		*ft_pid_table(void);
int			ft_pclose(int fd);
int			ft_popen(const char *file, char **const argv, char type);

void	test_read_mode(void)
{
	char	*argv[] = {"echo", "Success: Read from child!", NULL};
	char	buffer[128];
	int		bytes_read;
	int		fd;

	printf("--- Testing Read Mode ('r') ---\n");
	fd = ft_popen("echo", argv, 'r');
	if (fd < 0)
	{
		printf("ft_popen failed\n");
		return ;
	}
	bytes_read = read(fd, buffer, sizeof(buffer) - 1);
	if (bytes_read > 0)
	{
		buffer[bytes_read] = '\0';
		printf("Parent read: %s", buffer);
	}
	ft_pclose(fd);
}

void	test_write_mode(void)
{
	char		*argv[] = {"cat", "-e", NULL};
	char		*msg = "Success: Wrote to child!\n";
	int			fd;

	printf("--- Testing Write Mode ('w') ---\n");
	fd = ft_popen("cat", argv, 'w');
	if (fd < 0)
	{
		printf("ft_popen failed\n");
		return ;
	}
	write(fd, msg, strlen(msg));
	ft_pclose(fd);
}

// norminette &&  cc -Wall -Wextra -Werror *.c -o quick_test
__attribute__((weak))
int	main(void)
{
	test_read_mode();
	printf("\n");
	test_write_mode();
	printf("\nTests complete. Sleeping for 5s so you"
		"can check 'ps aux | grep Z' ...\n");
	sleep(5);
	return (0);
}
