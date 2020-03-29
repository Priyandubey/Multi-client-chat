#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#define MAXCLIENT 100
using namespace std;
 
int main(){
    // Create a socket

    int listening = socket(AF_INET, SOCK_STREAM, 0),sd;
    if (listening == -1)
    {
        cerr << "Can't create a socket! Quitting" << endl;
        return -1;
    }
 
    // Bind the ip address and port to a socket
    sockaddr_in hint;
    hint.sin_family = AF_INET;
    hint.sin_port = htons(54000);
    hint.sin_addr.s_addr = INADDR_ANY;

    int client_socket[MAXCLIENT] = {0};

    int hintlen = sizeof(hint);

    bind(listening, (sockaddr*)&hint, hintlen);
 
    fd_set readfds;

    listen(listening,MAXCLIENT);

    cout<< "Client where are you all? \n";

    FD_ZERO(&readfds);
    FD_SET(listening,&readfds);

    while (true){

        for(int i = 0;i < MAXCLIENT;i++){
            if(client_socket[i] > 0){
                FD_SET(client_socket[i],&readfds);
            }
        }

        int activity = select(MAXCLIENT,&readfds,nullptr,nullptr,nullptr);

        if(activity < 0 && errno != EINTR){
            cout<<"select error";
        }

        if(FD_ISSET(listening,&readfds)){

            int sock = accept(listening,(sockaddr*)&hint,(socklen_t*)&hintlen);

            cout<< "New connection is "<< hint.sin_addr.s_addr << " port No. "<<hint.sin_port<<endl;
         
            for(int i = 0;i < MAXCLIENT;i++){
                if(client_socket[i] == 0){
                    client_socket[i] = sock;
                    break;
                }
            }
        }

            for(int i = 0;i < MAXCLIENT;i++){
                int tClient = client_socket[i];

                if(FD_ISSET(tClient,&readfds)){
                    char buff[1025];
                    memset(buff,0,1024);
                    int readBytes = recv(tClient,buff,1025,0);
                    if(readBytes <= 0){
                        getpeername(tClient, (struct sockaddr*)&hint ,(socklen_t*)&hintlen);
                        cout<< hint.sin_addr.s_addr <<" disconnected on port "<< hint.sin_port<<endl;
                        close(tClient);
                    }else{
                        
                        for(int l = 0;l < MAXCLIENT;l++){
                            if(client_socket[l] != tClient && client_socket[l] > 0){
                                send(client_socket[l],buff,readBytes,0);
                            }
                        }
                    }
                }

            }

        
    }

    close(listening);

    return 0;
}

