/*
 * NAMES: Jose Suarez & Jorge Diaz
 * NETID: jsuarezm & jdiazort
 * DATE: OCTOBER 12, 2016
 * COMPUTE NETWORKS
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

int32_t fileSize(char * file_name)
{
	FILE *fp;
	int32_t file_size = -1;
	if(fp = fopen(file_name, "r")) {
		fseek(fp,0,SEEK_END);
		file_size = ftell(fp);
		fseek(fp,0,SEEK_SET);
		fclose(fp);
	}
	return file_size;
}

void requestFile(int sock){

	/* Declare variables */
        int file_size = 0;
	char file_name[100];
	char md5_recv[100];
	int file_len;
	float nBytes, start_time, end_time, throughput;
	struct timeval tv;

	/* Prompt user input */
	printf("Enter file name to request from server: ");
	scanf("%s",&file_name);

	/* Send length of file_name and file_name */
	file_len = strlen(file_name);
	file_len = htonl(file_len);
	if (send(sock,&file_len,sizeof(file_len),0)==-1){
		perror("ERROR: client-send()\n");
		exit(1);
	}
	if (send(sock,file_name,sizeof(file_name),0)==-1){
		perror("ERROR: client-send()\n");
		exit(1);
	}


	/* Receive and decode file size */
	recv(sock,&file_size,sizeof(int32_t),0);
	
        file_size = ntohl(file_size);
	
	if (file_size == -1) {
                printf("File wa not foound in the server.\n");		
		return;
	}	
	//int tempSize = file_size;
        //if (tempSize == -1) {
        //        printf("File does not exist on the server\n");
        //        return;
        //}



	/* Receive md5 hash from server */
	memset(md5_recv,'\0',sizeof(md5_recv));
	recv(sock,md5_recv,sizeof(md5_recv),0);
	md5_recv[strlen(md5_recv)] = '\0';

	/* Calculate starting time*/
	gettimeofday(&tv,NULL);
	start_time = tv.tv_usec;

	FILE *fp = fopen(file_name,"w");
	if(!fp)
	{
		printf("File does not exist on client side.");
		return;
	}


	/* Receive file from client */
	int n;
	char line[20000];
	memset(line,'\0',sizeof(line));
	int recv_len=0;
	int bytes_recv = 0;
	char recv_buf[10000];
        memset(recv_buf,'\0',sizeof(recv_buf)); //
	int recv_buf_max=sizeof(line);

	if (recv_buf_max>file_size) {
		recv_buf_max=file_size;
	}
	

	while ((recv_len=recv(sock,recv_buf,recv_buf_max,0))>0){
		bytes_recv += recv_len;
		//printf("bytes_recv  = %i\n", bytes_recv);
		//printf("recvbug = %s\n", recvbuf);
		//printf("sizeof(buf_line) = %i\n", sizeof(buf_line));
		int write_size = fwrite(recv_buf, sizeof (char), recv_len, fp);
		if (write_size<recv_len) {
			printf("File write failed!\n");
		}
		bzero(line, sizeof(line));
		memset(recv_buf,'\0',sizeof(recv_buf));
		if (bytes_recv>=file_size) break;
	}
	fclose(fp);



	/* Get end time and throughput */
	gettimeofday(&tv,NULL);
	end_time = tv.tv_usec; //in microsecond
	float RTT = (end_time-start_time) * pow(10,-6); //RTT in seconds
	throughput = (bytes_recv*pow(10,-6))/RTT;

	/* Work out md5 hash of received file and compare them */
	int size = file_size;
	unsigned char md5[MD5_DIGEST_LENGTH];
	char* file_buffer = (char*) malloc(20000);
	int file_description;
	file_description = open(file_name,O_RDONLY);
	file_buffer = mmap(0,size, PROT_READ, MAP_SHARED, file_description, 0);
	MD5((unsigned char*) file_buffer, size, md5);
	munmap(file_buffer, size);



	

	/* Turn md5hash into a string */
	int i,j;
	char md5_str[2*MD5_DIGEST_LENGTH+2];
	memset(md5_str,'\0',sizeof(md5_str));
	char temp_str[2];
	for(i=0; i<MD5_DIGEST_LENGTH; i++) {
		sprintf(temp_str,"%02x",md5[i]);
		md5_str[i*2]=temp_str[0];
		md5_str[(i*2)+1]=temp_str[1];
	}
	md5_str[2*MD5_DIGEST_LENGTH]='\0';



	/* Compare the md5 hashes */
	if (strcmp(md5_recv,md5_str)==0){
		printf("Successful Transfer\n");
		printf("%i bytes received in %f seconds : %f Megabytes/sec\n",bytes_recv,RTT,throughput);
		printf("File MD5sum: %s\n",md5_recv);
	} else {
		printf("Transfer unsuccessful\n");
	}
}


void upload(int sock) {

	/* Declare variables */
	char file_name[100];				// File name
	int file_len;					// Length of file name
        int file_size;					// File size
        int original_size;              		// File size used for MD5 hash
	int server_response;				// Server confirmation
        FILE *fp;                       		// File pointer
	char file_line[20000];
	unsigned char md5[MD5_DIGEST_LENGTH];
	char* file_buf = (char*) malloc(20000);		// File buffer
	int file_des;					// File description

	/* Prompt user input */
	printf("Please enter name of file to send: ");
	memset(file_name,'\0',sizeof(file_name));
	scanf("%s",file_name);

	/* Comput length of file name*/
	file_len = strlen(file_name);
	file_len = htonl(file_len);

	/* Send length of file name and file name to server */
	if (send(sock,&file_len,sizeof(file_len),0)==-1){
		perror("ERROR: client-send()\n");
		exit(1);
	}
        if (send(sock,file_name,sizeof(file_name),0)==-1){
                perror("ERROR: client-send()\n");
		exit(1);
        }

	/* Receive server confirmation regarding readiness to receive file */
	if (recv(sock,&server_response, sizeof(server_response),0) == -1) {
		perror("ERROR: client-recv()\n");
		exit(1);
	}
	
	/* Interpret server confirmation and output appropriate message to user */
	if (server_response != 1) {
		printf("Server not ready to receive local file\n");
		return;
	}

	/* Compute file size and send to server */
	file_size = fileSize(file_name);
	original_size = file_size;
	file_size = htonl(file_size);
	if (send(sock,&file_size,sizeof(int32_t),0) == -1) {
		perror("ERROR: client-send()\n");
		exit(1);
	}

	if (file_size == -1) {
		printf("File does not exist locally\n");
		return;
	}	

	/* Open file */
	fp = fopen(file_name, "r");
	memset(file_line,'\0',sizeof(file_line));

	/* Read file and send to server line by line */
	int len = 0;
	int sent_len = 0;
	while ( (len = fread(file_line,sizeof(char),sizeof(file_line),fp)) > 0) {
                sent_len = send(sock,file_line,len,0);
                memset(file_line,'\0',sizeof(file_line));
        }
	fclose(fp);

	/* Create MD5 hash */
	file_des = open(file_name,O_RDONLY);
	file_buf = mmap(0,original_size,PROT_READ,MAP_SHARED,file_des, 0);
	MD5((unsigned char*) file_buf, (int) original_size, md5);
	munmap(file_buf, original_size);

	/* Compute MD5 hash on file */
	int i;
        char line[2*MD5_DIGEST_LENGTH+2];
        memset(line,'\0',sizeof(line));
        char line_check[2];
        for(i=0; i<MD5_DIGEST_LENGTH; i++) {
                sprintf(line_check,"%02x",md5[i]);
                line[i*2]=line_check[0];
                line[(i*2)+1]=line_check[1];
        }
        line[2*MD5_DIGEST_LENGTH]='\0';

        char md5_line[strlen(line)+1];
        memcpy(md5_line,line,strlen(line));
        md5_line[strlen(line)] = '\0';

	/* Send MD5 hash to server for verification */
	if (send(sock,md5_line,strlen(line),0) == -1) {
		perror("ERROR: client-send()\n");
		exit(1);
	}

	/* Receive final confirmation from server and output appropriate message to user */
        char upload_result[150];
        memset(upload_result,'\0',sizeof(upload_result));

	if (recv(sock,upload_result,sizeof(upload_result),0) == -1) {
		perror("ERROR: client-recv()\n");
		exit(1);
	}

        if(!strcmp(upload_result,"Unsuccessful transfer")){
                printf("%s\n",upload_result);
        } else {
                printf("Successful Transfer\n%s\n",upload_result);
                printf("File MD5sum: %s\n",md5_line);
        }

}

void list(int sock ) {

	/* Declare variables */
	int dir_size=0;				// Directory size
	int dir_bytes=0;			// Bytes per directory entry
	float n_bytes=0;			// Total bytes received
	char list_buf[100];			// Buffer holding list of directory files

	/* Receive size of directory from server */
	recv(sock, &dir_size, sizeof(int32_t), 0);
	dir_size = ntohl(dir_size);

	/* Read directory listing and display listing to user */
	memset(list_buf, '\0', sizeof(list_buf));						// Empty buffer
	while (n_bytes < dir_size) {
		dir_bytes = recv(sock, list_buf, sizeof(list_buf), 0);
		n_bytes += dir_bytes;
		printf("%s\n",list_buf);
		fflush(stdout);
		memset(list_buf, '\0', sizeof(list_buf));
	} //end WHILE

	return;

} //end LISTDIRECTORY

void delete( int sock ) {

	/* Declare variables */
	char file_name[MAX_FILENAME];		// Directory name
	short int file_name_len;		// Length of directory name
	int server_response;			// Server confirmation
	char user_confirmation[5];		// User confirmation
	int server_ack_response;		// Server confirmation

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
	if (recv( sock,&server_response,sizeof(int),0) == -1) {
		perror("ERROR: client-recv()\n");
		exit(1);
	} // end receive check

	if (server_response == -1) {
		printf("The file does not exist on server\n");
		return;
	} // end if

	/* Confirm deletion of file */
	printf("File Exists\n");
	memset(file_name, '\0',sizeof(file_name));
	printf("Confirm deletion of file %s? (Yes\\No)\n", file_name);
	scanf("%s", user_confirmation);

	if (send(sock, user_confirmation, sizeof(user_confirmation), 0) == -1) {
		perror("ERROR: client-send()\n");
		exit(1);
	} // end send check

	if (strcmp(user_confirmation,"No") == 0) {
		printf("Delete abandoned by the user!\n");
		return;
	}

	/* Server confirmation of file deletion */
	if (recv(sock, &server_ack_response, sizeof(int), 0) == -1) {
		perror("ERROR: client-recv()\n");
		exit(1);
	}
	if (server_ack_response == 1) {
		printf("File deletion: successful\n");
	} else {
		printf("File deletion: not successful\n");
	}

} //end DELETEFILE

void makeDirectory (int sock) {

	/* Declare variables */
	char dir_name[100];			// Directory name
	int dir_name_len;			// Length of directory name
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
	char dir_name[100];			// Directory name
	int dir_name_len;			// Length of directory name
	int server_response;			// Server confirmation
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
		return;
	}

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
		return;
	}

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
      fprintf(stderr,"USAGE ERROR: %s <server ip> <port number>\n",argv[0]);
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

      if (strcmp(command,"REQ")==0) {
         requestFile(sock);
      } else if (strcmp(command,"UPL")==0) {
         upload(sock);
      } else if (strcmp(command,"LIS")==0) {
		list(sock);
	} else if (strcmp(command,"DEL")==0) {
		delete(sock);
	} else if (strcmp(command,"MKD")==0) {
		makeDirectory(sock);
	} else if (strcmp(command,"RMD")==0) {
		removeDirectory(sock);
	} else if (strcmp(command,"CHD")==0) {
		changeDirectory(sock);
	} else if (strcmp(command,"XIT")==0) {
		close(sock);
		printf("Session has been closed\n");
         	exit(1);
	}
   } //end WHILE
} // end MAIN
