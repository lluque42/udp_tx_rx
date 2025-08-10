/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   udp_client_main.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lluque <lluque@student.42malaga.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 11:17:07 by lluque            #+#    #+#             */
/*   Updated: 2025/08/10 19:42:16 by lluque           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "udptxrx.h"

void	utr_print_addr_info(struct addrinfo *addr_info)
{
	while  (1)
	{
		printf("Address information:\n");
		printf("\
                ai_flags = %d\n\
                ai_family = %d\n\
                ai_socktype = %d\n\
                ai_protocol = %d\n\
                ai_addrlen = %d\n\
                ai_addr = %p\n\
                ai_canonname = %s\n\n",
				addr_info->ai_flags,
               addr_info->ai_family,
               addr_info->ai_socktype,
               addr_info->ai_protocol,
               addr_info->ai_addrlen,
               addr_info->ai_addr,
               addr_info->ai_canonname);
		if (addr_info->ai_next == NULL)
			break ;
		addr_info = addr_info->ai_next;
	}
}

int	main(int argc, char **argv)
{
	int				sckt;
	struct addrinfo	*addr_info;
	struct addrinfo	*addr_info_hints;
	char			*other_end_address;
	char			*tx_port;

	if (argc != 3)
	{
		dprintf(STDERR_FILENO,
				"usage:\n\t%s other_end_address tx_port\n",
				argv[0]);
		exit (EXIT_FAILURE);
	}
	other_end_address = argv[1];
	tx_port = argv[2];
	// This is like the query parameters so the system return only the results
	// that fit. Fields in 0 are "don't cares".
	addr_info_hints = calloc(1, sizeof (struct addrinfo));
	addr_info_hints->ai_family = AF_INET;					// IPv4
	addr_info_hints->ai_socktype = SOCK_DGRAM;				// datagram based
	// Notice that it didn't need the ai_protocol.

	// The query to find our address data information. That is, since this
	// is the client, the node is the other_end_address and the service
	// is the tx_port.
	//
	// The system will reply with a list of address-infos that would allow
	// to comunicate with the other end from the tx_port in every matching
	// AF_INET SOCK_DGRAM protocol on the system. The address info is what's
	// required to connect or bind a socket.
	if (getaddrinfo(other_end_address,
					tx_port,
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
		sckt = socket(addr_info->ai_family,
						addr_info->ai_socktype,
						addr_info->ai_protocol);
		if (sckt == -1)
		{
			addr_info = addr_info->ai_next;
			continue ;
		}
		if (connect(sckt, addr_info->ai_addr, addr_info->ai_addrlen) != -1)
			break ;
		close(sckt);
	}
	if (addr_info == NULL)
	{
		dprintf(STDERR_FILENO,
				"couldn't get address, create a socket or connect to it\n");
		freeaddrinfo(addr_info_hints);
		close(sckt);
		exit (EXIT_FAILURE);
	}


	char			buffer[BUFFER_SIZE];
	int				bc_r;
	int				bc_w;

	buffer[BUFFER_SIZE - 1] = '\0';
	while (1)
	{
		bc_r = read(STDIN_FILENO, buffer, BUFFER_SIZE - 1);
		if (bc_r == -1)
		{
			// May be check bc against BUFFER_SIZE and flush?
			// freee stuff
			perror("reading from stdin");
			exit (EXIT_FAILURE);
		}
		if (bc_r == 0)
			break ;
		bc_w = write(sckt, buffer, bc_r);
		if (bc_w == -1)
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


