#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <iostream>

void get_line(char *buffer, int size)
{
    int ch = getchar();
    if (ch == '\n')
        ch = getchar();
    if (ch == EOF)
        printf("Eroare la getchar()\n");
    else
    {
        int i;
        for (i = 0; i < size - 1; i++)
        {
            if (ch == '\n' || ch == EOF)
                break;
            buffer[i] = ch;
            ch = getchar();
        }
        buffer[i] = '\0';
    }
}

int main()
{
    int sd, port;
    struct sockaddr_in server;
    char msg[100], IP[20] = "127.0.0.1";

    if ((sd = socket(AF_INET, SOCK_STREAM, 6)) == -1)
    {
        perror("Eroare: ");
        exit(2);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = inet_addr(IP);
    server.sin_port = htons(4000);

    if (connect(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
    {
        printf("Eroare la conectare server\n");
        exit(3);
    }
    printf("Bun venit in client. Comenzile trebuie sa fie de forma: x,y,0/1 unde:\n 1. Sunt in nodul x\n 2. Vreau sa ajung in nodul y\n 3. Vreau/nu vreau asistenta\n");
    get_line(msg, sizeof(msg));
    if (write(sd, msg, sizeof(msg)) <= 0)
    {
        printf("mesaj nereceptionat");
        exit(5);
    }
    if (read(sd, msg, sizeof(msg)) <= 0)
    {
        printf("mesaj nereceptionat");
        exit(5);
    }
    printf("Mesaj de la server : %s\n", msg);
    close(sd);
}