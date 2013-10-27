/*
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

#define IRC_MSG_MAX_PARAMS	10

typedef struct{

	char  *prefix;
	char  *command;
	char  *parameters[15];
}irc_message;

void init_irc_message 		(irc_message *msg);
void free_irc_message		(irc_message *msg);
void parse_irc_message		(irc_message *msg, char *input,int length);
void proccess_irc_message	(irc_session *session, irc_message *msg);

int parseComandoDCC(char *linea,int longitud, char *fichero, char *usr_serv);

int pong(irc_session *session);

int cmdConectar		(irc_session *session);
int cmdDesconectar	(irc_session *session);

int cmdAuth(irc_session *session, char * nick);

int cmdList(irc_session *session);

int cmdJoin(irc_session *session, const char * channel);
int cmdLeave(irc_session *session);

int cmdWho	(irc_session *session);
int cmdMsg	(irc_session *session, char *msg);

int cmdOffer(irc_session *session, char *file, char *user);
int cmdDownload	(irc_session *session, char *file, char *server, char *port);

int cmdDormir(int segundos);

void cmdHelp();

void proccessOffers(char *offer);

int findcrlf(const char * buf, int length);
