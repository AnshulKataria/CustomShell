#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "util.c"
#include "commands.c"


// child execute
void child_execute(char** arguments ,int count,char* command,char* pwd,char** history_storage, int* history_counter, int* history_start,char* temp_value)
{
    if(arguments!=NULL)
    {
		if(arguments[0]!=NULL &&  arguments[0][0]=='!')
		{
			check_and_execute_repeat(arguments, pwd, history_storage,  history_counter,  history_start, temp_value);
		}
		else if(strcmp(arguments[0],"history")==0)
		{
			// if command is of history
			display_history(history_storage,*history_counter,*history_start);
		}
		else
		{
			//  execute redirection if any - other wise continue
			int temp=check_and_execute_redirection(count,command ,temp_value,arguments);
			// check wheter executable or not ****************************************
			if(temp!=-1)
			{
				get_paths_and_search(temp_value,arguments);	
				printf("Error: command '%s' not found \n",command );
				exit(0);
			}
				


		}
    }
}

// check all condition and execute accordingly
void check_and_execute(char* command,char* pwd,char** history_storage, int* history_counter, int* history_start,char* temp_value)
{
	// check if its a backroud process
 	char* bg= checkbackground(command);
 	int index_pipe=-1;
 	index_pipe=is_pipe_return_index(command);
 	//printf("index of | is --%d--\n", index_pipe);
 	if(index_pipe!=-1)
	{
		pipe_execute(index_pipe,command,pwd,history_storage,history_counter,history_start,temp_value);			
	}
 	else if(strcmp(bg,"error")==0)
 	{
 		printf("User Error\n");
 		
 	}
 	else
 	{
	 	// tokenizing the command 
	 	int count = word_count(command,strlen(command));
		char *arguments[count+1];
		count_arguments (command," ",arguments);
		if(arguments[0]==NULL)
		{
			// nothing is entered
		}
		else if((strcmp(arguments[0],"exit")==0) || (strcmp(arguments[0],"quit")==0))
		{
			// checking for exit or quit command
			// exit the system 
			printf("bye\n");
			exit(0);
		}
		else if( strcmp(arguments[0],"cd")==0)
		{   // checking for cd command	
			change_directory(arguments,pwd);	
		}
		else
		{
			// setting default signal handler
			signal(SIGCHLD,SIG_DFL);
			if(strcmp(bg,"true")==0)
			{
				// setting signal handler fo bg process
				signal(SIGCHLD, childSignalHandler);
			}
		     //for storing value of fork() return
  			pid_t  pid=fork();
		 	if(pid < 0)
		 	{
		 		printf("fork error\n");
		 	}
		    else if(pid!=0)
			{  // parent process
				//printf("child %d created.\n",pid );
			   // should wait till child get completed IF BG IS NOT TRUE

			   if(strcmp(bg,"true")==0)
			   {
			   		// background process so parent dont wait	
			   		printf("child %d created.\n",pid );	   		
			   }
			   else
			   {
			   		// foreground process
			   		int status;
			   		pid_t pid =(pid_t)-1;
			   		pid=waitpid(pid,&status,0);
			   		//printf("%s\n", );
			   		//printf("child %d completed ",pid );
			   		if ( WIFSIGNALED( status ) )  /* core dump or kill or kill -9 */
			    	{
			      	//	printf( "abnormally with status code \n");
			    	}
			    	else if ( WIFEXITED( status ) ) /* child called return or exit() */
			    	{
			      	//	int rc = WEXITSTATUS( status );
			    	//	printf( "successfully with exit status %d\n", rc );
			    	}
			   } 
			}
			else
			{
			    // child process to execute the command
				child_execute( arguments,count, command, pwd, history_storage,  history_counter,  history_start, temp_value);
	    		exit(0);
			}
		}
 	}
 	
}


int main(int argc, char *argv[]) 
{
	char command[MAX_LENGTH]; // to input user command in shell
	char pwd[PWD_LENGTH];  // store current working directory
	pwd[PWD_LENGTH]='\0';
	// get current working directory  
	//obtaining $MYPATH variable to search command in the specified directories
	char * env_name="MYPATH";
	char * env_value=NULL;

	//to obtain $MYPATH value or to create if does not exist 
	get_set_path(env_name, &env_value);
    // temp variable to copy $MYPATH value, which will be used for $MYPATH tokenization
	char* temp_value=strdup(env_value);
	  
	// variable to store history of function
	int* history_counter=(int*)malloc(sizeof(int));
	    *history_counter=0;
	char* history_storage[MAX_COMMAND_HISTORY];
	// to keep starting index in array
	int*  history_start=(int*)malloc(sizeof(int));
	     *history_start=0;

  	while (1) 
 	{
	  	// setting the default sigal handler
	  	//signal(SIGCHLD,SIG_DFL);
	    // printing current working directory
	 	get_cwd(pwd);
		printf("$ ");
		printf("%s ", pwd);   
		fflush(NULL);

		// getting user command as input
		if (!fgets(command, MAX_LENGTH, stdin)) break;
		//remove new line from end
		int len = strlen(command);
		if( command[len-1] == '\n')
		{
			command[len-1] = '\0';
	    }
	    int i=0;
	 	while(command[i]==' ')
	 	{
	 		i++;
	 	}
	 	if(command[i]=='!')
	 	{
	 		// not saving command in case of history command
	 	}
		else
		{
			// adding history to history storage
			add_history(history_storage,command,history_counter,history_start);
		}
		check_and_execute(command,pwd,history_storage, history_counter, history_start, temp_value);
	}
  return 0;
}

