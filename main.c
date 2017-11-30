#include <stdio.h>

#define MAX_SIZE 200

typedef struct chat_entry{
    char timestamp[MAX_SIZE];
    char username[MAX_SIZE];
    char message[MAX_SIZE];
} chat_entry;

int countAllEntries();
void read_data(FILE*, int, chat_entry logs[]);

int main(void){

    FILE *chat_log = fopen("twitchlogs.txt", "r");

    int total_entries = countAllEntries();
    chat_entry logs[total_entries];
    read_data(chat_log,total_entries,logs);

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
void read_data(FILE *chat_log, int total_entries, chat_entry logs[total_entries]){

  int i;
  chat_entry data;
  for(i = 0; i < total_entries; ++i)
  {
      fscanf(chat_log,"[%[^][]] %[^:] %[*^A-Za-z] %[A-Z - a-z]",
            data.timestamp,
            data.username,
            data.message);
      printf("T: %s U: %s M: %s\n",data.timestamp,data.username,data.message);
    logs[i] = data; /* Transfer data from structs to array of structs */
  }
}
