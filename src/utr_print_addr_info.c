/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utr_print_addr_info.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lluque <lluque@student.42malaga.com>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/10 11:17:07 by lluque            #+#    #+#             */
/*   Updated: 2025/08/10 20:00:22 by lluque           ###   ########.fr       */
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
