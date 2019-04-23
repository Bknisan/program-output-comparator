//Nisan Agabyev
//322002080
#include <stdio.h>
#include <syscall.h>
#include<errno.h>
#include <fcntl.h>
#include <unistd.h>

// checks if a char similar to b char no matter the case.
int similarChar(char a, char b) {
    if (a == b) {
        return 1;
// check if a is upper case
    } else if (((64 < a) && (a < 91)) && ((a + 32) == b)) {
        return 2;
        // check if a is lower case
    } else if (((96 < a) && (a < 123)) && ((a - 32) == b)) {
        return 2;
    }
    return 0;
}

// checks if current char is white char.
int whiteChar(char a) {
    return ((a == ' ') || (a == '\t') || (a == '\n'));
}

int main(int argc, char **paths) {
    // current similarity of the 2 files.
    int similarity = 1;
    char firstF;
    char secondF;
    int first = 0;
    int second = 0;
    char *firstFile = paths[1];
    char *secondFile = paths[2];
    int fd1 = open(firstFile, O_RDONLY);
    int fd2 = open(secondFile, O_RDONLY);
    // check if opened successfully
    if ((fd1 < 0) || (fd2 < 0)) {
        fprintf(stderr, "Error in system call\n");
    }
    do {
        first = (int) read(fd1, &firstF, 1);
        second = (int) read(fd2, &secondF, 1);
        // skip all white chars.
        while (whiteChar(firstF) && (first != 0)) {
            first = (int) read(fd1, &firstF, 1);
        }
        //skip all white chars.
        while (whiteChar(secondF) && (second != 0)) {
            second = (int) read(fd2, &secondF, 1);
        }
        if ((first == 0) && (second == 0))
            break;
        if (similarChar(firstF, secondF) == 0) {
            close(fd1);
            close(fd2);
            return 2;
        } else if (similarChar(firstF, secondF) == 2) {
            similarity = 3;
        }
    } while ((first != 0) && (second != 0));
    close(fd1);
    close(fd2);
    return similarity;
}