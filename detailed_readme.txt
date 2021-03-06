More detailed information about the program, Sections*
*	How to compile
*	.main commands
*		select
*		insert
			help
*		update
*		stats
*		config
			load
			show
			year
			month
			table
			database
			backup
			revert
			help
*		read
*		help
*	SQL database setup

*	How to compile

Regular compilation:
$	gcc src/*.c sql/*.c -c &&
	gcc *.o -o aCBudget

Afterwards you can safely delete the '*.o' files, by running the appropriate command on your computer (ie. windows _del_).
I keep mine in a subdirectory 'o/', and thus run this command (after directory has been created);

Folder compilation
$	gcc src/*.c sql/*.c -c &&
	move *.o o/ &&
	gcc o/*.o -o aCBudget

If you have kept your '*.o' files, and have pulled a newer version of aCBudget, it would be smart not to recompile and assemble sql. Therefore I only delete the aCBudget compiled files 'a*.o' before recompiling by using this command;

Update compilation
$	cd o/ &&
	del a*.o &&
	cd ../ &&
	gcc src/*.c sql/*.c -c &&
	move *.o o/ && gcc o/*.o -o aCBudget 

If there are any questions, as always do not hesitate to contact me :) I would very much like to hear your feedback/problems and help you with them!


Pr. July 6th there are a total of six (7) commands you can call at '$	./aCBudget.main >':

*
$	aCBudget.select >
The most versatile and equally dangerous command available. You can call ANY database command through this statement.
A confirmation appears if any enquiry except 'select' are attempted to execute. I am not liable for any damage done to your system or database through this command.

*
$	aCBudget.insert >
You can manually enter new insertions using this command.
The format is displayed once, and can be brought up again using by typing 'h' or 'help' when entering command.

Format is: date, comment, type, amount, with these limitations:
date: 		YYYY-MM-DD
comment:	Length of 99 characters. Any characters exceeding the 99th character are ignored
type:			Length of 15 characters. Any characters exceeding the 15th character are ignored
amount:	Use dotted '.' format for typing float numbers; ie. 102.14

There must be some time inbetween each insertion. As all insertions are inserted with a unique id of length 5 which is generated by the program. However the program uses time for creating random seed, and too quick insertions will create equal seed (and therefore sql_exectuion will fail on UNIQUE_CONSTRAINT).

*
$	aCBudget.update >
Here you can update a row with either combination of new comment, type and/or amount. Immediately after it is possible to add another row with same date, but new comment, type and amount. You are only able to type the day within 'update' command. If a switch in month is desired, execute 'month=x', where 'x' is a number between 1 (january) and 12 (december)

*
$	aCBudget.stats >
Prints out a numbered menu for some quick stats. Only predefined commands here, with prompts where needed (for months and/or days etc.).

*
$	aCBudget.config >
In this command you can configurate the default database and date variables the program uses, save them to file, or load from file.
Default config file 'config.ini" is as follows:
```
#configuration file for aCBudget
database=regnskap.db
year=2014
table=r2014
month=01
read=0
```
Below is the long (and short) commands with explanations.
$	aCBudget.config > save (sv)
Saves the current configuration to file 'config.ini' if it exists, else it creates the file and then save the configuration.
$	aCBudget.config > load (ld)
Command loads variables from 'config.ini' if it exists, else it initializes default variables and stores them in a new file 'config.ini'
$	aCBudget.config > show (sw)
Shows the current configuration.
$	aCBudget.config > year (yr)^
Shows the saved year.
$	aCBudget.config > month (mn)^
Shows the saved month.
$	aCBudget.config > table (tb)^
Shows the saved table.
$	aCBudget.config > database (db)^�
Shows the saved database.
$	aCBudget.config > read (rd)^
Shows the number of lines currently being "skipped".
$	aCBudget.config > backup (bu)
Saves current database to backup-database.
$	aCBudget.config > revert (rv)
Reverts backup database to current database - canceling all executions since last backup.
$	aCBudget.config > help (h)
Shows a list of commands within 'aCBudget.config', their abbreviations and some explanations.
Commands marked with '^' are able to be redefined by typing the variablename followed by '=' and the new value. Ie. 'month=2' would change the current value of month to '02'.
$ When changing database with the 'database=newdatabase.filetype', the program also closes previous database and tries to open the new database. If unsuccessful, it reopens old database with notification to user.

*
$	aCBudget.read > 
Reads either files from DNB (1) or Sparebanken S�r (2) for quick insertions from "nettbank" (online bank).
Program reads line by line, displays the information and asks user if an insertion is wished.
If wished, it fetches date and amount, and prompts user for comment and type.
In DNB it only asks for TYPE, then asks user if it is correct, or an edit of comment is wished (y/n/e).
After all info is given the program creates unique ID and inserts the row into the database.
Because the ID is an unique constraint in database, and is randomly generated, there is a 1:34^5 chance it will be a duplicate and read will fail.
If this happens, or if you quit while reading the file - you must manually go through the file and remove lines you've "done", as the program only reads files, and it is boring saying "n(o)" to all the lines you don't want to add again on next 'read'.

*
$	aCBudget.main > help (h)
Shows the current list of commands explained over, with a short explanation.

*
SQL database setup:
How your database should look like:

File: regnskap.db (the program creates this file by default)

Go to 'select' command and run the following SQL statement;
```
create table r2014 (
	date date,
	comment varchar[99],
	type varchar[15],
	amount BIGINT,
	id varchar[5] unique
);
```
I choose to call the table r2014 as I started using this program in 2014, and wanted a short name for "regnskap 2014".
You can of course use any name you want, but remember to change it in configuration first.

That should be it!
If you find any parts of this information hard, please leave me a note on Github and I'll see if I can do something about it!
