/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   mini_serv.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/07/27 12:00:00 by dlesieur          #+#    #+#             */
/*   Updated: 2026/07/27 12:00:00 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

void	send_all(fd_set *sets, int except, char *sb)
{
	int	fd;

	fd = -1;
	while (++fd < 1024)
	{
		if (FD_ISSET(fd, &sets[2]) && fd != except)
			send(fd, sb, strlen(sb), MSG_NOSIGNAL);
	}
}

int	create_server(int ac, char **av, fd_set *sets, char (**msgp)[100000])
{
	struct sockaddr_in	addr;
	int					sock;

	if (ac != 2)
	{
		write(2, "Wrong number of arguments\n", 26);
		exit(1);
	}
	sock = socket(AF_INET, SOCK_STREAM, 0);
	*msgp = malloc(1024 * sizeof(**msgp));
	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(2130706433);
	addr.sin_port = htons(atoi(av[1]));
	if (!*msgp || sock < 0 || bind(sock, (void *)&addr, sizeof(addr)) < 0
		|| listen(sock, 128) < 0)
	{
		write(2, "Fatal error\n", 12);
		exit(1);
	}
	FD_ZERO(&sets[0]);
	FD_SET(sock, &sets[0]);
	return (sock);
}

int	client_event(int fd, fd_set *sets, int *id, char (*msg)[100000])
{
	static int	nid;
	char		sb[64];
	int			sock;

	sock = 0;
	while (!FD_ISSET(sock, sets))
		sock++;
	if (fd == sock)
	{
		fd = accept(sock, 0, 0);
		if (fd < 0)
			return (0);
		id[fd] = nid++;
		msg[fd][0] = 0;
		FD_SET(fd, sets);
		sprintf(sb, "server: client %d just arrived\n", id[fd]);
		send_all(sets, fd, sb);
		return (0);
	}
	sprintf(sb, "server: client %d just left\n", id[fd]);
	send_all(sets, fd, sb);
	FD_CLR(fd, sets);
	close(fd);
	return (0);
}

int	handle_msg(int fd, fd_set *sets, int *id, char (*msg)[100000])
{
	char	sb[100120];
	char	rb[65536];
	int		ret;
	int		i;
	int		j;

	ret = recv(fd, rb, sizeof(rb), 0);
	if (ret <= 0)
		return (client_event(fd, sets, id, msg));
	i = -1;
	j = strlen(msg[fd]);
	while (++i < ret)
	{
		msg[fd][j] = rb[i];
		if (rb[i] == '\n')
		{
			msg[fd][j] = 0;
			sprintf(sb, "client %d: %s\n", id[fd], msg[fd]);
			send_all(sets, fd, sb);
			j = -1;
		}
		j++;
	}
	msg[fd][j] = 0;
	return (0);
}

int	main(int ac, char **av)
{
	fd_set	sets[3];
	char	(*msg)[100000];
	int		id[1024];
	int		sock;
	int		fd;

	sock = create_server(ac, av, sets, &msg);
	while (1)
	{
		sets[1] = sets[0];
		sets[2] = sets[0];
		if (select(1024, &sets[1], &sets[2], 0, 0) < 0)
			continue ;
		fd = -1;
		while (++fd < 1024)
		{
			if (!FD_ISSET(fd, &sets[1]))
				continue ;
			if (fd == sock)
				client_event(fd, sets, id, msg);
			else
				handle_msg(fd, sets, id, msg);
			break ;
		}
	}
}
