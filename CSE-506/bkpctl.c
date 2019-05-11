#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <asm/unistd.h>
#include <errno.h>
#include <ctype.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#define  N           10

struct query
{
    char *name;
    int version;
    int oldest;
    int youngest;
    char buff[1024];
};

int main( int argc,char * const argv[ ] )
{
    int command_flag = -1;
    int  input_file_length;
    int error = 0;
    char *file_name = "/mnt/bkpfs/";
    char  *input_file = NULL;
    int version;
    char name[100];
    name[0] = '\0';
    int i;
    int j = 0;
    int fd = -1;
    char name1[100];
    
    if(argc == 4 || argc == 3)
    {
        if(strcmp("h",argv[1]) == 0)
        {
            printf("Syntax is: ./bkpctl d|r|v <input_file> <version number>    ./bkpctl l <input_file> \n");
            exit(1);
        }
        
        if( (strcmp("l",argv[1]) == 0) && argc == 3)
        {
            input_file = argv[2];
            command_flag = 4;
        }
        else if( ( (strcmp("d",argv[1]) == 0) || (strcmp("r",argv[1]) == 0) || (strcmp("v",argv[1]) == 0) ) && argc == 4)
        {
            if(strcmp("d",argv[1]) == 0)
                command_flag = 1;
            else if(strcmp("r",argv[1]) == 0)
                command_flag = 2;
            else
                command_flag = 3;
            
            input_file = argv[2];
            version = atoi(argv[3]);
        }
        else
        {
            printf("Syntax is: ./bkpctl d|r|v <input_file> <version number>    ./bkpctl l <input_file> \n");
            exit( 1 );
        }
        
        if( strcmp( input_file,"NULL" ) == 0)
        {
            error = -1;
            printf(" invalid input file \n");
            exit(error);
        }
        
        struct query *q =  malloc(sizeof(struct query));
        
        if( q == NULL )
        {
            error = -1;
            printf(" memory allocation failed\n ");
            exit(error);
        }
        
        if(argc == 4)
            q->version = version;
        
        input_file_length = strlen( input_file );
        
        q->name = malloc( ( input_file_length + 1)*sizeof(char));
        
        if (  q->name  == NULL )
        {
            error = -1;
            printf(" memory allocation failed \n ");
            goto memory_allocation_failure;
        }
        
        strcat(name, file_name);
        strcat(name, input_file);
        q->name = name;
        
        fd = open(name, O_RDONLY);
        if (fd <= 0)
        {
            printf("Incorrect file name\n");
            errno = -EINVAL;
            goto memory_allocation_failure;
        }
        
        if(command_flag == 1)
        {
            if(ioctl(fd,1000,q) == -1)
            {
                printf("ioctl command failed \n");
                error = -1;
                goto memory_allocation_failure;
            }
            
        }
        else if(command_flag == 2)
        {
            if(ioctl(fd,1001,q) == -1)
            {
                printf("ioctl command failed \n");
                error = -1;
                goto memory_allocation_failure;
            }
            
        }
        else if(command_flag == 3)
        {
            if(ioctl(fd,1002,q) == -1)
            {
                printf("ioctl command failed \n");
                error = -1;
                goto memory_allocation_failure;
            }
            else
            {
                printf("buffer is : \n");
                printf("------------------------------------------------------\n");
                printf("%s",q->buff);
            }
        }
        else if(command_flag == 4)
        {
            if(ioctl(fd,1003,q) == -1)
            {
                printf("ioctl command failed \n");
                error = -1;
                goto memory_allocation_failure;
            }
            else
            {
                printf("Available versions are : \n");
                printf("------------------------------------------------------\n");
               // printf("oldest - %d \n",q->oldest);
               // printf("youngest - %d \n",q->youngest);
                
                for(i = q->oldest;++j<=N ; i--)
                {
                    if( i > 0)
                    {
                       sprintf( name1, "%d %s", i ,input_file);
                       printf("%s \n",name1);
                    }
                }
            }
        }
        
    memory_allocation_failure:
        
        if(q)
            free(q);
        
        exit(error);
    }
    else
    {
        printf("Syntax is: ./bkpctl d|r|v <input_file> <version number>    ./bkpctl l <input_file> \n");
        exit( 1 );
    }
    
}
