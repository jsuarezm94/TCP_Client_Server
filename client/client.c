/*
 * NAMES: Jose Suarez & Jorge Diaz
 * NETID: jsuarezm & jdiazort
 * DATE: OCTOBER 12, 2016
 * COMPUTE NETWORKS
 * TCP PROGRAM - CLIENT
 * DESCRIPTION:
 */

/*
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
#include <openssl/md5.h>
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <openssl/md5.h>

#define BUF_SIZE 4096		// Max size of buffer used for message
#define MAX_FILENAME 100	// Max size of file name sent to server
#define MAX_MD5LENGTH 50

int32_t file_exists(char * filename)
{
	FILE *file;
	int32_t size;
	if(file = fopen(filename, "r"))
	{
		fseek(file,0,SEEK_END);
		size = ftell(file);
		fseek(file,0,SEEK_SET);
		fclose(file);
		return size;
	}
	return -1;
}

//void uploadFile( int sock ) {

	/* Declare variables */
/*
	char file_name[MAX_FILENAME];
	int file_len;
	char file_ack[10];
	int file_size;
	char line_to_send[20000];
	int line_sent;
	int check_line;
	FILE *fp;								// File to upload
	unsigned char md5[MD5_DIGEST_LENGTH];
	char *file_buffer = (char *)malloc(20000);
	int file_description;

	char md5server[100];

*/
	/* Prompt user input */
/*
	printf("Enter file to send: ");
	memset(file_name,'\0',sizeof(file_name));
	scanf("%s",file_name);
*/
	/* Get file details */
/*
	file_len = strlen(file_name);
	file_len = htonl(file_len);
*/
	/* Send file details to server */
	/* File name length */
/*
	if (send(sock, &file_len, sizeof(file_len), 0) == -1) {
		perror("ERROR: client-send()\n");
		exit(1);
	} //end send check
*/
	/* File name */
/*
	if (send(sock, file_name, sizeof(file_name), 0) == -1) {
		perror("ERROR: client-send()\n");
		exit(1);
	} //end send check

printf("File name sent: %s\n",file_name);
printf("Waiting to receive ACK from server\n");
*/
	/* Receive ack from server */
/*
	memset(file_ack, '\0', sizeof(file_ack));
	while (strlen(file_ack) == 0) {
		recv(sock,file_ack,sizeof(file_ack),0);
	}

printf("ACK received from server: %s\n", file_ack);

	if (strcmp(file_ack,"ACK") != 0) {
		printf("Acknowledgement not received from server\nProcess aborted\n");
		return;
	}

printf("Ready to send file size to server\n");
*/
	/* Send file size to server */
/*
	int file_size2 = file_exists(file_name);
	file_size = file_size2;
	file_size2 = htonl(file_size2);
	if (send(sock,&file_size2,sizeof(int32_t),0) == -1) {
		perror("ERROR: client-send()\n");
		exit(1);
	}

printf("Send file size\n");
*/
	/* Send file to server */
	/* Client must send file as stream - using a line by line approach */
/*
	fp = fopen(file_name, "r");
	memset(line_to_send, '\0', sizeof(line_to_send));
printf("HERE\n");
	while( check_line = fread(line_to_send,sizeof(char),sizeof(line_to_send),fp) > 0 ) {
printf("HERE 2\n");
		if( line_sent = send(sock,line_to_send,check_line,0) == -1) {
			perror("ERROR: client-send()\n");
			exit(1);
		} //end send check
		memset(line_to_send,'\0',sizeof(line_to_send));

	} //end fread check
	fclose(fp);
printf("HERE 3\n");
*/
	/* Open file to get md5 hash */
/*
	file_description = open(file_name,O_RDONLY);
	file_buffer = mmap(0,file_size, PROT_READ, MAP_SHARED, file_description, 0);
	MD5((unsigned char*) file_buffer, file_size, md5);
printf("HERE 4\n");
	int i,j;
	char str[2*MD5_DIGEST_LENGTH+2];
	memset(str,'\0',sizeof(str));
	char str2[2];
	for(i=0; i<MD5_DIGEST_LENGTH; i++)
	{
		sprintf(str2,"%02x",md5[i]);
		str[i*2]=str2[0];
		str[(i*2)+1]=str2[1];
	}
	str[2*MD5_DIGEST_LENGTH]='\0';

	char md5str[strlen(str)+1];
	memcpy(md5str,str,strlen(str));
	md5str[strlen(str)] = '\0';
	send(sock,md5str,strlen(str),0);
	char result[150];
	memset(result,'\0',sizeof(result));
	while(strlen(result) == 0)
	{
		recv(sock,result,sizeof(result),0);
	}

	if(!strcmp(result,"Unsuccessful transfer\n")){
		printf("%s\n",result);
	} else {
		printf("Successful Transfer\n%s\n",result);
		printf("File MD5sum: %s\n",md5str);
	}


} //end UPLOADFILE
*/

void requestFile(int s){
	char query[20];
	char filename[100];
	char md5server[100];
	int filelen;
	float nBytes, start_time, end_time, throughput;
	struct timeval tv;

	memset(query,'\0',sizeof(query));
	while(strlen(query)==0){
		recv(s,query,sizeof(query),0);
	}
	if (strlen(query)==0) return;

	printf("%s ",query);
	scanf("%s",&filename);

	filelen = strlen(filename);
	filelen = htonl(filelen);
//printf("Before sending file details\n");
	if (send(s,&filelen,sizeof(filelen),0)==-1){
		perror("client send error."); exit(1);
	}
	if (send(s,filename,sizeof(filename),0)==-1){
		perror("client send error."); exit(1);
	}
//printf("After sending file details\n");
	int filesize = 0;
	recv(s,&filesize,sizeof(int32_t),0);
	filesize = ntohl(filesize);
//printf("Received file size: %d\n", filesize);
	memset(md5server,'\0',sizeof(md5server));
	while(strlen(md5server)==0){
		recv(s,md5server,sizeof(md5server),0);
	}
	md5server[strlen(md5server)] = '\0';

//printf("Received md5server: %s\n",md5server);

	gettimeofday(&tv,NULL);
	start_time = tv.tv_usec;

//printf("Before opening file\n");
	FILE *fp = fopen(filename,"w");
	if(!fp)
	{
		printf("File does not exist");
		return;
	}
	int n;
	char line[20000];
	memset(line,'\0',sizeof(line));
	int recv_len=0;
	int bytesrevd = 0;
	char recvbuf[10000];
	int rcvbufmax=sizeof(line);
	if (rcvbufmax>filesize) {
		rcvbufmax=filesize;
	}
	//memset(recvbuf,'\0',sizeof(recvbuf));
	while ((recv_len=recv(s,recvbuf,rcvbufmax,0))>0){
	//while (recv_len = recv(s, recvbuf, sizeof(recvbuf), 0) > 0) {
		bytesrevd += recv_len;
		int write_size = fwrite(recvbuf, sizeof (char), recv_len, fp);
		if (write_size<recv_len) {
			printf("File write failed!\n");
		}
		bzero(line, sizeof(line));
		memset(recvbuf,'\0',sizeof(recvbuf));
printf("Bytes received: %d\n", bytesrevd);
printf("File size: %d\n", filesize);
		if (bytesrevd>=filesize) break;
	}
	fclose(fp);

	gettimeofday(&tv,NULL);
	end_time = tv.tv_usec; //in microsecond
	float RTT = (end_time-start_time) * pow(10,-6); //RTT in seconds
	throughput = (bytesrevd*pow(10,-6))/RTT;

	int size = filesize;
	unsigned char md5[MD5_DIGEST_LENGTH];
	char* file_buffer = (char*) malloc(20000);
	int file_description;

	file_description = open(filename,O_RDONLY);
	file_buffer = mmap(0,size, PROT_READ, MAP_SHARED, file_description, 0);
	MD5((unsigned char*) file_buffer, size, md5);
	munmap(file_buffer, size);

	int i,j;
	char str[2*MD5_DIGEST_LENGTH+2];
	memset(str,'\0',sizeof(str));
	char str2[2];
	for(i=0; i<MD5_DIGEST_LENGTH; i++) {
		sprintf(str2,"%02x",md5[i]);
		str[i*2]=str2[0];
		str[(i*2)+1]=str2[1];
	}
	str[2*MD5_DIGEST_LENGTH]='\0';

	if (strcmp(md5server,str)==0){
		printf("Successful Transfer\n");
		printf("%i bytes received in %f seconds : %f Megabytes/sec",bytesrevd,RTT,throughput);
		printf("File MD5sum: %s\n",md5server);
	} else {
		printf("Transfer unsuccessful.\n");
	}
}

void uploadFile(int s) {
	char filename[100];
	int filelen;
	char ack[4];

	printf("Please enter filename you would like to send: ");
	memset(filename,'\0',sizeof(filename));
	scanf("%s",filename);

	filelen = strlen(filename);
	filelen = htonl(filelen);
	if (send(s,&filelen,sizeof(filelen),0)==-1){
		perror("client send error."); exit(1);
	}

        if (send(s,filename,sizeof(filename),0)==-1){
                perror("client send error."); exit(1);
        }

        memset(ack,'\0',sizeof(ack));
        while(strlen(ack)==0)
        {
                recv(s,ack,sizeof(ack),0);
        }
        if(strcmp(ack,"ACK") != 0)
        {
                printf("ACK not received");
                return;
        }

	int file_size = file_exists(filename);
	int size = file_size;
	file_size = htonl(file_size);
	send(s,&file_size,sizeof(int32_t),0);

	char line[20000];
	FILE *fp = fopen(filename, "r");
	memset(line,'\0',sizeof(line));
	int len =0;
	int sent_len=0;
	while((len=fread(line,sizeof(char),sizeof(line),fp))>0)
	{
		sent_len=send(s,line,len,0);
		memset(line,'\0',sizeof(line));
	}
	fclose(fp);

	unsigned char md5[MD5_DIGEST_LENGTH];
	char* file_buffer = (char*) malloc(20000);
	int file_description;

	file_description = open(filename,O_RDONLY);
	file_buffer = mmap(0,size,PROT_READ,MAP_SHARED,file_description, 0);
	MD5((unsigned char*) file_buffer, (int) size, md5);
	munmap(file_buffer, size);

	int i,j;
	char str[2*MD5_DIGEST_LENGTH+2];
	memset(str,'\0',sizeof(str));
	char str2[2];
	for(i=0; i<MD5_DIGEST_LENGTH; i++)
	{
		sprintf(str2,"%02x",md5[i]);
		str[i*2]=str2[0];
		str[(i*2)+1]=str2[1];
	}
	str[2*MD5_DIGEST_LENGTH]='\0';

	char md5str[strlen(str)+1];
	memcpy(md5str,str,strlen(str));
	md5str[strlen(str)] = '\0';
	send(s,md5str,strlen(str),0);
	char result[150];
	memset(result,'\0',sizeof(result));
	while(strlen(result) == 0)
	{
		recv(s,result,sizeof(result),0);
	}

	if(!strcmp(result,"Unsuccessful transfer")){
		printf("%s\n",result);
	} else {
		printf("Successful Transfer\n%s\n",result);
		printf("File MD5sum: %s\n",md5str);
	}
}

void listDirectory( int sock ) {

	/* Declare variables */
	int dir_size=0;
	int dir_bytes=0;
	float n_bytes=0;
	char file_buf[100];

	/* Receive size of directory from server */
	recv(sock, &dir_size, sizeof(int32_t), 0);
	dir_size = ntohl(dir_size);

	/* Read directory listing */
	memset(file_buf, '\0', sizeof(file_buf));	// Empty buffer
	while (n_bytes < dir_size) {
		dir_bytes = recv(sock, file_buf, sizeof(file_buf), 0);
		n_bytes += dir_bytes;
		printf("%s\n",file_buf);
		fflush(stdout);
		memset(file_buf, '\0', sizeof(file_buf));
	} //end WHILE

} //end LISTDIRECTORY

void deleteFile( int sock ) {

	/* Declare variables */
	char file_name[MAX_FILENAME];
	short int file_name_len;
	int file_ack;
	char user_response[4];
	int server_ack;

	/* Prompt user input */
	printf("Enter file name to delete: ");
	scanf("%s",file_name);

	/* Prepare variables to send to server */
	file_name_len = strlen(file_name);
	file_name_len = htonl(file_name_len);

	/* Send file length/name to server */
	if (send(sock, &file_name_len, sizeof(int32_t), 0) == -1) {
		perror("ERROR: client-send()\n");
		exit(1);
	} //end send check
	if (send(sock, file_name, sizeof(file_name),0) == -1) {
		perror("ERROR: client-send()\n");
		exit(1);
	} //end send check
	
	/* Receive confirmation of file from server */
	file_ack = 0;
	if (file_ack = recv(sock, &file_ack, sizeof(int), 0) == -1) {
		perror("ERROR: client-recv()\n");
		exit(1);
	} // end receive check

	/* Confirm deletion of file */
	printf("File Exists\n");
	printf("Confirm deletion of file %s? (Yes\\No)\n", file_name);
	scanf("%s", user_response);
	user_response[3] = '\0';

	if (send(sock, user_response, sizeof(user_response), 0) == -1) {
		perror("ERROR: client-send()\n");
		exit(1);
	} // end send check

printf("SENT user response: %s\n",user_response);

	/* Server confirmation of file deletion */
	recv(sock, &server_ack, sizeof(int), 0);
	printf("RECEIVED ACK: %d\n", server_ack);
	if (server_ack == 1) {
		printf("File deletion: successful\n");
	} else {
		printf("File deletion: not successful\n");
	}

} //end DELETEFILE

void makeDirectory (int sock) {

	/* Declare variables */
	char dir_name[100];			// Directory name
	int dir_name_len;				// Length of directory name
	int server_response;			// Server confirmation

	/* Prompt user input */
	printf("Enter name of directory to be created at the server\n");
	memset(dir_name, '\0', sizeof(dir_name));
	scanf("%s", dir_name);

	/* Compute length of directory name */
	dir_name_len = strlen(dir_name);
	dir_name_len = htonl(dir_name_len);

	/* Send length of directory name and directory name to server */
	if( send( sock, &dir_name_len, sizeof(dir_name_len),0) == -1) {
		perror("ERROR: client-send()\n");
		exit(1);
	} // end send check
	if( send( sock, dir_name, sizeof(dir_name),0) == -1) {
      perror("ERROR: client-send()\n");
      exit(1);
	} // end send check

	/* Receive server confirmation regarding creation of directory */
	if( recv( sock, &server_response, sizeof(server_response),0) == -1) {
		perror("ERROR: client-recv()\n");
      exit(1);
	} //end recv check

	/* Interpret server confirmation and output appropriate message to user */
	if (server_response == -2) {
		printf("The directory already exists on server\n");
	} else if (server_response == -1) {
		printf("Error in making directory\n");
	} else if (server_response == 1) {
		printf("The directory was successfully made\n");
	} //end if
} //end MAKEDIRECTORY

void removeDirectory (int sock) {

	/* Declare variables */
	char dir_name[100];				// Directory name
	int dir_name_len;					// Length of directory name
	int server_response;				// Server confirmation
	char user_confirmation[5];		// User confirmation
	int server_ack_response;		// Server confirmation

   /* Prompt user input */
	printf("Enter name of directory to be removed at the server\n");
	memset(dir_name, '\0', sizeof(dir_name));
	scanf("%s", dir_name);

   /* Compute length of directory name */
   dir_name_len = strlen(dir_name);
   dir_name_len = htonl(dir_name_len);

   /* Send length of directory name and directory name to server */
   if( send( sock, &dir_name_len, sizeof(dir_name_len),0) == -1) {
      perror("ERROR: client-send()\n");
      exit(1);
   } // end send check
   if( send( sock, dir_name, sizeof(dir_name),0) == -1) {
      perror("ERROR: client-send()\n");
      exit(1);
   } // end send check

   /* Receive server confirmation regarding existence of directory */
   if( recv( sock, &server_response, sizeof(server_response),0) == -1) {
      perror("ERROR: client-recv()\n");
      exit(1);
   } //end recv check

   /* Interpret server confirmation and output appropriate message to user */
	if (server_response == -1) {
		printf("The directory does not exist on server\n");
	} else if (server_response == 1) {

   	/* Prompt user input */
		printf("Directory exists\n");
		printf("Confirm deletion of directory %s? (Yes\\No)\n", dir_name);
		scanf("%s", user_confirmation);

		/* Send user confirmation of directory deletion to server */
		if( send( sock, user_confirmation, sizeof(user_confirmation), 0) == -1) {
			perror("ERROR: client-recv()\n");
			exit(1);
		} // end send check

		if (strcmp(user_confirmation,"No") == 0) {
			printf("Delete abandoned by the user!\n");
		} else {

   		/* Receive server confirmation regarding deletion of directory */
			if( recv( sock, &server_ack_response, sizeof(server_ack_response),0) == -1) {
				perror("ERROR: client-recv()\n");
				exit(1);
			} // end recv check

   		/* Interpret server confirmation and output appropriate message to user */
			if( server_ack_response == -1) {
				printf("Failed to delete directory\n");
			} else if ( server_ack_response == 1) {
				printf("Directory deleted\n");
			} // end if
		} //end if
	} // end if
} //end REMOVEDIRECTORY

void changeDirectory (int sock) {

	/* Declare variables */
   char dir_name[100];				// Directory name
   int dir_name_len;					// Length of directory name
   int server_response;				// Server confirmation

   /* Prompt user input */
   printf("Enter name of directory to change to\n");
   memset(dir_name, '\0', sizeof(dir_name));
   scanf("%s", dir_name);

   /* Compute length of directory name */
   dir_name_len = strlen(dir_name);
   dir_name_len = htonl(dir_name_len);

   /* Send length of directory name and directory name to server */
   if( send( sock, &dir_name_len, sizeof(dir_name_len),0) == -1) {
      perror("ERROR: client-send()\n");
      exit(1);  
   } // end send check
   if( send( sock, dir_name, sizeof(dir_name),0) == -1) {
      perror("ERROR: client-send()\n");
      exit(1);
   } // end send check

   /* Receive server confirmation regarding change of directory */
   if( recv( sock, &server_response, sizeof(server_response),0) == -1) {
      perror("ERROR: client-recv()\n");
      exit(1);
   } //end recv check

printf("Server: %d\n",server_response);

   /* Interpret server confirmation and output appropriate message to user */
   if (server_response == -2) {
      printf("The directory does not exist on server\n");
   } else if (server_response == -1) {
      printf("Error in changing directory\n");
   } else if (server_response == 1) {
      printf("Changed current directory\n");
   } //end if
} //end CHANGEDIRECTORY

int main (int argc, char * argv[]) {

   /* Declare variables */
   FILE		*fn;						// File handle for file (argv3)
   struct	hostent *host;			// Host
   struct	sockaddr_in sin;		// Address struct used for communication
   char		*hp;						// Host
   int		sock;						// File handle for socket
   int		addr_len;				// Used by code
   int		port_num;				// Port number

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

	printf("Connection successful with server\n");

   while(1) {
      char command[10];

      printf("Enter Command: REQ, UPL, LIS, DEL, MKD, RMD, CHD or XIT: ");
      scanf("%s",&command);

      if(send(sock,command,sizeof(command),0) == -1) {
         perror("ERROR: client-send()\n");
         exit(1);
      } // end send check

      if (strcmp(command,"REQ")==0){
         requestFile(sock);
      } else if (strcmp(command,"UPL")==0){
         uploadFile(sock);
      } else if (strcmp(command,"LIS")==0){
         listDirectory(sock);
      } else if (strcmp(command,"DEL")==0){
         deleteFile(sock);
		} else if (strcmp(command,"MKD")==0){
			makeDirectory(sock);
		} else if (strcmp(command,"RMD")==0){
			removeDirectory(sock);
		} else if (strcmp(command,"CHD")==0){
			changeDirectory(sock);
      } else if (strcmp(command,"XIT")==0){
         close(sock);
         exit(1);
      }

   } //end WHILE

} // end MAIN
