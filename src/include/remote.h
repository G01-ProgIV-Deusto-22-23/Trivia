#ifndef TRIVIA_REMOTE_H
#define TRIVIA_REMOTE_H

#ifdef _WIN32
SOCKET
#else
int
#endif
locate_server (void);

#endif