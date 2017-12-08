#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define MAX_SIZE 50
#define MAX_SIZE_WITH_POINTS (MAX_SIZE + 5)
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

typedef struct wordlist_copy{
  char word[MAX_SIZE_WITH_POINTS];
}wordlist_copy;

int countAllEntries(FILE*);
void read_data_log(FILE*, int, chat_entry logs[]);
void read_wordlist(FILE*, int, wordlist words[]);
void check_for_question(int, chat_entry logs[], int, wordlist words[]);
void assign_points(int, int, chat_entry logs[], wordlist words[]);
void print_over_threshold(int, chat_entry logs[], int);
int compare_points (const void * a, const void * b);
void file_change_option(FILE *word_list, int total_entries_wordlist);
void file_addition(FILE *word_list/*, int total_entries_wordlist*/);
void file_subtraction(FILE *original, int);

int main(void){

    FILE *chat_log, *user_wordlist;
    chat_log = fopen("twitchlogs.txt", "r");
    user_wordlist = fopen("wordlist.txt", "r+");
    int total_entries_wordlist = countAllEntries(user_wordlist);
    int total_entries_log = countAllEntries(chat_log);
    int user_threshold = 0;
    wordlist *words = malloc(sizeof(wordlist) * total_entries_wordlist);
    chat_entry *logs = malloc(sizeof(chat_entry) * total_entries_log);
    read_wordlist(user_wordlist, total_entries_wordlist, words);
    read_data_log(chat_log, total_entries_log, logs);

    assign_points(total_entries_log, total_entries_wordlist, logs, words);
    
    check_for_question(total_entries_log, logs, total_entries_wordlist, words);
    file_change_option(user_wordlist, total_entries_wordlist);

    printf("\nEnter amount of points to show messages equal to or exceeding that value: ");
    scanf("%d",&user_threshold);
    print_over_threshold(total_entries_log,logs,user_threshold);

    fclose(user_wordlist);
    fclose(chat_log);
    free(words);
    
    
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

void file_change_option(FILE *word_list, int total_entries_wordlist){
  char user_input[10];
  do{
  printf("Do you want to add or remove a word from the database?\n");
  scanf("%s", user_input);
  }while(strcmp(user_input, "add") != 0 && 
         strcmp(user_input, "remove") != 0);

  if(strcmp(user_input, "add") == 0){
    file_addition(word_list);
  }
  else if(strcmp(user_input, "remove") == 0){
    file_subtraction(word_list, total_entries_wordlist);
  }
}

void file_addition(FILE *word_list){
  char user_input_word[MAX_SIZE],user_input_score[10], final_word[MAX_SIZE_WITH_POINTS];
  int i;

  printf("Add a word to the database: \n");
  scanf("%s", user_input_word);

  printf("Set the score for the new word: \n");
  scanf("%s", user_input_score);

  strcpy(final_word, "\n");
  strcat(final_word, user_input_word);
  strcat(final_word, ", ");
  strcat(final_word, user_input_score);

  fputs(final_word, word_list);
}

/*Copy all lines except the one to be deleted in to a new file, rename new file to old file.*/
void file_subtraction(FILE *original, int total_entries_wordlist){
  int line_count = 0, flag = 0, i;
  char delete_word[MAX_SIZE], word_storage_nopoints[MAX_SIZE], word_storage[MAX_SIZE_WITH_POINTS];
  wordlist_copy temp[total_entries_wordlist];
  FILE *new;
  new = fopen("new_wordlist.txt", "w");

  printf("Which word do you want to delete?: \n");
  scanf("%s", delete_word);
printf("Attempting dowhile statement...\n");

fseek(original, 0, SEEK_SET);
  do{
    fgets(word_storage, MAX_SIZE_WITH_POINTS, original);
    sscanf(word_storage, "%[^,]", word_storage_nopoints);
printf("fgets was successful\n%s\n", word_storage);

    if(strcmp(word_storage_nopoints, delete_word) != 0){
      strcpy(temp[line_count].word, word_storage);
printf("strcpy successful\n%s\n", temp[line_count].word);
      ++line_count;
    }

  }while(feof(original) == 0);
printf("Dowhile statement succes!\nAttempting for loop...\n");
  for(i = 0; i < line_count; ++i){
    fprintf(new, "%s", temp[i].word);
  }
printf("closing the files.\n");
  fclose(original);
  fclose(new);
  remove("wordlist.txt");
  rename("new_wordlist.txt", "wordlist.txt");
}
