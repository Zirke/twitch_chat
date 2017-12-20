Readme for Twitch Chat Analyzer
===============================

Syntax in textfiles
-------------------

#### Log file

Use chat log with format from [OverRustle Logs](https://overrustlelogs.net).

```
[YYYY-MM-DD HH:MM:SS TIMEZONE] USERNAME: MESSAGE
```

##### Example
```
[2017-11-30 02:46:33 UTC] Zirke: This is a sample message.
[2017-12-01 12:23:56 UTC] Zirke: This is another a sample message.
```

#### Wordlist file

A single word and number seperated by a comma per line. (**WORDS ARE CASE SENSITIVE!**)

```
Word, number
```

##### Example
```
Kappa, 3
Trihard, 4
   ```

#### Categories file

A single word per line which either indicates one of the three categories question, game term or emoji in uppercase letter and in brackets or word part of the white list corresponding a category is written in the categories.txt file. It is possible to add a word in a category by writting it on a line after the category's name directly in the text file. However, it is not possible for user to make a new category.

##### Example
```
[QUESTION]
?
What
```

Navigation of the program
-------------------------

#### Timestamps

Currently there are two ways to display timestamps in messages:

1. Time of day
2. Time after stream start

*Time of day* will show the time the message was sent in the timezone the chat is logged in (OverRustle Log uses UTC).
*Time after stream start* will display the timestamp as time after the stream started, with an option to show messages before the stream went live.
To select either, simply press the number to the left of your prefered option when prompted.
If you select *Time after stream start* you will have to enter the time your stream started in the UTC timezone (or the same timezone as used in the log file).
This is done when promted using the *HH:MM:SS* format. **REMEMBER THE COLONS (':')**

##### Example
```
13:42:30
or
19:30:40
```

After you have entered the time of stream start, you will be prompted with the option to display messages before the stream went live (if there is any).
Simply press *1* for Yes and *0* for No when prompted. In the example below, we have opted to show messages from before the stream started.

##### Example
```
Would you like to show messages from before the stream started?> 1

[-5:-37:-17] Zirke: This is an example message
[-3:-19:-9] Zirke: This message was sent 3 hours, 19 minutes and 9 seconds before the stream started
[0:40:23] Abimaster21: I don't like white people.
[1:23:30] Zirke: This message was sent 1 hour 23 minutes and 30 seconds into the stream.
```

The messages that show minus values as timestamps are messages sent before the stream went live.

#### File editing

It is possible to edit the list of words that is used to judge messages directly from the program.
This can be done by entering the number '4' at the main menu.

##### Example
```
Welcome to Twitch Chat Analyzer 1.0:

____________________________________
|1) Whitelist                      |
|2) Point Threshold                |
|3) Categories                     |
|4) Add or Remove Wordlist         |
|9) Exit.                          |
|----------------------------------|

Select an option by entering the number to the left of your choice.
Press (9) to exit. > 4
```

This will bring you to the file editing menu. From here you can either enter 'add' to add an entry to the list, enter 'remove' to remove an entry, or enter 'exit' to return to the main menu.

##### Example
```
Do you want to add or remove a word from the database?
Enter "add", "remove" or "exit": add
```

If you enter 'add', the program will prompt you for a word that you want to add.
After adding a word, it will then prompt you for how high the score should be set for that particular word.

##### Example
```
Add a word to the database:
fish
Set the score for the new word:
10
```

Once the word has been added, you will return to the file editing menu.
Should you enter 'remove' instead, you will be prompted to enter a word.
Once entered, all entries in the word list containing the exact same word will be deleted.
You need not enter the score the word has.

##### Example
```
Which word do you want to delete?:
fish
```

Once all entries with that word has been removed, you will be returned to the file editing menu.
Should you type 'exit' here, you will be returned to the main menu.

#### Categorisation of chat messages

It is possible to display messages related to one of the three categories questions, game terms, and emoji directly from the program.
This can be done by entering the number '3' at the main menu.
There are four options to display messages related to a category:

1. Questions
2. Game Terms
3. Emoji
4. Exit message categorisation function

*Questions* displays messages related to questions.
*Game term* displays messages related to game terms in League of Legends.
*Emoji* displays messages related to emojis.
To select Questions, press 1. 
To select Game terms, press 2. 
To select Emoji, press 3.
In order to exit the function, press 9.
