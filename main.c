#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <ncurses.h>

#define ERRMSG {perror("ERROR");exit(EXIT_FAILURE);}

#define HOST "192.168.1.245"
#define PORT 9999
#define BUFSIZE 128

int start_connection(int *sockfd)
{
	*sockfd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	if (inet_pton(AF_INET, HOST, &addr.sin_addr) == -1)
		return -1;
	if (connect(*sockfd, (struct sockaddr *)&addr, sizeof(addr)))
		return -1;
	return 0;
}

void *getsrvmsg(void *vpsfd)
{
	int sockfd = *(int*)vpsfd; 
	int q = 1;
	while (q) {
		char buf[BUFSIZE] = {0};
		read(sockfd, buf, BUFSIZE);
		printw("SRV:[%s]\n", buf);
		q = buf[0] != 0;
	}
	pthread_exit(NULL);
}

void initcurses()
{
	initscr();
	cbreak();
	noecho();
	scrollok(stdscr, TRUE);
	keypad(stdscr, TRUE);
}

void endcurses()
{
	endwin();
}

int main()
{
	int sockfd;
	pthread_t st;
	if (start_connection(&sockfd) == -1)
		ERRMSG
	if (pthread_create(&st, NULL, getsrvmsg, (void*)&sockfd))
		ERRMSG
	initcurses();
	int q = 1;
	char key;
	while (q) {
		refresh();
		key = getch();
		char str[2] = { key, '\0' };
		write(sockfd, str, strlen(str));
		if (key == 'q')
			q = 0;
	}
	endcurses();
	char *msg = "QUIT";
	write(sockfd, msg, strlen(msg));
	pthread_exit(NULL);
	sleep(1);
	close(sockfd);
	return 0;
}
