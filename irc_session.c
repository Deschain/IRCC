/*
 * session.c
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

#include "irc_session.h"
#include "socket.h"

void init_session(irc_session *session){
	session->state = IRC_SESSION_INIT;

	session->socket = -1;

	memset(session->server,'\0',64);
	memset(session->port,  '\0', 8);

	memset(session->channel,'\0',32);
	memset(session->nick,  '\0',32);
}

void free_session(irc_session *session)
{
	session->state = IRC_SESSION_INIT;

	session->socket = -1;

	memset(session->server,'\0',64);
	memset(session->port,  '\0', 8);

	memset(session->channel,'\0',32);
	memset(session->nick,  '\0',32);
}

/* TODO: Use write over sd of session */
int write_session(irc_session *session, char *msg)
{
	write(session->socket,msg,strlen(msg));
	return 1;
}

/* TODO: Use read over sd of session */
int read_session(irc_session *session,char *msg)
{
	read(session->socket,msg,IRC_BUFFER_SIZE);
	return 1;
}
