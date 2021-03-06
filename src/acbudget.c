#include "acbudget.h"

/*
 * Checks and sets argument-bits accordingly.
 * 0   0- --help [-h] -> help was called, stops main
 * 1   1- OPTION-BIT  -> implies argParse were successful
 * 2   2- --verbose [-v]
 * 3   4- --amount-only [-a]
 * 4   8- Unassigned
 * 5  16- Unassigned
 * 6  32- Unassigned
 * 7  64- Unassigned
 * 8   S- Signed bit, should not happen.
 */
char argParser( int argc, char *argv[] )
{
	uint32_t i, j;
	char options = 0;
	for ( i=0; i<argc; i++ ) {
		
		if ( argv[i][0]=='-' ) {
			
			if ( equals( argv[i], "--help" ) == 0 ||
			     equals( argv[i], "-h" ) == 0 ) {
				// help [-h] [--help]
				usage();
				// return '0' for main() to know help was called
				return 0;
				     
			} else if ( equals( argv[i], "--verbose" ) == 0 ||
			            equals( argv[i], "-v" ) == 0 ) {
				// verbose [-v] [--verbose]
				// will be implemented several prints in time
				options |= 0x02;
				
			} else if ( equals( argv[i], "--amount-only" ) == 0 ||
			            equals( argv[i], "-a" ) == 0 )
			{
				// amount only during read [-a] [--amount-only]
				options |= 0x04;
			}
		}
	}
	// 1 means option were set
	options |= 0x01;
	
	if ( options & 2 ) {
		if ( options & 4 ) {
			printf( "Amount only during read enabled.\n" );
		}
	}

	return options;
}

/*
 *	Fetches/creates default configuration in 'config.ini' file
 */
int configurate( char *command )
{
#if DEBUG
	fprintf(stderr, "Configurating..\n");
#endif
	//	Checking for configuration file
	if ( !CONFIG_FILENAME )	CONFIG_FILENAME = calloc( 1, sizeof( char ) * strlen( "config.ini" )+1 );
	strcpy( CONFIG_FILENAME, "config.ini" );
	FILE *config_file = fopen( CONFIG_FILENAME, "r" );
	
	if ( config_file == NULL ) {	// file does not exist 

		//	Initiating default values
		DATABASE = calloc( 1, sizeof(char)*strlen("database.db" )+1 );
		strcpy( DATABASE, "database.db" );
		MONTH = calloc( 1, sizeof( char ) * strlen( "01" )+1 );
		strcpy( MONTH, "01" );
		YEAR = calloc( 1, sizeof( char ) * strlen( "2015" )+1 );
		strcpy( YEAR, "2015" );
		TABLE = calloc( 1, sizeof( char ) * strlen( "r2015" )+1 );
		strcpy( TABLE, "r2015" );
		READ_COUNTER = calloc( 1, sizeof( int ) );
		(*READ_COUNTER) = 0;
		
		#if DEBUG
		fprintf( stderr, "Creating '%s'\n", CONFIG_FILENAME );
		#endif

		//  Create config-file and write information
		config_file = fopen( "config.ini", "w" );
		fprintf( config_file, "#configuration file for aCBudget\n" );
		fprintf( config_file, "database=%s\n", DATABASE );
		fprintf( config_file, "table=%s\n", TABLE );
		fprintf( config_file, "year=%s\n", YEAR );
		fprintf( config_file, "month=%s\n", MONTH );
		fprintf( config_file, "read=%d\n", (*READ_COUNTER) );
		fclose( config_file );
		
		#if DEBUG
		fprintf( stderr, "'%s' created\n", CONFIG_FILENAME );
		#endif
		
	}	//	file did not exist
	else  {	// file exists

		int	counter = 1,
			len = 0,
			end = 0;
		char *variable, *value;
		
		while ( fgets( command, COMMAND_LEN, config_file ) != 0 ) {
		
			counter++;
			if ( command[0] != '#' )	{	//	not a comment
				variable = strtok( command, "=" );
				value = strtok( NULL, "" );
				#ifdef DEBUG
				fprintf( stderr, "(%s) %s = %s\n", command, variable, value );
				#endif
				len = strlen( value );
				if ( len > 0 ) {
		
					if ( strncmp( variable, "year\0", 5 ) == 0 ) {
						
						free( YEAR );
						YEAR = calloc( 1, sizeof( char ) * len );
						strncpy( YEAR, value, len ); YEAR[len-1] = 0;
						#if DEBUG
						fprintf( stderr, "YEAR=%s\n", YEAR );
						#endif
						
					} else if ( strncmp( variable, "month\0", 6 ) == 0 ) {
						
						free( MONTH );
						MONTH = calloc( 1, sizeof(char) * len );
						strncpy( MONTH, value, len); MONTH[len-1] = 0;
						
					} else if ( strncmp( variable, "table\0", 6) == 0) {

						free( TABLE);
						TABLE = calloc( 1, sizeof( char)*len);
						strncpy( TABLE, value, len); TABLE[len-1] = 0;
						#if DEBUG
						fprintf( stderr, "TABLE=%s\n", TABLE);
						#endif
						
					} else if ( strncmp( variable, "database\0", 9) == 0) {

						free(DATABASE);
						DATABASE = calloc(1, sizeof(char)*len);
						strncpy(DATABASE, value, len); DATABASE[len-1] = 0;
						#if DEBUG
						fprintf(stderr, "DATABASE=%s\n", DATABASE);
						#endif
						
					} else if (strncmp(variable, "read", 4) == 0) {

						if (!READ_COUNTER)	READ_COUNTER = calloc(1, sizeof(int));
						(*READ_COUNTER) = atoi(value);
						
					}
				}
			}	//	done with line
		} // while ( !feof( config_file ) );	//	lines left to check
		fclose(config_file);

		#if DEBUG
		fprintf(stderr, "'%s' imported\n", CONFIG_FILENAME);
		#endif
		
		snprintf(command, COMMAND_LEN, "Settings loaded from '%s'\n", CONFIG_FILENAME);
	}	//	file existed
	return 0;
}

/*
 *	Main method to initialize program
 */
int main( int argc, char **argv )
{
	SETTINGS = argParser( argc, argv );
	
	if ( SETTINGS < 1 ) {

		/* options < 0 is illegal
		   options == 0 means help was called
		   all positive values are legal
		*/
		return SETTINGS;
		
	}
	
	database = 0;
	char *zErrMsg = 0, command[COMMAND_LEN] = "\0", *printout = 0;
	int rc = 0, len = 0;

	/*
	 *	First configurates database according to a config-file (if not present, creates default)
	 *	Then accepts user input and acts accordingly.
	 */
	configurate( command );
	rc = sqlite3_open( DATABASE, &database );
	
	if ( rc ) {
		
		fprintf( stderr, "Can't open database: %s\n", sqlite3_errmsg( database ) );
		return(1);
		
	}
	
	len = prompt( command, "main" );
	if ( len < 0 ) {
		free_all();
		return 4;
	}
	//	Loop user for input while user don't want to quit
	
	while ( ( strncmp( command, "q\0", 2 ) != 0 ) && ( strncmp( command, "quit\0", 5 ) != 0 ) ) {
		
#if DEBUG
		fprintf( stderr, "%s (%d)\n", command, len );
#endif
		printout = execute_command( command, database );
		//	Printout if there is anything to print
		if ( printout == 0 ) {
			
			fprintf( stderr, "Error in a command, please look above for other error-messages\n" );
			break;
			
		}
		
		if ( strlen(printout) > 1 ) {
			
			printf( "%s", printout );
			
		}
		
		//	Fetch a new command
		len = prompt( command, "main" );
		if ( len < 0 ) {
			free_all();
			return 4;
		}
	}
	
	sqlite3_close( database );
	free( CONFIG_FILENAME );
	
	if ( free_all() ) {
		
		#if DEBUG
		fprintf( stderr, "free_all() did not return correctly\n." );
		#endif
		return -1;
		
	}
	
	#if DEBUG
	fprintf( stderr, "free_all() returned correctly\n." );
	#endif
	return 0;
	
}

/*
 *	Saves current configuration to 'config.ini' 
 */
void save_config( char *command, sqlite3 *database )
{
	FILE *config_file;
	config_file = fopen( CONFIG_FILENAME, "r" );
	if ( config_file == NULL )  // File does not exist, creating new file
	{
		#if DEBUG
		fprintf( stderr, "No old config file, creating new.\n" );
		#endif
		config_file = fopen( CONFIG_FILENAME, "w" );
		fprintf( config_file, "#configuration file for aCBudget\n" );
		fprintf( config_file, "database=%s\n", DATABASE );
		fprintf( config_file, "month=%s\n", MONTH );
		fprintf( config_file, "year=%s\n", YEAR );
		fprintf( config_file, "table=%s\n", TABLE );
		fprintf( config_file, "read=%d\n", (*READ_COUNTER) );
		fclose( config_file );
		snprintf( command, COMMAND_LEN, "Settings saved to new file '%s'\n", CONFIG_FILENAME );
	}
	else  {	// file exists, merging
		#if DEBUG
		fprintf( stderr, "Old config file found, merging...\n" );
		#endif
		FILE *new_file;
		char *token, *tmpname = calloc( 1, sizeof( char ) * strlen( CONFIG_FILENAME )+1 );
		memset( tmpname, 'X', sizeof( strlen( CONFIG_FILENAME ) ) );
		
		#ifdef __linux__	//	Linux wants to use mkstemp()
		new_file = fdopen( mkstemp( tmpname ), "w" );
		#else	//	use tmpnam()
		tmpnam( tmpname );
		new_file = fopen( tmpname, "w" );
		#endif
		
		#if DEBUG
		fprintf( stderr, "Created file with temp-name: '%s'\n", tmpname );
		#endif
		int counter = 1, file_size = 0;
		do { // while ( !feof ( config_file ) )
			
			if ( fgets( command, COMMAND_LEN, config_file ) != NULL) {
				counter++;
				token = strtok( command, "=" ); // '=' is config variabel delim
					
				if ( equals( token, "year" ) == 0 ) {
					// year is stored
					fprintf( new_file, "year=%s\n", YEAR );
						
				} else if ( equals( token, "month" ) == 0 ) {
					//	month is stored
					fprintf( new_file, "month=%s\n", MONTH );
						
				} else if ( equals( token, "table" ) == 0 ) {
					// database table is stored
					fprintf( new_file, "table=%s\n", TABLE );
						
				} else if ( equals( token, "database" ) == 0 ) {
					// database name is stored
					fprintf( new_file, "database=%s\n", DATABASE );
						
				} else if ( equals( token, "read" ) == 0 ) {
					// read counter is stored
					fprintf( new_file, "read=%d\n", (*READ_COUNTER) );
						
				} else  {
					// not a config variabel, user has manually added lines
					fprintf(new_file, "%s", command);
					
				}
				
			}
			
		} while ( !feof( config_file ) );
		
		fclose( config_file );
		fclose( new_file );
		
		if ( remove( CONFIG_FILENAME ) )  {
			// failed to remove old config file
			printf( "Failed to remove old 'config.ini';\n%s\n", strerror( errno ) );
			
		}
		if ( rename( tmpname, CONFIG_FILENAME ) )  {
			// failed to rename new config file
			printf( "Failed to rename new 'config.ini';\n%s\n", strerror( errno ) );
			
		}
		
		snprintf( command, COMMAND_LEN, "Settings saved to '%s'\n", CONFIG_FILENAME );
		free( tmpname );
	}
	
	return;
}

void usage( void )
{
	int tabulateLen = 5;
	char *usageString, *tempString;
	printf( "Usage: $ ./aCBudget [options]\n" );
	asprintf( &tempString, "%%%%s\n%%-%ds%%%%s\n", tabulateLen );
	asprintf( &usageString, tempString, " " );
	free( tempString );
	printf( usageString, "--help -h", "Shows this information." );
	printf( usageString, "--verbose -v", "Be loud about what you do." );
	free( usageString );
}
