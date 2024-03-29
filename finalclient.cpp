#include <bits/stdc++.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include<unistd.h>
#include <string.h>
#include <iostream>
#include<pthread.h>
//#define port 9000
#define BUFF_SIZE 1024
#define trd 10
//#define ip "127.0.0.1"
//#define port1 9001
#define nm 100
using namespace std;

int port, port1;
int fl = 0;
string ip = "127.0.0.1";
vector<string> tracker_ip;
vector<int> tracker_port;


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
        fwrite(Buffer, sizeof(char), n, fp);
        file_size = file_size - n;
    }
    cout<<"file is closed"<<endl;
    fclose(fp);
	close(client_socket);
	pthread_exit(NULL);

}

void *fileSend(void *sock){
	
	char Buffer[BUFF_SIZE];
    int file_size,n,size;
	cout<<"inside sending thread"<<endl;
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

    cout<<"closing the file in sending theread"<<endl;
    fclose(fp);
	close(client_socket);
	pthread_exit(NULL);

}

void *peerServer(void *sock){

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
        sts = pthread_create(&thread[i], NULL, fileSend, &client_socket);
        i++;
    }
    //closing the socket
    close(svr_socket);
	pthread_exit(NULL);

}

void *peerClient(void *args){

	char *filepath = ((char *)args);
	cout<<filepath<<endl;
	int svr_socket;
	//creating socket for the communication
	svr_socket = socket(AF_INET, SOCK_STREAM, 0);
	//structure for defining the connection attributes
	struct sockaddr_in server;
	//assigning values to the structure like type, port and address 
	server.sin_family = AF_INET;
	server.sin_port = htons(port1);
	server.sin_addr.s_addr = inet_addr(ip.c_str());

	//connecting to the server
	int status = connect(svr_socket, (struct sockaddr*) &server, sizeof(server));
	if(status<0){
		cout<<"Error in connection establishment "<<endl;
	}
	char response[BUFF_SIZE];
	cout<<"starting communication from client thread"<<endl;
    FILE *fp1 = fopen("received","wb");
    send (svr_socket,filepath,sizeof(filepath), 0);

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
	pthread_exit(NULL);

}

//login into the sharing system
bool login(string user, string pass, int svr_socket, int tracker_desc){
    int option = 2;
    char username[nm];
    char passwd[nm];
    char buffer[BUFF_SIZE];
    int status;
    strcpy(buffer,user.c_str());
    //cout<<"first"<<endl;
    send(svr_socket,&option,sizeof(option),0);
    //cout<<"second"<<endl;
    send(svr_socket,buffer,BUFF_SIZE,0);
    //cout<<"third"<<endl;
    strcpy(buffer,pass.c_str());
    send(svr_socket,buffer,BUFF_SIZE,0);
    //cout<<"fourth"<<endl;
    recv(svr_socket,&status,sizeof(status),0);
    //cout<<"last"<<endl;
    if(status)
        return true;
    else return false;
}

//creating user
bool createUser(string user, string pass, int svr_socket, int tracker_desc){
	int option = 1;
	char username[nm];
	char passwd[nm];
	char buffer[BUFF_SIZE];
	int status;
	strcpy(buffer,user.c_str());
	//cout<<"first"<<endl;
	send(svr_socket,&option,sizeof(option),0);
	//cout<<"second"<<endl;
	send(svr_socket,buffer,BUFF_SIZE,0);
	//cout<<"third"<<endl;
	strcpy(buffer,pass.c_str());
	send(svr_socket,buffer,BUFF_SIZE,0);
	//cout<<"fourth"<<endl;
	recv(svr_socket,&status,sizeof(status),0);
	//cout<<"last"<<endl;
	if(status)
    	return true;
	else return false;
}

//logout from the tracker
bool logout(string user, int svr_socket, int tracker_desc){
	int option = 5;
    char buffer[BUFF_SIZE];
    int status;
    strcpy(buffer,user.c_str());
    send(svr_socket,&option,sizeof(option),0);
    send(svr_socket,buffer,BUFF_SIZE,0);
    recv(svr_socket,&status,sizeof(status),0);
    if(status)
        return true;
    else return false;
}

//crate group
bool createGroup(string user, int group_id, int svr_socket, int tracker_desc){
	int option = 6;
	char buffer[BUFF_SIZE];
    int status;
    strcpy(buffer,user.c_str());
    send(svr_socket,&option,sizeof(option),0);
    send(svr_socket,buffer,BUFF_SIZE,0);
	//strcpy(buffer,group_id.c_str());
    send(svr_socket,&group_id,sizeof(group_id),0);
    recv(svr_socket,&status,sizeof(status),0);
    if(status)
        return true;
    else return false;
}

//int socket(int domain, int type, int protocol);
int main(int argc, char* argv[]){
	
	//command line arguments
	ip = argv[1];
	port = stoi(argv[2]);
	//port1 = stoi(argv[3]);
	string conf_file = argv[3];
	
	//declarations
	string st;
    pthread_t td1,td2;
    char filepath[nm];
	string tfname,username,passwd;
	int tport;
	int option;
	int userFlag = 0;
	int loginFlag = 0;
	bool status;
	int log_option;
	ifstream file(conf_file);

	for(int i=0;i<2;i++){
		file >>tfname;
		tracker_ip.push_back(tfname);
		file >> tport;
		tracker_port.push_back(tport);
	}
	
	int statust = pthread_create(&td1, NULL, peerServer,NULL);

	//connecting to the tracker
	int svr_socket;
    //creating socket for the communication
    svr_socket = socket(AF_INET, SOCK_STREAM, 0);
    //structure for defining the connection attributes
    struct sockaddr_in server;
    //assigning values to the structure like type, port and address 
    server.sin_family = AF_INET;
    server.sin_port = htons(tracker_port[0]);
    server.sin_addr.s_addr = inet_addr(ip.c_str());
    int tracker_status = connect(svr_socket, (struct sockaddr*) &server, sizeof(server));
	
	while(1){
		cout<<"1.Create User "<<endl;
		cout<<"2.Login "<<endl;
		cin>>option;
		if(option == 1){
			cout<<"Enter username"<<endl;
			cin>>username;
			cout<<"Enter password"<<endl;
			cin>>passwd;
			status = createUser(username, passwd, svr_socket, tracker_status);
			if(status == true){
				cout<<"user create successful"<<endl;
				userFlag = 1;
			}
			else{
				cout<<"user create failed : user already exist"<<endl;
            	continue;
			}
		}
		else if(option == 2){
			cout<<"Enter username"<<endl;
            cin>>username;
            cout<<"Enter password"<<endl;
            cin>>passwd;
            status = login(username, passwd, svr_socket, tracker_status);
            if(status == true){
                cout<<"login successful"<<endl;
                loginFlag = 1;
			}
			else{
				cout<<"login failed"<<endl;
                continue;
			}
		}
		if(loginFlag == 1){
			while(1){
			cout<<"3.Upload"<<endl;
			cout<<"4.Download"<<endl;
			cout<<"5.Logout"<<endl;
			cout<<"6.Create Group"<<endl;
			/*cout<<""<<endl;
			cout<<""<<endl;
			cout<<""<<endl;
			cout<<""<<endl;
			cout<<""<<endl;
			cout<<""<<endl;*/
			/*cin>>st;
			if(st.compare("download") == 0){
				cout<<"Enter filepath "<<endl;
				cin>>filepath;
				int cstatus = pthread_create(&td1, NULL, peerClient,&filepath);
			}*/		
			cin>>log_option;
			if(log_option == 5){
				status = logout(username, svr_socket, tracker_status);
				if(status == 1){
					cout<<"logged out successfully"<<endl;
					break;
				}
			}
			else if(log_option == 6){
				int group_id;
				cin>>group_id;
				status = createGroup(username, group_id, svr_socket, tracker_status);
				if(status){
					cout<<"group created"<<endl;
				}
				else{
					cout<<"error in group creation"<<endl;
				}
			}
			}
		}
	}
	return 0;

}
