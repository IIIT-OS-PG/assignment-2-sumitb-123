#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
#include <iostream>
using namespace std;

//int socket(int domain, int type, int protocol);
int main(){
	
	int svr_socket;
	char response[1024] = "this is the response from the server";
	//creating socket for the communication
	svr_socket = socket(AF_INET, SOCK_STREAM, 0);
	//structure for defining the connection attributes
	struct sockaddr_in server;
	//assigning type, port and address 
	server.sin_family = AF_INET;
	server.sin_port = htons(9999);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//binding the address
	bind(svr_socket, (struct sockaddr*) &server, sizeof(server));
	//listening to respond
	listen(svr_socket,1);
	//accept the client requests
	int client_socket = accept(svr_socket, NULL, NULL);
	while(1){	
		//listening to respond
		//listen(svr_socket,1);
		//accept the client requests
		//int client_socket = accept(svr_socket, NULL, NULL);
		//receiving the request from the client
		recv(client_socket, response, sizeof(response),0);
		//printing the request
		printf("%s,\n",response);
		//getting the message to be passed
		cin.getline(response,sizeof(response));
		//sending the data to the client
		send(client_socket, response, sizeof(response), 0);
		//closing the client socket
		//close(client_socket);
	}
	//closing the client socket
	close(client_socket);
	//closing the socket
	close(svr_socket);
	//close(client_socket);

return 0;
}
