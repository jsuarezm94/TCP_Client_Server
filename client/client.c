/*
 * NAMES: Jose Suarez & Jorge Diaz
 * NETID: jsuarezm & jdiazort
 * DATE: OCTOBER 12, 2016
 * COMPUTE NETWORKS
 * TCP PROGRAM - CLIENT
 * DESCRIPTION:
 */

#include <stdio.h>		// Everything
#include <stdlib.h>		// Exit
#include <errno.h>		// PERROR
#include <string.h>		// Memset
#include <sys/types.h>		// Socket calls
#include <sys/socket.h>		// Socket calls
#include <arpa/inet.h>		// Socket calls
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>

#define BUF_SIZE 4096		// Max size of buffer used for message


int main (int argc, char * argv[]) {

   /* Declare variables */
   FILE		*fn;			// File handle for file (argv3)
   struct	hostent *host;		// Host
   struct	sockaddr_in sin;	// Address struct used for communication
   char		*hp;			// Host
   int		sock;			// File handle for socket
   int		addr_len;		// Used by code
   int		port_num;		// Port number

   /* Check arguments in command line */
   if (argc!=3) {
      fprintf(stderr,"USAGE ERROR: %s <server ip> <port number> <file or string>\n",argv[0]);
      exit(1);
   } // end arguments check

   /* Assign command-line arguments and check for errors*/
   /* Host - argv1 */
   /* Translate host name into peer's IP address */
   host = gethostbyname(argv[1]);
   if (!host) {
      printf("ERROR: host name/unknown host\n");
      exit(1);
   } //end hostname check

   /* Port number - argv2 */
   port_num = atoi(argv[2]);

   /* Build address data structure */
   bzero((char *)&sin, sizeof(sin));
   sin.sin_family = AF_INET;
   bcopy(host->h_addr, (char *)&sin.sin_addr, host->h_length);
   sin.sin_port = htons(port_num);

   /* Open socket and check for errors */
   if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
      perror("ERROR: client-socket()\n");
      exit(1);
   } // end socket check

   /* Connect client and server */
   if (connect(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
      perror("ERROR: client-connect()\n");
      close(sock);
      exit(1);
   } // end connect check

   while(1) {
      char command[10];

      printf("Enter Command: REQ, UPL, LIS, DEL or XIT: ");
      scanf("%s",&command);

      if(send(sock,command,sizeof(command),0) == -1) {
         perror("ERROR: client-send()\n");
         exit(1);
      } // end send check

      if (strcmp(command,"REQ")==0){
         //requestFile(s);
      } else if (strcmp(command,"UPL")==0){
         //uploadFile(s);
      } else if (strcmp(command,"LIS")==0){
         //listDirectory(s);
      } else if (strcmp(command,"DEL")==0){
         //deleteFile(s);
      } else if (strcmp(command,"XIT")==0){
         //close(s);
         exit(1);
      }

   } //end WHILE

} // end MAIN
