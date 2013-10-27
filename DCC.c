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

#include "DCC.h"

#include <pthread.h>

/*Variables globales */
char ip[16];

char *fichero=NULL;
char *usuario=NULL;

char bufferSalida [DDC_BUFFER_SIZE];
char bufferEntrada[DDC_BUFFER_SIZE];

int tamano,tamanoD;

int puerto;


pthread_t hilo;
pthread_attr_t attr;


void cleanDCC(){
	if(fichero != NULL){
		free(fichero);
		fichero=NULL;
	}
	if(usuario !=NULL){
		free (usuario);
		usuario=NULL;
	}
}

void *upload(){
	int sd, fin, res, leido,opt;
	unsigned int enviados,acktmp, ackchk;

	socklen_t optLen,len;
	struct sockaddr_in server_addr;

	char offer[256],ack[4];
	char *file, *mode;

	FILE *fd;

	fin=FALSE;
	enviados=0;
	opt = 1;

	sd=socket(AF_INET, SOCK_DGRAM, 0);

	optLen = sizeof(opt);

	if(sd == -1)
		return (void *)-1;

	if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (void *) &opt, optLen) == -1){
		if(debugMode())
			printf("DDD Error setting socket options\n");
		return (void *)-1;
	}

	memset(&server_addr, 0, sizeof(server_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(DCC_DEFAULT_PORT);

	if(bind(sd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		return (void *)-1;

	len = sizeof(server_addr);

	/* Receive initiation message for upload */
	res=recvfrom(sd, offer, 256, 0,(struct sockaddr *)&server_addr,&len);

	file = malloc(sizeof(char)*256);
	mode = malloc(sizeof(char)*9);

	strcpy(file,offer+2);
	strcpy(mode,offer+2+1+strlen(file));

	fd  = fopen(file, "r");

	if(debugMode())
		printf("DDD opcode %d %d, file %s, mode %s\n",offer[0],offer[1],file, mode);


	/* ACK initiation message*/
	acktmp = htonl(enviados);

	memcpy(&ack, &acktmp, 4);
	sendto(sd, ack,4, 0,(struct sockaddr *) &server_addr, sizeof(server_addr));
	if(debugMode())
		printf("DDD ACK %u\n", enviados);

	/* Uploading */
	while(!fin){

		memset(bufferSalida, 0, DDC_BUFFER_SIZE);
		leido=fread(bufferSalida,sizeof(char),DDC_BUFFER_SIZE,fd);

		if(leido == 0){
			/* EOF */
			fin=TRUE;

		}else if(leido == DDC_BUFFER_SIZE){
			/* Send data */
			res=sendto(sd, bufferSalida,sizeof(bufferSalida), 0,(struct sockaddr *) &server_addr, sizeof(server_addr));
			enviados+=(unsigned int)res;

		}else{
			/* Send last data */
			res=sendto(sd, bufferSalida,leido, 0,(struct sockaddr *) &server_addr, sizeof(server_addr));
			enviados+=(unsigned int)res;
		}

		if(!fin){
			/* ACK file size */
			recvfrom(sd, ack, 4, 0,(struct sockaddr *)&server_addr,&len );
			acktmp=0;
			memcpy(&acktmp,&ack, 4);
			ackchk=ntohl(acktmp);
			if(debugMode())
				printf("DDD ACK %u\n", ackchk);
		}
	}

	fclose(fd);
	close(sd);

	if(debugMode())
		printf("DDD Uploaded %s\n",file);

	free(file);
	free(mode);

	cleanDCC();
	return 0;
}

void *download(){
	int sd, fin, leido;
	unsigned int total,acktmp,ackchk;

	struct sockaddr_in server;

	FILE *fd;
	socklen_t len;

	char msg[256],ack[4];

	fin=FALSE;

	/* Create socket */
	if((sd=socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		return (void *)-1;
	}

	server.sin_family 		= AF_INET;
	server.sin_port 		= htons(puerto);
	server.sin_addr.s_addr 	= inet_addr(ip);


	len=sizeof(server);

	/* Message based on RRC from TFTP */
	memset(msg,0,256);
	/* Operation code */
	msg[0]='\000';
	msg[1]='\001';

	/* File name */
	memcpy(msg+2, fichero, strlen(fichero));

	/* Mode */
	memcpy(msg+2+strlen(fichero)+1, "octet", 6);

	if(debugMode())
		printf("DDD > file: %s ,mode: %s \n",msg+2, msg+2+strlen(fichero)+1);

	/* Sent initiation message */
	sendto(sd, msg, 9 + strlen(fichero), 0, (struct sockaddr *) &server, sizeof(server));

	/* ACK from peer */
	recvfrom(sd, ack, 4, 0,(struct sockaddr *)&server,&len );

	/* Create container file */
	fd = fopen(fichero, "w");

	acktmp=0;
	memcpy(&acktmp,&ack, 4);
	ackchk=ntohl(acktmp);

	if(debugMode())
		printf("DDD ACK %u\n", ackchk);

	total=0;

	/* Downloading */
	while(!fin){

		memset(bufferEntrada, 0, DDC_BUFFER_SIZE);
		/* Recieve data */
		leido=recvfrom(sd, bufferEntrada, DDC_BUFFER_SIZE, 0,(struct sockaddr *)&server,&len);
		total+=(unsigned int)leido;

		if(leido==0){
			fin=TRUE;

		}else if(leido < DDC_BUFFER_SIZE || total == (unsigned int)tamanoD){
			/* Last file data */
			fwrite(bufferEntrada, sizeof(char), leido*sizeof(char), fd);
			fin=TRUE;
		}else{
			/* Write file data */
			fwrite(bufferEntrada, sizeof(char), leido*sizeof(char), fd);

		}
		/* Send ACK*/
		acktmp=htonl(total);
		memcpy(&ack, &acktmp, 4);
		sendto(sd, ack,4, 0,(struct sockaddr *) &server, sizeof(server));
		if(debugMode())
			printf("DDD ACK %u\n", total);
	}

	fclose(fd);
	close(sd);

	if(debugMode())
		printf("DDD Downloaded %s\n",fichero);

	cleanDCC();
	return 0;
}

/* Función que prepara la subida y la inicia */
int prepare_upload(char *usr, char *fich, char *IP){

	int res;

	/*Seteamos la variables globales con los datos recibidos */
	memset(ip, 0, 16);
	/*Liberamos memoria si esta reservada de antes */
	cleanDCC();
	fichero=malloc((strlen(fich)+1)*sizeof(char));
	strcpy(fichero, fich);
	usuario=malloc((strlen(usr)+1)*sizeof(char));
	strcpy(usuario, usr);

	strcpy(ip,IP);

	pthread_attr_init(&attr);
	/* Iniciamos la subida en un hilo nuevo */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	res=pthread_create(&hilo, &attr, upload, NULL);
	if(res != 0){
		return -1;
	}
	return 0;
}

int prepare_download(char *fich, char *IP, char *port){
	int res;

	cleanDCC();
	/*Seteamos la variables globales con los datos recibidos */
	memset(ip, 0, 16);

	fichero=malloc((strlen(fich)+1)*sizeof(char));
	strcpy(fichero, fich);

	strcpy(ip,IP);
	puerto=atoi(port);
	pthread_attr_init(&attr);
	/* Iniciamos la descarga en un hilo nuevo */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	res=pthread_create(&hilo, &attr, download, NULL);

	if(res != 0){
		return -1;
	}
	return 0;
}


unsigned int iptoi(char *ip){

	unsigned int a, b, c, d;

	if (sscanf(ip, "%u.%u.%u.%u", &a, &b, &c, &d) != 4)
		return -1;
	return ((a*16777216)+(b*65536)+(c*256)+(d));
}
