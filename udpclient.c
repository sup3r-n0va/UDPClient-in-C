//UDPClient In C

#include <stdio.h> 	//for printf and fprintf()
#include <sys/socket.h>	//for socket(), connect(), sendto(), and recvfrom()
#include <arpa/inet.h>	//for sockaddr_in and inet_addr()
#include <stdlib.h>	//for atoi()
#include <string.h>	//for memset
#include <unistd.h>	// for close()

#define ECHOMAX 255	//Longest string to echo 

void DieWithError(char *errorMessage);	//for external error handling function

int main(int argc, char *argv[])
{

	int sock;		//socket descriptor
	struct sockaddr_in echoServAddr;	//Echo server address
	struct	sockaddr_in fromAddr;		//source address of echo
	unsigned short echoServPort;		//Echo server port
	unsigned int fromSize;			//in-out of address size for 
						//recvfrom()
	char *servIP;				//IP address of server
	char *echoString;			//string to send to echo server
	char echoBuffer[ECHOMAX + 1];		//Buffer for recieving 
						//echo string
	int echoStringLen;			//length of string to echo
	int respStringLen;			//Length of received response

	if ((argc < 3) || (argc > 4))		//Test for correct number of args
	{
		fprintf(stderr, "Usage: %s <Server IP> <Echo Word> [<Echo Port>]\n",
			argv[0]);
		exit(1);
	}
	
	servIP = argv[1];			//First arg: server IP address
	echoString = argv[2];			//Second arg: string to echo

	if ((echoStringLen = strlen(echoString)) > ECHOMAX) //check input length
		DieWithError("Echo Word to long");

	if (argc == 4)
		echoServPort = atoi(argv[3]);	//use given port if any
	else 
		echoServPort = 7;		// 7 is a well known port number

	//create a datagram udp socket
	if ((sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
		DieWithError("socket() failed");

	//construct the server address structure
	memset(&echoServAddr, 0, sizeof(echoServAddr));	//Zero out structure
	echoServAddr.sin_family = AF_INET;		//internet addr family
	echoServAddr.sin_addr.s_addr = inet_addr(servIP); //server ip address
	echoServAddr.sin_port = htons(echoServPort);	//server port
	
	//send the string to the server
	if (sendto(sock, echoString, echoStringLen, 0, (struct sockaddr *)
		   &echoServAddr, sizeof(echoServAddr)) != echoStringLen)
		DieWithError("sendto() sent a different number of bytes than expected");

	//recieve response
	fromSize = sizeof(fromAddr);
	if ((respStringLen = recvfrom(sock, echoBuffer, ECHOMAX, 0, 
		(struct sockaddr *) &fromAddr, &fromSize)) != echoStringLen)
		DieWithError("recvfrom() failed");

	if (echoServAddr.sin_addr.s_addr != fromAddr.sin_addr.s_addr)
	{
		fprintf(stderr, "Error: recieved a packet from unknown source.\n");
		exit(1);
	}
	
	//null terminate the recieved data
	echoBuffer[respStringLen] = '\0';
	printf("Recieved: %s\n", echoBuffer);	//print the echoed arg

	close(sock);
	exit(0);
}


