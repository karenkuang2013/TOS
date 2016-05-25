#include <kernel.h>
WINDOW shell_wnd = {0, 9, 60, 16, 0, 0, 0xDC};
WINDOW pac_wnd = {61, 7, 15, 16, 0, 0, ' '};
WINDOW train_wnd = {0, 0, 50, 9, 0, 0, ' '};

char* trainID = "20";

typedef struct _input_buffer
{
   int length;
   char buf[20];
}input_buffer;

input_buffer *input;

/*
* This function is to run pacman
*/
void run_pacman(WINDOW* wnd)
{
  init_pacman(wnd, 2);
}

/*
* This function is the TOS version strcmp.
* It is used to compare the command users type in with the command in TOS shell
* @param str1
* @param str2
* @return cmp
*/
int k_strcmp(char *str1, char *str2)
{
  int cmp;
  while(*str1 != '\0' && *str2 != '\0') 
  {
    cmp = *str1++ - *str2++;
    if (cmp != 0) 
    {
      return cmp;
    }
  }
  return *str1 - *str2;
}


/*
* This function is used to skip the whitespaces from the input
* @param str - pointer to the array in which the argument need to be stored
* @return newStr
*/
char * skip_space(char *str)
{
  char *newStr = str, *in = str;
  while(*str != '\0')
  {  
    if(*str != ' ')
     *in++ = *str;
    str++;
  }
  *in = '\0';

  return newStr;
}

/*
* This function is used to remove the newline from the input buffer
* @param str
* @return newStr
*/
char * rmv_newline(char *str)
{
  char *newStr = str, *in = str;

  while(*str != '\0')
  {  
    if(*str != '\n')
     *in++ = *str;
    str++;
  }
  *in = '\0';

  return newStr;
}

/*
* This function is used to clear shell
* @param wnd
*/
void clear_shell(WINDOW *wnd)
{
   clear_window(wnd);
}





/*
* This function is used to change the position of switches
* no configuration right now, just simply change the switches
* will modify later
*/
void change_positon_switch()
{
  wprintf(&train_wnd, "changing switches...Done\n");
   set_switch("8", "R");
   set_switch("6", "G");
   set_switch("9", "G");
   set_switch("1", "R");
   set_switch("2", "G");
   set_switch("7", "G");
   set_switch("5", "R");
   set_switch("4", "R");
   set_switch("3", "G");
}


/*
*This function is used to print a help page and list all commands for users.
*/

void print_help()
{
   wprintf(&shell_wnd, "Welcome to help page!\n");
   wprintf(&shell_wnd, "Below are the commands you need. \n");
   wprintf(&shell_wnd, "help      - print a list of all commands\n");
   wprintf(&shell_wnd, "ps        - print all the processes\n");
   wprintf(&shell_wnd, "clear     - clear the shell window\n");
   wprintf(&shell_wnd, "train     - initialize the train\n");
   wprintf(&shell_wnd, "tstop     - make the train stop\n");
   wprintf(&shell_wnd, "tgo       - make the train go\n");
   wprintf(&shell_wnd, "reverse   - reverse the direction of the train\n");
   wprintf(&shell_wnd, "change    - change the positon of switches\n");
   wprintf(&shell_wnd, "startpac  - start the PacMan game\n");
}

/*
* This function is used to execute the command
* @param cmd
*/
void exe_command(char * cmd)
{
  if (k_strcmp(cmd, "clear") == 0) {
    clear_window(&shell_wnd);
  } else if (k_strcmp(cmd, "help") == 0) {
    print_help();
  } else if (k_strcmp(cmd, "ps") == 0) {
    print_all_processes(&shell_wnd);
  } else if (k_strcmp(cmd, "train") == 0) {
    clear_shell(&train_wnd);
    init_train(&train_wnd);
    init_train_settings();
  } else if (k_strcmp(cmd, "tgo") == 0) {
    train_go();
  } else if (k_strcmp(cmd, "tstop") == 0) {
    train_stop();
  } else if (k_strcmp(cmd, "reverse") == 0) {
    reverse_train();
  } else if (k_strcmp(cmd, "startpac") == 0) {
    run_pacman(&pac_wnd);
  } else if (k_strcmp(cmd, "") == 0) {
    return;
  } else if (k_strcmp(cmd, "change") == 0) {
    change_positon_switch();
  } else {
    wprintf(&shell_wnd, "command not found. use help \n");
  }  
}

/*
* This function is used to read input from the keyboard
*/
void read()
{
  char ch;
  Keyb_Message msg;
  input->length = 0;
  do
  {
    msg.key_buffer = &ch;
    send(keyb_port, &msg);
    switch (ch) 
    {
      case '\b':
        if (input->length == 0)
          continue;
        input->length--;
        wprintf(&shell_wnd, "%c", ch);
        break;
      case 13:
        wprintf(&shell_wnd, "\n");
        break;
      default:
        input->buf[input->length++] = ch;
        wprintf(&shell_wnd, "%c", ch);
        break;
    }
  } while (ch != '\n' && ch != '\r');
  input->buf[input->length] = '\0';
}

/*
* This function is used to run the shell
* @param self
* @param param
*/
void shell_process (PROCESS self, PARAM param)
{
  clear_window(&shell_wnd);

  while (1) 
  {
    wprintf(&shell_wnd, "$$ ");
    read();
    char *oldcmd;
    oldcmd = rmv_newline(&input->buf[0]);
    char *cmd;
    cmd = skip_space(oldcmd);
    exe_command(cmd);
  }
}

/*
*  Initialize the shell
*/
void init_shell()
{
   create_process(shell_process, 5, 0,"Shell Process");
   resign();
}
