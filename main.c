#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/time.h>


int exit_code = 7777;

int main(int argc, char* argv[])
{
    int fd1[2]; //read write
    int fd2[2];
    srand(time(NULL));
    if(pipe(fd1) == -1)
    {
        printf("Error ocured while creating pipe1\n");
        return 1;
    }

    if(pipe(fd2) == -1)
    {
        printf("Error ocured while creating pipe1\n");
        return 1;
    }

    //deljenje procesa
    /*-------------------------------------------------------*/
    int pid1= fork();

    if(pid1 != 0)
    {
        //main process
        int broj[20];
        int sum = 0;
        for(int i = 0; i < 20; i++)
        {
            broj[i] = rand() % 100;
            sum+=broj[i];
        }

        printf("Suma je: %d\n", sum);
        if(sum % 2 != 0)
        {
            //neparna suma
            close(fd1[0]);
            close(fd2[0]);

            if(write(fd2[1], &exit_code, sizeof(exit_code)) == -1)
            {
                printf("Error ocured while writing to buffer2\n");
                return 0;
            }   

            
            if(write(fd1[1],broj, sizeof(broj)) == -1)
            {
                printf("Error ocurred while writing to first pipe!\n");
                return 0;
            }
            close(fd1[1]);
            close(fd2[1]);

        }
        else
        {
            //parna suma
            close(fd1[0]);
            close(fd2[0]);

            if(write(fd1[1], &exit_code, sizeof(exit_code)) == -1)
            {
                printf("Error ocured while writing to second buffer\n");
                return 0;
            }   

            if(write(fd2[1], broj, sizeof(broj)) == -1)
            {
                printf("error ocured while writing to third buffer!\n");
                return 0;
            }
            close(fd1[1]);
            
            
        }
        wait(NULL);
        /*------------------------------------------------------*/
    }
    else
    {   
        //dete proces
        int pid2 = fork();

        if(pid2 != 0)
        {
            wait(NULL);
            close(fd1[1]);
            close(fd2[0]);
            close(fd2[1]);

            int broj[20];
            int flag;

            for(int i = 0; i < 20; i++)
            {
                read(fd1[0],&broj[i], sizeof(broj[i]));
                if(i == 0 && broj[i] == exit_code)
                {
                    printf("Proces 2 se zavrsava!\n");
                    return 0;
                }
                else
                {
                    printf("Proces 2 iz pipe dobio: %d\n", broj[i]);
                }
            }
            close(fd1[0]);
        }
        else
        {
            //dete proces deteta procesa
            close(fd1[0]);
            close(fd1[1]);
            close(fd2[1]);
            int broj[20];
            FILE* f;
            f = fopen("BROJEVI.txt", "w");
            for(int i = 0; i < 20; i++)
            {
                read(fd2[0],&broj[i], sizeof(broj[i]));
                if(i == 0 && broj[i] == exit_code)
                {
                    printf("Proces 3 se zavrsava!\n");
                    return 0;
                }
                else
                {
                    printf("Proces 3 iz pipe dobio: %d\n", broj[i]);
                    fprintf(f,"%d\n",broj[i]);
                    
                }
            }
            close(fd2[0]);
            fclose(f);
            

        }
    }
    return 0;
}