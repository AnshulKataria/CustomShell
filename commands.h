// function to implement pipe
void pipe_execute(int index_pipe,char* command,char* pwd,char** history_storage, int* history_counter, int* history_start,char* temp_value);

// function of redirection
int check_and_execute_redirection(int count,char* command,char *temp_value,char ** arguments);

// function to check and implement history command
void check_and_execute_repeat(char** arguments,char* pwd,char** history_storage, int* history_counter, int* history_start,char* temp_value);

// function to change directory
void change_directory(char** arguments,char* pwd);

// function to change directory relative to current working directory
void change_directory_relative_to_cwd(char** arguments);

// function to change directory relative to HOME directory
void home_directory_path(char** arguments);

// function to remove < and  >  and >>
void execute_redirection(char** arguments,int count,char** new_arguments);

// function to add command to history
void add_history(char** history_storage, char* command,int* history_counter, int* history_start);

// displaying history
void display_history(char** history_list, int number, int start);

//  function to obtain all the directories to find command executable
void get_paths_and_search(char *environment_variable,char ** arguments);

// child execute
void child_execute(char** arguments ,int count,char* command,char* pwd,char** history_storage, int* history_counter, int* history_start,char* temp_value);

// check all condition and execute accordingly
void check_and_execute(char* command,char* pwd,char** history_storage, int* history_counter, int* history_start,char* temp_value);







