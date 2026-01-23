# tShell

A simple shell made in C - educational project to understand how shells work.

## How It Works

This shell demonstrates the core concepts of how command-line shells like bash work under the hood.

### Main Loop

The shell runs in an infinite loop that:
1. Shows a prompt with your username and current folder
2. Reads your input
3. Parses the input into commands and arguments
4. Executes the command
5. Repeats

### Key System Calls Used

#### `getcwd()` - Get Current Working Directory
Gets the full path of the directory you're currently in. Used to show the current folder in the prompt.

```c
char *cwd = malloc(sizeof(char) * MAX_CWD);
getcwd(cwd, MAX_CWD);
```

#### `chdir()` - Change Directory
Changes the current working directory. This is what the `cd` command uses internally.

```c
if(chdir(path) != 0) {
    perror("cd");
}
```

#### `fork()` - Create a New Process
Creates a copy of the current process. Returns:
- `0` in the child process
- greater than `0` for the parent process
- `-1` if it fails

```c
pid_t pid = fork();
```

#### `execvp()` - Execute a Program
Replaces the current child process with a new program.
This is how external commands like `ls`, `cat`, `gcc` are run.

```c
execvp(argv[0], argv);
// If execvp returns, it means it failed
perror("execvp");
exit(1);
```

#### `wait()` - Wait for Child Process
Makes the parent process wait until the child process finishes.
This prevents the prompt from showing up before your command completes.

```c
wait(NULL);
```

#### `pipe()` - Create a Pipe
Creates a pipe, which is like a queue for inter-process communication. A pipe has two ends:
- **Write End**: Where one process writes its output (stdout)
- **Read End**: Where the next process reads what the previous process wrote

```c
int pipefd[2];
pipe(pipefd);
```

#### `dup2()` - Duplicate File Descriptor
Redirects input/output by copying one file descriptor to another. Used to redirect a process's stdin or stdout to a pipe.

```c
dup2(pipefd[1], STDOUT_FILENO);
```

#### `close()` - Close File Descriptor
Closes a file descriptor (like a pipe end) when it's no longer needed. This is important to signal end-of-data in pipes.

```c
close(pipefd[0]);
```

#### `signal()` - Signal Handling
Manages how the shell responds to signals like Ctrl+C (SIGINT). Used to prevent the shell from exiting when you press Ctrl+C.

```c
signal(SIGINT, SIG_IGN);  // Ignore SIGINT in parent
signal(SIGINT, SIG_DFL);  // Default behavior in child
```

#### `setenv()` - Set Environment Variable
Creates or updates an environment variable.
The third argument controls whether the variable can be overwritten:
non-zero allows overwriting, 0 prevents overwriting if the variable already exists.

```c
setenv("VAR", "value", 1);  // 1 = allow overwrite
setenv("VAR", "value", 0);  // 0 = don't overwrite if exists
```

#### `unsetenv()` - Remove Environment Variable
Deletes an environment variable from the environment.

```c
unsetenv("VAR");
```

#### `getenv()` - Get Environment Variable
Retrieves the value of an environment variable.

```c
char *value = getenv("HOME");
```

#### `tcgetattr()` / `tcsetattr()` - Terminal Attribute Control
Gets and sets terminal attributes, used for raw mode input to handle arrow keys and special characters.

```c
struct termios old_tio, new_tio;
tcgetattr(STDIN_FILENO, &old_tio);  // Get current attributes
new_tio = old_tio;
new_tio.c_lflag &= ~(ICANON | ECHO);  // Disable canonical mode and echo
tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);  // Apply new attributes
```

### Built-in Commands

Some commands are built directly into the shell instead of being external programs:

**`cd [directory]`**
- Changes the current directory using `chdir()`
- If no directory is provided, goes to HOME directory using `getenv("HOME")`

**`exit [status]`**
- Exits the shell with an optional status code
- Uses `exit()` system call

**`pwd`**
- Prints the current working directory
- Uses `getcwd()` to get the full path

**`echo [args...]`**
- Prints arguments to stdout
- Supports environment variables like `echo $HOME`

**`export VAR=value`**
- Sets an environment variable
- Uses `setenv()` to create or update variables
- Variables persist for the shell session

**`unset VAR`**
- Removes an environment variable
- Uses `unsetenv()` to delete the variable

**`history`**
- Displays command history
- Shows all previously executed commands
- Each entry is numbered for reference

### External Commands

For commands that aren't built-in (like `ls`, `cat`, `echo`), the shell:

1. Creates a new process with `fork()`
2. In the child process, runs `execvp()` to execute the command
3. In the parent process, waits with `wait()` for the child to finish

### Pipes

Pipes allow you to chain commands together by connecting the output of one command to the input of another.

Example: `ls -la | grep txt | wc -l`

#### How Pipes Work

A pipe acts like a queue with two ends:
- **Write End**: One process writes its stdout here
- **Read End**: The next process reads from here as its stdin

When you use the `|` operator, the shell:

1. Creates pipes for inter-process communication using `pipe()`
2. The parent process creates multiple child processes (one for each command)
3. Each child process runs concurrently
4. Uses `dup2()` to redirect:
   - The first command's stdout to the pipe's write end
   - The next command's stdin to the pipe's read end
5. Closes unused pipe ends with `close()` to prevent deadlocks
6. Each child executes its command with `execvp()`
7. The parent waits for all children to finish

#### Process Model

The shell uses a single parent process that spawns multiple child processes.
These child processes run concurrently, with data flowing through pipes from one to the next.
The parent process manages all children and waits for them to complete.

### Signal Handling

The shell handles signals to improve user experience:

**Ctrl+C (SIGINT)**: The parent shell process ignores SIGINT using `signal(SIGINT, SIG_IGN)`,
so pressing Ctrl+C won't exit the shell.
However, child processes use default signal handling with `signal(SIGINT, SIG_DFL)`,
allowing you to stop running commands without killing the shell.

**How it works**:
1. When you press Ctrl+C, the kernel sends SIGINT to all processes in the foreground process group
2. The shell ignores it and stays running
3. Child processes receive it and terminate normally
4. The parent cleans up and shows the prompt again

### Input Parsing

The shell uses `strtok_r()` to split your input into separate words (tokens):
- Splits by spaces and newlines
- First token is the command
- Rest are arguments
- Stores everything in an array that ends with `NULL`
- Handles environment variable expansion (e.g., `$HOME` gets replaced with its value)
- Supports command chaining with `;` to execute multiple commands sequentially

Example: `ls -la /home` becomes `["ls", "-la", "/home", NULL]`

#### Command Chaining with `;`

You can run multiple commands in sequence by separating them with semicolons.

Example: `cd /home; ls; pwd`

When you use the `;` operator, the shell:
1. Splits the input by `;` using `strtok_r()`
2. Executes each command one after another
3. Each command runs independently - if one fails, the next still runs
4. All commands execute in the same shell session, so changes (like `cd`) persist

#### Environment Variable Expansion

When you type something like `echo $HOME`, the shell:
1. Detects the `$` prefix in the token
2. Uses `getenv()` to look up the variable's value
3. Replaces the token with the actual value
4. If the variable doesn't exist, it's replaced with an empty string

### Prompt Display

The prompt shows:
- Your username (using `getpwuid()` and `getuid()`)
- Current folder name (extracted from `getcwd()`)
- Git branch name (if in a git repository)
- Colors using ANSI escape codes

The git branch detection works by:
1. Running `git rev-parse --abbrev-ref HEAD` using `popen()`
2. Reading the output to get the current branch name
3. Displaying it in the prompt if successful

### Command History

The shell maintains a history of previously executed commands:
- Stores commands in a circular buffer with dynamic growth
- Navigate through history using arrow keys (up/down)
- Automatically filters duplicate consecutive commands
- Access history using the `history` builtin command

#### Raw Mode Terminal Input

To support arrow key navigation, the shell uses raw mode terminal input:
1. Reads input character by character using `read()`
2. Detects ANSI escape sequences for arrow keys (`\033[A` for up, `\033[B` for down)

When you press the up arrow:
1. The shell saves your current input to a scratch buffer (if any)
2. Retrieves the previous command from history
3. Clears the current line using backspace sequences (`\b \b`)
4. Displays the historical command

When you press the down arrow:
1. The shell moves forward in history
2. If you reach the end, it restores your original input from the scratch buffer
3. Otherwise, it displays the next command in history

## Build

```bash
make release
```

## Run

```bash
./tintShell_release
```

## Example Usage

```bash
@raffaele ➜ Documents  ls
file1.txt  file2.txt  projects

@raffaele ➜ Documents  cd projects

@raffaele ➜ projects git(main)  pwd
/home/raffaele/Documents/projects

@raffaele ➜ projects git(main)  echo $HOME
/home/raffaele

@raffaele ➜ projects git(main)  export MY_VAR=hello

@raffaele ➜ projects git(main)  echo $MY_VAR
hello

@raffaele ➜ projects git(main)  ls -la | grep txt
-rw-r--r-- 1 raffaele raffaele  1234 Jan 10 10:30 notes.txt
-rw-r--r-- 1 raffaele raffaele  5678 Jan 10 11:45 readme.txt

@raffaele ➜ projects git(main)  cd /home; pwd; echo $USER
/home
raffaele

@raffaele ➜ projects git(main)  unset MY_VAR

@raffaele ➜ projects git(main)  history
1  cd projects
2  pwd
3  echo $HOME
4  export MY_VAR=hello
5  echo $MY_VAR
6  ls -la | grep txt

@raffaele ➜ projects git(main)  exit
```
