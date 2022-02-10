#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Проверка выполения фунцкий на ошибки
void CHECK_RESULT(int res, char * msg)
{
    do {
        if (res < 0) {
            perror(msg);
            exit(EXIT_FAILURE); }
    } while (0);
}

#define BUF_SIZE 1024

int main(int argc, char *argv[]) {

    // Для создание соединения
    int clientSocket;
    char buffer[BUF_SIZE] = {0};
    struct sockaddr_in serverAddr = {0};

    // Для getopt()
    int option = 0;
    unsigned short int opt_port = 0;
    char * opt_ip = NULL;

    // Для getline()
    char * str = NULL;
    char * str_copy = NULL;
    size_t len = 0;

    // Переменные окружения
    setenv("L2PORT", "5555", 1);
    setenv("L2ADDR", "127.0.0.1", 1);

    // Принятие ключей через getopt()
    while ( (option = getopt(argc,argv,"a:p:vh")) != -1)
    {
        switch (option)
        {
        case 'a': opt_ip = optarg;                                                      break;
        case 'p': opt_port = (unsigned short int)atoi(optarg);                          break;
        case 'v': printf("Current version: 1.0.1\n");                                   return 0;
        case 'h': printf("lab2client -a \"IP\" | -p \"PORT\" | -v \"is version\"\n");   return 0;
        case '?': printf("Invalid argument\n");                                         return -1;
        }
    }

    // Замена переменных на переменные окружения, если не переданы через строку
    if (opt_ip==NULL) opt_ip = getenv("L2ADDR");
    if (opt_port==0) opt_port = (unsigned short int)atoi(getenv("L2PORT"));

    while (getline(&str, &len, stdin))
    {
        // Создание сокета
        clientSocket = socket(AF_INET, SOCK_STREAM, 6);
        CHECK_RESULT(clientSocket, "socket");

        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(opt_ip);
        serverAddr.sin_port = htons(opt_port);

        // Установление соединения с сокетом
        int res = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        CHECK_RESULT(res, "connect");

        // Отправка сообщения в сокет
        res = send(clientSocket, str, strlen(str)+1, 0);
        CHECK_RESULT(res, "sendto");

        // Закрытие соединения на отправку серверу
        res = shutdown(clientSocket, SHUT_WR);
        CHECK_RESULT(res, "shutdown");

        // Получение сообщения от сокета
        res = recv(clientSocket, buffer, BUF_SIZE, 0);
        CHECK_RESULT(res, "recvfrom");
        printf("%s\n", buffer);

        // Закрытие сокета
        close(clientSocket);
    }

    return 0;
}
