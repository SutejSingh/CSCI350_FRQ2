#include "types.h"
#include "stat.h"
#include "user.h"
#include "condvar.h"
#include "fcntl.h"

int main(){
    struct condvar cv;
    int fd = open("flag", O_RDWR | O_CREATE);
    init_lock(&cv.lk);

    int pid = fork();

    if(pid < 0){
        printf(1, "Error forking first child\n");
    }
    else if(pid == 0){
        printf(1, "Child 1 Executing\n");
        sleep(5);
        lock(&cv.lk);
        cv_signal(&cv);
        unlock(&cv.lk);
    }
    else{
        pid = fork();
        if(pid < 0){
            printf(1, "Error forking second child\n");
        }
        else if(pid == 0){
            lock(&cv.lk);
            struct stat stats;
            fstat(fd, &stats);
            printf(1, "file size = %d\n", stats.size);
            while(stats.size <= 0){
                cv_wait(&cv);
                fstat(fd, &stats);
                printf(1, "file size = %d\n", stats.size);
            }
            cv_wait(&cv);
            unlock(&cv.lk);
            printf(1, "Child 2 Executing\n");
        }
        else{
            printf(1, "Parent Executing\n");
            int i;
            for(i = 0; i < 2; i++)
                wait();
            printf(1, "Children completed\n"); 
            printf(1, "Parent Executing\n"); 
            printf(1, "Parent Exiting.\n"); 
        }
    }
    close(fd);
    unlink("flag");
    exit();
}