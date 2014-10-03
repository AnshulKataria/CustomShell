#include "util.h"
#define MAX_LENGTH 1024   //assuming input will not exceed 1000 character
#define PWD_LENGTH 512
#define MAX_COMMAND_HISTORY 1000

// get current working directory
void  get_cwd(char *pwd)
{	
	if(getcwd(pwd, PWD_LENGTH-1) == NULL) {
     	  printf ("Can not get current working directory\n");
          }
        else {
          
         }
}


// funtion to set the $MYPATH environment variable
void get_set_path( const char * name,  char ** value)
{
	*value=getenv(name);
	
	if(!(*value))
	{	
		*value="/bin:.";
		//setenv(name,"/bin:.",1);
	}
}

// size of char string
int size_string(char * str){
	
	int pointer=0;
	while (str[pointer]!='\0')
	{
		
		pointer++;
	}
	return pointer;
}

// to obtain arguments in the command given
int word_count(char* line, int size)
{
    int i;
    int count=1; //integer for the word count
    for(i=0;i<size;){
		if(line[i]==' ')
		{
			while(line[i]==' '&& i<size)
			{	
				if(i==0)
				{
					count--;
				}				
				i++;			
		    }
			if(i!=(size))
			{
				count++;				
			}
		}
		else
		{
		i++;
		}
		
	}
    return count; //return the integer
}


// to obtain arguments in the command given
void count_arguments (char* command_string, const char* delim,  char ** arguments) {
   char* start;
   int i = 0;

   start = strtok (command_string, delim);
   while (start != NULL) 
   {
   	//printf("--%s\n", start);
      arguments[i] = (char*)malloc((strlen(start)+1)*sizeof(char));
      arguments[i] = start;
      i++;
      start = strtok(NULL, delim);
    }
      arguments[i] = NULL;

  
}

// check bg process
char* checkbackground(char* command)
{
	 char* ready= malloc (strlen(command)*sizeof(char));
	 ready=strchr(command,'&');
	 if(ready==NULL)
	 {
	 	return "false";
	 }
	 else
	 {
	 	int index=ready-command;
	 	if (index!=(strlen(command)-1))
	 	{
	 		return "error";
	 	}
	 	else
	 	{
	 		
	 		command[index]='\0';
	 		return "true";
		}
	 }
}

//handling backgroud process signal
void childSignalHandler(int signum) {
    int status;
    pid_t pid;
    
    pid = waitpid(0, &status, WNOHANG);
    if(pid!=-1)
    {	
    	// not already captured by parent
    	printf("child %d completed ",pid );
	    if ( WIFSIGNALED( status ) )  /* core dump or kill or kill -9 */
	    {
	    	
	      printf( "abnormally with status code \n");
	    }
	    else if ( WIFEXITED( status ) ) /* child called return or exit() */
	    {
	      int rc = WEXITSTATUS( status );
	      printf( "successfully with exit status %d\n", rc );
	    }
    }
    
}

// function to check pipe is there or not
int is_pipe_return_index(char* command)
{
	int index=0;
	//int size=size_string(commmand);
	while (command[index]!='\0')
	{
		if(command[index]=='|')
		{
			return index;
		}
		
		index++;
	}
	return -1;

}

// function to change directory to  parent
void parent_directory(char* pwd )
{
	char* pointer;
	char* new_directory;
	// getting last index of /
	pointer=strrchr(pwd,'/');
	int index=(int)(pointer-pwd);
	new_directory=(char*) malloc((index+1)*sizeof(char));
	strncpy(new_directory,pwd,index);
	//changing directory
	int p=chdir(new_directory);
	if (p== -1)
	{
	 	perror("chdir");
	}
}

// function to trim leading and trailing character 
char* trim_character(char* string, char delim)
{
	while(*string==delim)
	{
		++string;
	}
	char *temp =string;
	while(*temp!='\0')
	{
		temp++;
	}
	temp--;
	while(*temp==delim)
	{
		temp--;
	}
	temp++;
	*temp='\0';
	return string;
	//printf("%s\n", string);
}
