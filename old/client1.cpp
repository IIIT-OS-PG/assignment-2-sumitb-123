#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include <iostream>
#define BUFF_SIZE 1024
#define port 9000
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
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	//connecting to the server
	int status;/* = connect(svr_socket, (struct sockaddr*) &server, sizeof(server));
	if(status<0){
		cout<<"Error in connection establishment "<<endl;
	}*/
	//buffer for reading response from the server
	char response[1024];
	int temp;
	string filename;
	while(1){
		cout<<"Enter the filename "<<endl;
		cin>>filename;
		status = connect(svr_socket, (struct sockaddr*) &server, sizeof(server));
    	if(status<0){
    	    cout<<"Error in connection establishment "<<endl;
	    }

		FILE *fp = fopen(filename.c_str(),"rb");
	    fseek (fp,0,SEEK_END);
    	int size = ftell(fp);
	    rewind (fp);
		send (svr_socket,&size,sizeof(size), 0);
		char Buffer[BUFF_SIZE] ;
		int n;
		while ((n= fread(Buffer,sizeof(char),BUFF_SIZE,fp)) > 0 && size > 0 ){
			cout<<Buffer<<endl;
			send (svr_socket,Buffer, n, 0);
   	 		memset( Buffer , '\0', BUFF_SIZE);
			size = size - n ;
		}
		cout<<"closing the file"<<endl;
		fclose(fp);
		close(svr_socket);
	}
	//closing the socket
	//close(svr_socket);
return 0;
}