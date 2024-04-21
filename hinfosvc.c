#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <arpa/inet.h>
#include <netinet/in.h>
//#include <sys/socket.h>
#include <unistd.h>

void hostname (char *tmp)
{
    FILE *file;
    file = popen("cat /proc/sys/kernel/hostname","r");

    fgets(tmp, 100, file);
    pclose(file);
}

void cpu_name (char *tmp)
{
    FILE *file;
    file = popen("cat /proc/cpuinfo | head -n 5 | grep \"model name\" | awk -F': ' '{ print $2 }'","r");
    
    fgets(tmp, 100, file);
    pclose(file);
}

double load ()
{
FILE *file1;
int tmp1 = 0;
int tmp2 = 0;
double CPU_Percentage;

    // vypocet predoslej zataze

file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $5 }'","r");
fscanf(file1,"%d",&tmp1);
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $6 }'","r");
fscanf(file1,"%d",&tmp2);

int PrevIdle = tmp1 + tmp2;

file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $2 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 = tmp1;
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $3 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 += tmp1;
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $4 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 += tmp1;
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $7 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 += tmp1;
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $8 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 += tmp1;
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $9 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 += tmp1;

int PrevNonIdle = tmp2;
int PrevTotal = PrevIdle + PrevNonIdle;

    // vypocet aktualnej zataze

popen("sleep 0.5s","r");
pclose(file1);

file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $5 }'","r");
fscanf(file1,"%d",&tmp1);
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $6 }'","r");
fscanf(file1,"%d",&tmp2);

int Idle = tmp1 + tmp2;

file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $2 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 = tmp1;
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $3 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 += tmp1;
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $4 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 += tmp1;
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $7 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 += tmp1;
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $8 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 += tmp1;
file1 = popen("cat /proc/stat | head -n 1| awk -F' ' '{ print $9 }'","r");
fscanf(file1,"%d",&tmp1);
tmp2 += tmp1;

int NonIdle = tmp2;
int Total = Idle + NonIdle;

    //aktualna hondota minus predosla hodnota

double totald = Total - PrevTotal;
double idled = Idle - PrevIdle;

CPU_Percentage = ((totald - idled)/totald)*100;

pclose(file1);
return CPU_Percentage;
}

void server(char **argv)
{
    int port_number = atoi(argv[1]);
    int optval = 1;
    int backlog = 3;
    char buffer[1024] = {0};
    struct sockaddr_in address;
    int dres_len = sizeof(address);

    ////////////////////////SOCKET////////////////////////////////

    int servername;
    if ((servername = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
        perror("ERROR: socket");
        exit(EXIT_FAILURE);
    }
    setsockopt(servername,SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &optval,sizeof(optval)); 

    ////////////////////BIND/////////////////////////////////////

    address.sin_family = AF_INET;
    address.sin_port = htons(port_number);
    address.sin_addr.s_addr = INADDR_ANY;
    
    if (bind(servername,(struct sockaddr *) &address, sizeof(address)) < 0)
    {
        perror("ERROR: bind");
        exit(EXIT_FAILURE);
    }

    ////////////////////LISTEN/////////////////////////////////////

    int new_socket;
    if (listen(servername,backlog) < 0)
    {
        perror("ERROR: listen");
        exit(EXIT_FAILURE);
    }

    ////////////////////ACCEPT/////////////////////////////////////

    while (1) 
    {
        new_socket = accept(servername , (struct sockaddr *)&address, (socklen_t *)&dres_len);
        if(new_socket > 0)
        {
            read( new_socket , buffer, 1024);
            char OK[400] = "HTTP/1.1 200 OK\r\nContent-Type: text/plain;\r\n\r\n";

            if(strncmp(buffer,"GET /hostname",strlen("GET /hostname"))==0)
            {
                char host[100]; hostname(host);
                strcat(OK,host);
                send(new_socket , OK , strlen(OK) , 0);
            }
            else if(strncmp(buffer,"GET /cpu-name",strlen("GET /cpu-name"))==0)
            {
                char cpu[100];  cpu_name(cpu);
                strcat(OK,cpu);
                send(new_socket , OK , strlen(OK) , 0);
            }
            else if(strncmp(buffer,"GET /load",strlen("GET /load"))==0)
            {
                int Percentage = load();
                char cpu[100];
                sprintf(cpu, "%d", Percentage);

                strcat(OK,cpu);strcat(OK,"%");
                send(new_socket , OK , strlen(OK) , 0);
            }
            else
            {
            char *er = "HTTP/1.1 400 Bad Request\r\nContent-Type: text/plain;\r\n\r\nBad Request";
            send(new_socket , er , strlen(er) , 0 );
            }
        }
        close(new_socket);
    }
    close(servername);
}

int main(int argc,char **argv){(void)argc;

    if (argv[1] != NULL)
    {
        server(argv);
    }  
    else
    {
        perror("ERROR: missing port number");
        exit(EXIT_FAILURE);
    }
return 0;}