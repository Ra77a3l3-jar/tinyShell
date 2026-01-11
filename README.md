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

### Built-in Commands

Some commands are built directly into the shell instead of being external programs:

**`cd [directory]`**
- Changes the current directory using `chdir()`
- If no directory is provided, goes to HOME directory using `getenv("HOME")`

**`exit [status]`**
- Exits the shell with an optional status code
- Uses `exit()` system call

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

### Input Parsing

The shell uses `strtok_r()` to split your input into separate words (tokens):
- Splits by spaces and newlines
- First token is the command
- Rest are arguments
- Stores everything in an array that ends with `NULL`

Example: `ls -la /home` becomes `["ls", "-la", "/home", NULL]`

### Prompt Display

The prompt shows:
- Your username (using `getpwuid()` and `getuid()`)
- Current folder name (extracted from `getcwd()`)
- Colors using ANSI escape codes

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
@raffaele ➜ Documents : ls
file1.txt  file2.txt  projects

@raffaele ➜ Documents : cd projects

@raffaele ➜ projects : pwd
/home/raffaele/Documents/projects

@raffaele ➜ projects : ls -la | grep txt
-rw-r--r-- 1 raffaele raffaele  1234 Jan 10 10:30 notes.txt
-rw-r--r-- 1 raffaele raffaele  5678 Jan 10 11:45 readme.txt

@raffaele ➜ projects : exit
```
