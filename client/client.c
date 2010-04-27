#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

#define DATASIZE 513

void nullstring(char *);
void quitcom(char *b,char *c,char *n, char *nick,char *user,char *jn, char *j, int *s,struct sockaddr_in *ta);
void startconnect(char *b, char *c, char *n, char *nick,char *user,char *jn, char *j, int *s,struct sockaddr_in *ta);
void rejoinchan(char *, char *, char *, char *, char *, int);

int main(int argc, char *argv[])
{

struct hostent *h;
struct sockaddr_in their_addr;
int sock, numbytes, i;
char buf[DATASIZE];
char command[DATASIZE];
char user[]="USER Bot Bot Bot Bot :Bot\n";
char nick[]="Lithios";
char j[]="#nf.open";
char jn[]="JOIN ";
char n[]="NICK ";
//char ident[]="IDENT meep\r\n";
//char real[]="REALNAME Bot\r\n";

if(argc != 3) {
printf("Usage: %s <host/ip> <port> \n", argv[0]);
exit(1);
}

if ((h=gethostbyname(argv[1])) == NULL) {
perror("gethostbyname");
exit(1);
}

#define PORT atoi(argv[2])
printf("Connecting on port: %d\n", PORT);

//socket information
their_addr.sin_family = AF_INET;
their_addr.sin_port = htons(PORT);
their_addr.sin_addr = *((struct in_addr *)h->h_addr);
memset(their_addr.sin_zero, '\0', sizeof(their_addr.sin_zero));

startconnect(buf, command, n, nick, user, jn, j, &sock, &their_addr);
send(sock, "PRIVMSG nickserv identify 420123", sizeof("PRIVMSG nickserv identify 420123"), 0);

/**************************************************************
*while the connection is live, recieve the data from          *
*the server, if the data is corrupt, exit. every string parsed*
*from the server will be null terminated and printed to the   *
*terminal.                                                    *
**************************************************************/
while((numbytes=recv(sock, buf, DATASIZE-1, 0)) !=0){
if (numbytes == -1) {
perror("recv");
exit(1);
}
for(; numbytes <= DATASIZE -1; numbytes++ )
buf[numbytes] = '\0';
printf("%s", buf);

//change the ping to a pong and send to server
if(strncmp(buf, "PING",4) == 0){
buf[1] = 'O';
send(sock, buf, strlen(buf), 0);
printf("%s\n",buf);
}

rejoinchan(buf, command, j, jn, nick, sock);

quitcom(buf,command, n, nick, user, jn, j, &sock, &their_addr);
}

close(sock);
return 0;
}

void nullstring(char *p){
int i;
for(i = 0; i <= DATASIZE -1; i++){
*p = '\0';
p++;
}
}

/**********************************************************************
*rejoin a channel upon being kicked,                                  *
*the parameters are as follows, p is the input buffer from the socket *
*p2 is the string to be built and compared with the buffer, and will  *
*be used to build the rejoin command. p3 is the channel name, p4 is   *
*the JOIN command, ni is hte nick, and i is the socket file descriptor*
**********************************************************************/
void rejoinchan(char *p, char *p2, char *p3, char *p4, char *ni, int i){
if(strstr(p, "KICK " ) !='\0'){
printf("%s\n", "I have been kicked" );
if(strstr(p, strncat(strncat(strncat(p2, p3, strlen(p3)), " ", strlen(" ")), ni, strlen(ni))) != '\0'){
nullstring(p2);
strncat(strncat(strncat(p2,p4,strlen(p4)),p3 ,strlen(p3)), "\r\n", strlen("\r\n"));
if(send(i, p2, strlen(p2), 0)  > 0)
printf("%s%s\n","Rejoining channel: ", p3);
else exit(1);
nullstring(p2);
}
}
}
/*************************************************************
*send a quit command to the server if the owner sends a      *
*command to quit or a reconnection command. If it is a       *
*reconnect the method will call an outside method to reset   *
*the connection.                                             *
*************************************************************/ 
void quitcom(char *b,char *c,char *n, char *nick,char *user,char *jn, char *j, int *s,struct sockaddr_in *ta){
if(strstr(b, ".quitr") != '\0'){
printf("Reconnecting\n");
close(*s);
startconnect(b, c, n, nick, user, jn, j, s, ta);
}
else if(strstr(b, ".quit") !='\0'){
printf("%s\n", "Disconnecting\n");
send(*s, "QUIT :Exiting\n", strlen("QUIT :Exiting\r\n"), 0);
}
}
void startconnect(char *b, char *c, char *n, char *nick,char *user,char *jn, char *j, int *s,struct sockaddr_in *ta){
if ((*s = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
perror("socket");
exit(1);
}
if (connect(*s, (struct sockaddr *)ta,sizeof(struct sockaddr)) == -1) {
perror("connect");
exit(1);
}
nullstring(c);
strncat(strncat(strncat(c,n,strlen(n)),nick ,strlen(nick)), "\r\n", strlen("\r\n"));
send(*s, c, strlen(c), 0);
send(*s,user,strlen(user), 0);
int i;
for(i = 0; i < 10000; i++);
nullstring(c);
strncat(strncat(strncat(c,jn,strlen(jn)),j ,strlen(j)), "\r\n", strlen("\r\n"));
send(*s, c, strlen(c), 0);
nullstring(c);
printf("%s\n", "Joined Channel\n");
}

/*void joinchan(char *b, char *c, int *s){
if(strstr(b, ".join ")){

}
}*/
