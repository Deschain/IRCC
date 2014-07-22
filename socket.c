/*
 * Copyright (C) 2012 Óscar <tierrabaldia@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is product of the practical assignment for the subject Computer
 * Networks II from Universidad Carlos III de Madrid in the hope that it will
 * make pass the assignment. If it gets distributed it will be WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 */

#include "common.h"
#include "socket.h"

#include "DCC.h"

int open_socket(char *server, int port, int *sd){

	struct hostent *hp;
	struct sockaddr_in addr;

	*sd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	
	if(*sd == -1)
		return -1;
	
	hp = gethostbyname(server);
	if(hp == NULL) 	
		return -1;

	memcpy(&(addr.sin_addr.s_addr), hp->h_addr_list[0], hp->h_length);

	addr.sin_family     = AF_INET;
	addr.sin_port       = htons(port);

	if(connect(*sd, (struct sockaddr *) &addr, sizeof(addr)) == 0)
		return 0;

	/*
	if(! getsockname(session->socket, (struct sockaddr *) &nombre, &logn))
		return -1;

	logn = sizeof(nombre);
	IP = malloc (16*sizeof(char));
  	inet_ntop(AF_INET, &nombre.sin_addr, IP, 16);
	if(debugMode())
		printf("DDD IP : %s \n",IP);
	*/
	return 1;
}

int close_socket(int sd){
	int error;
	error=close(sd);
	cleanDCC();
	return error;
}
