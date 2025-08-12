/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   udp_server_main.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lluque <lluque@student.42malaga.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 11:17:07 by lluque            #+#    #+#             */
/*   Updated: 2025/08/12 21:35:25 by lluque           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "udptxrx.h"

int	main(int argc, char **argv)
{
	int						sckt;
	struct addrinfo			*addr_info;
	struct addrinfo			*addr_info_hints;
	char					*rx_port;

	if (argc != 2)
	{
		dprintf(STDERR_FILENO,
				"usage:\n\t%s rx_port\n",
				argv[0]);
		exit (EXIT_FAILURE);
	}
	rx_port = argv[1];
	// This is like the query parameters so the system return only the results
	// that fit. Fields in 0 are "don't cares".
	addr_info_hints = calloc(1, sizeof (struct addrinfo));
	if (addr_info_hints == NULL)
	{
		perror("Calloc'ing");
	}
	addr_info_hints->ai_family = AF_INET;					// IPv4
	addr_info_hints->ai_socktype = SOCK_DGRAM;				// datagram based
	// Notice that it didn't need the ai_protocol.

	// The query to find our address data information. That is, since this
	// is the server, the node is NULL and the service is the rx_port.
	//
	// The system will reply with a list of address-infos that would allow
	// to respond to comunications that arrive at the rx_port in every matching
	// AF_INET SOCK_DGRAM protocol on the system. The address info is what's
	// required to connect or bind a socket.
	if (getaddrinfo(NULL,
					rx_port,
					//NULL, &addr_info))
					addr_info_hints, &addr_info))
	{
		perror("getaddrinfo");
		freeaddrinfo(addr_info_hints);
		exit (EXIT_FAILURE);
	}
	utr_print_addr_info(addr_info);
	
	// For each result from the query, we try to create a socket and then
	// connect to it. If anything fails we move to the next result.
	while (addr_info != NULL)
	{
		// This is: domain aka protocol family (IPv4), protocol type (datagram), 
		// and a number given by the system that hopefully refers to UDP.
		// NOTEL: Said number could be obtained through the getprotobyname()
		// function (that looks for it in /etc/protocols).
		printf("The addr_info->ai_family = %d AF_INET = %d\n",
				addr_info->ai_family, AF_INET);
		sckt = socket(addr_info->ai_family,
						addr_info->ai_socktype,
						addr_info->ai_protocol);
		if (sckt == -1)
		{
			addr_info = addr_info->ai_next;
			continue ;
		}
		if (bind(sckt, addr_info->ai_addr, addr_info->ai_addrlen) != -1)
			break ;
		close(sckt);
	}
	if (addr_info == NULL)
	{
		dprintf(STDERR_FILENO,
				"couldn't get address, create a socket or bind to it\n");
		freeaddrinfo(addr_info_hints);
		close(sckt);
		exit (EXIT_FAILURE);
	}


	char					buffer[BUFFER_SIZE];
	int						bc_r;
	int						bc_w;
	struct sockaddr_storage	peer_addr;		// For sendto()/recvfrom()
	socklen_t				peer_addrlen;	// For the above, we need to find
											// out the actual length

	buffer[BUFFER_SIZE - 1] = '\0';
	while (1)
	{
		//bc_r = read(sckt, buffer, BUFFER_SIZE - 1);
		// The addrlen must be initializaed altough it gets updated
		// later by recvfrom() with the actual length of the response.
		// THIS MADE ME LOSE HOURS.
		peer_addrlen = sizeof (peer_addr);
		bc_r = recvfrom(sckt, buffer, BUFFER_SIZE - 1,
						0,	// No flags
						(struct sockaddr *) &peer_addr, &peer_addrlen);
		if (bc_r == -1)
		{
			// May be check bc against BUFFER_SIZE and flush?
			// freee stuff
			perror("reading from socket");
			exit (EXIT_FAILURE);
		}
		if (bc_r == 0)
			break ;

//		ft_print_memory(&peer_addr, peer_addrlen);
		printf("peer_addr->sa_data = %s, peer_addrlen = %d\n",
				((struct sockaddr *) &peer_addr)->sa_data, peer_addrlen);
		buffer[bc_r] = '\0';
		printf("This was read from the socket: %s\n", buffer);

		
		// The opposite of getaddrinfo() just to retrieve info, I think
		// this appeared in the getaddrinfo() man page
		char	host[NI_MAXHOST], service[NI_MAXSERV];
		int		s;

		s = getnameinfo((struct sockaddr *) &peer_addr,
			   peer_addrlen, host, NI_MAXHOST,
			   service, NI_MAXSERV, NI_NUMERICSERV);
								// NI_NUMERICHOST
		if (s == 0)
	  		printf("Received %d bytes from %s:%s\n", bc_r, host, service);
		else
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));
		printf("&peer_addr = %p, peer_addrlen = %d\n",
				(char *)&peer_addr, peer_addrlen);



		//bc_w = write(sckt, buffer, bc_r);
		bc_w = sendto(sckt, buffer, bc_r,
						0,	// No flags
						(struct sockaddr *) &peer_addr, peer_addrlen);
		//if (bc_w == -1)
		if (bc_w != bc_r)
		{
			// May be check bc against BUFFER_SIZE and flush?
			// freee stuff
			perror("writing to socket");
			exit (EXIT_FAILURE);
		}
	}
	printf("Bye!\n");









	freeaddrinfo(addr_info);
	freeaddrinfo(addr_info_hints);

	if (close (sckt) == -1)
		perror("closing socket");
	return (EXIT_SUCCESS);
}
	// Apparently not needed, we use the one that let us connect...
	// Ask the system what number it knows the udp protocol 
//	struct protoent	*protocol_data;
//
//	protocol_data = getprotobyname("udp");
//
//	if (protocol_data == NULL)
//		perror("getprotobyname");
//
//
//	printf("UDP has this protocol number %d\n", protocol_data->p_proto);
//
//addr_info_hints->ai_protocol = protocol_data->p_proto;	// The nbr. for UDP


