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
	
	char Buffer[BUFF_SIZE];
    int file_size,n,size;
	cout<<"inside the thread"<<endl;
	int client_socket = *((int *)sock);
    recv(client_socket, Buffer, BUFF_SIZE, 0);
	FILE *fp = fopen(Buffer,"rb");
    fseek (fp,0,SEEK_END);
    size = ftell(fp);
    rewind (fp);
	send (client_socket,&size,sizeof(size), 0);
	cout<<Buffer<<endl;
	memset(Buffer,'\0', BUFF_SIZE);

	while ((n= fread(Buffer,sizeof(char),BUFF_SIZE,fp)) > 0 && size > 0 ){
        send (client_socket,Buffer, n, 0);
        size = size - n ;
    }

    cout<<"closing the file"<<endl;
    fclose(fp);
	close(client_socket);
	pthread_exit(NULL);

}

/*void downloadFile(string filename, string clientname, ){
	
}*/
/*string getIp(string filepath){
	
	FILE *fp = fopen(filename.c_str(), "rb");
    char Buffer[BUFF_SIZE];
    
}*/

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
	int temp,i=0,sts;
	string filename;
	while(1){
		//accepting request from clients
		cout<<"communication started"<<endl;
		int client_socket = accept(svr_socket, (struct sockaddr *)&server, (socklen_t*)&addrlen);
		sts = pthread_create(&thread[i], NULL, fileRead, &client_socket); 
		i++;
	}
	//closing the socket
	close(svr_socket);
	return 0;
}
