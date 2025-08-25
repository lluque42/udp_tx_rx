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
	addr_info_hints->ai_flags = AI_PASSIVE;				// This is a wildcar for address, like listen on EVERY address
														// This should prevent binding to 127.0.0.1
														// NEWWWWW
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


/*
I believe I'm getting fucked...

Check ip(7) and is a new world, fucking history
Things get clearer:
           struct sockaddr_in {
               sa_family_t    sin_family; // address family: AF_INET 
               in_port_t      sin_port;   // port in network byte order 
               struct in_addr sin_addr;   // internet address 
           };

           // Internet address 
           struct in_addr {
               uint32_t       s_addr;     //address in network byte order
           };
       #include <sys/socket.h>
       #include <netinet/in.h>
       #include <netinet/ip.h> //superset of previous

       tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
       udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
       raw_socket = socket(AF_INET, SOCK_RAW, protocol);

sin_addr is the IP host address.  The s_addr member of struct in_addr contains the host interface address in  net‐
       work  byte order.  in_addr should be assigned one of the INADDR_* values (e.g., INADDR_LOOPBACK) using htonl(3) or
       set using the inet_aton(3), inet_addr(3), inet_makeaddr(3) library functions or directly with  the  name  resolver
       (see gethostbyname(3)).



Some example I found:


When developing networking software (especially when using the BSD socket interface), it's important to keep things as simple as possible until you've established basic communication. Then you can incrementally add functionality, while making sure that you don't break anything along the way.

On the client side, keeping things simple means

    Don't call bind in the client. The OS will choose an appropriate interface and assign a random port number, so there's no need to bind the socket.

    Use a hard-coded server address (e.g. 127.0.0.1). Address 127.0.0.1 (0x7f000001) is the local host address, suitable for sending packets to a server on the same machine.

    Use a hard-coded port number (e.g. 50037). Ephemeral port numbers should be greater than 0xC000 hex (49152 decimal).

    Use a hard-coded message, e.g. "hello".

With that in mind, here's what the client software looks like

int main( void )
{
    int fd;
    if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket failed");
        return 1;
    }

    struct sockaddr_in serveraddr;
    memset( &serveraddr, 0, sizeof(serveraddr) );
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons( 50037 );
    serveraddr.sin_addr.s_addr = htonl( 0x7f000001 );

    for ( int i = 0; i < 4; i++ ) {
        if (sendto( fd, "hello", 5, 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0 ) {
            perror( "sendto failed" );
            break;
        }
        printf( "message sent\n" );
    }

    close( fd );
}

On the server side, keeping things simple means

    Bind to INADDR_ANY, i.e. let the OS pick an appropriate interface.
    Bind to a hard-coded port, e.g. 50037 (must be the same port the client uses).
    Don't request the address information from recvfrom, i.e. pass NULL, 0 as the last two parameters.

With that in mind, here's what the server software looks like

int main( void )
{
    int fd;
    if ( (fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror( "socket failed" );
        return 1;
    }

    struct sockaddr_in serveraddr;
    memset( &serveraddr, 0, sizeof(serveraddr) );
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons( 50037 );
    serveraddr.sin_addr.s_addr = htonl( INADDR_ANY );

    if ( bind(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0 ) {
        perror( "bind failed" );
        return 1;
    }

    char buffer[200];
    for ( int i = 0; i < 4; i++ ) {
        int length = recvfrom( fd, buffer, sizeof(buffer) - 1, 0, NULL, 0 );
        if ( length < 0 ) {
            perror( "recvfrom failed" );
            break;
        }
        buffer[length] = '\0';
        printf( "%d bytes: '%s'\n", length, buffer );
    }

    close( fd );
}







*/




		// This is: domain aka protocol family (IPv4), protocol type (datagram), 
		// and a number given by the system that hopefully refers to UDP.
		// NOTEL: Said number could be obtained through the getprotobyname()
		// function (that looks for it in /etc/protocols).
		printf("The addr_info->ai_family = %d AF_INET = %d\n",
				addr_info->ai_family, AF_INET);
	//	printf("Trying this getaddrinfo result for the server: '%s'\n",
	//			addr_info->ai_addr->sa_data);

//		addr_info = addr_info->ai_next;
//		continue ;
		sckt = socket(addr_info->ai_family,
						addr_info->ai_socktype,
						addr_info->ai_protocol);
		if (sckt == -1)
		{
			addr_info = addr_info->ai_next;
			continue ;
		}
		printf("Once socket was created: '%s'\n",	addr_info->ai_addr->sa_data);
		if (bind(sckt, addr_info->ai_addr, addr_info->ai_addrlen) != -1)
			break ;
		close(sckt);
	}


	// NEW
	char 	host[100];
	char	service[100];
	getnameinfo(addr_info->ai_addr, addr_info->ai_addrlen,
			host, sizeof(host),
            service, sizeof(service),
            NI_NUMERICHOST | NI_NUMERICSERV);
	printf("%s listening on UDP port %s\n", host, service);
	/* Doesn't work as expected, because of the wildcard AI_PASSIVE:
If you then print it with getnameinfo(), you’ll still see 0.0.0.0.
It doesn’t change into “192.168.1.23” or “127.0.0.1” because the kernel hasn’t chosen a single local address — it’s listening on all IPv4 addresses at once.

 If you want to see the actual NICs and addresses your socket is listening on, you can’t get that from the socket itself. You need to enumerate system interfaces (with getifaddrs() on Linux/BSD, or ioctl(SIOCGIFCONF) in older code). The socket just knows “I’m bound to the wildcard.”
 */
	// NEW



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
		bc_r = recvfrom(sckt, buffer, BUFFER_SIZE,
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
		//printf("peer_addr->sa_data = %s, peer_addrlen = %d\n",
		//		((struct sockaddr *) &peer_addr)->sa_data, peer_addrlen);



		if (bc_r < BUFFER_SIZE)
		{
			buffer[bc_r] = '\0';
			printf("This was read from the socket: %s\n", buffer);
		}
		else
			printf("Reply was too long, increase BUFFER_SIZE (i'm lazy)\n");




		
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


		// The echo
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


