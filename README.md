# aCBudget
A simple terminal budget program written in C, with SQLite3* to handle database.
Database must be set up beforehand in current state. Also, be careful not to delete entire databases, as it stands when using the implemented "aCBudget.select >" interface; any SQL command _WILL_ be executed.

##	 Current features:
- quick-insertion to database
- quick update/divide command
- read from DNB and Sparebanken S�r files
- manual select statements on database
- predefined stats menu for quick printouts of balances
- configuration file for easy default setup
- backup functionality

## Wishes from the future:
* UI
* Network capability
* Smartphone capability

## Setup:
* See 'detailed_readme'.txt or use the accompanying Makefile (run 'make sql' if first time, then use 'make' if you've changed the code)
* If you do not have a database, the program will create one according to 'config.ini' *or* when first time launching *without* the 'config.ini' file it will create the database 'regnskap.db'.

## SQLite
See [their webpage](http://www.sqlite.org/ "SQLite.org") for download/interface and information on SQLite. I have now added the folder 'sql/' with the required files - but be sure to spread the word of SQLite and their awesome work!