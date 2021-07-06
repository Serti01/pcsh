#include "pcsh.h"

#define ARGHELP printf(VERSION " usage\n\
\n\
--help    Shows this help page\n\
--version Shows the version\n\
\n\
Sharing is caring, no copyright.\n")

// Runtime variables
char *input;
char *prompt;

/*
the parent forks children that prompt for a command and exec it
*/

void _beforexit() {
  unlink("/tmp/pcsh.pid");
  exit(0);
}

int main(int argc, char **argv) {
  struct stat st;
  stat("/tmp/pcsh.pid", &st);
  // parent
  if (st.st_size > 12) {
    // CLI Arguments
    for (int i = 0; i < argc; i++) {
      // FIXME: neofetch report 0.2.0-alpha /shrug
      if (cmpstr(argv[i], "--version")) {
        printf(VERSION);
        return 0;
      } else if (cmpstr(argv[i], "--help")) {
        ARGHELP;
        return 0;
      }
    }

    FILE *pidf = fopen("/tmp/pcsh.pid","w");
    char *pids = malloc(12);
    sprintf(pids,"%i",getpid());
    fwrite(pids, 1, strlen(pids), pidf);
    fclose(pidf);
    free(pids);

    atexit(_beforexit);
    signal(SIGINT, _beforexit);
    setenv("SHELL", "/usr/bin/pcsh", 1);

    while (1) {
      pid_t pid = fork();
      if (pid == 0) break;
      waitpid(pid,0,0);
    }
  }
  // child
  // every character gets put into stdin without enter being pressed
  // characters dont automatically echo back to stdout
  struct termios _told, _tnew;
  tcgetattr(STDIN_FILENO, &_told);
  _tnew = _told;
  _tnew.c_lflag &= ~(ICANON | ECHO);
  tcsetattr(STDIN_FILENO, TCSANOW, &_tnew);

  // Initialize runtime variables
  input = malloc(1024);
  // print prompt
  printf("\r%s] ", VERSION);

  // get input
  memset(input, 0, 1024);
  for (int i = 0; i < 1024; i++) {
    char c = fgetc(stdin);

    if ((c >= '!' && c < 127) || c == ' ') {
      putchar(c);
      input[i] = c;
      continue;
    }
    
    // FIXME: post-rewrite this erases everything
    if (c == '\b' || c == 127) { // backspace
      int row, col;
      getcursorpos(&row, &col);

      // FIXME: if you erase everything in the prompt the first character remains
      // example: prompt  == pcsh] []
      //          input   == h (if "hi" was erased or similar)
      if (col > strlen(prompt) + 1) {
        input[(i--)] = 0;
        printf("\b \b"); // best i can do
      }
    }
    
    // used later on
    if (c == '\e') // escape
      ;
    
    if (c == '\n'){ // enter
      putchar('\n');
      break;
    }
    i--; // this is so the i isnt incremented if no new character was added
  }

  //restore terminal and execute program
  tcsetattr(STDIN_FILENO, TCSANOW, &_told);
  pcshexec(input);

  return 0;
}


// int main(int argc, char **argv) {


//   // Ignore SIGINT
//   signal(SIGINT, SIG_IGN);
//   static struct termios _told, _tnew;
//   tcgetattr(STDIN_FILENO, &_told);
//   _tnew = _told;
//   _tnew.c_lflag &= ~(ICANON | ECHO);
//   tcsetattr(STDIN_FILENO, TCSANOW, &_tnew);
//   // tcsetattr(STDIN_FILENO, TCSANOW, &_told);

//   char *input = malloc((INPUT_LIMIT + 1) * sizeof(char));
//   char *prompt = malloc(LIMIT_SMALL * sizeof(char));
//   int row = 0, col = 0;

//   // Read the env
//   char *s_username = malloc(LIMIT_SMALL * sizeof(char));
//   char *s_hostname = malloc(LIMIT_SMALL * sizeof(char));
//   char *s_pwd = malloc(LIMIT_BIG * sizeof(char));
//   __uid_t s_uid = getuid();
//   getlogin_r(s_username, LIMIT_SMALL);
//   gethostname(s_hostname, LIMIT_SMALL);
//   getcwd(s_pwd, LIMIT_BIG);

//   // TODO: change this to the actual path, it shouldnt matter anyway

//   while (1) {
//     // TODO: add prompt/theme support
//     getcwd(s_pwd, LIMIT_BIG);
//     sprintf(prompt, "\r%s@%s %s> ", s_username, s_hostname, s_pwd);
//     printf("\r%s", prompt);

//     // TODO: support for rollback, ctrl+l, etc.
//     // By making this a for loop, we can avoid a buffer overflow because
//     // anything past INPUT_LIMIT doesnt get put into input
//     for (int i = 0; i < INPUT_LIMIT; i++) {
//       getcursorpos(&row, &col);
//       char nextc = fgetc(stdin);

//       if (nextc >= '!' && nextc != 127) {
//         input[i] = nextc;
//         putchar(nextc);
//       } else if (nextc == 10) { // return / enter
//         input[i] = '\x00';
//         putchar('\n');
//         break;
//       } else if (nextc == 27 && fgetc(stdin) == 91) { // escape
//         switch (fgetc(stdin)) {
//         case 65: // up
//           // scrollback
//           break;
//         case 66: // down
//           // scrollback
//           break;
//         // TODO: make text insert and not replace
//         // TODO: limit select to only the input, restrict boundaries
//         case 68: // left
//           i--, i--;
//           printf("\e[1D");
//           break;
//         case 67: // right
//           printf("\e[1C");
//           break;
//         }
//       } else if (nextc == ' ') {
//         input[i] = ' ';
//         putchar(' ');
//       } else if (nextc == 127) { // backspace
//         // hacky, i know.
//         if (col <= strlen(prompt) + 1)
//           printf("\e[%luG", strlen(prompt) + 1), i = 1;
//         input[(i--)] = '\x00';
//         printf("\b \b");
//         i--;
//       } else {
//         i--;
//       }
//     }

//     if (input == NULL)
//       continue;

//     // TODO: memory leak
//     char **args = malloc((LIMIT_BIG * sizeof(char)) * 100);
//     args[0] = malloc(LIMIT_BIG * sizeof(char));
//     int argsc = 0; // args count

//     // seperate args with delim ' ' and '\x00'
//     for (int i = 0, j = 0, k = 0; i <= strlen(input); i++, k++) {
//       if (input[i] != ' ' && input[i] != '\x00' && input[i] != 10) {
//         args[j][k] = input[i];
//       } else {
//         args[j][i] = '\x00';
//         k = -1;
//         j++, argsc++;
//         args[j] = malloc(LIMIT_BIG * sizeof(char));
//       }
//     }

//     // execute command
//     // TODO: aliases
//     if (cmpstr(args[0], "cd")) {
//       if (args[1] != NULL) {
//         if (chdir(args[1]) != 0)
//           perror("cd");
//       }
//       continue;
//       // FIXME: when spamming 2048 'a's into the input, typing exit afterwards
//       // resulted in "malloc(): corrupted top size"
//     } else if (cmpstr(args[0], "exit"))
//       break;

//     // TODO: variables and substitution

//     // craft a system command
//     char *cmd = malloc(LIMIT_BIG * sizeof(char));
//     for (int i = 0, j = 0, k = 0; i < strlen(input); i++, k++)
//       if (args[j][k] == ' ' || args[j][k] == '\x00')
//         j++, k = -1, cmd[i] = ' ';
//       else
//         cmd[i] = args[j][k];

//     // TODO: instead of relying on sh to execute commands, do it ourselves
//     system(cmd);

//     free(args);
//     free(cmd);
//   }

//   free(input);
//   free(prompt);
//   free(s_hostname);
//   free(s_username);
//   free(s_pwd);

//   return 0;
// }
