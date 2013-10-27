all:
	gcc socket.c irc_session.c irc_commands.c  DCC.c ircc.c -o ircc -lpthread -Wfatal-errors -Werror -Wall -Wextra -std=c89 -pedantic-errors -g -O0	
clean:
	rm ircc
