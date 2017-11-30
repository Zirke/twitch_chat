#include <stdio.h>

#define MAX_SIZE 160

int countAllEntries();
void read_data(FILE*, int, chat_entry logs[])

typedef struct chat_entry{
    char timestamp[MAX_SIZE];
    char username[MAX_SIZE];
    char message[MAX_SIZE];
} chat_entry;

int main(void){

    FILE *chat_log = fopen("twitchlogs.txt", "r");

    int total_entries = countAllEntries();
    chat_entry logs[total_entries];
    read_data(total_entries,logs);

    fclose(chat_log);

  return 0;
}

int countAllEntries(){

  int  totalLines = 1;
  char ch;
  FILE *chat_log = fopen("twitchlogs.txt", "r");


  while(!feof(chat_log))
  {
    ch = fgetc(chat_log);

    if(ch == '\n') /* Increment counter when encountering newline */
    {
      ++totalLines;
    }
  }

  fclose(chat_log);
  return totalLines;
}

/* Function for reading data from file to struct */
void read_data(FILE *chat_log, int total_entires, chat_entry logs[total_entires]){

  int i;
  chat_entry data;
  for(i = 0; i < total_riders; ++i)
  {
      fscanf(results,"%s %s %s",
            data.timestamp,
            data.username,
            data.message);
    logs[i] = data; /* Transfer data from structs to array of structs */
  }
}
