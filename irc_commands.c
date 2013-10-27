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

#define _BSD_SOURCE
#define _POSIX_C_SOURCE 200112L

#include "common.h"
#include "irc_commands.h"

#include "socket.h"
#include "DCC.h"

#include <limits.h>

void init_irc_message(irc_message *msg)
{
	int i;

	msg->prefix = 0;
	msg->command = 0;

	for( i = 0; i < 15; i++){
		msg->parameters[i] = 0;
	}
}

void free_irc_message(irc_message *msg)
{
	int i;


	if(msg->prefix) free(msg->prefix);
	if(msg->command)free(msg->command);

	for( i = 0; i < 15; i++){
		if(msg->parameters[i]) free(msg->parameters[i]);
	}
}

void parse_irc_message(irc_message *msg,char *input,int length)
{
	char *p, *s;
	int paramindex = 0;

	p = input;

	if ( input[0] == ':' )
	{
		while ( *p && *p != ' ')
			p++;
		msg->prefix = calloc(1,p-(input+1));
		memcpy(msg->prefix,input+1,p-(input+1));
		p++;
	}



	if ( isdigit (p[0]) && isdigit (p[1]) && isdigit (p[2]) )
	{
		msg->command = calloc(1,3);
		memcpy(msg->command,p,3);
		p+=4;
	}
	else
	{
		s = p;

		while ( *p && *p != ' ' && *p != '\n')
			p++;
		msg->command = calloc(1,p-s);
		memcpy(msg->command,s,p-s);
		p++;
	}

	while (*p && p < input+length){

		if ( *p == ':' )
		{
			msg->parameters[paramindex] = calloc(1,(input+length) - (p+1));
			memcpy(msg->parameters[paramindex],p+1,((input+length) - (p+1)));
			break;
		}

		for ( s = p; *p && *p != ' ' && *p != '\n' && p < input+length; p++ ){}

		msg->parameters[paramindex] = calloc(1,p-s);
		memcpy(msg->parameters[paramindex++],s,p-s);
		p++;
	}
}



void proccess_irc_message(irc_session *session, irc_message *msg){
	char *content = msg->command;

	if(strcmp(content,"PING") == 0)
	{
		pong(session);
	}else if(strcmp(content,"001") == 0){
		printf("*** %s\n",msg->parameters[1]);
	}else if(strcmp(content,"002") == 0){
		printf("*** %s (from %s)\n",msg->parameters[1],msg->prefix);
	}else if(strcmp(content,"003") == 0){
		printf("*** %s\n",(msg->parameters[1]));
	}else if(strcmp(content,"004") == 0){
		printf("*** umodes available %s, channel modes available %s\n", msg->parameters[3],msg->parameters[4] );
	}else if(strcmp(content,"005") == 0){
		printf("*** Unknown message from server with command <005>\n");
	}else if(strcmp(content,"251") == 0){
		printf("*** %s\n",(msg->parameters[1]));
	}else if(strcmp(content,"254") == 0){
		printf("*** %s %s\n",msg->parameters[1], msg->parameters[2]);
	}else if(strcmp(content,"255") == 0){
		printf("*** %s %s\n",msg->parameters[1], msg->parameters[2]);
	}else if(strcmp(content,"315") == 0){
		/* ignore */
	}else if(strcmp(content,"322") == 0){
		printf("*** %s\t%s\t%s\n",msg->parameters[1], msg->parameters[2], msg->parameters[3]);
	}else if(strcmp(content,"323") == 0){
		/* ignore */
	}else if(strcmp(content,"332") == 0){
		printf("*** Topic for %s: %s\n",msg->parameters[1],msg->parameters[2]);
	}else if(strcmp(content,"352") == 0){
			printf("*** %s\t%s\n",session->channel,msg->parameters[5]);
	}else if(strcmp(content,"353") == 0){
		printf("*** Users in channel %s: %s\n", msg->parameters[2], msg->parameters[3]);
	}else if(strcmp(content,"372") == 0){
		printf("*** - %s message of the day\n",msg->prefix);
		printf("*** %s\n",msg->parameters[1]);
	}else if(strcmp(content,"366") == 0){
		/* ignore */
	}else if(strcmp(content,"376") == 0){
		/* ignore */
	}else if(strcmp(content, "JOIN") == 0){
		printf("*** %s has joined channel %s\n", msg->prefix, msg->parameters[0]);
	}else if(strcmp(content, "PART") == 0){
		printf("*** %s has left channel %s\n",msg->parameters[1],msg->parameters[0]);
	}else if(strcmp(content, "QUIT") == 0){
		printf("*** %s has left server\n",msg->parameters[0]);
	}else if(strcmp(content,"PRIVMSG") == 0){

		if(msg->parameters[1][0] == '\001'){
			proccessOffers(msg->parameters[1]+1);
		}else{
			printf("<%s> %s\n",msg->prefix,msg->parameters[1]);
		}

	}else{
		printf("*** Message received with a command unknown to RFC2812 (%s)\n", content);
	}
}

int parseComandoDCC(char *linea,int longitud, char *fichero, char *usr_serv){
	int encontrado;
	int i,cc,cp;

	encontrado=0;
	cc=0;
	cp=0;
	
	for(i=0; i<longitud; i++){
		if(linea[i] == ' '){
			if(encontrado == 1)
				fichero[cc]='\0';
			encontrado++;
		}else{
			if(encontrado == 1){
				fichero[cc]=linea[i];
				cc++;
			}else if(encontrado == 2){
				usr_serv[cp]=linea[i];
				cp++;
			}
		}		
	}
	if(encontrado==0){ 
		fichero[cc]='\0';
		usr_serv[cp]='\0';
	}else{
		usr_serv[cp]='\0';
	}

	return encontrado;
}

void cmdHelp()
{
	printf("*** A typical user session goes like this:	\n");
	printf("*** 										\n");
	printf("***     /connect localhost:7101				\n");
	printf("***     /auth alcortes						\n");
	printf("***     /list								\n");
	printf("***     /join #ro2							\n");
	printf("***     hello								\n");
	printf("***     /leave								\n");
	printf("***     /disconnect							\n");
	printf("***     /quit								\n");
	printf("*** 										\n");
	printf("*** The full list of available commands are	\n");
	printf("*** 										\n");
	printf("***     /help                    Show this text.					\n");
	printf("***     /connect <host>:<port>   Connect to a server.				\n");
	printf("***     /auth <nick>             Authenticate with the server,		\n");
	printf("***                              sets nick, username & real name.	\n");
	printf("***     /list                    Lists all current irc channels,	\n");
	printf("***                              number of users, and topic.		\n");
	printf("***     /join [<channel>]        Join a channel or tell which is	\n");
	printf("***                              the current channel.				\n");
	printf("***     /leave                   Leaves your current channel.		\n");
	printf("***     /who                     List users in current channel.		\n");
	printf("***     /msg <txt>               Send <txt> to the channel.			\n");
	printf("***     /offer <file> <nick>     Launch a DCC server for a file		\n");
	printf("***                              transfer. The recipient must accept\n");
	printf("***                              your offer with a \'/download\'.	\n");
	printf("***     /download <file> <host:port>								\n");
	printf("***                              Starts a file transfer by DCC.		\n");
	printf("***                              The  sender must first have offered\n");
	printf("***                              the file with an '/offer' command.	\n");
	printf("***     /disconnect              Disconnect from server.			\n");
	printf("***     /quit                    Close program.						\n");
	printf("***     /nop [<text>]            (debugging) no operation.			\n");
	printf("***     /sleep [<secs>]          (debugging) pause for <secs>		\n");
	printf("***                              seconds.							\n");
	printf("***																	\n");
	printf("*** Lines from the user that don't begin with \'/\' are equivalent to\n");
	printf("*** 																\n");
	printf("***     /msg line													\n");
	printf("*** 																\n");
	printf("*** Lines beginning with \"*** \" are messages from the program.	\n");
	printf("*** Lines beginning with \"<user> \" are messages from <user>.		\n");
	printf("*** Lines beginning with \"DDD \" are debug messages from the program.\n");
	printf("*** Lines beginning with \"> \" are your messages to other users.	\n");
	printf("*** The rest of the lines are echoes of the user commands.			\n");
}

int cmdDormir(int time){
	int tiempoRes;

	if(time < 1)
		return -1;
	tiempoRes=sleep(time);
	if(tiempoRes!=0)
		return -1;
	return 0;
}


int cmdConectar(irc_session *session){

	if(open_socket(session->server,atoi(session->port),&session->socket) != 0)
	{
		printf("*** Unable to connect to server %s:%s\n",session->server,session->port);
		return -1;
	}

	session->state = IRC_SESSION_CONNECTED;

	printf("*** Connected to server %s:%s\n",session->server,session->port);
	return 0;
}

int cmdDesconectar(irc_session *session){

	char *command="QUIT\r\n";

	write(session->socket, command, strlen(command));

	if(close_socket(session->socket))
	{
		printf("*** Unable to disconnect from %s\n",session->server);
		session->state = IRC_SESSION_ERROR_SOCKET;
		return -1;
	}

	session->state = IRC_SESSION_DISCONNECTED;

	free_session(session);
	printf("*** Disconnected from server\n");
	return 0;

	/*
	 * printf("*** Disconnected from server\n");
	 */
}

int pong(irc_session *session)
{
	char command[512];
	char *server = session->server;

	sprintf(command,"PONG %s\r\n",server);

	if(! write(session->socket, command, strlen(command)))
		return -1;

	return 0;
}

int cmdAuth(irc_session *session, char * nick)
{
	char command[512];


	sprintf(command,"NICK %s\r\n",nick);
	if(! write(session->socket, command, strlen(command)))
		return -1;

	sprintf(command,"USER %s 0 * :%s\r\n",nick,"0083976");
	if(! write(session->socket, command, strlen(command)))
		return -1;

	strcpy(session->nick,nick);
	return 0;
}

int cmdList(irc_session *session)
{
	char *comando="LIST\r\n";

	if(! write(session->socket, comando, strlen(comando)))
	{
		printf("*** Unable to list channels\n");
		return -1;
	}
	return 0;
}

int cmdJoin(irc_session *session, const char *channel)
{
	char command[512];

	if(session->channel[0] == '\0')
	{
		sprintf(command,"JOIN %s\r\n", channel);

		if(! write(session->socket, command, strlen(command)))
		{
			printf("*** Unable to join channel %s\n",channel);
			return -1;
		}

		strcpy(session->channel,channel);
	}else{
		/* Should leave the old one and join the new one? */
		printf("*** Already in channel %s, leave it first\n",session->channel);
	}


	return 0;
}


int cmdLeave(irc_session *session){

	char  command[512];

	if(session->channel[0] != '\0')
	{
		sprintf(command,"PART %s\r\n",session->channel);

		if(! write(session->socket, command, strlen(command)))
		{
			printf("*** Error leaving channel %s\n",session->channel);
			return -1;
		}

		memset(session->channel,'\0',32);
	}else{
		printf("*** You are not in any channel\n");
	}

	return 0;
}

int cmdWho(irc_session *session){

	char *comando="WHO\r\n";

	if(session->channel[0] != '\0')
	{
		if(! write(session->socket, comando, strlen(comando)))
		{
			printf("*** Error retrieving users\n");
			return -1;
		}

	}else{
		printf("*** You are not in any channel\n");
	}

	return 0;
}

int cmdMsg(irc_session *session, char *msg){

	char  command[1024];

	if(session->channel[0] != '\0'){
		printf("> %s\n",msg);
		sprintf(command,"PRIVMSG %s :%s\r\n",session->channel,msg);

		if(! write(session->socket, command, strlen(command)))
		{
			printf("*** Error sending message \n");
			return -1;
		}

	}else{
		printf("*** You are not in any channel\n");
	}

	return 0;
}

int cmdOffer(irc_session *session, char *file, char *user)
{

	char command[512];
	char *fl = 0,*usr = 0;
	char hostname[HOST_NAME_MAX+1];
	char *ip;
	struct hostent *hostentry;
	FILE *fd;
	int   size;

	/* Get file size */
	fd = fopen(file, "rb");
	if(!fd)
	{
		printf("*** File %s does not exist\n",file);
		return -1;
	}

	usr = malloc(strlen(user)+1);
	strcpy(usr,user);
	fl  = malloc(strlen(file)+1);
	strcpy(fl,file);

	fseek(fd, 0, SEEK_END);
	size = ftell(fd);
	fclose(fd);

	gethostname(hostname,sizeof(hostname));

	hostentry = gethostbyname(hostname);

	ip = inet_ntoa(*((struct in_addr *)hostentry->h_addr_list[0]));

	sprintf(command,"PRIVMSG %s :\001DCC SEND %s %u %u %u\001\r\n",usr,fl,iptoi(ip),DCC_DEFAULT_PORT,size);

	if(! write(session->socket, command, strlen(command))){
		return -1;
	}

	printf("*** Sent offer request of file %s to %s\n",fl, usr);

	/*
	 * Preparamos e iniciamos la subida del fichero
	 * prepararSubida(user, file, ip, size);
	 */

	prepare_upload(usr, fl, ip);

	free(fl);
	free(usr);
	return 0;
}

int cmdDownload(irc_session *session, char *file, char *server, char *port){
	if(session->state == IRC_SESSION_CONNECTED)
	{
		prepare_download(file, server, port);
		printf("*** Downloading file %s\n", file);
		return 0;
	}
	return 1;
}

void proccessOffers(char *offer)
{
	char file[64];
	unsigned ip,port,size;
	unsigned ip04,ip03,ip02,ip01;

	sscanf(offer,"%*s %*s %s %u %u %u",file,&ip,&port,&size);

	ip04 = (ip/16777216)%256;
	ip03 = (ip/65536)%256;
	ip02 = (ip/256)%256;
	ip01 = ip%256;

	printf("*** Download offer: /download %s %d.%d.%d.%d:%u\n",file,ip04,ip03,ip02,ip01,port);
}

int findcrlf(const char * buf, int length)
{
	int offset = 0;
	for ( ; offset < (length - 1); offset++ )
		if ( buf[offset] == 0x0D && buf[offset+1] == 0x0A )
			return (offset + 2);

	return 0;
}
