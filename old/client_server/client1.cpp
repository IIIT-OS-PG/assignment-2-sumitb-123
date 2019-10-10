#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
#include <iostream>
using namespace std;

//int socket(int domain, int type, int protocol);
int main(){
	
	int svr_socket;
	//creating socket for the communication
	svr_socket = socket(AF_INET, SOCK_STREAM, 0);
	//structure for defining the connection attributes
	struct sockaddr_in server;
	//assigning values to the structure like type, port and address 
	server.sin_family = AF_INET;
	server.sin_port = htons(9999);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//connecting to the server
	int status = connect(svr_socket, (struct sockaddr*) &server, sizeof(server));
	if(status<0){
		cout<<"Error in connection establishment "<<endl;
	}
	//buffer for reading response from the server
	char response[1024];
	while(1){
		cin.getline(response,sizeof(response));
		send(svr_socket,response,sizeof(response),0);
		//receiving the response
		recv(svr_socket, response, sizeof(response), 0);
		//printing the response
		printf("%s\n",response);
	}
	//closing the socket
	close(svr_socket);
return 0;
}
