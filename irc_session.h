/*
 * session.h
 *
 * Copyright (C) 2012 Óscar Rodríguez Zaloña <100083976@alumnos.uc3m.es>
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

#ifndef SESSION_H_
#define SESSION_H_

#include "config.h"
#include "common.h"

#include <netdb.h>

#define IRC_SESSION_INIT			1
#define IRC_SESSION_CONNECTING		2
#define IRC_SESSION_CONNECTED		3
#define IRC_SESSION_DISCONNECTED	4

#define IRC_SESSION_ERROR_SOCKET	1

typedef struct{
	int				state;

	int				error;

	int				socket;

	char 			server[64];
	char			port[8];
	char			channel[32];
	char			nick[32];

	struct sockaddr_in	addr;
}irc_session;


void init_session(irc_session *session);
void free_session(irc_session *session);

int write_session(irc_session *session, char *msg);
int read_session(irc_session *session,char *msg);

#endif /* SESSION_H_ */
