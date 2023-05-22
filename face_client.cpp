#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <iostream>
#include <chrono>
#include <thread>

#define NAME_SIZE 20

void* sending_message(void*);
void* receive_message(void*);
void error_handling(const char *);
char name[NAME_SIZE] = {'\0', };
char message[BUFSIZ] = {'\0', };
extern void detect_dnn_face();
extern double compare_images();

int main(int argc, const char* argv[])
{
    int sock = 0;
    struct sockaddr_in serv_addr;
    pthread_t send_thread = 0ul;
    pthread_t receive_thread = 0ul;
    void* thread_return = NULL; //thread 종료될때 NULL로 받겠다.
    if(argc != 4)
    {
        error_handling("ClIENT SERVER_IP 9999 NICK_MANE");
    }
    sprintf(name,"[%s]", argv[3]);
    if((sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        error_handling("socket() error");
    }
    memset(&serv_addr, 0, sizeof serv_addr);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));
    if(connect(sock, (const struct sockaddr*)&serv_addr, sizeof serv_addr) == -1)
    {
        error_handling("connect() error");
    }




    const char* authentication_message = "얼굴 인증을 시작하겠습니다.";
    std::cout << authentication_message << std::endl;
    write(sock, authentication_message, strlen(authentication_message));

    detect_dnn_face();

    std::this_thread::sleep_for(std::chrono::seconds(4));

    const char* confirmation_message = "얼굴이 인식되었습니다. 확인을 시작합니다.";
    std::cout << confirmation_message << std::endl;
    write(sock, confirmation_message, strlen(confirmation_message));

    std::this_thread::sleep_for(std::chrono::seconds(4));
    double similarity = compare_images();
    if (similarity <= 0.7)
    {
        const char* approval_message = "승인되었습니다.";
        std::cout << approval_message << std::endl;
        write(sock, approval_message, strlen(approval_message));
        std::this_thread::sleep_for(std::chrono::seconds(10));
        
    }
    else
    {
        const char* failure_message = "인증에 실패했습니다.";
        std::cout << failure_message << std::endl;
        write(sock, failure_message, strlen(failure_message));
        

    }


    pthread_create(&send_thread, NULL,sending_message,(void*)&sock);
    pthread_create(&receive_thread, NULL, receive_message, (void*)&sock);
    pthread_join(send_thread, &thread_return);
    pthread_join(receive_thread, NULL);
    close(sock);

    return 0;
}
// task function
void* sending_message(void* args)
{
     int sock = *((int*)args);
    char message[BUFSIZ] = {'\0', };
    int str_length = 0;
    while(true)
    {
        fgets(message, BUFSIZ, stdin);
        if(!strcmp(message, "q\n") || !strcmp(message, "Q\n"))
          {
            close(sock);
            exit(0);
           }
           write(sock, message, strlen(message));
    }
    return NULL;
    
}

void* receive_message(void* args)
{
    int sock = *((int*)args);
    char name_message[NAME_SIZE + BUFSIZ] = {'\0', };
    int str_length = 0;
    while(true)
    {
        if((str_length = read(sock, name_message, NAME_SIZE + BUFSIZ - 1)) == -1)
        {

            return NULL;
        }
        name_message[str_length]= '\0';
        std::cout << name_message << std::endl;
    }
    return NULL;
}

void error_handling(const char* _message)
{
    std::cerr << _message << std::endl;
    exit(1);
    return;
}
