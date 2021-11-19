// Server side C/C++ program to demonstrate Socket programming
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <pwd.h>
#define PORT 8080
void red()
{
    printf("\033[1;31m");
}
void green()
{
    printf("\033[0;32m");
}
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    char *currentUser = "nobody";
    struct passwd* pwd;
    // Creating socket file descriptor
    // get the arguments set in line 96
    if (strcmp(argv[0], "inheritedProcess") == 0)
    {
        int duplicateSocket = atoi(argv[1]);// get the duplicate socket using argv 1 
        valread = read(duplicateSocket, buffer, 1024);// read the value
        send(duplicateSocket, hello, strlen(hello), 0);// send the message to the client 
        printf("Hello message sent\n");// print the message
        exit(0);
    }
    else{
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
        {
            perror("socket failed");
            exit(EXIT_FAILURE);
        }

        // Forcefully attaching socket to the port 8080 
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR , &opt, sizeof(opt)))
        {
            perror("setsockopt");
            exit(EXIT_FAILURE);
        }
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons( PORT );
        if (bind(server_fd, (struct sockaddr *)&address,sizeof(address)) < 0)
        {
            perror("bind failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen))<0)
        {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        // privilege separation starts here
        green();
        printf("Forking ........\n");
        pid_t childProcess = fork();
        printf("execve=0x%p\n", execve);

        if(childProcess < 0){
            perror("Error in forking\n");
            return -1;
        }
        else if (childProcess == 0){
            printf("Parent process forked\n");
            int copiedParentSocket = dup(new_socket);
            if(copiedParentSocket == -1)perror("Duplication of new socket did not work");
            struct passwd *pw = getpwnam(currentUser);
            if(pw == NULL){
                red();
                printf("Can't find UID for currentUser\n");
                return -1;
            }
            if (setuid(pw->pw_uid ) < 0){
                red();
                perror("Error in dropping privileges\n");
                return -1;
            }   
            char temp[10];
            sprintf(temp,"%d",copiedParentSocket); // copy to temp variable 
            char *args[] = {"inheritedProcess",temp,NULL}; // store the values in the args array
            execvp(argv[0],args); // calling execvp function again 
            /* If execvp returns, it must have failed. */
            printf("Unknown command\n");
            return 0;
        }
        else{
          wait(NULL);
          green();
          printf("Child process finished.\nParent Terminating\n" );
        }

    }
    return 0;
}