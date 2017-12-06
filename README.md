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
This is done when promted using the *HH:MM:SS* format. **REMEMBER THE COLONS (':'))**

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
