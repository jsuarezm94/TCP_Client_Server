/*
 * NAMES: Jose Suarez & Jorge Diaz
 * NETID: jsuarezm & jdiazort
 * DATE: OCTOBER 12, 2016
 * COMPUTE NETWORKS
 * TCP PROGRAM - CLIENT
 * DESCRIPTION:
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define MAX_PENDING 5				// Max number of pending connections
/*
#define BUF_SIZE 4096           	// Max size of buffer used for message
*/

int main (int argc, char * argv[]) {

   /* Declare variables */
   struct	sockaddr_in sin;		// Address structure
   int		addr_len;				// Length
   char		buf[10];					// Buffer for commands
   int		sock, new_sock;		// Sockets
   int		opt = 1;
   int		port_num;				// Port number

   /* Check arguments in command line */
   if (argc!=2) {
      fprintf(stderr,"USAGE ERROR: %s <port number>\n",argv[0]);
      exit(1);
   } // end arguments check

   /* Assign command-line arguments and check for errors*/
   /* Port number - argv1 */
   port_num = atoi(argv[1]);

   /* Build address data structure */
   bzero((char *)&sin, sizeof(sin));
   sin.sin_family = AF_INET;
   sin.sin_addr.s_addr = INADDR_ANY;
   sin.sin_port = htons(port_num);

   /* Setup passive open */
   /* Create a socket on the server side */
   if ((sock = socket(PF_INET, SOCK_STREAM,0)) < 0) {
      perror("ERROR: server-socket()\n");
      exit(1);
   } // end socket check

   /* Set socket options */
   if ((setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)& opt, sizeof(int))) < 0) {
      perror("ERROR: server-setsockopt()\n");
      exit(1);
   } //end socket options check

   /* Bind the created socket to the specified address */
   if ((bind(sock,(struct sockaddr *)&sin, sizeof(sin))) < 0) {
      perror("ERROR: server-bind()\n");
      exit(1);
   } //end bind check

	while(1) {
		
		if ((listen(sock, MAX_PENDING)) < 0) {
			perror("ERROR: server-listen()\n");
		} //end listen check

		if ((new_sock = accept(sock, (struct sockaddr *)&sin, &addr_len)) < 0) {
			perror("ERROR: server-accept()\n");
		} //end accept check

		int connected = 1;
		while (connected) {
			while( strlen(buf) == 0) {
				recv(new_sock, buf, sizeof(buf), 0);
			}

			if (strcmp(buf,"REQ")==0){
				//clientRequest(new_s);
			} else if (strcmp(buf,"UPL")==0){
				//upload(new_s);
			} else if (strcmp(buf,"DEL")==0){
				//deleteFile(new_s);
			} else if (strcmp(buf,"LIS")==0){
				//listDirectory(new_s);
			} else if (strcmp(buf,"XIT")==0){
				//connected = 0;
			}
			memset(buf,'\0',sizeof(buf));

		} //end connection WHILE

	} // end WHILE

} // end MAIN
