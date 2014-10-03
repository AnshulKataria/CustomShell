#include "commands.h"

//  function to obtain all the directories to find command executable
void get_paths_and_search(char *environment_variable,char ** arguments)
{
	char path[MAX_LENGTH];
	char* temp_value=malloc(MAX_LENGTH);
	strcpy(temp_value,environment_variable);
	//printf("in in --%s--\n", environment_variable);
	//printf("in in --%s--\n",arguments[0] );
	struct stat file_info_buffer;
	int lstat_return;
   	 while ((temp_value = strtok(temp_value,":")) != NULL)
    {
			 strcpy(path, temp_value);
			 strcat(path, "/");
			 strcat(path, arguments[0]);
			 // checking  for executable rights
			 lstat_return=lstat(path, &file_info_buffer);
			 if(lstat_return==-1){}
			 else
			 {	// S_IXUSR is the flag for checking executable by owner

			 	if ( S_ISREG( file_info_buffer.st_mode ) )
				{	// regular file
				   
				    if ( file_info_buffer.st_mode & ( S_IXUSR | S_IXGRP | S_IXOTH ) )
				    {
				      		// check if command is echo and if it has $ present in first argument
				 		if(strcmp(arguments[0],"echo")==0)
				 		{
				 			if(arguments[1][0]=='$')
				 			{
				 				arguments[1]=getenv(++arguments[1]);
				 			}
				 			else if (arguments[1][0]=='~')
				 			{
				 				arguments[1]=getenv("HOME");
				 			}
				 			else if(arguments[1][0]=='\\')
				 			{
				 				++arguments[1];
				 			}
				 			int i=0;
				 			while(arguments[i]!=NULL)
				 			{
				 				arguments[i]= trim_character(arguments[i],'"');
				 				i++;
				 			}
				 		}
				 		execv(path,arguments);
					}
					else
				 	{
				 		 printf("not executable\n");
				 	}
				}
			 }
		     temp_value = NULL;
   	 }
}

// displaying history
void display_history(char** history_list, int number, int start)
{
	int pos=0;
	int max_value=0;
	int temp_start=0;
	if(start!=0)
	{
		max_value=MAX_COMMAND_HISTORY;	
		temp_start=number % MAX_COMMAND_HISTORY;
	}
	else
	{
		max_value=number;
		temp_start=0;
	}
	
	for(pos=0; pos< max_value ;pos++)
	{	
		printf("%03d   %s\n",pos,(history_list[temp_start]));
		temp_start=temp_start+1;
		temp_start=temp_start % MAX_COMMAND_HISTORY;
	}	
}

// function to add command to history
void add_history(char** history_storage, char* command,int* history_counter, int* history_start)
{	
		if(strcmp(command,"")==0)
		{

		}
		else
		{
			char* temp=malloc(MAX_LENGTH);
			strcpy(temp,command);
			if ((*history_counter) < MAX_COMMAND_HISTORY)
		  	{	
		  		history_storage[(*history_counter)]= (char*)malloc (strlen(command)*sizeof(char));
		  		strcpy(history_storage[(*history_counter)],temp);
				(*history_counter)++;
		  	}
			else if ((*history_counter)>=MAX_COMMAND_HISTORY)
			{	
				//(*history_counter)=((*history_counter)%MAX_COMMAND_HISTORY);
				// now we are replacing commands
				(*history_start)=1;
				//(*history_start)=((*history_start)%MAX_COMMAND_HISTORY);
		  		strcpy(history_storage[(*history_counter)%MAX_COMMAND_HISTORY],temp);
		  		(*history_counter)++;
			}
		}
		
}


// function to remove < and  >  and >>
void execute_redirection(char** arguments,int count,char** new_arguments)
{

	//char* new_arguments[count+1];
	int i=0;
	int j=0;
	while (arguments[i]!=NULL)
	{
		if((strcmp(arguments[i],"<")!=0) && (strcmp(arguments[i],">")!=0) && (strcmp(arguments[i],">>")!=0) )
		{
			new_arguments[j] = (char*)malloc((strlen(arguments[i]))*sizeof(char));
			new_arguments[j] =arguments[i];
			j++;
		}
		else
		{
			i++;
		}
		i++;
	}
	new_arguments[j]=NULL;
}



// function to change directory relative to HOME directory
void home_directory_path(char** arguments)
{
	if(size_string(arguments[1])==1 )
	{
		// only ~
		//changing to home directory
		chdir(getenv("HOME"));
	}
	else
	{
		// append rest of string after home directory

		// getting home directory
		char * home=(char*)malloc(strlen(getenv("HOME"))*sizeof(char));
		home=getenv("HOME");

		//appeding home with parameter pass
		strcat(home,++arguments[1]);
		
		//handling trailing / after argument
		int leng=size_string(home);
		while(home[leng-1]=='/')
		{
			home[leng-1]='\0';
			leng--;
		}
	
		// /strcat(home,"/");
		// change to obtained folder
		int p=chdir(home);
		if (p== -1)
		{
			printf("cd: %s: No such file or directory\n", home);
		}
	
	}
}
// function to change directory relative to current working directory
void change_directory_relative_to_cwd(char** arguments)
{
	char new_pwd[PWD_LENGTH];
	new_pwd[PWD_LENGTH]='\0';
	//getting pwd
	get_cwd(new_pwd);
	//appeding /
	strcat(new_pwd,"/");
	//appending parameter passed to cwd
	strcat(new_pwd,arguments[1]);
	// handling excess /
	int leng=size_string(new_pwd);
	while(new_pwd[leng-1]=='/')
	{
		new_pwd[leng-1]='\0';
		leng--;
	}

	//changing directory
	int p=chdir(new_pwd);
	if (p== -1)
	{
		printf("cd: %s: No such file or directory\n", new_pwd);
	}
}

// function to change directory
void change_directory(char** arguments,char* pwd)
{
	if(arguments[1]==NULL)
	{
		// in no argument return it to home
		chdir(getenv("HOME"));
	
	}
	else if( strcmp(arguments[1],"..")==0)
	{
		// return to parent directory
		parent_directory(pwd);

	}
	else if(arguments[1][0]=='~')
	{	// go back to home directory
		home_directory_path(arguments);
	}
	else 
	{	// new folder inside cwd 
		change_directory_relative_to_cwd(arguments);
	
	}

}



// function to check and implement history command
void check_and_execute_repeat(char** arguments,char* pwd,char** history_storage, int* history_counter, int* history_start,char* temp_value)
{
	char* temp_history_value=malloc(MAX_LENGTH);
	// check if it is command to 'repeat command in history'
	if((size_string(arguments[0])==2) && (arguments[0][1]=='!'))
	{	
		// repeat last command	
		
			//check if more than max stored command 
			// counting token in command
			printf("%s\n",history_storage[((*history_counter)-1)%MAX_COMMAND_HISTORY]);
			strcpy(temp_history_value,history_storage[((*history_counter)+(MAX_COMMAND_HISTORY-1))%MAX_COMMAND_HISTORY]);
			check_and_execute(temp_history_value,pwd,history_storage, history_counter, history_start, temp_value);
		


	} // not in if -means its gonna check in history --not just repeat the last commmand
	else if(size_string(arguments[0])<2)
	{
		// only ! --nothing to do
	}
	else
	{
		// check first for number in command number --passing three digit
		char *num_search=(char*)malloc(3*sizeof(char));
		//copying 3 character
		strncpy(num_search,++(arguments[0]),(size_t)3);
		char ** return_number=malloc(sizeof(char));
		int num =strtol(num_search,return_number,10);
		//printf("DEBUG: history repeat --%s--\n",arguments[0] );
		if((arguments[0])[0]!='0'&& num==0)
		{
		     //repeat command starting with charater not number-
			// to check whether it is a number or it is return by strtol failure --there is no number in the string immediately after ! 
			
			int var=0;	
			int temp_counter=0;
			int temp_start=0;
			int flag=0;
			
			if((*history_start)!=0)
			{
				temp_counter=MAX_COMMAND_HISTORY;
				temp_start=(*history_counter)%MAX_COMMAND_HISTORY;
			}
			else
			{
				temp_counter=(*history_counter);
				temp_start=0;
			}
			for (var=0 ; var < temp_counter;var++)
			{
				// match the sring with command avaibale in command history
				if(strstr(history_storage[temp_start],arguments[0])!=NULL)
				{
					if(flag==0)
					{
						flag=1;
						printf("%s\n",history_storage[temp_start] );
						strcpy(temp_history_value,history_storage[temp_start]);
						check_and_execute(temp_history_value,pwd,history_storage, history_counter, history_start, temp_value);
					}
					
				}
				temp_start=temp_start+1;
				temp_start=temp_start%MAX_COMMAND_HISTORY;
			}
				
			// in command not found in history
			if(flag==0)
			{
				printf("ERROR: no command with '%s' prefix in history\n", num_search);
			}
			
		}
		else
		{
			// it can be number or combi of number and string
			if((*return_number) != '\0' && (**return_number)=='\0')
			{
				// first three CHARATER is number only
				// obtain the command from the number generated
				// check there are less than 3 letter
				//printf("DEBUG: history repeat--number only --%s--\n",arguments[0] );
				if(size_string(arguments[0])<3){
					//they are less than 3 character-should be three
					printf("!%s: event not found\n",arguments[0]);
				}
				else
				{ 	// all 3 are digit 
					if(num>=0 && num < MAX_COMMAND_HISTORY)
					{
						// if number is in the history
						int temp_start=0;
						if(*history_start!=1){
							temp_start=0;
						}
						else{
							temp_start=(*history_counter)%MAX_COMMAND_HISTORY;
						}
						
						temp_start=temp_start+num;
						temp_start=temp_start%MAX_COMMAND_HISTORY;
						char* tem=(char*)malloc (1000*sizeof(char));
						strcpy(tem,history_storage[temp_start]);
						strncat(tem,arguments[0]+3,size_string(arguments[0])-3);
						check_and_execute(tem,pwd,history_storage, history_counter, history_start, temp_value);										
					}
					else
					{
						printf("!%d: event with number not found\n",num);
					}
				}
	
			}
			else
			{
				// invalid command since command number is not 3 digit
				// need to join commnad
				printf("4\n");
				printf("!%s: commmand not found\n",arguments[0]);
			}
		}	
	}	
}


// function of redirection
int check_and_execute_redirection(int count,char* command,char *temp_value,char ** arguments)
{
	// check for input output redirection			
	int input_counter=0;
	int output_counter=0;
	int input_index;
	// wheter to truncate or not
	int truncate =1;
	int output_index;
	int loop;
	for(loop=0;loop<count;loop++)
	{
		if(strcmp(arguments[loop],"<")==0 && input_counter==0)
		{
			// input redirection
			input_counter=1;
			if(loop!=(count-1))
			{	
				input_index=loop+1;
			}
			else
			{
				// last argument is < --error
			}
		}
		else if(strcmp(arguments[loop],">")==0 && output_counter==0)
		{
			// output redirection
			output_counter=1;
			if(loop!=(count-1))
			{	
				output_index=loop+1;
				
			}
			else
			{
				// last argument is < --error
			}
		}
		else if(strcmp(arguments[loop],">>")==0 && output_counter==0)
		{
			// output redirection with append
			output_counter=1;
			if(loop!=(count-1))
			{
				
				truncate=0;
				output_index=loop+1;
				
			}
			else
			{
				// last argument is < --error
			}
		}
	}


	int fd;
	int fd1;
	char* new_arguments[count+1];
	if(input_counter==1 && output_counter==1){
		// both are there
		fd = open( arguments[input_index], O_RDONLY,0 );
		if(truncate==1)
		{
			fd1 = open( arguments[output_index], O_WRONLY | O_CREAT | O_TRUNC,0660 );
		}
		else
		{
			fd1 = open( arguments[output_index], O_WRONLY | O_TRUNC,0660 );
		}
		
		if(fd>0 && fd1 >0)
		{
			execute_redirection(arguments,count,new_arguments);
			int duplicate_ref_input=dup(0);
			dup2(fd, 0);
			//close input stream
			close(fd);
			int duplicate_ref_output=dup(1);
			dup2(fd1,1);
			close(fd1);
			get_paths_and_search(temp_value,new_arguments);	
			printf("%s: command not found \n",command );
			dup2(duplicate_ref_input,0);
			dup2(duplicate_ref_output,1);	
		}
		else
		{
			if(fd<0){
				printf("Error: no file to read from (%s)\n",arguments[input_index] );
			}
			if(fd1<0)
			{
				printf("Error: no file to write to (%s)\n",arguments[output_index] );
			}
			return -1;
		}
		
		
	}

	else if(input_counter==1)
	{
		fd = open( arguments[input_index], O_RDONLY,0 );
		if(fd>0)
		{
			execute_redirection(arguments,count,new_arguments);
			// copy input stream to reference
			int duplicate_ref_input=dup(0);
			dup2(fd, 0);
			//close input stream
			close(fd);
			// check wheter executable or not ****************************************
			get_paths_and_search(temp_value,new_arguments);	
			printf("%s: command not found \n",command );
			dup2(duplicate_ref_input,0);	
		}
		else
		{
			printf("Error : no file to read from (%s)\n",arguments[input_index] );
			return -1;
		}
			
	}
	else if (output_counter==1)
	{
		/* code */
		if (truncate==1)
		{
			// called for >>
			fd1 = open( arguments[output_index], O_WRONLY | O_CREAT | O_TRUNC,0660 );
			if(fd1>0)
			{
				execute_redirection(arguments,count,new_arguments);
				int duplicate_ref_output=dup(1);
				dup2(fd1,1);
				close(fd1);
				// check wheter executable or not ****************************************
				get_paths_and_search(temp_value,new_arguments);		
				dup2(duplicate_ref_output,1);	
				printf("%s: command not found \n",command );
			}
			else
			{
				printf("Error in file opening\n");
				return -1;
			}
			
		}
		else
		{
			//callled for  >
			 fd1 = open( arguments[output_index], O_WRONLY | O_APPEND,0660 );
			 if(fd1<0)
			 {
			 	printf("Error: no such file to append to (%s)\n",arguments[output_index] );
			 	return -1;
			 }
			 else
			 {
			 	execute_redirection(arguments,count,new_arguments);
				 //printf("sign is --%s--\n",arguments[2] );
				int duplicate_ref_output=dup(1);
				dup2(fd1,1);
				close(fd1);
				// check wheter executable or not ****************************************
				get_paths_and_search(temp_value,new_arguments);	
				dup2(duplicate_ref_output,1);	
				printf("%s: command not found \n",command );
			 }
			 
		}
		
	}
	return 0;
}

// function to implement pipe
void pipe_execute(int index_pipe,char* command,char* pwd,char** history_storage, int* history_counter, int* history_start,char* temp_value)
{
		// check wheter pipe is there
		// spliting the array on |
		int length =size_string(command);
		char* command1=(char*)malloc(length*sizeof(char));
		char* command2=(char*)malloc(length*sizeof(char));
	  	
	  	// copying command
	  	char* command_copy =(char*)malloc(length*sizeof(char));
	  	strcpy(command_copy,command);
	  	int counter=0;
	  	for (counter = 0 ; counter < index_pipe ; counter++) 
      	{
      		command1[counter]=command_copy[counter];
      	}
      	command1[counter]='\0';
      	counter++;
      	int counter2=0;
      	while (command_copy[counter]!='\0')
      	{
      		command2[counter2]=command_copy[counter];
      		counter++;
      		counter2++;
      	}
      	command2[counter2]='\0';

      	int command_pipe[2];
		// first child
 		//int duplicate_ref_input=dup(1);
 		pipe(command_pipe);		
	 	int old_output=dup(1);
	 	int old_input=dup(0);
	 	// transfering standard output to pipe
		dup2(command_pipe[1],1);
		check_and_execute(command1,pwd,history_storage, history_counter, history_start, temp_value);
		// again getting output to standard output
		dup2(old_output,1);
		close(old_output);
		dup2(command_pipe[0], 0);
		close(command_pipe[1]);
		check_and_execute(command2,pwd,history_storage, history_counter, history_start, temp_value);
		close(command_pipe[0]);
		dup2(old_input,0);
}
