#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
#include<string.h>
#include <iostream>
#include <string>
#define BUFF_SIZE 1024
#define port 9000
#define nm 100
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
	int status = connect(svr_socket, (struct sockaddr*) &server, sizeof(server));
	if(status<0){
		cout<<"Error in connection establishment "<<endl;
	}
	//buffer for reading response from the server
	char response[BUFF_SIZE];
	string filename;
	cout<<"starting communication "<<endl;
    cin>>filename;
	int j = 0;
	char fname[nm];
	while(filename[j]){
		fname[j] = filename[j];
		j++;
	}
	fname[j]='\0';	
    FILE *fp = fopen(filename.c_str(),"rb");
    FILE *fp1 = fopen("new.txt","wb");
    fseek (fp,0,SEEK_END);
    int size = ftell(fp);
    rewind (fp);
	fclose(fp);
    send (svr_socket,fname,sizeof(fname), 0);

    char Buffer[BUFF_SIZE] ;
    int n;
	int file_size;
	recv(svr_socket, &file_size, sizeof(file_size), 0);
	while ((n=recv(svr_socket,Buffer,BUFF_SIZE,0))>0 && file_size>0){
        fwrite(Buffer, sizeof(char), n, fp1);
        file_size = file_size - n;
    }
    cout<<"closing the file"<<endl;
    fclose(fp1);

return 0;
}
