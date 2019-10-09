#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
#include <string.h>
#include <iostream>
#include<pthread.h>
#include<fstream>
#define BUFF_SIZE 1024
#define def_ip "127.0.0.1"
#define def_port 11123
#define trd 100
using namespace std;
int port = def_port;
int fl = 0;
string ip = def_ip;

//method to parse the info file and set ip and port 
void parseFile(char *filename, int tno){
	
	char fname[trd];
	ifstream rfile;
	int i,item=0;
	string temp="",temp1,temp2;
	rfile.open(filename);
	while(tno--){
		rfile>>temp1>>temp2;
		//cout<<"from the file "<<temp1<<" "<<temp2<<endl;
	}
	i = 0;
	//extracting ip and port of tracker from info file
	ip = temp1;
	port = stoi(temp2);
	while(fname[i]){
		if(fname[i] != ' ')
			temp+=fname[i];
		else{
			item++;
			if(item == 1){
				cout<<"temp "<<temp<<endl;
			}
			else if(item == 2){
				cout<<"temp "<<temp<<endl;
			}
			temp = "";
		}
		i++;
		//cout<<fname[i];
	}
	//cout<<" ip "<<ip<<" port "<<port<<endl;
}

void *trackerServer(void *sock){

    int svr_socket;
    pthread_t thread[trd];
    char response[BUFF_SIZE];
    //creating socket for the communication
    svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    //structure for defining the connection attributes
    struct sockaddr_in server;
    //assigning type, port and address 
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    int addrlen = sizeof(sockaddr);
    //binding the address
    bind(svr_socket, (struct sockaddr*) &server, sizeof(server));
    //listening to respond
    listen(svr_socket,trd);
    int temp,i=0,sts;
    string filename;
    while(1){
        //accepting request from clients
        cout<<"server is listening"<<endl;
        int client_socket = accept(svr_socket, (struct sockaddr *)&server, (socklen_t*)&addrlen);
        //sts = pthread_create(&thread[i], NULL, fileSend, &client_socket);
        i++;
    }
    //closing the socket
    close(svr_socket);
    pthread_exit(NULL);

}


int main(int argc, char *argv[]){
	
	if(argc != 3){
		cout<<"check the arguments"<<endl;
		exit(0);
	}

	int tracker_no = stoi(argv[2]);
	string quit;
	parseFile(argv[1], tracker_no);
	cout<<"port "<<port<<" ip "<<ip<<endl;
    pthread_t td1;
    int status = pthread_create(&td1, NULL, trackerServer,NULL);
	//pthread_join(td1,NULL);
	while(1){
		//cout<<"quit to close the tracker"<<endl;
		cin>>quit;
		if(!quit.compare("quit")){
			cout<<"Bye !"<<endl;
			break;
		}
	}
	return 0;
}
