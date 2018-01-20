#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 50
#define MAX_SIZE_MESSAGE 1536
#define MAX_SIZE_WITH_POINTS (MAX_SIZE + 5)
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

typedef struct wordlist_copy{
  char word[MAX_SIZE_WITH_POINTS];
}wordlist_copy;

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
/* Categories Functions */
void main_message(chatlog *logs, int total_entries_log);
void read_category_file(FILE* fp, int category_total_entry, wordlist category_messages[]);
void message_categoriser(wordlist category_messages[], chatlog messages[], int category_total_entry, chatlog logs[], int* total_message, int total_entries_log);
void category_start_position(int category_total_entry, wordlist category_messages[], int *question_begin, int *gameterm_begin, int *emoji_begin);
void database_maker(int start_position, int fin_position, wordlist database[], wordlist category_messages[], int* total_entries);
void print_category(chatlog messages[], int total_message);
/* File */
void file_change_option(FILE *word_list, int total_entries_wordlist);
void file_addition(FILE *word_list);
void file_subtraction(FILE *original, int);

int main(void){

  FILE *user_log, *user_wordlist;
  user_log = fopen("twitchlogs.txt", "r");
  user_wordlist = fopen("wordlist.txt", "r+");

  int total_entries_wordlist = count_all_entries(user_wordlist);
  int total_entries_log = count_all_entries(user_log);
  int user_navigation = 0;
  
  wordlist *words = malloc(sizeof(wordlist) * total_entries_wordlist);
  chatlog *logs = malloc(sizeof(chatlog) * total_entries_log);
  time *logs_hms = malloc(sizeof(time) * total_entries_log);
  
  read_wordlist(user_wordlist, total_entries_wordlist, words);
  read_data_log(user_log, total_entries_log, logs);
  
  assign_points(total_entries_log, total_entries_wordlist, logs, words);

  do{
  printf("Welcome to Twitch Chat Analyzer 1.0:\n\n");
  printf("____________________________________\n");
  printf("|1) Whitelist                      |\n");
  printf("|2) Point Threshold                |\n");
  printf("|3) Categories                     |\n");
  printf("|4) Add or Remove Wordlist         |\n");
  printf("|9) Exit.                          |\n");
  printf("|----------------------------------|\n\n");
  printf("Select an option by entering the number to the left of your choice.\nPress (9) to exit. > ");
  scanf(" %d",&user_navigation);
  switch(user_navigation){
    case(1): time_user_navigation(total_entries_log, logs, total_entries_wordlist, words, logs_hms); break;
    case(2): threshold_user_navigation(total_entries_log, logs, total_entries_wordlist, words, logs_hms); break;
    case(3): main_message(logs, total_entries_log); break;
    case(4): file_change_option(user_wordlist, total_entries_wordlist); break;
    case(9): printf("Exiting. Bye!\n"); break;
    default: printf("Error: Unrecognized input. Please try again.\n");
  }}while(user_navigation != 9);

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
  wordlist data;
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
    //for (i = 0; i < total_entries_log; ++i){
      temp = logs;
    //}
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
  do{
  printf("Select an option by entering the number to the left of your choice.\nPress (9) to exit. > ");
  scanf("%d",&user_navigation);
  switch(user_navigation){
    case(1): whitelist(total_entries_log, logs, total_entries_wordlist, words, logs_hms, user_navigation, 0); break;
    case(2): {user_stream_start = stream_start(&show_before); time_in_stream(total_entries_log, logs, logs_hms, user_stream_start); 
              whitelist(total_entries_log, logs, total_entries_wordlist, words, logs_hms, user_navigation, show_before); break;}
    case(9): printf("Exiting.\n"); return;
    default: printf("Error: Unrecognized input. Please try again.\n");
  }}while(user_navigation != 9);
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
  do{
  printf("Select an option by entering the number to the left of your choice.\nPress (9) to exit. > ");
  scanf("%d",&user_navigation);
  switch(user_navigation){
    case(1): print_over_threshold(total_entries_log, logs, user_threshold, logs_hms, user_navigation, 0); break;
    case(2): {user_stream_start = stream_start(&show_before); time_in_stream(total_entries_log, logs, logs_hms, user_stream_start); 
              print_over_threshold(total_entries_log, logs, user_threshold, logs_hms, user_navigation, show_before); break;}
    case(9): printf("Exiting.\n"); return;
    default: printf("Error: Unrecognized input. Please try again.\n");
  }}while(user_navigation != 9);
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

/* function to categorisation of messages*/
void main_message(chatlog *logs, int total_entries_log){

  FILE *fp;
  fp = fopen("categories.txt", "r");
  int category_total_entry = count_all_entries(fp);
  int question_begin, gameterm_begin, emoji_begin,question_fin, gameterm_fin, emoji_fin,total_message, user_input;
  wordlist *category_messages = malloc(sizeof(wordlist) * category_total_entry);
  chatlog *messages = malloc(sizeof(chatlog) * category_total_entry);
  
  read_category_file(fp, category_total_entry, category_messages);
  category_start_position(category_total_entry, category_messages, &question_begin, &gameterm_begin, &emoji_begin); 
  wordlist *questions = malloc(sizeof(wordlist) * gameterm_begin);
  wordlist *gameterm = malloc(sizeof(wordlist) * (emoji_begin));
  wordlist *emoji = malloc(sizeof(wordlist) * (category_total_entry - emoji_begin));
  
  database_maker(question_begin, gameterm_begin -1, questions, category_messages, &question_fin);
  database_maker(gameterm_begin, emoji_begin -1 , gameterm, category_messages, &gameterm_fin);
  database_maker(emoji_begin, category_total_entry, emoji, category_messages, &emoji_fin);

  printf("Choose a category to display:\n\n");
  printf("____________________________________\n");
  printf("|1) Questions.                     |\n");
  printf("|2) Game Terms.                    |\n");
  printf("|3) Emotes.                        |\n");
  printf("|9) Exit.                          |\n");
  printf("|----------------------------------|\n");
  do{
  printf("\nSelect an option by entering the number to the left of your choice.\nPress (8) to show menu or (9) to exit. > ");
  scanf(" %d", &user_input);
  switch(user_input){
    case(1): message_categoriser(questions, messages, question_fin, logs, &total_message, total_entries_log); print_category(messages, total_message); break;
    case(2): message_categoriser(gameterm, messages, gameterm_fin, logs, &total_message, total_entries_log); print_category(messages, total_message); break;
    case(3): message_categoriser(emoji, messages, emoji_fin, logs, &total_message, total_entries_log); print_category(messages, total_message); break;
    case(8): printf("Choose a category to display:\n\n");
             printf("____________________________________\n");
             printf("|1) Questions.                     |\n");
             printf("|2) Game Terms.                    |\n");
             printf("|3) Emotes.                        |\n");
             printf("|9) Exit.                          |\n");
             printf("|----------------------------------|\n\n"); break;
    case(9): printf("Exiting.\n"); break;
  }}while(user_input != 9);

  free(messages);
  free(questions);
  free(gameterm);
  free(emoji);
  fclose(fp);
  /* Mangler at tjekke om [CATEGORY] er med i databaserne*/
}
/* function reads all of the words from the file categories.txt into an array of structs (wordlist)*/
void read_category_file(FILE* fp, int category_total_entry, wordlist category_messages[]){

  int i, j=0;
  wordlist data;

  for(i = 0; i < category_total_entry; ++i){
    fscanf(fp," %[^\n]",data.word);                     /* Scans untill newline an saves the word into "data.word"*/
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

/* Function assigns/saves the messages from a particular category in the array of structs "messages" (chatlog)*/
void message_categoriser(wordlist category_messages[], chatlog messages[], 
  int category_total_entry, chatlog logs[], int* total_message, int total_entries_log){
  
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

/* function prints timestamp, username and message for each message part of the chosen category*/
void print_category(chatlog messages[], int total_message){

  int i;
  
  for(i = 0; i < total_message; i++){
    printf("[%s] %s: %s\n",messages[i].timestamp, messages[i].username, messages[i].message);
  }
}

void file_change_option(FILE *word_list, int total_entries_wordlist){

  char user_input[10];

  do{
    printf("Do you want to add or remove a word from the database?\nEnter \"add\", \"remove\" or \"exit\": ");
    scanf("%s", user_input);

    if(strcmp(user_input, "add") == 0 || strcmp(user_input, "Add") == 0){
      file_addition(word_list);
    }
    if(strcmp(user_input, "remove") == 0 || strcmp(user_input, "Remove") == 0){
      file_subtraction(word_list, total_entries_wordlist);
    }
  }while(strcmp(user_input, "exit") != 0 && strcmp(user_input, "Exit") != 0);
}

void file_addition(FILE *word_list){

  char user_input_word[MAX_SIZE], user_input_score[10], final_word[MAX_SIZE_WITH_POINTS];
  //int i;

  printf("Add a word to the database: \n");/*The user specifies a word to be added.*/
  scanf("%s", user_input_word);

  printf("Set the score for the new word: \n");/*Then the user specifies how highly that word is scored.*/
  scanf("%s", user_input_score);

  /*The word and the score are then combined, following the standard structure
    of the database. The combined string is then placed in the database file.*/
  strcpy(final_word, " \n");
  strcat(final_word, user_input_word);
  strcat(final_word, ", ");
  strcat(final_word, user_input_score);

  printf("%s \n", final_word);
  
  fputs(final_word, word_list);
}

/*Copies all lines except the one to be deleted in to a new file, rename new file to old file.*/
void file_subtraction(FILE *original, int total_entries_wordlist){

  int line_count = 0, flag = 0, i;
  char delete_word[MAX_SIZE], word_storage_nopoints[MAX_SIZE], word_storage[MAX_SIZE_WITH_POINTS];
  wordlist_copy temp[total_entries_wordlist];
  FILE *new;
  new = fopen("new_wordlist.txt", "w");

  printf("Which word do you want to delete?: \n");/*User specifies a word to be deleted.*/
  scanf("%s", delete_word);

fseek(original, 0, SEEK_SET);/*Ensures the file is read from the beginning to avoid issues.*/

  /*Finds and stores which entries should be kept in the database.*/
  do{
    fgets(word_storage, MAX_SIZE_WITH_POINTS, original);/*Fetches a line of data.*/
    sscanf(word_storage, "%[^,]", word_storage_nopoints);/*Then stores it for analytical purposes.*/

  /*Checks whether or not an entry should be kept. Stores it if it should.*/
    if(strcmp(word_storage_nopoints, delete_word) != 0){
      //printf("%s %s %d \n", word_storage_nopoints, delete_word, line_count);
      strcpy(temp[line_count].word, word_storage);
      ++line_count;
    }

  }while(feof(original) == 0);/*Repeats the code above until there are no more entries.*/

  /*All lines that are not to be deleted are stored in a new file.*/
  for(i = 0; i < line_count; ++i){
    fprintf(new, "%s", temp[i].word);
  }

  /*Deletes the current database and renames the new one so that it can 
    be used in place of the old one. This effectively deletes the unwanted
    entry, as it will not be moved over to the new database.*/
  fclose(original);
  fclose(new);
  remove("wordlist.txt");
  rename("new_wordlist.txt", "wordlist.txt");
  original = fopen("wordlist.txt", "r");
  printf("Done.\n");
}