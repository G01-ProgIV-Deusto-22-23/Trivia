#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

int main (int argc, char *argv []) {
    int   portno   = 22;
    char *hostname = "localhost";

    int                sockfd;
    struct sockaddr_in serv_addr;
    struct hostent    *server;

    sockfd = socket (AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        printf ("ERROR opening socket"), exit (1);

    server = gethostbyname (hostname);

    if (server == NULL) {
        fprintf (stderr, "ERROR, no such host\n");
        exit (0);
    }

    bzero ((char *) &serv_addr, sizeof (serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy ((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons (portno);
    if (connect (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0)
        printf ("Port is closed");
    else {
        printf ("Port is active");

        if (connect (sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
            printf ("Port is closed");
            perror (strerror (errno));
        } else
            printf ("Port is active");
    }

    close (sockfd);
    return 0;
}