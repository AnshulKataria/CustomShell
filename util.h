

// get current working directory
void  get_cwd(char *pwd);

// funtion to set the $MYPATH environment variable
void get_set_path( const char * name,  char ** value);

// size of char string
int size_string(char * str);

// to obtain arguments in the command given
int word_count(char* line, int size);

// to obtain arguments in the command given
void count_arguments (char* command_string, const char* delim,  char ** arguments);

// check bg process
char* checkbackground(char* command);

//handling backgroud process signal
void childSignalHandler(int signum);

// function to check pipe is there or not
int is_pipe_return_index(char* command);

// function to change directory to  parent
void parent_directory(char* pwd );

// trimmming leading and trailing character
char* trim_character(char* string, char delim);
