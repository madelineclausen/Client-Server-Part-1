// Madeline Clausen (clausenm)

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>
#define PORT 38081

char* make_date(char first_date[10], char* date)
{
    char* year = strtok(date, "-");
    char* month = strtok(NULL, "-");
    char* day = strtok(NULL, "\n");
    if(month[0]=='0')
    {
        month++;
    }
    if(day[0]=='0')
    {
        day++;
    }
    strcat(first_date, month);
    strcat(first_date,"/");
    strcat(first_date, day);
    strcat(first_date, "/");
    strcat(first_date, year);
    return first_date;
}

char* file_parser(char* date, char* full_file)
{
    FILE* fp = fopen(full_file, "r");
    char file_buffer[1024];
    int row = 0;
    int column = 1;
    bool found = false;
    while (fgets(file_buffer, 1024, fp))
    {
        column = 0;
        row++;
        if (row == 1)
        {
            continue;
        }
        char* value = strtok(file_buffer, ",");
        while (value)
        {
            if (column == 0 && strcmp(date, value) == 0)
            {
                found = true;
            }
            if (column == 4 && found)
            {
                char value_str[10];
                strcat(value_str, value);
                strcat(value_str, "\0");
                return value;
            }
            column++;
            value = strtok(NULL, ",");
        }
    }
    fclose(fp);
    return NULL;
}

int main(int argc, char const* argv[])
{
    int new_socket, server_fd, new_command;
    const char* file1 = argv[1];
    const char* file2 = argv[2];
    int port = atoi(argv[3]);
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    if (setsockopt(server_fd, SOL_SOCKET,SO_REUSEADDR | SO_REUSEPORT, &opt,sizeof(opt))) 
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);
    if (bind(server_fd, (struct sockaddr*)&address,sizeof(address))< 0) 
    {
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) 
    {
        exit(EXIT_FAILURE);
    }
    if ((new_socket= accept(server_fd, (struct sockaddr*)&address,(socklen_t*)&addrlen))< 0) 
    {
        exit(EXIT_FAILURE);
    }
    
    printf("server started\n");
    while(true)
    {
        char buffer[1024] = {0};
        char *buffer_p = buffer;
        int valread = read(new_socket, buffer, 1024);
        printf("%s\n", buffer_p);
        char* first_value = strtok(buffer, " ");
        char* pfe_file = "./PFE.csv";
        char* mrna_file = "./MRNA.csv";
        char* next_value = strtok(NULL, " ");
        char* third_value = strtok(NULL, " ");

        if (strcmp("quit", first_value) == 0) // quit command
        {
            close(new_socket);
            shutdown(server_fd, SHUT_RDWR);
            break;
        }
        else if (third_value == NULL) // PricesOnDate
        {
            char first_date[10];
            char message[60];
            char* stock_date = make_date(first_date, next_value);
            strcat(message, "PFE: ");
            char* pfe_price = file_parser(stock_date, pfe_file);
            // float ftemp = atof(pfe_price);
            // sprintf(message, "PFE: %0.2f | MRNA: ", ftemp);
            strcat(message, pfe_price);
            strcat(message, " | MRNA: ");
            char* mrna_price = file_parser(stock_date, mrna_file);
            strcat(message, mrna_price);
            strcat(message, "\n");
            if (pfe_price == NULL || mrna_price == NULL)
            {
                char message[50];
                strcat(message, "Unknown\n");
            }
            send(new_socket, message, strlen(message), 0);
        }
        else // MaxPossible
        {
            char given_1[10];
            char given_2[10];
            char* given_day_1 = make_date(given_1, strtok(NULL, " "));
            char* given_day_2 = make_date(given_2, strtok(NULL, " "));
            char* chosen_file;
            if (strcmp(third_value, "PFE") == 0)
            {
                chosen_file = pfe_file;
            }
            else
            {
                chosen_file = mrna_file;
            }
            float price_1 = atof(file_parser(given_day_1, chosen_file)); // CHANGED
            float price_2 = atof(file_parser(given_day_2, chosen_file));
            float final_price;
            if (strcmp(next_value, "profit") == 0)
            {
                final_price = price_2 - price_1;
            }
            else
            {
                final_price = price_1 - price_2;
            }
            char message[10];
            gcvt(final_price, 5, message);
            send(new_socket, message, strlen(message), 0);
        }
    }  
}