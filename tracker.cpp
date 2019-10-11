#include <sys/types.h>
#include <bits/stdc++.h>
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

//map for saving user informations
map<string, string> users;
map<string, int> active;
map<int, vector<string>> requests;
map<int, string> groups;

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
	}
}

//read user password from file
bool readUserU(string user, string pass){
    string uname, passwd;
	if(users.find(user) != users.end())
		return true;
	else return false;
    /*ifstream file("users");
    while(!file.eof()){
        file >> uname;
        file >> passwd;
        if(!user.compare(uname)){
            return true;
        }
    }*/
    return false;
}

//read user password from file
bool readUser(string user, string pass){
	//string uname, passwd;
	cout<<user<<" "<<pass<<endl;
	cout<<users[user] <<endl;
	if(users.find(user) != users.end() && users[user] == pass)
		return true;
	else return false;
	/*ifstream file("users");
	while(!file.eof()){
		file >> uname;
		file >> passwd;
		if(!user.compare(uname) && !pass.compare(passwd)){

			return true;
		}
	}
	return false;*/
}

//write user password into the file
void writeUser(string user, string pass){
	ofstream file;
	file.open("users");
	users[user] = pass;
	for(auto u: users){
		file << u.first << " " << u.second << endl;
	}
}

//maintaining the active status
void writeActive(string us){
	ofstream file("active");
	for(auto it: active){
		file << it.first <<" "<<it.second<<endl;
	}
}

//create group
int createGroup(string user, int group_id){
	ifstream file("groups");
	int grp_id;
	string uname;
    /*while(!file.eof()){
        file >> grp_id;
        file >> uname;
        if(grp_id == group_id){
            return 0;
        }
    }*/
	if(groups.find(group_id) == groups.end()){
		groups[group_id] = user;
		ofstream ofile("groups");
		for(auto g: groups){
			ofile <<g.first<<" "<<g.second<<endl;
		}
		return 1;
	}
	else return 0;
	
}

//request for joining group
bool joinGroupRequest(string us, int gid){
	if(groups.find(gid) != groups.end()){	
		requests[gid].push_back(us);
		ofstream file("requests");
		for(auto it: requests){
			file << it.first;
			for(auto e: it.second){
				file <<" "<<e;
			}
			file<<endl;
		}
		return true;
	}
	else{
		return false;
	}
	
}

//thread for handling the client request
void *clientHandler(void *arg){
	
	char Buffer[BUFF_SIZE];
	string us;
	string pass;
	int option;
	int size;
	int status = 1;
    int client_socket = *((int *)arg);
	cout<<"connected to client handler"<<endl;
	while(1){
		//cout<<"first "<<endl;
    	recv(client_socket, &option, sizeof(int), 0);
		if(option == 1){
		 	recv(client_socket, Buffer, BUFF_SIZE, 0);
			us = Buffer;
		 	recv(client_socket, Buffer, BUFF_SIZE, 0);
			pass = Buffer;
			int check = readUserU(us,pass);
			if(!check){
				writeUser(us,pass);
				status = 1;
                send(client_socket, &status, sizeof(status),0);
			}
			else{
				status = 0;
                send(client_socket, &status, sizeof(status),0);
			}
			/*if(users.find(us) == users.end()){
				users[us] = pass;
				status = 1;
	            send(client_socket, &status, sizeof(status),0);
			}
			else{
				status = 0;
				send(client_socket, &status, sizeof(status),0);
			}*/
		}
		else if(option == 2){
            recv(client_socket, Buffer, BUFF_SIZE, 0);
            us = Buffer;
            recv(client_socket, Buffer, BUFF_SIZE, 0);
            pass = Buffer;
			bool check = readUser(us,pass);
		    if(check){
                //writeUser(us,pass);
                status = 1;
				active[us] = 1;
				writeActive(us);
                send(client_socket, &status, sizeof(status),0);
            }
            else{
                status = 0;
                send(client_socket, &status, sizeof(status),0);
            }

            /*if(users.find(us) != users.end() && users[us] == pass){
                status = 1;
				active[us] = 1;
                send(client_socket, &status, sizeof(status),0);
            }
            else{
                status = 0;
                send(client_socket, &status, sizeof(status),0);
            }*/
        }
		else if( option == 5){
			recv(client_socket, Buffer, BUFF_SIZE, 0);
            us = Buffer;
			//recv(client_socket, Buffer, BUFF_SIZE, 0);
			//pass = Buffer;
			//writeUser(us,pass);
            active[us] = 0;
			writeActive(us);
            status = 1;
            send(client_socket, &status, sizeof(status),0);
        }
		else if(option == 6){
			int grp;
			recv(client_socket, Buffer, BUFF_SIZE, 0);
            us = Buffer;
			recv(client_socket, &grp, sizeof(grp), 0);
			//cout<<"group"<<grp<<endl;
			status = createGroup(us, grp);
			send(client_socket, &status, sizeof(status),0);
		}
		else if(option == 7){
			int more_grp = 1;
			ifstream file("groups");
    		int grp_id;
    		string uname;
			char user[50];
    		while(!file.eof()){
        		file >> grp_id;
       		 	file >> uname;
				cout<<"grp "<<grp_id<<" un "<<uname<<endl;
				//if(file.tellg() == -1) break;
				strcpy(user, uname.c_str());
				send(client_socket, &more_grp, sizeof(more_grp),0);
				send(client_socket, &grp_id, sizeof(grp_id),0);
				send(client_socket, user, sizeof(user),0);
    		}
			file.close();
			more_grp = 0;
			cout<<"exited "<<endl;
			send(client_socket, &more_grp, sizeof(more_grp),0);
			
		}
		else if(option == 8){
			int gid;
			bool st;
			recv(client_socket, &gid, sizeof(gid), 0);			
			st = joinGroupRequest(us,gid);
			send(client_socket, &st, sizeof(st),0);
		}
	}
    close(client_socket);
    pthread_exit(NULL);	
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
        sts = pthread_create(&thread[i], NULL, clientHandler, &client_socket);
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
