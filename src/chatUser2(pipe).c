// C program to implement one side of FIFO
// This side reads first, then reads
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
 
int main()
{           printf("User2\n");

    int fd1;
 
    // FIFO file path
    char * myfifo = "/tmp/myfifo";
 
    // Creating the named file(FIFO)
    // mkfifo(<pathname>,<permission>)
    mkfifo(myfifo, 0666);
 
    char str1[80], str2[80];

    while (1)
    {
        // First open in read only and read
        fd1 = open(myfifo,O_RDONLY);
        read(fd1, str1, 80);
 
        printf("User1: %s\n", str1);
        close(fd1);
 
        // Now open in write mode and write
        // string taken from user.
        fd1 = open(myfifo,O_WRONLY);
        fgets(str2, 80, stdin);
        if(str2[0]=='e' && str2[1]=='x'&& str2[2]=='i' && str2[3]=='t')
            return 1;
        write(fd1, str2, strlen(str2)+1);
        close(fd1);
    }
    return 0;
}
