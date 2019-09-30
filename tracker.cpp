#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
#include <string.h>
#include <iostream>
#include<pthread.h>
#define port 9000
#define BUFF_SIZE 1024
using namespace std;

int fl = 0;

void *fileRead(void *sock){

	cout<<"inside the thread"<<endl;
	int client_socket = *((int *)sock);
	string filename = "rfile"+to_string(++fl);
	FILE *fp = fopen(filename.c_str(), "wb");
    char Buffer[BUFF_SIZE];
    int file_size,n;
    recv(client_socket, &file_size, sizeof(file_size), 0);
    cout<<"file size received "<<file_size<<endl;
    while ((n=recv(client_socket,Buffer,BUFF_SIZE,0))>0 && file_size>0){
        //cout<<Buffer<<" n "<<n<<endl;
        fwrite(Buffer, sizeof(char), n, fp);
        memset(Buffer,'\0', BUFF_SIZE);
        file_size = file_size - n;
    }
    cout<<"file is closed"<<endl;
    fclose(fp);
	close(client_socket);
	pthread_exit(NULL);

}

//int socket(int domain, int type, int protocol);
int main(){
	int svr_socket;
	pthread_t thread[10]; 
	char response[1024] = "this is the response from the server";
	//creating socket for the communication
	svr_socket = socket(AF_INET, SOCK_STREAM, 0);
	//structure for defining the connection attributes
	struct sockaddr_in server;
	//assigning type, port and address 
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	int addrlen = sizeof(sockaddr);
	//binding the address
	bind(svr_socket, (struct sockaddr*) &server, sizeof(server));
	//listening to respond
	listen(svr_socket,10);
	//accept the client requests
	//int client_socket;
	//client_socket = accept (svr_socket, (struct sockaddr *)&server, (socklen_t*)&addrlen);
	int temp,i=0,sts;
	string filename;
	while(1){
		//accepting request from clients
		cout<<"communication started"<<endl;
		int client_socket = accept(svr_socket, (struct sockaddr *)&server, (socklen_t*)&addrlen);
		sts = pthread_create(&thread[i], NULL, fileRead, &client_socket); 
		i++;
		//close(client_socket);
		//cout<<"communication closed"<<endl;
	}

	//closing the socket
	close(svr_socket);

return 0;
}
