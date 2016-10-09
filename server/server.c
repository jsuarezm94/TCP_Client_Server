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
#include <sys/time.h>
#include <openssl/md5.h>
#include <sys/types.h>
#include <sys/stat.h>
#define MAX_MD5LENGTH 50
#define MAX_FILENAME 100
#define MAX_PENDING 5
#define MAX_LINE 256


int32_t fileSize(char * file_name)
{
	FILE * fp;
	int32_t file_size = -1;
	if (fp = fopen(file_name, "r")) {
		fseek(fp,0,SEEK_END);
		file_size = ftell(fp);
		fseek(fp,0,SEEK_SET);
		fclose(fp);
	}
	return file_size;
}

void makeDirectory(int sock) {

        int dir_name_len = 0;
        int dir_status;

        recv(sock,&dir_name_len,sizeof(dir_name_len),0);
        dir_name_len = ntohl(dir_name_len);

        char dir_name[dir_name_len+1];

        memset(dir_name,'\0',sizeof(dir_name));
        while(strlen(dir_name)==0){
                recv(sock,dir_name,sizeof(dir_name),0);
        }

        if (access(dir_name,F_OK)!=-1){
                dir_status = -2;
        } 
	else if ((dir_status = mkdir(dir_name, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)) !=  -1) {
		dir_status = 1;
	}
	
        send(sock,&dir_status,sizeof(int),0);
}


void removeDirectory(int sock) {

        int dir_name_len = 0;
        int dir_status;

        recv(sock,&dir_name_len,sizeof(dir_name_len),0);
        dir_name_len = ntohl(dir_name_len);
        char dir_name[dir_name_len+1];

        memset(dir_name,'\0',sizeof(dir_name));

        while(strlen(dir_name)==0){
                recv(sock,dir_name,sizeof(dir_name),0);
        }

        if (access(dir_name,F_OK)!=-1){
                dir_status = 1;
        }
	else {
		dir_status = -1;
		send(sock,&dir_status,sizeof(int),0);
		return;
	}	

        printf("before receiveing\n");
        send(sock,&dir_status,sizeof(int),0);
	printf("sfter sendin...\n");


	printf("After sending dir exists\n");	
	char yes_delete[5];
	int confirm_deletion = -1;
	memset(yes_delete, '\0', sizeof(yes_delete));
	printf("before loop\n");
	
	//while (strlen(yes_delete)==0) {
	//	printf("inside loop, yes del: %i\n", strlen(yes_delete));
	while (	recv(sock, yes_delete, sizeof(yes_delete), 0) <= 0) {}	

	printf("yes del: %c", yes_delete);

	if (strcmp(yes_delete,"Yes")) {
		if ( rmdir(dir_name) == 0 ) {
			confirm_deletion = 1;		
		}
	}
	send(sock, &confirm_deletion, sizeof(int), 0);
	printf("sent!");

}


void changeDirectory(int sock) {

        int dir_name_len = 0;
        int dir_status;

        recv(sock,&dir_name_len,sizeof(dir_name_len),0);
        dir_name_len = ntohl(dir_name_len);

        char dir_name[dir_name_len+1];
        memset(dir_name,'\0',sizeof(dir_name));

        while(strlen(dir_name)==0){
                recv(sock,dir_name,sizeof(dir_name),0);
        }

	printf("received name, no ceking dir");
      
	//printf("chdir status %i\n", chdir(dir_name)); 

	//int tesdir(dir_name);
	if (chdir(dir_name) == 0 ) {
		printf("Setting dir_name to 0\n");
		dir_status = 1;
	}
	else if ( access(dir_name,F_OK) == -1) {
                printf("Setting dir_name to -2\n");
		dir_status = -2;
	}
	else {
                printf("Setting dir_name to -1\n");
		dir_status = -1;
	}
	
	printf("Sent %i\n", dir_status);
        send(sock,&dir_status,sizeof(int),0);
}


void upload(int sock) {
	
	/* Initializing variables */
        int file_name_len;
        char md5_recv[100];
        struct timeval tv;
        float start_time, end_time, nBytes, throughput;
        char file_name[100];

        int file_size = 0; // gt 1

	/* Reveiving file name length*/
        recv(sock,&file_name_len,sizeof(file_name_len),0);
        file_name_len = ntohl(file_name_len);
        printf("Received file_name_length\n");

	/* Receiving file name */
        memset(file_name,'\0',sizeof(file_name));
        //while(strlen(file_name)<file_name_len){
                recv(sock,file_name,sizeof(file_name),0);
        //}
        printf("file_name received.\n");

	/* Sending file name and its file name length received confirmation*/
	int y = 1;
        if(send(sock,&y,sizeof(int),0)==-1){
                perror("Server send error!"); exit(1);
        }


	/* Encoding file size. */
        //gt 1 int file_size = 0;
        recv(sock,&file_size,sizeof(int32_t),0);
        file_size = ntohl(file_size);
        printf("filesize received = %d\n", file_size);

	/* Starting timer for RTT calculation */
        gettimeofday(&tv,NULL);
        start_time = tv.tv_usec;

	/* Opening file */
        FILE *fp = fopen(file_name,"w");

	printf("Here1\n");


        int n;
        char file_line[20000];
        memset(file_line,'\0',sizeof(file_line));
        int recv_len=0;
        int bytes_recv = 0;
        char recv_buf[10000];
        int recv_buf_max=sizeof(file_line);

	/* Receiving file from client */
        if (recv_buf_max>file_size)
                recv_buf_max=file_size;
        while ((recv_len=recv(sock,recv_buf,recv_buf_max,0))>0){
                bytes_recv += recv_len;
                int write_size = fwrite(recv_buf, sizeof (char), recv_len, fp);
                if (write_size<recv_len)
                        printf("File write failed!\n");
                bzero(file_line, sizeof(file_line));
                if (bytes_recv >= file_size) break;
        }
        fclose(fp);

        printf("Here2\n");

	/* Stopping timer for RTT calculation */
        gettimeofday(&tv,NULL);
        end_time = tv.tv_usec; //in microsecond
        float RTT = (end_time-start_time) * pow(10,-6); //RTT in seconds
        throughput = (bytes_recv*pow(10,-6))/RTT;

        printf("Here3\n");

	/* Receiving md5 from client */
        memset(md5_recv,'\0',sizeof(md5_recv));
        //while(strlen(md5client)==0){
                recv(sock,md5_recv,sizeof(md5_recv),0);
        //}
        md5_recv[strlen(md5_recv)] = '\0';

        printf("Here4\n");

        int size_orig = file_size;
        unsigned char md5_buf[MD5_DIGEST_LENGTH];
        char* file_buffer = (char*) malloc(20000);
        int file_description;

	/* Opening file and md5 */
        file_description = open(file_name,O_RDONLY);
        file_buffer = mmap(0,size_orig,PROT_READ,MAP_SHARED,file_description,0);
        MD5((unsigned char*) file_buffer, size_orig, md5_buf);

        munmap(file_buffer, size_orig);

        int i,j;
        char line[2*MD5_DIGEST_LENGTH+2];
        memset(line,'\0',sizeof(line));
        char line_check[2];
	/* md5 computation*/
        for(i= 0; i<MD5_DIGEST_LENGTH; i++)
        {
                sprintf(line_check,"%02x",md5_buf[i]);
                line[i*2]=line_check[0];
                line[(i*2)+1]=line_check[1];
        }
        line[2*MD5_DIGEST_LENGTH]='\0';

        char md5_line[strlen(line)+1];
        memcpy(md5_line,line,strlen(line));
        md5_line[strlen(line)] = '\0';

	/* Sending Response to Client with bytes received, RTT, and throughpout*/
        char response[150];
        memset(response,'\0',sizeof(response));
        if (strcmp(md5_recv,md5_line)==0){
                snprintf(response,sizeof(response),"%i bytes received in %f seconds : %f Megabytes/sec",bytes_recv,RTT,throughput);
        } else {
                strcpy(response,"Unsuccessful transfer");
        }
        send(sock,response,sizeof(response),0);
	printf("sent transfer\n");
}


void clientRequest (int s) {

        int file_len;
        char query[20] = "Enter filename:";

        printf("before sending: Enter file.\n");
        if(send(s,query,sizeof(query),0)==-1){
                perror("Server send error!"); exit(1);
        }

        printf("after sending file");


        recv(s,&file_len,sizeof(file_len),0);
        file_len = ntohl(file_len);
        char filename[100];

        printf("received file name");
        memset(filename,'\0',sizeof(filename));
        while(strlen(filename)<file_len){
                recv(s,filename,sizeof(filename),0);
        }
        printf("received file size\n");
        int file_size = fileSize(filename);
        if (file_size < 0){
                return;
        }
        int size = file_size;
        file_size = htonl(file_size);
        send(s,&file_size,sizeof(int32_t),0);
        printf("after sending file size\n");


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

        char md5str[strlen(str)+1];
        memcpy(md5str,str,strlen(str));
        md5str[strlen(str)] = '\0';

        send(s,md5str,strlen(md5str),0);

        char line[20000];
        FILE *fp = fopen(filename, "r");
        memset(line,'\0',sizeof(line));
        int len =0;
        int sent_len=0;
        while((len=fread(line,sizeof(char),sizeof(line),fp))>0)
        {
                //printf("Sent line = %s\n", line);
                sent_len=send(s,line,len,0);
                memset(line,'\0',sizeof(line));
                //printf("sent length = %i\n", sent_len);
		bzero(line,sizeof(line));
        }
        fclose(fp);
        printf("at the end\n");

}




void delete(int s){

	/* Initializing Variables*/
	int file_name_len = 0;
	int file_exists;
	char delete_confirm[5];

	/* Receiving file name length */
	recv(s,&file_name_len,sizeof(file_name_len),0);
	file_name_len = ntohl(file_name_len);

	char file_name[file_name_len+1];

	/* Receiving file name */
	memset(file_name,'\0',sizeof(file_name));
	recv(s,file_name,sizeof(file_name),0);

	/* Determinin if dile exists */
	if (access(file_name,F_OK)!=-1){
		file_exists = 1;
	} else {
		file_exists = -1;
	        send(s,&file_exists,sizeof(int),0);
		return;
	}

	/* Sending file existance confirmation */
	send(s,&file_exists,sizeof(int),0);


	/* Receiving confirmation to delete */
	memset(delete_confirm,'\0',sizeof(delete_confirm));
	while(strlen(delete_confirm)==0){
		recv(s,delete_confirm,sizeof(delete_confirm),0);
	}

	/* Deleting file */
	if (strcmp(delete_confirm,"Yes") == 0){
		int removed;
		int success = remove(file_name);
		if (success!=0){
			removed = -1;
		} else {
			removed = 1;
		}
		send(s,&removed,sizeof(int),0);
	}
}

void list (int sock) {

	/* Declare Variables */
	int 	dir_size = 0;		// Directory size
	char 	list_buf[100];		// Directory listing
	DIR	*dir_stream;		// Directory stream
	struct	dirent *dir_read;	// Format of directoy entries

	/* Opening directory and reading it */
	dir_stream = opendir("./");
	if (dir_stream != NULL) {
		/* Calculating the size of directory */
		while ( dir_read = readdir(dir_stream) ) {
			strcpy (list_buf, dir_read->d_name);
			dir_size += sizeof(list_buf);
			memset(list_buf, '\0', sizeof(list_buf));
		}
		closedir(dir_stream);
		dir_size = htonl(dir_size);
		/* Sending directory size to client */
		send (sock, &dir_size, sizeof(int32_t), 0);
		/* Send directory listing to cleint */	
		dir_stream = opendir("./");
		while ( dir_read = readdir(dir_stream) ) {
			strcpy ( list_buf, dir_read->d_name );
			send ( sock, list_buf, sizeof(list_buf), 0 );
			memset(list_buf, '\0', sizeof(list_buf));
		}

	}
} 




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

		if ((sock = accept(sock, (struct sockaddr *)&sin, &addr_len)) < 0) {
			perror("ERROR: server-accept()\n");
		} //end accept check
		int connected = 1;
		while (connected) {
			while( strlen(buf) == 0) {
				recv(sock, buf, sizeof(buf), 0);
			}
	                printf("\n\nWaiting for operation from client.\n");

			if (strcmp(buf,"REQ")==0){
				printf("clientREQ\n");
				clientRequest(sock);
			} 
			else if (strcmp(buf,"UPL")==0){
				upload(sock);
			} 
                        else if (strcmp(buf,"LIS")==0){
                                list(sock);
                        }
			else if (strcmp(buf,"MKD")==0){
				makeDirectory(sock);
			}
			else if (strcmp(buf, "RMD")==0){
				removeDirectory(sock);
			}
			else if (strcmp(buf, "CHD")==0){
				changeDirectory(sock);
			}
			else if (strcmp(buf,"DEL")==0){
				delete(sock);
			} 
			else if (strcmp(buf,"XIT")==0){
				connected = 0;
				close(sock);
			}
			memset(buf,'\0',sizeof(buf));

		} //end connection WHILE

	} // end WHILE

} // end MAIN
