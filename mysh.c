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
        write(STDERR_FILENO, "Usage: mysh [batch-file]\n", 26);
        //fprintf(stderr,"Usage: mysh [batch-file]\n");
        //fflush();
        return 1;
    }
    if (argc == 2) {
        if (!strchr(argv[1], '.')) {
            write(STDERR_FILENO, "Error: Cannot open file this_file_does_not_exist.\n", 50);
            //fprintf(stderr, "Error: Cannot open file this_file_does_not_exist.\n");
            //fflush();
            return 1;
        }
    }
    if (argv[1]) {
        FILE *fp = fopen(argv[1],"r");
        char buffer[100];
        while (fgets(buffer, 100, fp) != NULL) {
            if (feof(fp)) {
                fclose(fp);
                fflush(stdout);
                _exit(EXIT_SUCCESS);
                return 0;
            }
            if (buffer == NULL) {
                break;
            }
            write(STDOUT_FILENO,buffer, 512);
            // if (strchr(buffer, '>') != NULL) {
                
            // }
            if (strcmp(buffer, "exit\n") == 0) {
                fclose(fp);
                write(STDOUT_FILENO, "exit\n", 6);
                fflush(stdout);
                _exit(EXIT_SUCCESS);
                return 0;
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
                    if (tok != NULL) {
                        write(STDERR_FILENO, "Redirection misformatted.\n", 30);
                        fflush(stdout);
                        //fprintf(stderr, "Redirection misformatted.\n");
                        return 0;
                    }
                    j = j + 1;
                    
                } else if (strchr(tok, '>')) {
                    argv[j] = tok;
                    tok = strtok(NULL, " ");
                    if (tok == NULL) {
                        write(STDERR_FILENO, "Redirection misformatted.\n", 30);
                        //fprintf(stderr, "Redirection misformatted.\n");
                        fflush(stdout);
                        return 0;
                    } else if (strchr(tok, '>')) {
                        write(STDERR_FILENO, "Redirection misformatted.\n", 30);
                        fflush(stdout);
                        //fprintf(stderr, "Redirection misformatted.\n");
                        return 0;
                    }
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
                    execv(path2,argv);
                    fflush(stdout);
                    _exit(EXIT_SUCCESS);
                } else {
                    execv(path2,argv);
                    fflush(stdout);
                    _exit(EXIT_SUCCESS);
                }

            } else {
                wait(NULL);
            }
        
        }
        fclose(fp);
        fflush(stdout);
        _exit(EXIT_SUCCESS);
    } else {
        int helper = 1;
        while (helper) {
            write(STDOUT_FILENO,"mysh> ", 6);
            if (!fgets(commandLine, 512, stdin)) {
                break;
            }
            if (strcmp(commandLine, "exit\n") == 0) {
                write(STDOUT_FILENO, "exit\n", 6);
                _exit(EXIT_SUCCESS);
                return 0;
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
                    
                    _exit(EXIT_SUCCESS);
                } else {
                    execv(path2,argv);
                    _exit(EXIT_SUCCESS);
                }

            } else {
                wait(NULL);
            }
        }
        _exit(EXIT_SUCCESS);
        return 0;
    }
    return 0;
}
