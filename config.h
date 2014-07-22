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

#ifndef CONFIG_H_
#define CONFIG_H_

/* Boolean values */
#define FALSE 0
#define TRUE  1

/* IRC default values */
#define IRC_DEFAULT_PORT 6667	/* < Default IRC port */

/* DCC default values */
#define DCC_DEFAULT_PORT 8007	/* < Default DCC port */

/* Buffer sizes */
#define IRC_BUFFER_SIZE	1024	/* < Size buffer for the IRC Socket */
#define DDC_BUFFER_SIZE 1024	/* < Size buffer for the DCC Sockets*/

/* DCC debug properties */
#define DDC_ENABLE_RETARD 0		/* < Disabled 0, Enabled 1 */
#define DCC_RETARD_VALUE  500 	/* < In miliseconds */

#endif /* CONFIG_H_ */
