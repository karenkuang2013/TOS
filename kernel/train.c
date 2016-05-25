
#include <kernel.h>
#define TICK 10
WINDOW train_wnd;
char* train_id = "20";
char* default_speed = "5";

void clear_buffer();
void send_cmd(char* cmd, int inputLength, char* inputBuf);
void set_switch(char* switchID, char* setting);
void set_train_speed(char* speed);
void train_cmd_setting(char* str1, char* str2);
char get_contact_id(char* contactID);
int check_dead_end();
int get_contact_status(char* contactID, char* status);
int check_zamboni();
void train_go();
void train_stop();
void reverse_train();

void config1();
void config1z();
void config2z();
void config3();
void config3z();
void config4();
void config4z();

void init_train_settings();
void train_process(PROCESS self, PARAM param);
void init_train(WINDOW* wnd);


/*******************
 * helper functions*
 *******************/

/*
* This function is used to concatenate the command
* @param str1 - whole command
* @param str2 - input string
*/
void train_cmd_setting(char* str1, char* str2)
{
   int i = 0;
   int strLength = 0;
   for(strLength = 0; str1[strLength] != '\0'; strLength++);
   for(i = 0; str2[i] != '\0'; i++)
   {
      str1[i + strLength] = str2[i];
   }
   str1[i + strLength] = '\0';
}

/*
* This function is used to send commands to com1 port
* @param cmd          - fragment command
* @param inputLength  - input length
* @param inputBuf     - the buffer to store the input
*/
void send_cmd(char* cmd, int inputLength, char* inputBuf)
{
  COM_Message msg;
  char wholeCmd[20];

  wholeCmd[0] = '\0';
  train_cmd_setting(wholeCmd, cmd);
  train_cmd_setting(wholeCmd, "\015");
  msg.output_buffer = wholeCmd;
  msg.len_input_buffer = inputLength;
  msg.input_buffer = inputBuf;
  send(com_port, &msg);
}


/**
* This function is used to clear the s88 memory buffer. 
*/
void clear_buffer()
{
   char cmd[10];
   char temp;

   cmd[0] = 'R';
   cmd[1] = '\0';
   send_cmd(cmd, 0, &temp);
}


/*
* This function is used to change the position of switches
* @param switchID - switch id
* @param X  - setting of the switch
*/
void set_switch(char* switchID, char* X)
{
   //sleep(TICK);
   char cmd[10];
   char temp;

   cmd[0] = 'M';
   cmd[1] = '\0';
   train_cmd_setting(cmd, switchID);
   train_cmd_setting(cmd, X);
   send_cmd(cmd, 0, &temp);
   
}

/*
* This function is used to change the speed and direction of the train
* @param speed  - the speed of the train.
*/
void set_train_speed(char* speed)
{
   char cmd[10];
   char temp;

   cmd[0] = 'L';
   cmd[1] = '\0';
   train_cmd_setting(cmd, train_id);
   if (speed[0] != 'D') {
      train_cmd_setting(cmd, "S");
      train_cmd_setting(cmd, speed);
   } else 
      train_cmd_setting(cmd, "D");
   send_cmd(cmd, 0, &temp);
}
/*
* This function is used to get a contact ID.
* A contact is a track segment with a sensor.
* Must be preceeded by a "C"
* @param  contactID - number of a contact
* 
*/
char get_contact_id(char* contactID)
{
   char cmd[10];
   char result[3];
   clear_buffer();
   cmd[0] = 'C';
   cmd[1] = '\0';
   train_cmd_setting(cmd, contactID);
   send_cmd(cmd, 3, result);
   return result[1];
}

/**
* This function is used to check status of each dead end
* @return an integer indicated the status of a dead end
*/
int check_dead_end()
{
   char input;
   input = get_contact_id("8");

   if (get_contact_id("8") == '1')
   {
     return 1;
   }
   else if (get_contact_id("16") == '1')
   {
     return 4;
   }
   else
   {
     return 3;
   }
}

/**
* This function is used to get the status of a contact.
* @param contact ID
* @param status
* @return there is a vehicle on the contact or not.
*/
int get_contact_status(char* contactID, char* status)
{
  char input;

  while (42)
  {
    input = get_contact_id(contactID);
    if (input == status[0])
    {
       return 1;
    }
  }
  return 0;
}

/**
 * This function is used to check whether Zamboni is running
 * @return direction & Zamboni is found or not.
 */
int check_zamboni()
{
  int i;
  int found = 0;
  int direction = 0;
  wprintf(&train_wnd, "Checking if Zamboni is running...\n");

  for (i = 0; i < 30; i++) 
  {
    sleep(10);
    if (get_contact_id("7") == '1')
    {
      wprintf(&train_wnd, "Zamboni is running\n");
      found = 1;
      break;
    }
  }

  if (found) 
  {
    wprintf(&train_wnd, "Checking direction of Zamboni...\n");
    for (i = 0; i < 30; i++) 
    {
      sleep(10);
      if (get_contact_id("10") == '1') 
      {
        wprintf(&train_wnd, "Zamboni is clockwise\n");
        direction = 123;
        break;
      }
      if (get_contact_id("6") == '1') 
      {
        wprintf(&train_wnd, "Zamboni is counter-clockwise\n");
        direction = 321;
        break;
      }
    }
  }

  if (!found && !direction) 
  {
    wprintf(&train_wnd, "No Zamboni is running\n");
    return found;
  }
  return direction;
}

/*
* This function is used to make the train go
*/
void train_go()
{
  set_train_speed(default_speed);
}

/*
* This function is used to make the train stop
*/
void train_stop()
{
   set_train_speed("0");
}

/*
* This function is used to change the direction of the train
*/
void reverse_train()
{
   set_train_speed("D");
}


/****************
 *configurations*
 ****************/

/*Configuration 1 without Zamboni*/
void config1()
{
  wprintf(&train_wnd, "Running config 1 without Zamboni\n");
  set_switch("5", "R");
  set_switch("4", "R");
  set_switch("3", "G");

  set_train_speed(default_speed);
  wprintf(&train_wnd, "Train starting\n");

  get_contact_status("1", "1");

  set_train_speed("0");
  wprintf(&train_wnd, "Changing direction...\n");
  set_train_speed("D");
  set_train_speed(default_speed);

  get_contact_status("8", "1");
  set_train_speed("0");
  wprintf(&train_wnd, "Welcome home!\n");
}

/*Configuration 1 with Zamboni*/
void config1z()
{
  wprintf(&train_wnd, "Running config 1 with Zamboni\n");
  get_contact_status("14","1");
  set_switch("9", "R");
  set_switch("1", "R");
  set_switch("4", "R");
  set_switch("3", "G");
  set_train_speed(default_speed);
  wprintf(&train_wnd, "Train starting...\n");

  clear_buffer();
  get_contact_status("1", "1");

  wprintf(&train_wnd, "Waiting\n");
  set_train_speed("0");
  wprintf(&train_wnd, "Changing direction\n");
  set_train_speed("D");

  set_switch("5", "R");

  wprintf(&train_wnd, "Train restarting...\n");
  set_train_speed(default_speed);
  get_contact_status("8", "1");
  set_switch("5", "G");
  set_train_speed("0");
  wprintf(&train_wnd, "Welcome home!\n");
}

/*Configuration 2 with Zamboni*/
void config2z()
{
  wprintf(&train_wnd, "Running config 2 with Zamboni\n");
  get_contact_status("14", "1");
  set_switch("8", "R");
  wprintf(&train_wnd, "Train starting...\n");
  set_train_speed(default_speed);
  set_switch("4", "R");

  get_contact_status("1", "1");
  set_switch("4", "G");
  set_train_speed("0");
  wprintf(&train_wnd, "Changing direction...\n");
  set_train_speed("D");

  wprintf(&train_wnd, "Waiting\n");
  wprintf(&train_wnd, "Train restarting...\n");
  set_train_speed(default_speed);
  set_switch("5", "R");

  get_contact_status("8", "1");
  wprintf(&train_wnd, "Welcome home!\n");
  set_train_speed("0");
}

/*Configuration 3 without Zamboni*/
void config3()
{
  wprintf(&train_wnd, "Running config 3 without Zamboni\n");
  set_switch("5", "R");
  set_switch("6", "G");
  wprintf(&train_wnd, "Train starting...\n");
  set_train_speed(default_speed);
  
  get_contact_status("12", "1");
  set_train_speed("0");
  wprintf(&train_wnd, "Changing direction\n");
  set_train_speed("D");

  set_train_speed(default_speed);
  
  get_contact_status("13", "1");
  set_train_speed("0");
  wprintf(&train_wnd, "Changing direction\n");
  set_train_speed("D");

  set_train_speed(default_speed);

  set_switch("4", "R");
  set_switch("3", "R");

  get_contact_status("5", "1");
  wprintf(&train_wnd, "Welcome home!\n");
  set_train_speed("0");

}

/*Configuration 3 with Zamboni*/
void config3z()
{
  wprintf(&train_wnd, "Running config 3 with Zamboni\n");
  get_contact_status("10", "1");

  wprintf(&train_wnd, "Train starting...\n");
  set_train_speed(default_speed);

  get_contact_status("6", "1");
  set_switch("4", "R");
  set_switch("3", "G");
  set_train_speed("0");
  wprintf(&train_wnd, "Changing direction...\n");
  set_train_speed("D");
  set_train_speed(default_speed);

  set_switch("8", "R");

  get_contact_status("12", "1");
  set_train_speed("0");
  wprintf(&train_wnd, "Changing direction...\n");
  set_train_speed("D");
  
  set_switch("7", "G");
  get_contact_status("10","1");
  set_train_speed(default_speed);
  set_switch("3", "R");

  get_contact_status("5", "1");
  set_train_speed("0");
  wprintf(&train_wnd, "Changing direction...\n");
  set_train_speed("D");
  wprintf(&train_wnd, "Welcome home!\n");  

}

/*Configuration 4 without Zamboni*/
void config4() {
  wprintf(&train_wnd, "Running config 4 without Zamboni\n");
  set_switch("9", "G");
  wprintf(&train_wnd, "Train starting...\n");
  set_train_speed(default_speed);

  get_contact_status("14", "1");
  sleep(180);
  set_train_speed("0");
  wprintf(&train_wnd, "Changing direction...\n");
  set_train_speed("D");

  set_train_speed(default_speed);
  
  set_switch("4", "R");
  set_switch("3", "R");
  
  get_contact_status("5", "1");
  set_train_speed("0");
  wprintf(&train_wnd, "Welcome home!\n");

}

/*Configuration 4 with Zamboni*/
void config4z()
{
   wprintf(&train_wnd, "Running config 4 with Zamboni\n");
   get_contact_status("4","1");
   wprintf(&train_wnd, "Train starting...\n");
   set_train_speed(default_speed);
   //reversing direction
   get_contact_status("6","1");
   set_train_speed("0");
   set_switch("4","G");
   set_train_speed("D");

   //when Zamboni reaches switch 13, start the train
   wprintf(&train_wnd, "Waiting...\n");
   get_contact_status("13","1");
   set_train_speed(default_speed);

   //when the train reaches switch 14, reverse direction
   wprintf(&train_wnd, "Stopping train\n");
   get_contact_status("14","1");
   set_train_speed("0"); 
   set_train_speed("D");
   set_train_speed(default_speed);
   set_switch("9","G");
   sleep(168);
   
   //when the train reaches the abandoned car, stop and reverse
   wprintf(&train_wnd, "Stopping train\n");
   get_contact_status("16","1");
   set_train_speed("0");
   set_train_speed("D");
   
   //when Zamboni reaches switch,14, trap Zamboni into the circle
   get_contact_status("14","1");
   set_switch("8","R");
   set_train_speed(default_speed);
   sleep(99);
   
   //when the train reaches switch, reverse its direction
   wprintf(&train_wnd, "Stopping train\n");
   get_contact_status("14","1");
   set_train_speed("0"); 
   set_train_speed("D");
   set_switch("9","R");
   set_switch("1","G");
   set_switch("2","G");
   set_train_speed(default_speed);

   //reverse and back to home
   wprintf(&train_wnd, "Stopping train\n");
   get_contact_status("6","1");
   set_train_speed("0"); 
   set_train_speed("D");
   set_switch("4","R");
   set_switch("3","R");
   set_train_speed(default_speed);

   get_contact_status("5","1");
   set_train_speed("0");
   wprintf(&train_wnd, "Welcome home!\n");
   
   
}

//**************************
//run the train application
//**************************

void train_process(PROCESS self, PARAM param)
{
   int status;
   int zamboni;
   zamboni = 0;

   init_train_settings();
   status = check_dead_end();
   zamboni = check_zamboni();

  switch (status) 
  {
    case 1:
      if (!zamboni)
        config1();
      else if (zamboni == 123)
        config1z();
      else if (zamboni == 321)
        config2z();
      break;
    case 3:
      if (!zamboni)
        config3();
      else
        config3z();
      break;
    case 4:
      if (!zamboni)
        config4();
      else
        config4z();
      break;
    default:
      break;
   }
   remove_ready_queue(active_proc);
   resign();
   while(1);
}


void init_train_settings()
{
   wprintf(&train_wnd, "Initializing switches...Done\n");  
   set_switch("8", "G");
   set_switch("6", "R");
   set_switch("9", "R");
   set_switch("1", "G");
   set_switch("2", "R");
   set_switch("7", "R");
   set_switch("5", "G");
   set_switch("4", "G");
   set_switch("3", "G");
}

void init_train(WINDOW* wnd)
{
   create_process(train_process, 5, (PARAM) wnd, "Train process");
}
