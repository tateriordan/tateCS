#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    char commandLine[512];
    char* path = "";
    if (argc > 2) {
        write(STDERR_FILENO, "Usage: mysh [batch-file]\n", strlen("Usage: mysh [batch-file]\n"));
        //fprintf(stderr,"Usage: mysh [batch-file]\n");
        //fflush();
        return 1;
    }
    if (argc == 2) {
        if (!strchr(argv[1], '.')) {
            write(STDERR_FILENO, "Error: Cannot open file this_file_does_not_exist.\n", strlen("Error: Cannot open file this_file_does_not_exist.\n"));
            //fprintf(stderr, "Error: Cannot open file this_file_does_not_exist.\n");
            //fflush();
            return 1;
        }
    }
    if (argv[1]) {
        FILE *fp = fopen(argv[1],"r");
        char buffer[512];
        while (fgets(buffer, 512, fp)) {
            if (feof(fp)) {
                fclose(fp);
                fflush(stdout);
                _exit(EXIT_SUCCESS);
                return 0;
            }
            if (buffer == NULL) {
                fclose(fp);
                fflush(stdout);
                _exit(EXIT_SUCCESS);
                return 0;
            }
            if (strcmp(buffer, "exit\n") == 0) {
                fclose(fp);
                write(STDOUT_FILENO, "exit\n", strlen("exit\n"));
                fflush(stdout);
                _exit(EXIT_SUCCESS);
                return 0;
            }
            write(STDOUT_FILENO,buffer, strlen(buffer));
            fflush(stdout);
            char *tok;
            tok = strtok(buffer, " \t");
            int j = 0;
            char *fi = NULL;
            int i = 0;
            int d = 0;
            while (tok != NULL) {
                if (j == 0 && strchr(tok, '>')) {
                    fclose(fp);
                    write(STDERR_FILENO, "Redirection misformatted.\n", strlen("Redirection misformatted.\n"));        
                    fflush(stderr);
                    write(STDOUT_FILENO, "exit\n", strlen("exit\n"));
                    fflush(stdout);
                    _exit(EXIT_SUCCESS);
                    return 0;
                } else if (i == 1) {
                    //argv[j] = tok;
                    fi = tok;
                    tok = strtok(NULL, " \t");
                    if (tok != NULL) {
                        fclose(fp);
                        write(STDERR_FILENO, "Redirection misformatted.\n", strlen("Redirection misformatted.\n"));
                        fflush(stderr);
                        fflush(stdout);
                        write(STDOUT_FILENO, "exit\n", strlen("exit\n"));
                        fflush(stdout);
                        _exit(EXIT_SUCCESS);
                        return 0;
                    }
                } else if (strchr(tok, '>')) {
                    if (d != 0) {
                        for (int r = 1; r < j; ++r) {
                            argv[r] = argv[r+1];
                        }
                        j = j - 1;
                    }
                    tok = strtok(NULL, " \t");
                    if (tok == NULL) {
                        fclose(fp);
                        write(STDERR_FILENO, "Redirection misformatted.\n", strlen("Redirection misformatted.\n"));
                        fflush(stderr);
                        write(STDOUT_FILENO, "exit\n", strlen("exit\n"));
                        fflush(stdout);
                        _exit(EXIT_SUCCESS);
                        return 0;
                    } else if (strchr(tok, '>')) {
                        fclose(fp);
                        write(STDERR_FILENO, "Redirection misformatted.\n", strlen("Redirection misformatted.\n"));
                        
                        fflush(stderr);
                        write(STDOUT_FILENO, "exit\n", strlen("exit\n"));
                        fflush(stdout);
                        _exit(EXIT_SUCCESS);
                        return 0;
                    }
                    i = 1;
                    //j = j + 1;
                    
                    
                } else if (strcmp(tok, "/bin/echo") == 0) {
                    argv[j] = tok;
                    argv[j+1] = "-n";
                    d = j + 1;
                    tok = strtok(NULL, " \t");
                    j = j + 2;
                } else {
                    argv[j] = tok;
                    tok = strtok(NULL, " \t");
                    j = j + 1;
                }
            }
            if (strcmp(argv[j-1], " \t") == 0) {
                argv[j-1] = NULL;
            }
            argv[j] = NULL;
            argc = j;
            // if (strchr(argv[argc-1], '>') != NULL) {
            //     char *toke;
            //     toke = strtok(argv[argc-1], ">");
            //     argv[argc-1] = toke;
            // }
            // if (strchr(fi, '>') != NULL) {
            //     char *token1;
            //     token1 = strtok(fi, ">");
            //     token1 = strtok(fi, " ");
            //     fi = token1;
            // }
            char path2[512];
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
                    //free(buffer);
                    _exit(EXIT_SUCCESS);
                } else {
                    execv(path2,argv);
                    fflush(stdout);
                    //free(buffer);
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
            write(STDOUT_FILENO,"mysh> ", strlen("mysh> "));
            fflush(stdin);
            if (!fgets(commandLine, 512, stdin)) {
                break;
            }
            if (strcmp(commandLine, "exit\n") == 0) {
                //write(STDOUT_FILENO, "exit\n", strlen("exit\n"));
                _exit(EXIT_SUCCESS);
                return 0;
            }
            char *tok;
            tok = strtok(commandLine, " \t");
            int j = 0;
            char *fi = NULL;
            int i = 0;
            while (tok != NULL) {
                if (i == 1) {
                    argv[j] = tok;
                    fi = tok;
                    tok = strtok(NULL, " \t");
                    j = j + 1;
                    
                } else if (strchr(tok, '>')) {
                    argv[j] = tok;
                    tok = strtok(NULL, " \t");
                    i = 1;
                    j = j + 1;
                    
                } else if (strcmp(tok, "/bin/echo") == 0) {
                    argv[j] = tok;
                    argv[j+1] = "-n";
                    tok = strtok(NULL, " \t");
                    j = j + 2;
                } else {
                    argv[j] = tok;
                    tok = strtok(NULL, " \t");
                    j = j + 1;
                }
            }
            argv[j] = NULL;
            argc = j;
        // for (int i = 0; i < argc; ++i) {
        //     write(1, argv[i], strlen(argv[i]));
        // }
            char path2[512];
            strcpy(path2, path);
            // if (strcmp(argv[0], "/bin/echo") == 0) {
            //     argv[0] = "/bin/echo -n";
            // }
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
