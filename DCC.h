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
#include "ircc.h"

typedef struct{
	char *file;
	int	  size;

	char *ip;
	char *port;

	char *user;
}dcc_session;

void cleanDCC();

int prepare_upload	(char *usr, char *fich, char *IP);
int prepare_download(char *fich, char *IP, char *port);

void *upload	();
void *download	();

unsigned int iptoi(char *ip);
char *itoip(unsigned int ip);
