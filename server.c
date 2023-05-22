#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <pthread.h>

#define MAX_CLIENT 256
#define MAX_TABOO_WORDS 100
#define MAX_WORD_LEN 100

int client_count = 0;
int client_sockets[MAX_CLIENT] = {0,};
pthread_mutex_t mutex_key = PTHREAD_MUTEX_INITIALIZER; // mutex_key 초기화
char taboo_words[MAX_TABOO_WORDS][MAX_WORD_LEN];
int num_taboo_words = 0;

void error_handling(const char*);
void sending_message(const char*, int);
void* handling_client(void*); //tast function
bool check_for_taboo_words(const char*, const char[][MAX_WORD_LEN], int);

int main(int argc, const char* argv[])
{
    int serv_sock = 0;
    int clnt_sock = 0;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clnt_addr;
    memset(&serv_addr, 0, sizeof serv_addr);
    memset(&clnt_addr, 0, sizeof clnt_addr);
    int client_addr_size = 0;
    pthread_t pthread_id= 0ul;
  

    if(argc != 2)
    {
        error_handling("MULTI GAME SERVER 9999");
    }
    pthread_mutex_init(&mutex_key, NULL);
    if((serv_sock = socket(PF_INET, SOCK_STREAM, 0)) == -1)
    {
        error_handling("socket() error");
    }
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(argv[1]));
    if(bind(serv_sock, (const struct sockaddr*)&serv_addr, sizeof serv_addr) == -1)    {
        error_handling("bind() error");
    }
    if(listen(serv_sock, 5) == -1)
    {
        error_handling("listen() error");
    }
    

    printf("호스트는 금칙어 갯수를 입력하시오: ");
    scanf("%d", &num_taboo_words);
    getchar(); 
    printf("호스트는 금칙어 단어를 입력하시오 : \n");
    for (int i = 0; i < num_taboo_words; ++i) {
        fgets(taboo_words[i], MAX_WORD_LEN, stdin);
        taboo_words[i][strcspn(taboo_words[i], "\n")] = 0;  
    }

   while (true)
   {
        client_addr_size = sizeof clnt_addr;
        if((clnt_sock = accept(serv_sock, (struct sockaddr*)&clnt_addr, (socklen_t*)&client_addr_size)) == -1)
        {
            error_handling("accept() error");
        }
        pthread_mutex_lock(&mutex_key);
        client_sockets[client_count++] = clnt_sock;
        pthread_mutex_unlock(&mutex_key);
        pthread_create(&pthread_id, NULL, handling_client, (void*)&clnt_sock);
        pthread_detach(pthread_id);
        fprintf(stdout,"Connected client IP : %s\r\n", inet_ntoa(clnt_addr.sin_addr));
 }
    close(serv_sock);
    return 0;
}

void error_handling(const char* _message)
{
    fputs(_message, stderr);
    fputs("\r\n",stderr);
    exit(1);
    return;
}

void sending_message(const char* _message, int _str_length)
{
    pthread_mutex_lock(&mutex_key);
    for(int i = 0; i < client_count; ++i)
    {   

        write(client_sockets[i], _message, _str_length);
    }
    pthread_mutex_unlock(&mutex_key);
    return;
}

bool check_for_taboo_words(const char* message, const char taboo_words[][MAX_WORD_LEN], int num_taboo_words) {
    for (int i = 0; i < num_taboo_words; i++) {
        if (strstr(message, taboo_words[i]) != NULL) {
            return true;
        }
    }
    return false;
}

void* handling_client(void * args)
{
    int client_sock = *((int *)args);
    int str_length = 0;
    char message[BUFSIZ] = {'\0', };
    bool is_taboo = false;
    
    // 금칙어 송신
    extern char taboo_words[MAX_TABOO_WORDS][MAX_WORD_LEN];
    extern int num_taboo_words;

    while((str_length = read(client_sock, message, BUFSIZ - 1)) != 0)
    {
        is_taboo = check_for_taboo_words(message, taboo_words, num_taboo_words);

        if(is_taboo){
            char warning[] = " 경고! 사용하신 단어 중 금칙어가 발견되었습니다. \n";
            char lose[] = "패배하였습니다. \n";
            write(client_sock, warning, strlen(warning));
            write(client_sock, lose, strlen(lose));
            close(client_sock);
            return NULL:
        }   else {
           
                sending_message(message, str_length);
                break;
            }
        }
    }

    //GC 기능을 수행
    for(int i = 0; i < client_count; ++i)
    {
        pthread_mutex_lock(&mutex_key);
        if(client_sock == client_sockets[i])
        {
            while(i++ < client_count -1)
            {
                client_sockets[i] = client_sockets[i + 1];
            }
            break;
        }
    }
    --client_count;
    pthread_mutex_unlock(&mutex_key);
    close(client_sock);
    return NULL;
}
