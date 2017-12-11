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
void category_start_position(int category_total_entry, wordlist category_messages[], int *question_begin, int *gameterm_begin, int *emoji_begin);
void database_maker(int start_position, int fin_position, wordlist database[], wordlist category_messages[], int* total_entries);
void print_category(chatlog messages, int total_message);

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
  fp = fopen("categories.txt", "r");
  int category_total_entry = countAllEntries(fp);
  int question_begin, gameterm_begin, emoji_begin,question_fin, gameterm_fin, emoji_fin,total_message, user_input;
  wordlist *category_messages = malloc(sizeof(wordlist) * category_total_entry);
  chat_entry *messages = malloc(sizeof(chat_entry) * category_total_entry);
  
  read_category_file(*fp, category_total_entry, category_messages);
  category_start_position(category_total_entry, category_messages, &question_begin, &gameterm_begin, &emoji_begin); 
  wordlist *questions = malloc(sizeof(wordlist) * gameterm_begin);
  wordlist *gameterm = malloc(sizeof(wordlist) * (emoji_begin));
  wordlist *emoji = malloc(sizeof(wordlist) * (category_total_entry - emoji_begin));
  
  database_maker(0, gameterm_begin -1, questions, category_messages, &question_fin);
  database_maker(gameterm_begin, emoji_begin, gameterm, category_messages, &gameterm_fin); //det skal nok være emoji_begin - 1
  database_maker(emoji_begin, category_total_entry, emoji, category_messages, &emoji_fin);

  printf("Please enter the a number to print the corresponding messages within the category \n"); //lyder lidt ondsvagt, skal nok ændre til noget andet
  printf("Enter 1) for questions\n Enter 2) for messages with gameterms \n Enter 3) for messages with Emoji\n");
  scanf(" %d", &user_input);

  if( user_input == 1){
    message_saver(questions, messages, question_fin, logs, &total_message, total_entries_log);
  }
  else if(user_input == 2){
    message_saver(gameterm, messages, gameterm_fin, logs, &total_message, total_entries_log);
  }
  else if(user_input == 3){
    message_saver(emoji, messages, emoji_fin, logs, &total_message, total_entries_log);
  }
  
  print_category(messages, total_message);



  free(messages);
  free(questions);
  free(gameterm);
  free(emoji);
  fclose(fp);
  /* Mangler at tjekke om [CATEGORY] er med i databaserne*/
}
/* function reads all of the words from the file categories.txt into an array of structs (wordlist)*/
void read_category_file(FILE fp, int category_total_entry, wordlist category_messages[]){
  int i, question_begin, gameterm_begin, j=0;
  wordlist data = {0};

  for(i = 0; i < category_total_entry; ++i){
    fscanf(&fp," %[^\n]",data.word);                     /* Scans untill newline an saves the word into "data.word"*/
    category_messages[i] = data;
  }
}

/* Function saves the position of the categories into the corresponding int pointer*/
void category_start_position(int category_total_entry, wordlist category_messages[], int *question_begin, int *gameterm_begin, int *emoji_begin){
  int i;

  for(i = 0; i < category_total_entry; ++i){
    if(strcmp(category_messages[i].word, "[QUESTION]") == 0){
       (*question_begin) = i + 1;
    }
    else if(strcmp(category_messages[i].word, "[GAMETERM]") == 0){
       (*gameterm_begin) = i +1;
    }
    else if(strcmp(category_messages[i].word, "[EMOJI]") == 0){
      (*emoji_begin) = i +1;
    }
  }
}

/* Function initializes an array of structs with words corresponding a category*/
void database_maker(int start_position, int fin_position, wordlist database[], wordlist category_messages[], int* total_entries){
  int i, j = 0;
  for(i = start_position; i < fin_position; i++){  /* the for-loop runs from the startposition of the particular category till the finish position. */
    database[j] = category_messages[i];
    j++;
  } 
  (*total_entries) = j; /* Assigns the amount of entries to the int pointer "*total_enties"*/
}

/* Function assigns/saves the messages from a particular category in the array of structs "messages" (chat_entry)*/
void message_saver(wordlist category_messages[], chat_entry messages[], 
  int category_total_entry, chat_entry logs[], int* total_message, int total_entries_log){
  
  int i, j, k=0;
  
  for(i = 0; i < total_entries_log; ++i){          /* The for-loop runs through every chat messages */
    for (j = 0; j < category_total_entry; ++j){    /* the for-loop runs through a category database */
      /* If a part of the message string from the chat log is the same as the database then log[i] is assignes to messages[k]*/
      if(strstr(logs[i].message, category_messages[j].word) != NULL){
        messages[k] = logs[i];
        k++; 
        break;
      }
    }
  }
  (*total_message) = k;  /* the total amount of entries in the array of struct */

}

void print_category(chatlog messages, int total_message){
  int i;
  
  for(i = 0; i < total_message; i++){
    printf("[%s] %s: %s\n",messages[i].timestamp, messages[i].username, messages[i].message);
  }

}
