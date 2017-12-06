#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAX_SIZE 50
#define MAX_SIZE_MESSAGE 300
#define MINUTES 60
#define HOURS (MINUTES*60)


typedef struct chat_entry{
  int points;   /* Messages will be given points and only messages above x amount will be shown */
  char timestamp[MAX_SIZE];
  char username[MAX_SIZE];
  char message[MAX_SIZE_MESSAGE];
} chat_entry;

typedef struct wordlist{
  int points; /* In the wordlist, assign each word a point value */
  char word[MAX_SIZE];
} wordlist;

typedef struct time_in_hms{
  int points;
  int hours;
  int minutes;
  int seconds;
  char username[MAX_SIZE];
  char message[MAX_SIZE_MESSAGE];
} time_in_hms;

int countAllEntries(FILE*);
void read_data_log(FILE*, int, chat_entry logs[]);
void read_wordlist(FILE*, int, wordlist words[]);
void check_for_question(int, chat_entry logs[], int, wordlist words[]);
void assign_points(int, int, chat_entry logs[], wordlist words[]);
void print_over_threshold(int, chat_entry logs[], int);
int stream_start();
void time_in_stream(int, chat_entry logs[], int);
void timestamp_to_seconds(int total_entries_log, chat_entry logs[], time_in_hms new_logs[]);
int compare_points (const void * a, const void * b);

int main(void){

    FILE *chat_log, *user_wordlist;
    chat_log = fopen("twitchlogs.txt", "r");
    user_wordlist = fopen("wordlist.txt", "r");
    int total_entries_wordlist = countAllEntries(user_wordlist);
    int total_entries_log = countAllEntries(chat_log);
    int user_threshold = 0, user_stream_start = 0;
    wordlist *words = malloc(sizeof(wordlist) * total_entries_wordlist);
    chat_entry *logs = malloc(sizeof(chat_entry) * total_entries_log);
    read_wordlist(user_wordlist, total_entries_wordlist, words);
    read_data_log(chat_log, total_entries_log, logs);
    
    user_stream_start = stream_start();
    time_in_stream(total_entries_log, logs, user_stream_start);

    /*assign_points(total_entries_log, total_entries_wordlist, logs, words);
    
    check_for_question(total_entries_log, logs, total_entries_wordlist, words);

    printf("\nEnter amount of points to show messages equal to or exceeding that value: ");
    scanf("%d",&user_threshold);
    print_over_threshold(total_entries_log,logs,user_threshold);*/

    fclose(user_wordlist);
    fclose(chat_log);
    free(words);
    free(logs);
    
    
  return 0;
}

int countAllEntries(FILE *datafile){

  int  totalLines = 1;
  char ch;

  while(!feof(datafile)){
    ch = fgetc(datafile);

    if(ch == '\n'){   /* Increment counter when encountering newline */
      ++totalLines;
    }
  }
  rewind(datafile);
  return totalLines;
}

/* Function for reading data from file to struct */
void read_data_log(FILE *chat_log, int total_entries_log, chat_entry logs[]){

  int i;
  char temp_timestamp[MAX_SIZE];
  chat_entry data = {0};
  for(i = 0; i < total_entries_log; ++i){
      fscanf(chat_log," [%[^][]] %[^:] %*[:] %[^\n]",
                                      temp_timestamp,
                                      data.username,
                                      data.message);
      sscanf(temp_timestamp," %*s %s %*s",data.timestamp); /* Scan only hours, minutes, seconds */
    logs[i] = data; /* Transfer data from structs to array of structs */
  }
}

void read_wordlist(FILE *user_wordlist, int total_entries_wordlist, wordlist words[]){

  int i;
  wordlist data = {0};
  for (i = 0; i < total_entries_wordlist; ++i){
    fscanf(user_wordlist," %[^,] %*[,] %d",data.word, &data.points);
    words[i] = data;
  }
}

void assign_points(int total_entries_log, int total_entries_wordlist, chat_entry logs[], wordlist words[]){

  int i, j;
  for (i = 0; i < total_entries_log; ++i){
    for (j = 0; j < total_entries_wordlist; ++j){
      if(strstr(logs[i].message, words[j].word) != NULL){
      logs[i].points += words[j].points;
      } 
    }
  }
}

void print_over_threshold(int total_entries_log, chat_entry logs[], int threshold){

  int i;
  chat_entry *temp = malloc(sizeof(chat_entry) * total_entries_log);
  for (i = 0; i < total_entries_log; ++i){
    temp[i] = logs[i];
  }
  qsort(temp, total_entries_log, sizeof(chat_entry), compare_points);

  for (i = 0; i < total_entries_log; ++i){
    if(temp[i].points >= threshold){
      printf("Points: %d |--| Timestamp: %s |--| Username: %-15s |--| Message: %s\n",temp[i].points, temp[i].timestamp, temp[i].username, temp[i].message);
    }
  }
}

void check_for_question(int total_entries_log, chat_entry logs[], int total_entries_wordlist, wordlist words[]){

  int i, j;
  for(i = 0; i < total_entries_log; ++i){
    for (j = 0; j < total_entries_wordlist; ++j){
      if(strstr(logs[i].message, words[j].word) != NULL){
        printf("[%s] %s: %s\n",logs[i].timestamp,logs[i].username,logs[i].message); 
        break;
      }
    }
  }
}

int stream_start(){

  char user_stream_start[MAX_SIZE], temp_h[MAX_SIZE], temp_m[MAX_SIZE], temp_s[MAX_SIZE];
  int stream_start_seconds = 0;

  printf("Enter time of stream start in format HH:MM:SS > ");
  scanf("%s",user_stream_start);
  sscanf(user_stream_start," %[^:] %*[:] %[^:] %*[:] %s",temp_h,temp_m,temp_s);
  stream_start_seconds = (atoi(temp_h)*HOURS) + (atoi(temp_m)*MINUTES) + atoi(temp_s);

  return stream_start_seconds;
}

void time_in_stream(int total_entries_log, chat_entry logs[], int user_input){

  /*char temp_h[MAX_SIZE], temp_m[MAX_SIZE], temp_s[MAX_SIZE];*/ 
  int i, time = 0, temp = 0;
  time_in_hms *new_logs = malloc(sizeof(time_in_hms) * total_entries_log);

  for (i = 0; i < total_entries_log; ++i){
    *new_logs[i].username = *logs[i].username;
    *new_logs[i].message = *logs[i].message;
  }
  timestamp_to_seconds(total_entries_log, logs, new_logs);

  for(i = 0; i < total_entries_log; ++i){
    temp = new_logs[i].seconds;
    time = temp - user_input;
    new_logs[i].hours = time / HOURS;
    time = time % HOURS;
    new_logs[i].minutes = time / MINUTES;
    time = time % MINUTES;
    new_logs[i].seconds = time;
    printf("After stream: [%d:%d:%d]\n",new_logs[i].hours,new_logs[i].minutes,new_logs[i].seconds);
  }


}

void timestamp_to_seconds(int total_entries_log, chat_entry logs[], time_in_hms new_logs[]){

  char temp_h[MAX_SIZE], temp_m[MAX_SIZE], temp_s[MAX_SIZE]; 
  int i;

  for (i = 0; i < total_entries_log; ++i){
    sscanf(logs[i].timestamp," %[^:] %*[:] %[^:] %*[:] %s",temp_h,temp_m,temp_s);
    new_logs[i].seconds = (atoi(temp_h)*HOURS) + (atoi(temp_m)*MINUTES) + atoi(temp_s);
  }
}

int compare_points (const void * a, const void * b){

    chat_entry *ia = (chat_entry *)a;
    chat_entry *ib = (chat_entry *)b;
    if(ia->points > ib->points){
      return -1;
    }
    else if (ia->points < ib->points){
      return +1;
    }
    else{ 
      return 0;
    }
}
