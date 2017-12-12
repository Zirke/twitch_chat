#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 50
#define MAX_SIZE_MESSAGE 300
#define MINUTES 60
#define HOURS (MINUTES*60)


typedef struct chatlog{
  int points;   /* Messages will be given points and only messages above x amount will be shown */
  char timestamp[MAX_SIZE];
  char username[MAX_SIZE];
  char message[MAX_SIZE_MESSAGE];
} chatlog;

typedef struct wordlist{
  int points; /* In the wordlist, assign each word a point value */
  char word[MAX_SIZE];
} wordlist;

typedef struct time{
  int points;
  int hours;
  int minutes;
  int seconds;
  char username[MAX_SIZE];
  char message[MAX_SIZE_MESSAGE];
} time;

/* Read and count functions */
int count_all_entries(FILE*);
void read_data_log(FILE*, int, chatlog logs[]);
void read_wordlist(FILE*, int, wordlist words[]);
int stream_start(int*);
/*Filtration and Print functions*/
void whitelist(int, chatlog logs[], int, wordlist words[], time logs_hms[], int, int);
void assign_points(int, int, chatlog logs[], wordlist words[]);
void print_over_threshold(int, chatlog logs[], int, time logs_hms[], int, int);
void time_user_navigation(int, chatlog logs[], int, wordlist words[], time logs_hms[]);
void time_in_stream(int, chatlog logs[], time logs_hms[], int);
/* Calc functions */
void timestamp_to_seconds(int total_entries_log, chatlog logs[], time logs_hms[]);
/* Navigation Functions */
void threshold_user_navigation(int, chatlog logs[], int, wordlist word[], time logs_hms[]);
/* Qsort compare functions */
int compare_points (const void * a, const void * b);
int compare_points_hms (const void * a, const void * b);

int main(void){

  FILE *user_log, *user_wordlist;
  user_log = fopen("twitchlogs.txt", "r");
  user_wordlist = fopen("wordlist.txt", "r");

  int total_entries_wordlist = count_all_entries(user_wordlist);
  int total_entries_log = count_all_entries(user_log);
  int user_threshold = 0, user_navigation = 0;

  wordlist *words = malloc(sizeof(wordlist) * total_entries_wordlist);
  chatlog *logs = malloc(sizeof(chatlog) * total_entries_log);
  time *logs_hms = malloc(sizeof(time) * total_entries_log);

  read_wordlist(user_wordlist, total_entries_wordlist, words);
  read_data_log(user_log, total_entries_log, logs);

  assign_points(total_entries_log, total_entries_wordlist, logs, words);
  
  
  scanf(" %d",&user_navigation);
  switch(user_navigation){
    case(1): time_user_navigation(total_entries_log, logs, total_entries_wordlist, words, logs_hms); break;
    case(2): threshold_user_navigation(total_entries_log, logs, total_entries_wordlist, words, logs_hms); break;
  }



  
    
  /*whitelist(total_entries_log, logs, total_entries_wordlist, words);

  */

  fclose(user_wordlist);
  fclose(user_log);
  free(words);
  free(logs);
  free(logs_hms);
    
    
  return 0;
}

int count_all_entries(FILE *datafile){

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
void read_data_log(FILE *user_log, int total_entries_log, chatlog logs[]){

  int i;
  char temp_timestamp[MAX_SIZE];
  chatlog data = {0};
  for(i = 0; i < total_entries_log; ++i){
      fscanf(user_log," [%[^][]] %[^:] %*[:] %[^\n]",
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

void assign_points(int total_entries_log, int total_entries_wordlist, chatlog logs[], wordlist words[]){

  int i, j;
  for (i = 0; i < total_entries_log; ++i){
    for (j = 0; j < total_entries_wordlist; ++j){
      if(strstr(logs[i].message, words[j].word) != NULL){
      logs[i].points += words[j].points;
      } 
    }
  }
}

void print_over_threshold(int total_entries_log, chatlog logs[], int threshold, time logs_hms[], int input, int show_before){

  int i;
  chatlog *temp = malloc(sizeof(chatlog) * total_entries_log);
  time *temp_hms = malloc(sizeof(time) * total_entries_log);
  if (input == 1){
    for (i = 0; i < total_entries_log; ++i){
      temp[i] = logs[i];
    }
    qsort(temp, total_entries_log, sizeof(chatlog), compare_points);
  }
  else if (input == 2){
    for (i = 0; i < total_entries_log; ++i){
      temp_hms[i] = logs_hms[i];
    }
    qsort(temp_hms, total_entries_log, sizeof(time), compare_points_hms);
  }
  
  if (input == 1){
  for (i = 0; i < total_entries_log; ++i){
    if(temp[i].points >= threshold){
      printf("Points: %d |--| Timestamp: %s |--| Username: %-15s |--| Message: %s\n",temp[i].points, temp[i].timestamp, temp[i].username, temp[i].message);
    }
  }
}
  if (input == 2 && show_before == 0){
    for (i = 0; i < total_entries_log; ++i){
      if (temp_hms[i].points >= threshold && (temp_hms[i].hours >= 0 && temp_hms[i].minutes >= 0 && temp_hms[i].seconds >= 0)){
        printf("Points: %d |--| Timestamp: %d:%d:%d |--| Username: %-15s |--| Message: %s\n",temp_hms[i].points, temp_hms[i].hours, temp_hms[i].minutes, temp_hms[i].seconds, temp_hms[i].username, temp_hms[i].message);
      }
    }
  }
  else if (input == 2 && show_before == 1){
    for (i = 0; i < total_entries_log; ++i){
      if (temp_hms[i].points >= threshold){
        printf("Points: %d |--| Timestamp: %d:%d:%d |--| Username: %-15s |--| Message: %s\n",temp_hms[i].points, temp_hms[i].hours, temp_hms[i].minutes, temp_hms[i].seconds, temp_hms[i].username, temp_hms[i].message);
      }
    }
  }
}

void whitelist(int total_entries_log, chatlog logs[], int total_entries_wordlist, wordlist words[], time logs_hms[], int input, int show_before){

  int i, j;
  if(input == 1){
    for(i = 0; i < total_entries_log; ++i){
      for (j = 0; j < total_entries_wordlist; ++j){
        if(strstr(logs[i].message, words[j].word) != NULL){
          printf("[%s] %s: %s\n",logs[i].timestamp,logs[i].username,logs[i].message); 
          break;
        }
      }
    }
  }
  else if(input == 2 && show_before == 1){
    for(i = 0; i < total_entries_log; ++i){
      for (j = 0; j < total_entries_wordlist; ++j){
        if(strstr(logs_hms[i].message, words[j].word) != NULL){
          printf("[%d:%d:%d] %s: %s\n",logs_hms[i].hours,logs_hms[i].minutes,logs_hms[i].seconds,logs_hms[i].username,logs_hms[i].message); 
          break;
        }
      }
    }
  }
  else if(input == 2 && show_before == 0){
    for(i = 0; i < total_entries_log; ++i){
      for (j = 0; j < total_entries_wordlist; ++j){
        if(strstr(logs_hms[i].message, words[j].word) != NULL && (logs_hms[i].hours >= 0 && logs_hms[i].minutes >= 0 && logs_hms[i].seconds >= 0)){
          printf("[%d:%d:%d] %s: %s\n",logs_hms[i].hours,logs_hms[i].minutes,logs_hms[i].seconds,logs_hms[i].username,logs_hms[i].message); 
          break;
        }
      }
    }
  }
  else if(show_before != 1 && show_before != 0){
    printf("Incorrect input, please try again.\n\n");
    time_user_navigation(total_entries_log, logs, total_entries_wordlist, words, logs_hms);
  }
}

int stream_start(int* show_before){

  char user_stream_start[MAX_SIZE], temp_h[MAX_SIZE], temp_m[MAX_SIZE], temp_s[MAX_SIZE];
  int stream_start_seconds = 0, temp = 0;

  printf("Enter time of stream start in format HH:MM:SS > ");
  scanf(" %s",user_stream_start);
  sscanf(user_stream_start," %[^:] %*[:] %[^:] %*[:] %s",temp_h,temp_m,temp_s);
  stream_start_seconds = (atoi(temp_h)*HOURS) + (atoi(temp_m)*MINUTES) + atoi(temp_s);
  printf("\nWould you like to show messages before the stream started?\n");
  printf("Enter 1 for yes or 0 for no > ");
  scanf(" %d",&temp);
  *show_before = temp;

  return stream_start_seconds;
}

void time_user_navigation(int total_entries_log, chatlog logs[], int total_entries_wordlist, wordlist words[], time logs_hms[]){

  int user_stream_start = 0, user_navigation = 0;
  int show_before;
  printf("Choose a prefered method of timestamp display:\n\n");
  printf("____________________________________\n");
  printf("|1) Time of day.                   |\n");
  printf("|2) Time according to stream start.|\n");
  printf("|9) Exit.                          |\n");
  printf("|----------------------------------|\n\n");
  printf("Select an option by entering the number to the left of your choice.\nPress (9) to exit. > ");
  scanf("%d",&user_navigation);
  switch(user_navigation){
    case(1): whitelist(total_entries_log, logs, total_entries_wordlist, words, logs_hms, user_navigation, 0); break;
    case(2): {user_stream_start = stream_start(&show_before); time_in_stream(total_entries_log, logs, logs_hms, user_stream_start); 
              whitelist(total_entries_log, logs, total_entries_wordlist, words, logs_hms, user_navigation, show_before); break;}
    case(9): printf("Exiting.\n"); break;
  } 
}

/* Threshold navigation */
void threshold_user_navigation(int total_entries_log, chatlog logs[], int total_entries_wordlist, wordlist word[], time logs_hms[]){

  int user_threshold = 0, user_stream_start = 0, show_before, user_navigation = 0;
  printf("\nEnter amount of points to show messages equal to or exceeding that value: ");
  scanf("%d",&user_threshold);
  printf("Choose a prefered method of timestamp display:\n\n");
  printf("____________________________________\n");
  printf("|1) Time of day.                   |\n");
  printf("|2) Time according to stream start.|\n");
  printf("|9) Exit.                          |\n");
  printf("|----------------------------------|\n\n");
  printf("Select an option by entering the number to the left of your choice.\nPress (9) to exit. > ");
  scanf("%d",&user_navigation);
  switch(user_navigation){
    case(1): print_over_threshold(total_entries_log, logs, user_threshold, logs_hms, user_navigation, 0); break;
    case(2): {user_stream_start = stream_start(&show_before); time_in_stream(total_entries_log, logs, logs_hms, user_stream_start); 
              print_over_threshold(total_entries_log, logs, user_threshold, logs_hms, user_navigation, show_before); break;}
    case(9): printf("Exiting.\n"); break;
  }
}

/* Function to convert timestamps to time after stream */
void time_in_stream(int total_entries_log, chatlog logs[], time logs_hms[], int user_input){

  int i, time = 0, temp = 0;

  for (i = 0; i < total_entries_log; ++i){
    logs_hms[i].points = logs[i].points;
    memcpy(logs_hms[i].username, logs[i].username, strlen(logs[i].username)+1);
    memcpy(logs_hms[i].message, logs[i].message, strlen(logs[i].message)+1);
  }
  timestamp_to_seconds(total_entries_log, logs, logs_hms);

  for(i = 0; i < total_entries_log; ++i){
    temp = logs_hms[i].seconds;
    time = temp - user_input;
    logs_hms[i].hours = time / HOURS;
    time = time % HOURS;
    logs_hms[i].minutes = time / MINUTES;
    time = time % MINUTES;
    logs_hms[i].seconds = time;
  }
}

void timestamp_to_seconds(int total_entries_log, chatlog logs[], time logs_hms[]){

  char temp_h[MAX_SIZE], temp_m[MAX_SIZE], temp_s[MAX_SIZE]; 
  int i;

  for (i = 0; i < total_entries_log; ++i){
    sscanf(logs[i].timestamp," %[^:] %*[:] %[^:] %*[:] %s",temp_h,temp_m,temp_s);
    logs_hms[i].seconds = (atoi(temp_h)*HOURS) + (atoi(temp_m)*MINUTES) + atoi(temp_s);
  }
}

int compare_points (const void * a, const void * b){

    chatlog *ia = (chatlog *)a;
    chatlog *ib = (chatlog *)b;
    if(ia->points > ib->points){
      return -1;
    }
    else if (ia->points < ib->points){
      return +1;
    }
    else if (ia->timestamp > ib->timestamp){
      return -1;
    }
    else if (ia->timestamp < ib->timestamp){
      return +1;
    }
    else{ 
      return 0;
    }
}

int compare_points_hms (const void * a, const void * b){

    time *ia = (time *)a;
    time *ib = (time *)b;
    if(ia->points > ib->points){
      return -1;
    }
    else if (ia->points < ib->points){
      return +1;
    }
    else if (ia->hours > ib->hours){
      return +1;
    }
    else if (ia->hours < ib->hours){
      return -1;
    }
    else if (ia->minutes > ib->minutes){
      return +1;
    }
    else if (ia->minutes < ib->minutes){
      return -1;
    }
    else if (ia->seconds > ib->seconds){
      return +1;
    }
    else if (ia->seconds < ib->seconds){
      return -1;
    }
    else{ 
      return 0;
    }
}
