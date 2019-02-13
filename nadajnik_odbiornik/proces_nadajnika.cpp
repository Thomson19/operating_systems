#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>


#define DATA_SIZE 128

typedef struct Data {
	char data[DATA_SIZE]; 
	int data_size; 
	int isEOF; 
} Data;


int main()
{
    char fname[50];
    Data d ={"", 0,0};
    Data *buff;
	FILE *f;
	HANDLE hWriteEvent, hReadEvent, hMapFile;

    printf("Podaj nazwe pliku wejsciowego: ");
	scanf("%s", fname);
	getchar();
    int len = strlen(fname);
    if(len < 4 ||  *(fname+len-1)!='t' || *(fname+len-2)!='x' || *(fname+len-3)!='t' ||*(fname+len-4)!='.')
    {
		
        if(len < 4 ||  *(fname+len-1)!='n' || *(fname+len-2)!='i' || *(fname+len-3)!='b' || *(fname+len-4)!='.')
        { 
           if(len < 4 ||  *(fname+len-1)!='e' || *(fname+len-2)!='x' || *(fname+len-3)!='e' || *(fname+len-4)!='.')
        	{
            	printf("wrong filename");
            	return 1;
        	}
        }   
    }



    f=fopen(fname, "rb");
    if(f==NULL)
    {
        printf("nie udalo sie otworzyc pliku");
        return 1;
    }

    hWriteEvent = CreateEvent(NULL, FALSE, 1, "write event");
	if (hWriteEvent == NULL) 
    {
		printf("CreateEvent error");
		fclose(f);
		return 2;
	}

    hReadEvent = CreateEvent(NULL, FALSE, FALSE, "read event");
    if (hWriteEvent == NULL) 
    {
		printf("CreateEvent error");
		CloseHandle(hWriteEvent);
		fclose(f);
		return 2;
	}

    hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(Data), "dane");
    if (hMapFile == NULL) 
    {
		printf("CreateFileMapping error");
        CloseHandle(hWriteEvent);
        CloseHandle(hReadEvent);
		fclose(f);
		return 3;
	}

    buff = (Data*)MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(Data));
	if(buff==NULL)
	{
		printf("map view of file error");
		CloseHandle(hWriteEvent);
        CloseHandle(hReadEvent);
		fclose(f);
		return 4;
	}
    while (!feof(f)) {
    	
        d.data_size = fread(&d.data, sizeof(char), DATA_SIZE, f);
    
		if (d.data_size < DATA_SIZE) {
			d.isEOF = 1;
		}

		WaitForSingleObject(hWriteEvent, INFINITE);
		ResetEvent(hWriteEvent);

		CopyMemory(buff, &d, sizeof(Data));

		SetEvent(hReadEvent);

		if (d.isEOF) break;

	}

	CloseHandle(hReadEvent);
	CloseHandle(hWriteEvent);
	CloseHandle(hMapFile);
	fclose(f);
	
	printf("Gotowy\n");
    printf("wcisnij dowolny klawisz");
	getchar();

    return 0;
}
