#include<stdio.h>
#include<sys/types.h>//for datatypes in syscalls
#include<sys/stat.h>//fstat,lstat etc
#include <fcntl.h>//for fnctl,open
#include <unistd.h>//for read,write 
#include<string.h>//for strlen
int main(){
char* text1 = "Hello World";
char* text2 = "Hello World 2"; 
//char buffer[50] ="patle goat"
int fd_file1 = open("file1.txt",O_RDWR|O_CREAT,S_IRUSR|S_IWUSR);
int fd_file2 = open("file2.txt",O_WRONLY|O_CREAT,S_IRUSR|S_IWUSR);
if (fd_file1==-1 || fd_file2 == -1)
{
printf("error hogaya bhai");
return 1;
}
//write(fd_file1,buffer,sizeof(buffer)-1); -1 for null terminator value 
ssize_t byte1 =write(fd_file1,text1,strlen(text1));
ssize_t byte2 = write(fd_file2,text2,strlen(text2));//ssize  
printf("the bytes written are %zd\n",byte1);
printf("the bytes written are %zd\n",byte2);
char buffer[50];
lseek(fd_file1,0,SEEK_SET);
read(fd_file1,buffer,sizeof(buffer)-1);
buffer[50] = '\0';
printf("%s",buffer);//The %s tells printf to keep printing until it hits \0. If read() didn’t write a \0, it could print:

}
