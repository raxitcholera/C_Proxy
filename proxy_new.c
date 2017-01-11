//Includes
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<strings.h>
#include<string.h>
#include<stdlib.h>
#include<netdb.h>
#include<stdbool.h>

#define BACKLOG_QUEUE 1 
char* find_next_step(char response[]);
//main start {
int main(int arg_cnt,char *arg_val[])
{
//variables
	int proxy_socket,client_socket,clilen,n,server_socket,temp_n,ftp_transfer_socket;                                       
  	char read_buffer[2097152], write_buffer[4194304];                                                                    
  	struct sockaddr_in proxy_addr,accept_addr;                         
//check the arguments passed
	if(arg_cnt<2)                                                               
	{
		fprintf(stderr,"\nError, No port provided ");
		exit(1);                                                                  
	}

//open proxy_socket socket
	proxy_socket=socket(AF_INET,SOCK_STREAM,0);                                   
  
	if(proxy_socket<0)                                                            
	{
		perror("\nError in opening proxy socket ");
		close(proxy_socket);
		exit(1);                                                                  
	}

//set proxy address structures	
	bzero((char *)&proxy_addr,sizeof(proxy_addr));                              
	bzero((char *)&accept_addr,sizeof(accept_addr));                         

	proxy_addr.sin_family=AF_INET;                                              
	proxy_addr.sin_addr.s_addr=INADDR_ANY;                                      
	proxy_addr.sin_port=htons(atoi(arg_val[1])); 

	if(bind(proxy_socket,(struct sockaddr *)&proxy_addr,sizeof(proxy_addr))<0)    
	{
		perror("\nError in binding proxy socket and proxy addresses ");
		close(proxy_socket);
		exit(1);                                                                  
	}
  
	listen(proxy_socket,BACKLOG_QUEUE);                                                                         
	clilen=sizeof(accept_addr); 

//while loop 
	while(1) {
    char host[100],request[7],site[300],*trim_host=NULL,*find_conn=NULL,sitecpy[300],*trim_port=NULL,ftp_port[7],ftp_host[64],ftp_file_path[300];      
    bool portprovided=false,proceed=false,isftp=false,ishttp=false;                                                                 
    int port,i,ftp_fileSize,start_from; 
    struct addrinfo check, *response;
	//accept (action_socket)
	client_socket=accept(proxy_socket,(struct sockaddr *)&accept_addr,&clilen);                                
    
    if(client_socket<0)                                                                                      
    {
      perror("\nError in accepting socket on proxy ");
      close(proxy_socket);
      close(client_socket);
      exit(1);                                                                                             
    }
	//read the buffer from browser (action_socket)
	n=read(client_socket,read_buffer,sizeof(read_buffer));                                 
    printf("read_buffer from browser -> %s",read_buffer);
    sscanf(read_buffer,"%s %s ",request,site);
	//parse the request

	//check if the request is valid as per scope
	if((strcmp(request,"GET")!=0))                                             
	{
		printf("\nNot a GET Request!");
		proceed=false;                                    
	}
	else if((strncmp(site,"http://",7)!=0) && (strncmp(site,"ftp://",6)!=0))                                    
	{
		printf("\nNot a HTTP or FTP Request!");
		proceed=false;
	}
	else
		proceed=true;

	if(proceed) {
		if(strncmp(site,"ftp://",6)==0) {
			isftp = true;
			port = 21; 
			start_from =6; 
			printf("Its an ftp connection request\n");
		}
		if(strncmp(site,"http://",7)==0) {
			ishttp = true;
			port = 80; 
			start_from =7; 
			printf("Its an http connection request\n");
		}
		//check if there is any port provided
		for(i=start_from;i<strlen(site);i++)                                            
			{
				if(site[i]==':')
				{
					memcpy(sitecpy,&site[i],strlen(site)-i);                           
					sitecpy[strlen(site)-i]='\0';                                      
					trim_port=strtok(sitecpy,":/");                                    

					for(i=0;i<strlen(trim_port);i++)                   
					{
						if(trim_port[i]<'0'||trim_port[i]>'9')
							{
								portprovided=false;
								printf("Port Not provided\n");
								break;
							}
						else
							{
								portprovided=true;
								printf("Port Provided as %s \n",trim_port);
							}
					}
				break;
				}
			}
	//set the port if provided	
	if(portprovided==true)
		port=atoi(trim_port);
    bzero(sitecpy,sizeof(sitecpy));
	strncpy(sitecpy,site,strlen(site)); 
	trim_host=strtok(site,"//");                                           
    trim_host=strtok(NULL,"/:");
	sprintf(host,"%s",trim_host); 

	start_from = start_from+strlen(host);
	bzero(ftp_file_path,sizeof(ftp_file_path));
	strncpy(ftp_file_path,sitecpy+start_from,strlen(sitecpy)); 

    server_socket=socket(AF_INET,SOCK_STREAM,0);

    if(server_socket<0)                                                         
	{
		perror("\nError in opening web socket ");
		close(proxy_socket);
		close(server_socket);
		close(client_socket);
		exit(1);                                                             
	}
	//find the addrinfo (getaddrinfo)
		memset(&check,0,sizeof(check));
		check.ai_family = INADDR_ANY;
		check.ai_socktype = SOCK_STREAM;
		bzero((char *)&response,sizeof(response));                             
       
    // takes care of port and other anomalies
        char r_port[10];
        sprintf(r_port, "%d", port);
        printf("This is the port in string %s and %d in integet",r_port,port);
        if(getaddrinfo(host,r_port,&check,&response)!=0)
        {
            printf("\nBad host or error in host");
            char bad_req[]="400 : BAD REQUEST\n\nNo Such host!";                 
            write(client_socket,bad_req,strlen(bad_req));
            close(client_socket);
            continue;
        }
	//connect to the web socket
		if(connect(server_socket,response->ai_addr,response->ai_addrlen)<0)
		{
			perror("\nError connecting to webserver ");
			close(server_socket);
			continue;                                                              
		}
	//set web_socket
	//write to server_socket
		if(ishttp){
	      	write(server_socket,read_buffer,strlen(read_buffer));                                   
		}else if (isftp){
			
			do                                            
			{
				read_again:;
				bzero(write_buffer,sizeof(write_buffer));                              
				n=read(server_socket,write_buffer,1048576);
				printf("%d server -> %s\n",n,write_buffer );
				if(n>0)
				{
					char* next_step = find_next_step(write_buffer);
					// printf("%s\n",next_step );
					if(strcmp(next_step,"RETR #### ****")==0)
					{
						char new_response[100];
						memset(new_response,'\0', sizeof(new_response));
						strncpy(new_response,"RETR ",5);
						strcat(new_response,ftp_file_path);
						strcat(new_response,"\r\n");
						next_step = new_response;
						// sprintf(next_step,"RETR %s\r\n",ftp_file_path);
						char port1[3],port2[3],ip1_temp[50],ip1[3],ip2[3],ip3[3],ip4[3],port2_temp[10],ftp_host[100],ftp_port[100]; 
						int i;
						memset(ftp_port, '\0', sizeof(ftp_port));
						memset(ftp_host, '\0', sizeof(ftp_host));
						sscanf(write_buffer,"%[^,],%[^,],%[^,],%[^,],%[^,],%[^,]",ip1_temp,ip2,ip3,ip4,port1,port2_temp);
						for(i=0;i<strlen(ip1_temp);i++)
						{
							if(ip1_temp[i]=='(')
							{
								strncpy(ip1,ip1_temp+i+1,strlen(ip1_temp));
							}
						}
						for(i=0;i<strlen(port2_temp);i++)
						{
							if(port2_temp[i]==')')
							{
								strncpy(port2,port2_temp,strlen(port2_temp)-i);
							}
						}
						sprintf(ftp_port,"%d",atoi(port1)*256+atoi(port2));
						sprintf(ftp_host,"%s.%s.%s.%s",ip1,ip2,ip3,ip4);

						//open socket to accept info from the new server port
						ftp_transfer_socket = socket(AF_INET,SOCK_STREAM,0);
						//connect the new socket with calculated socket of server
						bzero((char *)&response,sizeof(response)); 
						if(getaddrinfo(ftp_host,ftp_port,&check,&response)!=0)
					        {
					            printf("\nBad host or error in host");
					            char bad_req[]="400 : BAD REQUEST\n\nNo Such host!";                 
					            write(client_socket,bad_req,strlen(bad_req));
					            close(client_socket);
					            continue;
					        }
						//write RETR cmd for server_socket
					    printf("client : %s\n",next_step);
					    temp_n = write(server_socket,next_step,strlen(next_step));
						//while loop for reading on that socket
						if(connect(ftp_transfer_socket,response->ai_addr,response->ai_addrlen)<0)
							{
								perror("\nError connecting to webserver ");
								close(ftp_transfer_socket);
								close(server_socket);
								continue;                                                              
							}
						//read server_socket for 105
						do                                            
						{
							bzero(write_buffer,sizeof(write_buffer));                              
							n=read(ftp_transfer_socket,write_buffer,1048576);
							// printf("response buffer -> \n %s\n",write_buffer );
							if(n>0){
								write(client_socket,write_buffer,n);
							}

						}while(n>0);
						close(ftp_transfer_socket);
						goto read_again;

					}else if (strcmp(next_step,"SIZE ####")==0)
					{
						// bzero(next_step,sizeof(next_step));
						char new_response[100];
						memset(new_response,'\0', sizeof(new_response));
						strcpy(new_response,"SIZE ");
						strcat(new_response,ftp_file_path);
						strcat(new_response,"\r\n");
						next_step = new_response;

						// sprintf(next_step,"SIZE %s\r\n",ftp_file_path);
					} else if(strcmp(next_step,"PASV ****")==0) {
						char temp_code[10],ftp_string_fileSize[100];
						sscanf(write_buffer,"%s %s",temp_code,ftp_string_fileSize);
						ftp_fileSize = atoi(ftp_string_fileSize);
						next_step = "PASV \r\n";
					}
					else if(strcmp(next_step,"Continue")==0){
						goto read_again;
					}
					else if(strcmp(next_step,"Error")==0) {
						printf("Error called\n");
			            char bad_req[]="400 : BAD REQUEST\n\nError in communication !";                 
			            write(client_socket,bad_req,strlen(bad_req));
			            close(ftp_transfer_socket);
			            close(client_socket);
			            continue;
					}
					printf("client : %s\n",next_step);
					temp_n = write(server_socket,next_step,strlen(next_step));
				}else {
					char bad_req[]="400 : BAD REQUEST\n\nError in communication !";                 
			        write(client_socket,bad_req,strlen(bad_req));
			        close(ftp_transfer_socket);
			        close(client_socket);
			        continue;
				} 

			}while(n>0);
			close(client_socket);

		}
		//Code to read from server to client (file write or response)
		do                                            
		{
			bzero(write_buffer,sizeof(write_buffer));                              

			n=read(server_socket,write_buffer,1048576);
			printf("response buffer -> \n %s\n",write_buffer );
			if(n>0){
				write(client_socket,write_buffer,n);
			}

		}while(n>0);
		//close the server_socket
		close(server_socket); 

	//read the response from the server
	//write to the action_socket till the server has response
		}//end if with proceed
		else                                           
		{
			char bad_req[]="400 : BAD REQUEST\n\nOnly HTTP Protocols and GET requests accepted";
			write(client_socket,bad_req,strlen(bad_req));    
			close(client_socket);                            
		}
	} //end while

//close proxy_socket
	close(server_socket);
	close(proxy_socket);
//close web_socket

}

char* find_next_step(char response[])
{
	char response_code[50],message[100];
	char * next_request;
	sscanf(response,"%s %s ",response_code,message);
	switch(atoi(response_code)){
		case 220:
			next_request = "USER anonymous\r\n";
			break;
		case 331:
			next_request = "PASS anonymous\r\n";
			break;
		case 230: //to get size
			next_request = "SIZE ####";
			break;
		case 213: 
			next_request = "PASV ****";
			break;
		case 227:
			next_request = "RETR #### ****";
			break;
		case 150:
		case 221:
			next_request = "Continue";
			break;
		case 226:
			next_request = "QUIT\r\n";
			break;
		default:
			next_request = "Error";
			break;

	}
	return next_request;
}
