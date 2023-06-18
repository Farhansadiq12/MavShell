//Requirement 16:
/*

Farhan Sadiq
1001859500

*/

// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 11    // Mav shell only supports ten arguments
#define MAX_PIDS 20              // Mav shell will list last 20 pids
#define MAX_HIST 15              // Mav shell will list last 15 commands

//Function declarations
void listpids(pid_t *list_pid, pid_t pid, int x);
void listcommands(char **list_command, char *command_string, int x);


int main()
{
  //To store the last 20 listpids
  pid_t list_pid[MAX_PIDS] = {};

  //To store the last 15 commands
  char *list_command[MAX_HIST] = {};

  char * command_string = (char*) malloc( MAX_COMMAND_SIZE );

  while( 1 )
  {
    // Print out the msh prompt
    printf ("msh> ");

    // Read the command from the commandline.  The
    // maximum command that will be read is MAX_COMMAND_SIZE
    // This while command will wait here until the user
    // inputs something since fgets returns NULL when there
    // is no input
    while( !fgets (command_string, MAX_COMMAND_SIZE, stdin) );

    /* Parse input */
    char *token[MAX_NUM_ARGUMENTS];

    int   token_count = 0;                                 
                                                           
    // Pointer to point to the token
    // parsed by strsep
    char *argument_ptr;                                         
                                                           
    char *working_string  = strdup( command_string );                

    // we are going to move the working_string pointer so
    // keep track of its original value so we can deallocate
    // the correct amount at the end
    char *head_ptr = working_string;

    //Requirement 6: If the user inputs a blank line, nothing will happen and the msh prompt will print out again.
    if(command_string[0] == '\n')
    {
      continue;
    }
    
    // Tokenize the input strings with whitespace used as the delimiter
    while ( ( (argument_ptr = strsep(&working_string, WHITESPACE ) ) != NULL) && 
              (token_count<MAX_NUM_ARGUMENTS))
    {
      token[token_count] = strndup( argument_ptr, MAX_COMMAND_SIZE );
      if( strlen( token[token_count] ) == 0 )
      {
        token[token_count] = NULL;
      }
        token_count++;
    }

    //Requirement 5: If the user enters "quit" or "exit", the shell shall exit with status 0
    if(strcmp(token[0], "quit") == 0 || strcmp(token[0], "exit") == 0)
    {
      exit(0);
    }

    //Requirement 10: Adding the cd command in the shell.
    if(strcmp(token[0], "cd") == 0)
    {
      int c = chdir(token[1]);
      
      //If the file or directory is not found then c should return -1
      if(c == -1)
      {
        printf("%s: no such file or directory: %s\n", token[0], token[1]);
      }

      //Adding this command to the list_command after the user enters it.
      listcommands(list_command, command_string, MAX_HIST);
      continue;
    }

    //Requirement 11: List the last 20 pids, if the user enters listpids command.
    if(strcmp(token[0], "listpids") == 0)
    {
       for(int d = 0; d < MAX_PIDS; d++)
       {
        if(list_pid[d] == '\0')
        {
          //If the index in which the list_pid is empty, it shall break out the loop and must print upto where the list has been filled.
          break;
        }

        printf("%d: %d\n", d, list_pid[d]);

       }
       
       //Adding this command to the list_command after the user enters it.
       listcommands(list_command, command_string, MAX_HIST);
       continue;
    }

    //Requirement 12: List the last 15 commands entered by the user.
    if(strcmp(token[0], "history") == 0)
    {
      for(int d = 0; d < MAX_HIST; d++)
      {
        if(list_command[d] == NULL)
        {
          //If the index in which the list_command is empty, it shall break out the loop and must print upto where the list has been filled.
          break;
        }

        printf("%d: %s\n", d, list_command[d]);
      }
       
       //Adding this command to the list_command after the user enters it.
       listcommands(list_command, command_string, MAX_HIST);
       continue;
    }

    //Requirement 12: Implementing !n
    if(command_string[0] == '!')
    {
      int index = atoi(&command_string[1]);

      if(index < 0 || index > 14)
      {
        printf("Command not in history.\n");
        listcommands(list_command, command_string, MAX_HIST);
      }

      else
        {
          //Stuck.
        }
        continue;
      }
    
    //Requirement 9: Using the function call fork()
    pid_t pid = fork( );
    
    //This is the child process. Returns 0 in the child.
    if( pid == 0 )
    {

      //Finding out the current directory using getcwd()
      char *current_directory = NULL;
      current_directory = getcwd(NULL, 0);

      //Running the execvp command
      int ret = execvp( token[0], &token[0] ); 

      //If ret returns -1, then there is an error running the command.
      if( ret == -1 )
      {
         printf("%s: Command not found.\n", token[0]);
      }

      //Exiting the child process. Preventing seg fault.
      exit(EXIT_SUCCESS);
    }

    //This is the parent process.
    else 
    {
      int status;

      //Waits for the child process to exit
      wait( & status );
      
      //Calling the following functions to add the pids in the list_pid and the commands in the list_command.
      listpids(list_pid, pid, MAX_PIDS);
      listcommands(list_command, command_string, MAX_HIST);
    }

    free( head_ptr );

  }
  return 0;
   // e2520ca2-76f3-90d6-0242ac120003
}

/*
Function name: listpids
Function type: void
Parameters expected: pid_t *list_pid , pid_t pid, int x 
What function does: It adds the pids to the pid list.
*/

void listpids(pid_t *list_pid, pid_t pid, int x)
{
  for(int i = 0; i < x; i++)
  {
    if(list_pid[i] == '\0')
    {
      list_pid[i] = pid;

      //Exiting the loop if the empty space is filled up, otherwise all the other next empty spaces in the list_pid would be filled up by the same pid. 
      return;
    }
  }

  for(int z = 0; z < x - 1; z++)
  {
      //If the entire list is already filled up, the latest pid gets added to the list and oldest one gets replaced.
      list_pid[z] = list_pid[z + 1];
  }
    
  //The new pid is stored in the last index.
  list_pid[x - 1] = pid;
}

/*
Function name: listcommands
Function type: void
Parameters expected: char **list_command, char *command_string, int x
What function does: It adds the commands to the command list.
*/

void listcommands(char **list_command, char *command_string, int x)
{
  for(int i = 0; i < x; i++)
  {
    if(list_command[i] == NULL)
    {
      list_command[i] = (char*) malloc( MAX_COMMAND_SIZE );
      strcpy(list_command[i], command_string);

      //Exiting the loop if the empty space is filled up, otherwise all the other next empty spaces in the list_command would be filled up by the same command. 
      return;
    }
  }

  for(int y = 0; y < x - 1; y++)
  {
    //If the entire list is already filled up, the latest command gets added to the list and oldest one gets replaced.
    strcpy(list_command[y], list_command[y + 1]);
  }
    
  //The new command is stored in the last index.
  strcpy(list_command[x - 1], command_string);
}
