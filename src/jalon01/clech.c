
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "command.h"

#define PORT_NUMBER     9010
#define SERVER_ADDRESS  "127.0.0.1"

int recvfile(void *argument)
{
        //struct arg *arg2 = argument;
        char *namefile = argument;
        int client_socket;
        ssize_t len;
        struct sockaddr_in remote_addr;
        char buffer[1000];
        int file_size;
        int received_file;
        int remain_data = 0;
        //long leng = 256;

        /* Zeroing remote_addr struct */
        memset(&remote_addr, 0, sizeof(remote_addr));

        /* Construct remote_addr struct */
        remote_addr.sin_family = AF_INET;
        inet_pton(AF_INET, SERVER_ADDRESS, &(remote_addr.sin_addr));
        remote_addr.sin_port = htons(PORT_NUMBER);

        received_file = open(namefile, O_WRONLY | O_CREAT);
        if (received_file == -1)
        {
                fprintf(stderr, "Failed to open file foo --> %s\n", strerror(errno));

                exit(EXIT_FAILURE);
        }

        /* Create client socket */
        client_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket == -1)
        {
                fprintf(stderr, "Error creating socket --> %s\n", strerror(errno));

                exit(EXIT_FAILURE);
        }

        /* Connect to the server */
        if (connect(client_socket, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr)) == -1)
        {
                fprintf(stderr, "Error on connect --> %s\n", strerror(errno));

                exit(EXIT_FAILURE);
        }

        /* Receiving file size */
        recv(client_socket, buffer, BUFSIZ, 0);
        file_size = atoi(buffer);
        //fprintf(stdout, "\nFile size : %d\n", file_size);

        remain_data = file_size;
        printf("%d\n", remain_data);

        //close(client_socket);
        //recv(client_socket, buffer, BUFSIZ, 0);
        //printf("%s \n", buffer);

        send(client_socket, "coucou",256,0);

        //Getting file Data
        char buf[remain_data];
        //while (remain_data > 0)
        //{
                //fwrite(buffer, sizeof(char), len, received_file);
                //len = recv(client_socket, buffer, BUFSIZ, 0);
                printf("read : %i \n ", read(client_socket, buf, remain_data));
                //len = write(received_file,buffer,BUFSIZ);
                printf("write : %i \n", write(received_file, buf, remain_data));
                remain_data -= len;
                fprintf(stdout, "Receive %d bytes and we hope :- %d bytes\n", len, remain_data);
        //}

        //write(received_file, buffer,0);
        close(received_file);
        return 0;
}
