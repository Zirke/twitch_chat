#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAX_SIZE 50
#define MAX_SIZE_MESSAGE 300

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

typedef struct category{
  char name[MAX_SIZE]; 
  int place;
} category;

int countAllEntries(FILE*);
void read_data_log(FILE*, int, chat_entry logs[]);
void read_wordlist(FILE*, int, wordlist words[]);
void check_for_question(int, chat_entry logs[], int, wordlist words[]);
void assign_points(int, int, chat_entry logs[], wordlist words[]);
void print_over_threshold(int, chat_entry logs[], int);
int compare_points (const void * a, const void * b);
void message_categoriser(chat_entry *logs, int total_entries_log);
void read_category_file(FILE fp, int category_total_entry, wordlist category_messages[]);
void message_saver(wordlist category_messages[], chat_entry messages[], int category_total_entry, chat_entry logs[], int* total_message, int total_entries_log);
void category_start_position(int category_total_entry, wordlist category_messages[], int* question_begin, int* gameterm_begin);
void database_maker(int start_position, int fin_position, wordlist database[], wordlist category_messages[]);

int main(void){

    FILE *chat_log, *user_wordlist;
    chat_log = fopen("twitchlogs.txt", "r");
    user_wordlist = fopen("wordlist.txt", "r");
    int total_entries_wordlist = countAllEntries(user_wordlist);
    int total_entries_log = countAllEntries(chat_log);
    int user_threshold = 0;
    wordlist *words = malloc(sizeof(wordlist) * total_entries_wordlist);
    chat_entry *logs = malloc(sizeof(chat_entry) * total_entries_log);
    read_wordlist(user_wordlist, total_entries_wordlist, words);
    read_data_log(chat_log, total_entries_log, logs);

    assign_points(total_entries_log, total_entries_wordlist, logs, words);
    
    //check_for_question(total_entries_log, logs, total_entries_wordlist, words);

    //printf("\nEnter amount of points to show messages equal to or exceeding that value: ");
    //scanf("%d",&user_threshold);
    //print_over_threshold(total_entries_log,logs,user_threshold);

    fclose(user_wordlist);
    fclose(chat_log);
    free(words);
    
    message_categoriser(logs, total_entries_log);
    
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

void message_categoriser(chat_entry *logs, int total_entries_log){
  
  FILE *fp;
  fp = fopen("question.txt", "r");
  int category_total_entry = countAllEntries(fp);
  int count = 0;
 
  wordlist *category_messages = malloc(sizeof(wordlist) * category_total_entry);
  chat_entry *messages = malloc(sizeof(chat_entry) * category_total_entry);
  
  int total_message = 0;
  
  
  read_category_file(*fp, category_total_entry, category_messages);
  message_saver(category_messages, messages, category_total_entry, logs, &total_message, total_entries_log);
  
  for(int i = 0; i < total_message; i++){
    printf("[%s] %s: %s\n",messages[i].timestamp, messages[i].username, messages[i].message);
  }
  printf("%d\n", category_total_entry);
  fclose(fp);
}


void read_category_file(FILE fp, int category_total_entry, wordlist category_messages[]){
  int i, question_begin, gameterm_begin, j=0;
  wordlist data = {0};
  
  
  for(i = 0; i < category_total_entry; ++i){
    fscanf(&fp," %[^\n]",data.word);
    category_messages[i] = data;
  }
  
  category_start_position(category_total_entry, category_messages, &question_begin, &gameterm_begin);

  
  wordlist *questions = malloc(sizeof(wordlist) * gameterm_begin);
  wordlist *gameterm = malloc(sizeof(wordlist) * (category_total_entry - gameterm_begin));
  
  database_maker(0, gameterm_begin, questions, category_messages);
  database_maker(gameterm_begin, category_total_entry, gameterm, category_messages);
  
}

void category_start_position(int category_total_entry, wordlist category_messages[], int* question_begin, int* gameterm_begin){
  int i;

  for(i = 0; i < category_total_entry; ++i){
    if(strcmp(category_messages[i].word, "[QUESTION]") == 0){
       (*question_begin) = i;
    }
    else if(strcmp(category_messages[i].word, "[GAMETERM]") == 0){
       (*gameterm_begin) = i;
    }
  }
}

void database_maker(int start_position, int fin_position, wordlist database[], wordlist category_messages[]){
 int i, j = 0;
 for(i = start_position; i < fin_position; i++){
   database[j] = category_messages[i];
   j++;
 } 
}

void message_saver(wordlist category_messages[], chat_entry messages[], 
  int category_total_entry, chat_entry logs[], int* total_message, int total_entries_log){
  int i, j, k=0;
  
  for(i = 0; i < total_entries_log; ++i){
    for (j = 0; j < category_total_entry; ++j){
      if(strstr(logs[i].message, category_messages[j].word) != NULL){
        messages[k] = logs[i];
        k++; 
        break;
      }
    }
  }
  (*total_message) = k;

}
