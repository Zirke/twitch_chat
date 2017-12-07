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
void message_categoriser(chat_entry *logs);
void read_category_database(FILE fp, int question_total_entry, wordlist questions[]);
void message_category_saver(wordlist questions[], chat_entry messages[], 
  int question_total_entry, chat_entry *logs, int* total_message);

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
    
    message_categoriser(logs);
    
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

void message_categoriser(chat_entry *logs){
  
  FILE *fp;
  fp = fopen("question.txt", "r");
  int question_total_entry = countAllEntries(fp);
  int count = 0;
 
  wordlist *questions = malloc(sizeof(wordlist) * question_total_entry);
  chat_entry *messages = malloc(sizeof(chat_entry) * question_total_entry);
  int total_message = 0;
  
  
  read_category_database(*fp, question_total_entry, questions);
  message_category_saver(questions, messages, question_total_entry, logs, &total_message);
  
  for(int i = 0; i < total_message; i++){
    printf("[%s] %s: %s\n",messages[i].timestamp, messages[i].username, messages[i].message);
  }
  printf("%d\n", question_total_entry);
  fclose(fp);
}

void read_category_database(FILE fp, int question_total_entry, wordlist questions[]){
  int i;
  wordlist data = {0};
  for (i = 0; i < question_total_entry; ++i){
    fscanf(&fp," %[^\n]",data.word);
    questions[i] = data;
  } 
}

void message_category_saver(wordlist questions[], chat_entry messages[], 
  int question_total_entry, chat_entry *logs, int* total_message){
  
  int i, j, k=0;
  //Der er noget galt i if'en
  for(i = 0; i < question_total_entry; ++i){
    for (j = 0; j < question_total_entry; ++j){
      //der er noget galt med logs[i].message
      if(strstr(logs[i].message, questions[j].word) != NULL){
        //printf("if\n");
        messages[k] = logs[i];
        k++; 
        //break;
      }
    }
  }
  (*total_message) = k;

}
