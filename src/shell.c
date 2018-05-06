#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define BUFFER_SIZE 50

#define  CB_BUFSIZE 80
#define  CB_DELIM " \t\r\n\a"
static char* currentDirectory;


char history[10][BUFFER_SIZE]; //history array to store history commands
int count = 0;

void displayHistory()
{
    
    printf("Shell command history:\n");
    
    int i;
    int j = 0;
    int histCount = count;
    
    //loop for iterating through commands
    for (i = 0; i<10;i++)
    {
        //command index
        printf("%d.  ", histCount);
        while (history[i][j] != '\n' && history[i][j] != '\0')
        {	
		//printing command
            printf("%c", history[i][j]);
            j++;
        }
        printf("\n");
        j = 0;
        histCount--;
        if (histCount ==  0)
            break;
    }
    printf("\n");
} 

int shell_cd(char **args);
int shell_help(char **args);
int shell_exit(char **args);
int shell_history(char **args);
int shell_webserver(char **args);
int shell_chatbox(char **args);
int shell_server(char **args);
char *shell_commands[] = {
  "cd",
  "help",
  "exit",
  "history",
  "webserver",
  "chatbox",
  "server"
};

int (*shell_commands_func[]) (char **) = {
  &shell_cd,
  &shell_help,
  &shell_exit,
  &shell_history,
  &shell_webserver,
  &shell_chatbox,
  &shell_server
};

int shell_commands_num() 
{
  return sizeof(shell_commands) / sizeof(char *);
}

int shell_chatbox(char **args){
     pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
      const char *cmd = "konsole -e ./chatUser1 ";

        system(cmd);
    }
  
  else {
    const char *cmd = "konsole -e ./chatUser2 ";
system(cmd);
  }
    return 1;
}

int shell_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "shell: expected argument to \"cd\"\n");
  } 
  else {
    if (chdir(args[1]) != 0) {
      perror("shell");
    }
  }
  return 1;
}

int shell_server(char ** args){
 pid_t pid1,pid2;
  int status;

  pid1 = fork();
  if (pid1 == 0) {
        pid2 = fork();
        if (pid2 == 0) {
                  const char *cmd = "konsole -e ./shellServer ";
                system(cmd);
        }
        else{
                const char *cmd = "konsole -e ./shellClient ";
                system(cmd);
        }

    }
  
  else {
    const char *cmd = "konsole -e ./shellClient ";
    system(cmd);
  }
    return 1;    
}

int shell_help(char **args)
{
  int i;
  printf("Rajat, Vaishnavi, Lipika, Nishtha's Shell\n");
  printf("Type your command for the program names and arguments, and hit enter.\n");
  printf("The following are our commands:\n");

  for (i = 0; i < shell_commands_num(); i++) {
    printf("  %s\n", shell_commands[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int shell_history(char **args)
{
  displayHistory();
  return 1;
}

int shell_webserver(char ** args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
      char appPath[40]="konsole -e ./webserver";
      if (args[1] != NULL) {
          strcat(appPath," -p ");
          strcat(appPath,args[1]);
      }
            system(appPath);

      /*char *args1[]={"./webserver.out",NULL};
    if (execvp(args1[0], args1) == -1) {
      perror("shell");
    }
    exit(EXIT_FAILURE);*/
  } 
  else if (pid < 0) {
    // Error forking
    perror("shell");
  } 
  else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

int shell_exit(char **args)
{
  return 0;
}


int shell_launch(char **args)
{
  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0) {
    if (execvp(args[0], args) == -1) {
      perror("shell");
    }
    exit(EXIT_FAILURE);
  } 
  else if (pid < 0) {
    // Error forking
    perror("shell");
  } 
  else {
    do {
      waitpid(pid, &status, WUNTRACED);
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}



int execute_command(char **args)
{
  int i;

  if (args[0] == NULL) {
    return 1;
  }
  
      if (strcmp(args[0], "history") != 0) {

     for (i = 9;i>0; i--) //Moving the history elements one step higher
       	strcpy(history[i], history[i-1]);
    
    strcpy(history[0],args[0]); //Updating the history array with input buffer
    count++;
	if(count>10)
	 count=10;
	
    }
	
	

  for (i = 0; i < shell_commands_num(); i++) {
    if (strcmp(args[0], shell_commands[i]) == 0) {
      return (*shell_commands_func[i])(args);
    }
  }
    //printf("Command Not found");
  return shell_launch(args);
}

#define  RL_BUFSIZE 1024

char *read_input(void)
{
  int bufsize =  RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "shell: Memory allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    c = getchar();

    if (c == EOF) {
      exit(EXIT_SUCCESS);
    } 
    else if (c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } 
    else {
      buffer[position] = c;
    }
    position++;

    if (position >= bufsize) {
      bufsize +=  RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "shell: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}



char **split_input(char *line)
{
  int bufsize =  CB_BUFSIZE, position = 0;
  char **command_buffers = malloc(bufsize * sizeof(char*));
  char *command_buffer, **command_buffers_backup;

  if (!command_buffers) {
    fprintf(stderr, "shell: Cannot allocate memory\n");
    exit(EXIT_FAILURE);
  }

  command_buffer = strtok(line,  CB_DELIM);
  while (command_buffer != NULL) {
    command_buffers[position] = command_buffer;static char* currentDirectory;

    position++;

    if (position >= bufsize) {
      bufsize +=  CB_BUFSIZE;
      command_buffers_backup = command_buffers;
      command_buffers = realloc(command_buffers, bufsize * sizeof(char*));
      if (!command_buffers) {
		free(command_buffers_backup);
        fprintf(stderr, "shell: Cannot allocate memory\n");
        exit(EXIT_FAILURE);
      }
    }

    command_buffer = strtok(NULL,  CB_DELIM);
  }
  command_buffers[position] = NULL;
  return command_buffers;
}


int main(int argc, char **argv){

    char *line;
  char **args;
  int exit_status=1;

  while(exit_status) {
    printf("\n:> %s \n",getcwd(currentDirectory, 1024));
    printf("$ ");
    line = read_input();
    args = split_input(line);
    exit_status = execute_command(args);
    free(line);
    free(args);
  } 
 

  return EXIT_SUCCESS;
}

