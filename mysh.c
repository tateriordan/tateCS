#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    char commandLine[512];
    char* path = "/";
    if (argc > 2) {
        fprintf(stderr,"Usage: mysh [batch-file]\n");
        return 1;
    }
    if (argc == 2) {
        if (!strchr(argv[1], '.')) {
            fprintf(stderr, "Error: Cannot open file this_file_does_not_exist.\n");
            return 1;
        }
    }
    if (argv[1]) {
        FILE *fp = fopen(argv[1],"r");
        char buffer[100];
        while (fgets(buffer, 100, fp)) {
            if (buffer == NULL) {
                break;
            }
            write(STDOUT_FILENO,buffer, 512);
            // if (strchr(buffer, '>') != NULL) {
                
            // }
            if (strcmp(buffer, "exit\n") == 0) {
                write(STDOUT_FILENO, "exit\n", 6);
                exit(EXIT_SUCCESS);
            }
            
            char *tok;
            tok = strtok(buffer, " ");
            int j = 0;
            char *fi = NULL;
            int i = 0;
            while (tok != NULL) {
                if (i == 1) {
                    argv[j] = tok;
                    fi = tok;
                    tok = strtok(NULL, " ");
                    j = j + 1;
                    
                } else if (strchr(tok, '>')) {
                    argv[j] = tok;
                    tok = strtok(NULL, " ");
                    i = 1;
                    j = j + 1;
                    
                } else {
                    argv[j] = tok;
                    tok = strtok(NULL, " ");
                    j = j + 1;
                }
            }
            argv[j] = NULL;
            argc = j;
        // for (int i = 0; i < argc; ++i) {
        //     write(1, argv[i], strlen(argv[i]));
        // }
            char path2[100];
            strcpy(path2, path);
            strcat(path2, argv[0]);
            for (int k = 0; k < strlen(path2); ++k) {
                if (path2[k] == '\n') {
                    path2[k] = '\0';
                }
            }   
            int pid = fork();
            if (pid == 0) {
                if (fi != NULL) {
                    //FILE *fp = fopen(fi,"w");
                    int fd = open(fi, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    //int fd = popen(fi);
                    dup2(fd,1);
                    dup2(fd,2);
                    close(fd);
                } else {
                    execv(path2,argv);
                }

            } else {
                wait(NULL);
            }
        
        }
    } else {
        int helper = 1;
        while (helper) {
            write(STDOUT_FILENO,"mysh> ", 6);
            if (!fgets(commandLine, 512, stdin)) {
                break;
            }
            if (strcmp(commandLine, "exit\n") == 0) {
                write(STDOUT_FILENO, "exit\n", 6);
                exit(EXIT_SUCCESS);
            }
            char *tok;
            tok = strtok(commandLine, " ");
            int j = 0;
            char *fi = NULL;
            int i = 0;
            while (tok != NULL) {
                if (i == 1) {
                    argv[j] = tok;
                    fi = tok;
                    tok = strtok(NULL, " ");
                    j = j + 1;
                    
                } else if (strchr(tok, '>')) {
                    argv[j] = tok;
                    tok = strtok(NULL, " ");
                    i = 1;
                    j = j + 1;
                    
                } else {
                    argv[j] = tok;
                    tok = strtok(NULL, " ");
                    j = j + 1;
                }
            }
            argv[j] = NULL;
            argc = j;
        // for (int i = 0; i < argc; ++i) {
        //     write(1, argv[i], strlen(argv[i]));
        // }
            char path2[100];
            strcpy(path2, path);
            strcat(path2, argv[0]);
            for (int k = 0; k < strlen(path2); ++k) {
                if (path2[k] == '\n') {
                    path2[k] = '\0';
                }
            }   
            int pid = fork();
            if (pid == 0) {
                if (fi != NULL) {
                    //FILE *fp = fopen(fi,"w");
                    int fd = open(fi, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
                    //int fd = popen(fi);
                    dup2(fd,1);
                    dup2(fd,2);
                    close(fd);
                    execv(path2, argv);
                } else {
                    execv(path2,argv);
                }

            } else {
                wait(NULL);
            }
        }
    }
    return 0;
}
