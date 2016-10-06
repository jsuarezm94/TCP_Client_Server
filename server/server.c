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
#define MAX_MD5LENGTH 50
#define MAX_FILENAME 100
#define MAX_PENDING 5
#define MAX_LINE 256


int32_t file_exists(char * filename)
{
	FILE * file;
	int32_t size;
	if (file = fopen(filename, "r"))
	{
		fseek(file,0,SEEK_END);
		size = ftell(file);
		fseek(file,0,SEEK_SET);
		fclose(file);
		return size;
	}
	return -1;
}

void makeDirectory(int sock) {
/*
        int file_name_len = 0;
        int file_exists;
        char status[5];

        recv(s,&file_name_len,sizeof(file_name_len),0);
        file_name_len = ntohl(file_name_len);
        char file_name[file_name_len+1];

        memset(file_name,'\0',sizeof(file_name));
        while(strlen(file_name)==0){
                recv(s,file_name,sizeof(file_name),0);
        }

        if (access(file_name,F_OK)!=-1){
                file_exists = -2;
        } 
	else if (access(file_name, ))  {
                file_exists = 1;
        }
	
        send(s,&file_exists,sizeof(int),0);
*/
}

void upload(int s) {

	int filename_len;
	char md5client[100];
	struct timeval tv;
	float start_time, end_time, nBytes, throughput;

	recv(s,&filename_len,sizeof(filename_len),0);
	filename_len = ntohl(filename_len);
	printf("Received filename_length\n");

	char filename[100];

	memset(filename,'\0',sizeof(filename));
	while(strlen(filename)<filename_len){
		recv(s,filename,sizeof(filename),0);
	}
        printf("filename received.\n");


	char ack[4] = "ACK";
	if(send(s,ack,sizeof(ack),0)==-1){
		perror("Server send error!"); exit(1);
	}

        int filesize = 0;
        recv(s,&filesize,sizeof(int32_t),0);
        filesize = ntohl(filesize);
        printf("filesize received = %d\n", filesize);


	gettimeofday(&tv,NULL);
	start_time = tv.tv_usec;

	FILE *fp = fopen(filename,"w");


	int n;
	char line[20000];
	memset(line,'\0',sizeof(line));
	int recv_len=0;
	int bytesrevd = 0;
	char recvbuf[10000];
	int rcvbufmax=sizeof(line);
	if (rcvbufmax>filesize)
		rcvbufmax=filesize;
	while ((recv_len=recv(s,recvbuf,rcvbufmax,0))>0){
		bytesrevd += recv_len;
		int write_size = fwrite(recvbuf, sizeof (char), recv_len, fp);
		if (write_size<recv_len)
			printf("File write failed!\n");
		bzero(line, sizeof(line));
		if (bytesrevd>=filesize) break;
	}
	fclose(fp);


	gettimeofday(&tv,NULL);
	end_time = tv.tv_usec; //in microsecond
	float RTT = (end_time-start_time) * pow(10,-6); //RTT in seconds
	throughput = (bytesrevd*pow(10,-6))/RTT;


	memset(md5client,'\0',sizeof(md5client));
	while(strlen(md5client)==0){
		recv(s,md5client,sizeof(md5client),0);
	}
	md5client[strlen(md5client)] = '\0';


	int size = filesize;
	unsigned char md5[MD5_DIGEST_LENGTH];
	char* file_buffer = (char*) malloc(20000);
	int file_description;

	file_description = open(filename,O_RDONLY);
	file_buffer = mmap(0,size,PROT_READ,MAP_SHARED,file_description,0);
	MD5((unsigned char*) file_buffer, size, md5);
	munmap(file_buffer, size);

	int i,j;
	char str[2*MD5_DIGEST_LENGTH+2];
	memset(str,'\0',sizeof(str));
	char str2[2];
	for(i= 0; i<MD5_DIGEST_LENGTH; i++)
	{
		sprintf(str2,"%02x",md5[i]);
		str[i*2]=str2[0];
		str[(i*2)+1]=str2[1];
	}
	str[2*MD5_DIGEST_LENGTH]='\0';

	char md5str[strlen(str)+1];
	memcpy(md5str,str,strlen(str));
	md5str[strlen(str)] = '\0';



	char result[150];
	memset(result,'\0',sizeof(result));
	if (strcmp(md5client,md5str)==0){
		snprintf(result,sizeof(result),"%i bytes received in %f seconds : %f Megabytes/sec",bytesrevd,RTT,throughput);
	} else {
		strcpy(result,"Unsuccessful transfer");
	}
	send(s,result,sizeof(result),0);
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
	int file_size = file_exists(filename);
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
		sent_len=send(s,line,len,0);
		memset(line,'\0',sizeof(line));
	}
	fclose(fp);
	printf("at the end\n");
}





void deleteFile(int s){


	int filename_len = 0;
	int file_exists;
	char status[5];

	recv(s,&filename_len,sizeof(filename_len),0);
	filename_len = ntohl(filename_len);
	char filename[filename_len+1];


	memset(filename,'\0',sizeof(filename));
	while(strlen(filename)==0){
		recv(s,filename,sizeof(filename),0);
	}


	if (access(filename,F_OK)!=-1){
		file_exists = 1;
	} else {
		file_exists = -1;
	}


	send(s,&file_exists,sizeof(int),0);

	printf("after sending confirmation, before while\n");

	memset(status,'\0',sizeof(status));
	while(strlen(status)==0){
		printf("receiving...\n");
		recv(s,status,sizeof(status),0);
	}


	printf("status = %s\n", status);
	//printf("strcom %d",strcmp(status,"Yes") );
	if (strcmp(status,"Yes\n")){
		int ack2;
		int result = remove(filename);
		if (result!=0){
			ack2 = -1;
		} else {
			ack2 = 1;
		}
		printf("before sending ack...\n");
		send(s,&ack2,sizeof(int),0);
	}
}

void listDirectory (int sock) {

	int 	dir_size = 0;
	char 	file_buf[100];
	DIR	*dir_stream;	
	struct	dirent *dir_read;		// change var name

	dir_stream = opendir("./");

	if (dir_stream != NULL) {
		while ( dir_read = readdir(dir_stream) ) {
			strcpy (file_buf, dir_read->d_name);
			dir_size += sizeof(file_buf);
			memset(file_buf, '\0', sizeof(file_buf));
		}

		closedir(dir_stream);
		dir_size = htonl(dir_size);
		send (sock, &dir_size, sizeof(int32_t), 0);

		dir_stream = opendir("./");
		while ( dir_read = readdir(dir_stream) ) {
			strcpy ( file_buf, dir_read->d_name );
			send ( sock, file_buf, sizeof(file_buf), 0 );
			memset(file_buf, '\0', sizeof(file_buf));
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

		if ((new_sock = accept(sock, (struct sockaddr *)&sin, &addr_len)) < 0) {
			perror("ERROR: server-accept()\n");
		} //end accept check

		int connected = 1;
		while (connected) {
			while( strlen(buf) == 0) {
				recv(new_sock, buf, sizeof(buf), 0);
			}

			if (strcmp(buf,"REQ")==0){
				printf("clientREQ\n");
				clientRequest(new_sock);
			} 
			else if (strcmp(buf,"UPL")==0){
				upload(new_sock);
			} 
                        else if (strcmp(buf,"LIS")==0){
                                listDirectory(new_sock);
                        }
			else if (strcmp(buf,"MKD")==0){
				makeDirectory(new_sock);
			}
			else if (strcmp(buf, "RMD")==0){

			}
			else if (strcmp(buf, "CHD")==0){

			}
			else if (strcmp(buf,"DEL")==0){
				deleteFile(new_sock);
			} 
			else if (strcmp(buf,"XIT")==0){
				connected = 0;
			}
			memset(buf,'\0',sizeof(buf));

		} //end connection WHILE

	} // end WHILE

} // end MAIN
