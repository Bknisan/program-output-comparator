// Nisan Agabyev
// 322002080
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <memory.h>
#include <sys/wait.h>
#include <stdlib.h>

int checkProgram(char *dir, char *firstDir, char *input, char *output, int tmpFile, char *studentName) {
    DIR *dirp;
    int status = 0;
    int isThereCFile = 0;
// open the dir where the *.c should be.
    struct dirent *ent;
    // open current dir.
    if ((dirp = opendir(dir)) != NULL) {
        // while there are files in the dir read them.
        while ((ent = readdir(dirp)) != NULL) {
            if ((ent->d_type == DT_DIR) && strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                char newdir[250] = {0};
                strcpy(newdir, dir);
                strcat(newdir, "/");
                strcat(newdir, ent->d_name);
                if (studentName == NULL) {
                    checkProgram(newdir, firstDir, input, output, tmpFile, ent->d_name);
                } else {
                    checkProgram(newdir, firstDir, input, output, tmpFile, studentName);
                }
            }
                // program found.
            else if (ent->d_name[strlen(ent->d_name) - 1] == 'c' && ent->d_name[strlen(ent->d_name) - 2] == '.') {
                isThereCFile = 1;
                strcat(dir, "/");
                strcat(dir, ent->d_name);
                //program arguments.
                char *argv[] = {"gcc", dir, NULL};
                //son process
                pid_t son;
                if ((son = fork()) == 0) {
                    execvp("gcc", argv);
                    exit(-1);
                }
                wait(&status);
                //COMP ERROR
                if ((status != 0) && (status != 9)) {
                    char line[250] = {0};
                    strcpy(line, studentName);
                    strcat(line, ",20,COMPILATION_ERROR\n");
                    write(tmpFile,line,strlen(line));
                    //fprintf(res, "%s", line);
                    closedir(dirp);
                    return 0;
                }
                    //SUCCESSFUL COMPILE
                else {
                    pid_t sonEx;
                    int statusPro = 0;
                    int fdout = open("realOut.txt", O_CREAT | O_RDWR, 0666);
                    int fdin = open(input, O_RDONLY);
                    if ((sonEx = fork()) == 0) {
                        // make output go to my file.
                        dup2(fdout, STDOUT_FILENO);
                        // take input from my file.
                        dup2(fdin, STDIN_FILENO);
                        // execute program.
                        char *arguments[] = {"./a.out", NULL};
                        execvp("./a.out", arguments);
                    }
                    // check TIME_OUT
                    sleep(5);
                    close(fdin);
                    close(fdout);
                    // program takes too much time.
                    if (waitpid(sonEx, &statusPro, WNOHANG) == 0) {
                        // kill running process.
                        char line[250] = {0};
                        strcpy(line, studentName);
                        strcat(line, ",40,TIMEOUT\n");
                        write(tmpFile,line,strlen(line));
                        //fprintf(res, "%s", line);
                        unlink("a.out");
                        unlink("realOut.txt");
                        closedir(dirp);
                        return 0;
                    }
                        // check if outputs are similar.
                    else {
                        int statusRet;
                        //son process.
                        pid_t sonProc;
                        // run compiled program.
                        char path[250] = {0};
                        getcwd(path, 150);
                        strcat(path, "/realOut.txt");
                        char *comp[] = {"./comp", output, path, NULL};
                        if ((sonProc = fork()) == 0) {
                            execvp("./comp", comp);
                        }
                        // get return value.
                        wait(&statusRet);
                        int val = WEXITSTATUS(statusRet);
                        // identical output.
                        if (val == 1) {
                            char line[250] = {0};
                            strcpy(line, studentName);
                            strcat(line, ",100,GREAT_JOB\n");
                            write(tmpFile,line,strlen(line));
                            //fprintf(res, "%s", line);
                            unlink("a.out");
                            unlink("realOut.txt");
                            closedir(dirp);
                            return 0;
                        }
                            // not fully identical but same content.
                        else if (val == 3) {
                            char line[250] = {0};
                            strcpy(line, studentName);
                            strcat(line, ",80,SIMILAR_OUTPUT\n");
                            write(tmpFile,line,strlen(line));
                           // fprintf(res, "%s", line);
                            unlink("a.out");
                            unlink("realOut.txt");
                            closedir(dirp);
                            return 0;
                        }
                            // different output
                        else if (val == 2) {
                            char line[250] = {0};
                            strcpy(line, studentName);
                            strcat(line, ",60,BAD_OUTPUT\n");
                            write(tmpFile,line,strlen(line));
                           // fprintf(res, "%s", line);
                            unlink("a.out");
                            unlink("realOut.txt");
                            closedir(dirp);
                            return 0;
                        }
                    }
                }
            }
        }
        // no file found
        char fullPath[250] = {0};
        strcpy(fullPath, firstDir);
        strcat(fullPath, "/");
        // check if its not main folder.
        if (studentName != NULL)
            strcat(fullPath, studentName);
        if ((!isThereCFile) && (strcmp(fullPath, dir) == 0)) {
            char line[250] = {0};
            strcpy(line, studentName);
            strcat(line, ",0,NO_C_FILE\n");
            write(tmpFile,line,strlen(line));
           // fprintf(res, "%s", line);
            closedir(dirp);
            return 0;
        }
        closedir(dirp);
        return 0;
    }
    return 0;
}

int main(int argc, char **argv) {
    // open cong file.
    int fd1 = open(argv[1], O_RDONLY);
    char dir[150] = {0};
    char input[150] = {0};
    char output[150] = {0};
    int i = 0;
    int reader;
    do {
        reader = (int) read(fd1, (dir + i), 1);
        i++;
        if (dir[i - 1] == '\n') {
            dir[i - 1] = '\0';
            i = 0;
            break;
        }
    } while (reader != 0);
    do {
        reader = (int) read(fd1, (input + i), 1);
        i++;
        if (input[i - 1] == '\n') {
            input[i - 1] = '\0';
            i = 0;
            break;
        }
    } while (reader != 0);
    do {
        reader = (int) read(fd1, (output + i), 1);
        i++;
        if (output[i - 1] == '\n') {
            output[i - 1] = '\0';
            break;
        }
    } while (reader != 0);
    // try to run the program.
    int tmpFd = open("tmp.csv",  O_CREAT | O_RDWR, 0666);
    checkProgram(dir, dir, input, output, tmpFd, NULL);
    unlink("realOut.txt");
    lseek(tmpFd,0,SEEK_SET);
    // create my file.
    int fileDesOriginal = open("results.csv", O_CREAT | O_RDWR, 0666);
    char tmp;
    char oneAfterNewLine;
    // indicator.
    int bool = 0;
    // read char by char until last '\n'
    int Reader = read(tmpFd, &tmp, 1);
    while (Reader != 0) {
        // if its newline char, check the 1 after it.
        if (tmp == '\n') {
            Reader = read(tmpFd, &oneAfterNewLine, 1);
            // still more data.
            if (Reader != 0) {
                bool = 1;
                write(fileDesOriginal, &tmp, 1);
                write(fileDesOriginal, &oneAfterNewLine, 1);
            }
                // EOF is reached.
            else {
                break;
            }
        }
        if (Reader == 0)
            break;
        // not newline char.
        if(bool == 0) {
            write(fileDesOriginal, &tmp, 1);
        }
        bool = 0;
        Reader = read(tmpFd,&tmp,1);
    }
    close(tmpFd);
    close(fileDesOriginal);
    // delete tmp files.
    unlink("tmp.csv");
    unlink("a.out");
    return 0;
}