// Madeline Clausen (clausenm)

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#define PORT 38081 // can change

int main(int argc, char *argv[])
{
    int sock = 0, valread, client_fd;
    int port = atoi(argv[2]);
    struct sockaddr_in serv_addr;
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        return -1;
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    if (inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<= 0) 
    {
        return -1;
    }
    if ((client_fd= connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr))) < 0)
    {
        return -1;
    }
    printf("> ");
    char input[80];
    fgets(input, 80, stdin);
    char* command = strtok(input, " \n\t");

    while(strcmp("quit", command) != 0)
    {
        char buffer[1024] = {0};
        char *buffer_p = buffer;
        if(strcmp("PricesOnDate", command) == 0)
        {
            char pod_message[80];
            char* next_command = strtok(NULL, " \n\t");
            strcat(pod_message, command);
            strcat(pod_message, " ");
            strcat(pod_message, next_command);
            send(sock, pod_message, strlen(pod_message), 0);
            int valread = read(sock, buffer, 1024);
            printf("%s", buffer_p);
        }
        else if(strcmp("MaxPossible", command) == 0)
        {
            char mp_message[100];
            strcat(mp_message, command);
            char* next_command;
            for (int i=0; i<4; i++)
            {
                strcat(mp_message, " ");
                next_command = strtok(NULL, " \n\t");
                strcat(mp_message, next_command);
            }
            send(sock, mp_message, strlen(mp_message), 0);
            int valread = read(sock, buffer, 1024);
            printf("%s", buffer_p);
        }
        else
        {
            printf("Invalid syntax\n");
        }
        printf("> ");
        fgets(input, 80, stdin);
        char* command = strtok(input, " \n\t");
    }
    send(sock, "quit", strlen("quit"), 0);
    close(client_fd);
    return 0;
}