
# TCP Client and  Server
## Programming Assignment 03
## Fall 2016 CSE30264
## netId: jdiazort
## Authors: 	Jorge Diaz-Ortiz
##		Jose Suarez

### Files in project.
	- server.c :	contains the server code for the TCP  
	- client.c :	contains the client code for the TCP

### Directories in this project 
	- server
		- server.c	
		- myftpd
		- Makefile
	- client
		- client.c
		- myftp
		- Makefile

### Compilation
	- From the respective directory:
		- client
			$ make
		- server
			$ make
	
### Usage / Execution
	- Client Server Communication:
	Log into student00.cse.nd.edu or student01.cse.nd.edu in order to run the client code.
	Log into student02.cse.nd.edu or student03.cse.nd.edu in order to run the server code.

	- Terminal Command for the client from the client directory:
	$ ./mytcp <server_address> <port_number> 
	
	- Terminal Command for the server from the server directory:
	$ ./mytcp <server_port_number> 

	- Sever Address: 	student02.cse.nd.edu
				student03.cse.nd.edu

	- Port Number:		for Jorge and Jose	41017, 41018
				for instructor		TBD

	- Client commnad options:
		- REQ: request a file from the server
		- UPL: upload a file to the server
		- DEL: delete a file in the server
		- MKD: create a directory in te server
		- RMD: remove a directory from the server
		- CHD: change the directory at the server
		- LIS: list the files and directories in te server
		- XIT: disconnect from the server
### Example
	- This is an example of the TCP server client communication. The client will perform the LIS, REQ, and XIT operations, and the server will respond accordingly.
	
	
	Server:
	jdiazort@student03:~/CSE30264/TCP_Client_Server/server$ myftpd 41017
	Connection succesful with client
	Waiting for operation from client.
	Waiting for operation from client.
	Waiting for operation from client.
	Waiting for operation from client.




	Client:
	jdiazort@student00:~/CSE30264/TCP_Client_Server/client$ myftp student03.cse.nd.edu 41017
	Connection successful with server
	Enter Command: REQ, UPL, LIS, DEL, MKD, RMD, CHD or XIT: LIS
	.
	..
	Makefile
	upld.txt
	server.c
	SmallFile.txt
	myftpd
	server
	Enter Command: REQ, UPL, LIS, DEL, MKD, RMD, CHD or XIT: REQ
	Enter file name to request from server: SmallFile.txt
	Successful Transfer
	60 bytes received in 0.004745 seconds : 0.012645 Megabytes/sec
	File MD5sum: 70c314bf60af1113cc2350ff94f57d78
	Enter Command: REQ, UPL, LIS, DEL, MKD, RMD, CHD or XIT: XIT
	Session has been closed






