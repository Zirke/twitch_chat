#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 50
#define MAX_SIZE_MESSAGE 300

typedef struct chat_entry{
    char timestamp[MAX_SIZE];
    char username[MAX_SIZE];
    char message[MAX_SIZE_MESSAGE];
} chat_entry;

int countAllEntries();
void read_data(FILE*, int, chat_entry logs[]);
void check_for_question(int, chat_entry logs[]);

int main(void){

    FILE *chat_log = fopen("twitchlogs.txt", "r");

    int total_entries = countAllEntries();
    chat_entry logs[total_entries];
    read_data(chat_log,total_entries,logs);
    check_for_question(total_entries,logs);

    fclose(chat_log);

  return 0;
}

int countAllEntries(){

  int  totalLines = 1;
  char ch;
  FILE *chat_log = fopen("twitchlogs.txt", "r");

  while(!feof(chat_log)){
    ch = fgetc(chat_log);

    if(ch == '\n'){   /* Increment counter when encountering newline */
      ++totalLines;
    }
  }
  fclose(chat_log);
  return totalLines;
}

/* Function for reading data from file to struct */
void read_data(FILE *chat_log, int total_entries, chat_entry logs[total_entries]){

  int i;
  char temp_timestamp[MAX_SIZE];
  chat_entry data;
  for(i = 0; i < total_entries; ++i)
  {
      fscanf(chat_log," [%[^][]] %[^:] %*[:] %[^\n]",
            temp_timestamp,
            data.username,
            data.message);
      sscanf(temp_timestamp," %*s %s %*s",data.timestamp); /* Scan only hours, minutes, seconds */
      //printf("Timestamp: %s |--| Username: %25s |--| Message: %s\n",data.timestamp,data.username,data.message);
    logs[i] = data; /* Transfer data from structs to array of structs */
  }
}

void check_for_question(int total_entries, chat_entry logs[total_entries]){

  int i;
  for(i = 0; i < total_entries; ++i){
    if(strstr(logs[i].message, "?") != NULL){

      printf("[%s] %s: %s\n",logs[i].timestamp,logs[i].username,logs[i].message);
    }
    else if(strstr(logs[i].message, "What") != NULL || strstr(logs[i].message, "what") != NULL){

      printf("[%s] %s: %s\n",logs[i].timestamp,logs[i].username,logs[i].message);
    }
    else if(strstr(logs[i].message, "When") != NULL || strstr(logs[i].message, "when") != NULL){

      printf("[%s] %s: %s\n",logs[i].timestamp,logs[i].username,logs[i].message);
    }
    else if(strstr(logs[i].message, "Why") != NULL || strstr(logs[i].message, "why") != NULL){

      printf("[%s] %s: %s\n",logs[i].timestamp,logs[i].username,logs[i].message);
    }
    else if(strstr(logs[i].message, "Which") != NULL || strstr(logs[i].message, "which") != NULL){

      printf("[%s] %s: %s\n",logs[i].timestamp,logs[i].username,logs[i].message);
    }
    else if(strstr(logs[i].message, "Where") != NULL || strstr(logs[i].message, "where") != NULL){

      printf("[%s] %s: %s\n",logs[i].timestamp,logs[i].username,logs[i].message);
    }
    else if(strstr(logs[i].message, "Who") != NULL || strstr(logs[i].message, "who") != NULL){

      printf("[%s] %s: %s\n",logs[i].timestamp,logs[i].username,logs[i].message);
    }
    else if(strstr(logs[i].message, "How") != NULL || strstr(logs[i].message, "how") != NULL){

      printf("[%s] %s: %s\n",logs[i].timestamp,logs[i].username,logs[i].message);
    }
  }
}
