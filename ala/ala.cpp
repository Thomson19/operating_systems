#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>

HANDLE events[10];
int thread_counter=0;

typedef struct data
{
    char word[50];
    int id;
}data;

DWORD WINAPI print_word(LPVOID lpParameter)
{
    data *d = (data*)lpParameter;

    while(1)
    {
        DWORD wait_for_event = WaitForSingleObject(events[d->id],INFINITE);
        printf("%s ", d->word);
        if(d->id == thread_counter-1)
            printf("\n");
        ResetEvent(events[d->id]);
        SetEvent(events[(d->id + 1) % thread_counter ]);
        Sleep(100);
    }
}

int getword(char dst[])
{
    int i=0;
    char c;
    while((c=getchar())!=' ')
    {
        if(c=='\n') 
        {
            dst[i]='\0';
            return -1;
        }
        dst[i]=c;
        i++;
    }
    dst[i]='\0';
    return i;
}

int main()
{
    HANDLE threads[10];
    data d[10];
    int error=0;

    printf("enter a text: ");
    while(error!=-1)
    {
        error = getword(d[thread_counter].word);
        if(!error) break;
        d[thread_counter].id=thread_counter;
        if(thread_counter==0)
            events[thread_counter] = CreateEvent(NULL, FALSE, 1, NULL);
        else
            events[thread_counter] = CreateEvent(NULL,FALSE, FALSE, NULL);
        
        threads[thread_counter] = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)print_word, &d[thread_counter], CREATE_SUSPENDED, NULL);
        thread_counter++;
    }

    for(int i=0; i<thread_counter; i++)
        ResumeThread(threads[i]);
    
    WaitForMultipleObjects(thread_counter, threads,TRUE, INFINITE);

    for(thread_counter-=1; thread_counter >= 0; thread_counter--)
        CloseHandle(threads[thread_counter]);
    
    CloseHandle(events);

    return 0;
}

