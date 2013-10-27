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


#include "ircc.h"

#include "common.h"

#include "irc_commands.h"

#include <string.h>
#include <getopt.h>
#include <signal.h>

/* TODO: Use debug variable in session */
int debug =	0;


int debugMode()
{
	return debug;
}

int getServerInfo(irc_session* session, char *param){

	char *colon = strchr(param,':');

	if(colon && colon != param){
		memset(session->server,'\0',64);
		memset(session->port,'\0',8);
		memcpy(session->server,param,colon-param);
		memcpy(session->port,colon+1,strlen(colon));

		return 1;
	}

	return 0;
}

int main(int argc, char *argv[]){

	int c = 0;
	irc_session session;

    init_session(&session);

   	while ((c = getopt (argc, argv, "s:n:c:d")) != -1)
    {
    	switch (c)
    	{
/* Server argument */
			case 's':
				if(getServerInfo(&session,optarg))
				{
					cmdConectar(&session);
				}else{
					printf("*** The /connect command needs a port\n");
				}
				break;

/* Nick argument */
			case 'n':
				if(session.state == IRC_SESSION_CONNECTED)
				{
					cmdAuth(&session, optarg);
				}else{
					printf("*** Unable to authenticate, not connected to any server\n");
				}
				break;

/* Channel argument */
			case 'c':
				if(session.state == IRC_SESSION_CONNECTED)
				{
					cmdJoin(&session, optarg);
				}else{
					printf("*** Unable to join channel: not connected any server\n");
				}
				break;

/*Debug argument*/
			case 'd':
				printf("DDD Debug mode\n");
				debug=1;
				break;
				
			case '?':
				printf("*** Unknown argument -%c\n",optopt);
				return 0;
				break;
    	}
    }

    irc_run(session);
    return 1;
}

int irc_run(irc_session session){

	char buffTerm[256];					/* < Buffer para almacenar la entrada por teclado 		*/
	char buffSck [IRC_BUFFER_SIZE*2];	/* < Buffer para almacenar la entrada del socket 		*/

	fd_set socket_in;					/* < fd para socket de entrada de entrada del socket	*/

	irc_message msg;

	char *svr = NULL, *puerto = NULL, *fichero = NULL, *usr_serv = NULL;
    int res;
    int offset = 0;

    for(;;)
    {
		FD_ZERO(&socket_in);
		FD_SET(STDIN_FILENO, &socket_in);

		if(session.state == IRC_SESSION_CONNECTED)
		{
			FD_SET(session.socket, &socket_in);
		}

		select(FD_SETSIZE, &socket_in, NULL, NULL, NULL);

		if(session.state == IRC_SESSION_CONNECTED && FD_ISSET(session.socket, &socket_in)){

			/* Clean buffer before using */
			memset(buffSck, 0, IRC_BUFFER_SIZE*2);

			/* Read from the socket */
			if(read(session.socket, buffSck, IRC_BUFFER_SIZE*2) < 0)
			{
				if(debug) perror("DDD Error reading the socket\n");
				exit(0);
			}

			/*!
			 * Print socket content
			 *
			 * printf("\nSocket: %s\n",buffSck);
			 */

			while( (offset = findcrlf (buffSck, strlen(buffSck))) > 0){

/* Process msg */
				init_irc_message(&msg);
				parse_irc_message(&msg, buffSck,offset-2);
				proccess_irc_message(&session,&msg);
				free_irc_message(&msg);

				memmove (buffSck, buffSck + offset, strlen(buffSck)-offset);
				memset  (buffSck+strlen(buffSck)-offset,0,offset);
			}

		}else if(FD_ISSET(STDIN_FILENO, &socket_in)){

			/* Clean buffer before using */
			memset(buffTerm, 0, 256);

			if (! fgets(buffTerm, 255, stdin))
			{
				if(debug) perror("DDD Error reading command\n");
				exit(0);
			}

			/*
			 * Print user command
			 * printf("\n%s\n",buffTerm);
			 */


			if (strlen(buffTerm) > 0)
			{
/* Process msg */
				init_irc_message(&msg);
				parse_irc_message(&msg, buffTerm,strlen(buffTerm));
/* commands */
				if ((msg.command) && *(msg.command) == '/')
				{
/* CONNECT command */
					if(strcmp(msg.command,"/connect") == 0)
					{
						if(session.state != IRC_SESSION_CONNECTED)
						{
							if(msg.parameters[0])
							{
								if(getServerInfo(&session,msg.parameters[0]))
								{
									if(cmdConectar(&session) == -1){
										if(debug)
										{
											printf("DDD Unable to connect to %s:%s\n",session.server,session.port);
										}
									}
								}else{
									printf("*** The /connect command also needs a port\n");
								}
							}else{
								printf("*** The /connect command needs a host:port\n");
							}
						}else{
							printf("*** Already connected to %s\n",session.server);
						}
/* DISCONNECT command */
					}else if(strcmp(msg.command,"/disconnect") == 0){

						if(session.state == IRC_SESSION_CONNECTED)
						{
							cmdDesconectar(&session);
						}else{
							printf("*** Unable to disconnect. Not connected\n");
						}
/* QUIT command */
					}else if(strcmp(msg.command,"/quit") == 0){

						if(session.state == IRC_SESSION_CONNECTED)
						{
							cmdDesconectar(&session);
						}
						free_session(&session);
						printf("*** bye!\n");
						exit(0);
/* AUTH command */
					}else if(strcmp(msg.command,"/auth") == 0){

						if(session.state == IRC_SESSION_CONNECTED)
						{
							if(msg.parameters[0])
							{
								res=cmdAuth(&session,msg.parameters[0]);
								if(res!=0)
								{
									printf("*** No se ha podido autenticar\n");
								}
							}else{
								printf("*** The /auch command needs a nickname\n");
							}
						}else{
							printf("*** Unable to authenticate, not connected to any server\n");
						}
/* LIST command */
					}else if(strcmp(msg.command,"/list") == 0){

						if(session.state == IRC_SESSION_CONNECTED)
						{
							cmdList(&session);
						}else{
							printf("*** Unable to list channels, not connected to any server\n");
						}
/* JOIN command */
					}else if(strcmp(msg.command,"/join") == 0){

						if(session.state == IRC_SESSION_CONNECTED)
						{
							if(msg.parameters[0])
							{
								cmdJoin(&session,msg.parameters[0]);
							}else{
								printf("*** The /join command needs a channel\n");
							}

						}else{
							printf("*** Unable to join channel, not connected to any server\n");
						}
/* LEAVE command */
					}else if(strcmp(msg.command,"/leave") == 0){

						if(session.state == IRC_SESSION_CONNECTED)
						{
							cmdLeave(&session);
						}else{
							printf("*** Unable to leave the channel: not connected any to server\n");
						}
/* WHOIS command */
					}else if(strcmp(msg.command,"/who") == 0){

						if(session.state == IRC_SESSION_CONNECTED)
						{
							cmdWho(&session);

						}else{
							printf("*** Unable to retrieve users: not connected to any server\n");
						}
/* MSG command */
					}else if(strcmp(msg.command,"/msg") == 0){

						if(session.state == IRC_SESSION_CONNECTED)
						{
							if(msg.parameters[0])
							{
								cmdMsg(&session,msg.parameters[0]);
							}else{
								printf("*** The /msg command needs a message\n");
							}

						}else{
							printf("*** Unable to deliver message: not connected to any server\n");
						}
/* HELP command */
					}else if(strcmp(msg.command,"/help") == 0){

						cmdHelp();
/* NOP command */
					}else if(strcmp(msg.command,"/nop") == 0){

						continue;
/* SLEEP command */
					}else if(strcmp(msg.command,"/sleep") == 0){

						if(cmdDormir(atoi(msg.parameters[0])) == -1)
						{
							printf("*** Unable to sleep for %s seconds\n",msg.parameters[0]);
						}
/* OFFER command */
					}else if(strcmp(msg.command,"/offer") == 0){

						/*
						 * res=parseComandoDCC(msg->parameters[1],strlen(msg->parameters[1]), fichero, usr_serv);
						 */
						if(session.state == IRC_SESSION_CONNECTED){
							if(msg.parameters[0] && msg.parameters[1])
							{
								res=cmdOffer(&session, msg.parameters[0], msg.parameters[1]);
								if(res!=0)
									printf("*** Error sending the file\n");
							}else{
								printf("The command /offer need a file and a user\n");
							}
						}else{
							printf("*** Debes conectarte primero\n");
						}

/* DOWNLOAD command */
					}else if(strcmp(msg.command,"/download") == 0){

						fichero  = malloc(256 * sizeof(char));
						usr_serv = malloc(256 * sizeof(char));

						strcpy(fichero,msg.parameters[0]);
						strcpy(usr_serv,msg.parameters[1]);

						svr 	= malloc(256 * sizeof(char));
						puerto 	= malloc(256 * sizeof(char));

						/*Separamos el servidor y el puerto */
						parseServidorPuerto(usr_serv,svr, puerto);

						if(debug) printf("DDD Download file %s from %s:%s\n",fichero,svr,puerto);

						if(session.state == IRC_SESSION_CONNECTED)
						{
							res=cmdDownload(&session,fichero, svr, puerto);
							if(res!=0)
								printf("*** Error downloading file\n");
						}else{
							printf("*** Unable to download file: not connected to any server\n");
						}

						free(fichero);
						free(usr_serv);
						free(svr);
						free(puerto);
/* Unsupported command */
					}else{
						printf("*** Unsupported command %s\n",msg.command);
					}
			}else{
/* Default command */
				if(session.state == IRC_SESSION_CONNECTED)
				{
					res=cmdMsg(&session, buffTerm);
					if(res==-1)
					{
						printf("*** Error sending message\n");
					}else if(res==-2){
						printf("*** You are not in any channel\n");
					}
				}else{
					printf("*** Unable to deliver message: not connected to any server\n");
				}
			}
			free_irc_message(&msg);
			}
		}

    }
    return 1;
}

int parseServidorPuerto(char *linea, char *servidor, char *puerto)
{
	char *colon;

	colon = strchr(linea,':');

	if(colon)
	{
		strncpy(servidor,linea,colon-linea);
		strcpy (puerto,colon+1);

		return 1;
	}
	return 0;
}
