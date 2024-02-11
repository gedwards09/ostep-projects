#include <fcntl.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#define MaxLine (128)
#define Prompt "wish> "
#define PrintPrompt(batchMode) if (!batchMode) printf(Prompt)
#define ErrorMessage "An error has occurred\n"
#define Error() fprintf(stderr, ErrorMessage);
#define ShellArgMax (64)
#define InitialDirectory "/bin"

void setup();
void setupPath();
void setupWorkingDirectory();
int resizeBuffer();
void initShell(FILE* stream, int batchMode);
void exCommand(char* line);
void cd(int argc, char* argv[]);
void path(int argc, char* argv[]);
void execute(int argc, char* argv[]);
char* getOutputFile(int argc, char* argv[]);
int redirectOutput(char* fout);
char* getExecutable(char* arg);
int addToProcessList(int pid);
void clean();

struct node
{
	int pid;
	struct node* next;
};

/* array to hold ':'-delimited string of directories on PATH */
char _path[ShellArgMax * MaxLine];
/* Size of the buffer for the name of the working directory */
size_t _workingDirectoryBufferSize;
/* The program's current working directory */
char* _workingDirectory;
/* linked list to track pids of processes running in the background */
struct node* _backgroundProcessHead = NULL;


int main(int argc, char* argv[])
{
	int batchMode = 0;
	FILE* fp = stdin;

	if (argc > 2)
	{
		Error();
		exit(1);
	}
	else if (argc == 2)
	{
		batchMode = 1;
		fp = fopen(argv[1], "r");
		if (fp == NULL)
		{
			Error();
			exit(1);
		}
	}
	
	setup();

	initShell(fp, batchMode);

	clean();

	return 0;
}

void setup()
{
	setupPath();
	setupWorkingDirectory();
}

void setupPath()
{
	strcpy(_path, InitialDirectory);
}

void setupWorkingDirectory()
{
	_workingDirectoryBufferSize = MaxLine;
	_workingDirectory = (char*) malloc(sizeof(char) * MaxLine);
	if (_workingDirectory == NULL)
	{
		printf("error:wish.c: could not alocate memory\n");
	}
	while (getcwd(_workingDirectory, _workingDirectoryBufferSize) == NULL)
	{
		if (resizeBuffer(&_workingDirectory, &_workingDirectoryBufferSize) == 1)
		{
			exit(1);
		}
	}
}

int resizeBuffer(char** buf, size_t* size)
{
	*size *= 2;
	*buf = (char*) realloc(*buf, *size);
	if (*buf == NULL)
	{
		printf("error:wish.c: could not alocate memory\n");
		return 1;
	}
	return 0;
}

void initShell(FILE* streamin, int batchMode)
{
	char* linebuf = NULL;
	int cmdct;
	char* cmd[ShellArgMax];
	size_t linemax = 0;
	int wpid;
	int wstatus;

	while (1)
	{
		PrintPrompt(batchMode);
		getline(&linebuf, &linemax, streamin);

		cmdct = 0;
		cmd[cmdct] = strtok(linebuf, "&");
		while(cmd[cmdct] != NULL)
		{
			cmdct++;
			cmd[cmdct] = strtok(NULL, "&");
		}

		for (int i=0; i < cmdct; i++)
		{
			exCommand(cmd[i]);
		}

		//wait for all child processes to complete
		while((wpid = wait(&wstatus)) > 0) {  }
	}
}

void exCommand(char* line)
{
	int argc = 0;
	char* argv[ShellArgMax];
	
	argv[argc] = line;
	while((argv[argc] = strtok(argv[argc], " \t\n")) != NULL)
	{
		argv[++argc] = NULL;
	}

	if (argc == 0)
	{
		return;
	}

	if (strcmp(argv[0], "exit") == 0)
	{
		if (argc > 1)
		{
			Error();
		}
		else
		{
			exit(0);
		}
	}
	else if (strcmp(argv[0], "path") == 0)
	{
		path(argc, argv);
	}
	else if (strcmp(argv[0], "cd") == 0)
	{
		cd(argc, argv);
	}
	else
	{
		execute(argc, argv);
	}

}

void path(int argc, char* argv[])
{
	if (argc == 1)
	{
		_path[0] = '\0';
		return;
	}

	int i;
	int j;
	int arg;

	i = 0;
	arg = 0;
	while (++arg < argc && i < ShellArgMax * MaxLine)
	{
		j = 0;
		while((_path[i++] =  argv[arg][j++]) != '\0') {  }
		_path[i-1] = ':';
	}
	_path[i-1] = '\0';
}

void cd(int argc, char* argv[])
{
	if (argc != 2 || chdir(argv[1]) != 0)
	{
		Error();
	}
	else
	{
		strcpy(_workingDirectory, argv[1]);
	}
}

void execute(int argc, char* argv[])
{
	int pid;
	char* fout;
	char* pathname;
	
	// for each parallel process

	fout = getOutputFile(argc, argv);

	pathname = getExecutable(argv[0]);
	if (pathname == NULL)
	{
		Error();
		return;
	}

	pid = fork();
	if (pid == 0)
	{
		if (redirectOutput(fout))
		{
			exit(0);
		}

		if (execv(pathname, argv) != -1)
		{
			exit(0);
		}
	}
	else
	{
		addToProcessList(pid);
	}

	free(fout);
	free(pathname);
}

char* getExecutable(char* arg)
{
	if (arg == NULL)
	{
		return NULL;
	}

	char* path = (char*) malloc(sizeof(char) * MaxLine);
	if (path == NULL)
	{
		printf("error:wish.c: could not alocate memory\n");
	}
	
	char* pathc = path;
	char* pathp = _path;
	char* cur = arg;
	while (*pathp != '\0')
	{
		while ((*pathc++ = *pathp++) != ':' && *(pathc-1) != '\0') {  }
		*(pathc-1) = '/';
		while ((*pathc++ = *cur++) != '\0') {  }
		if (!access(path, X_OK))
		{
			return path;
		}
		pathc = path;
		cur = arg;
	}
	
	return NULL;
}

char* getOutputFile(int argc, char* argv[])
{
	char* fout = NULL;
	int i = 0;
	char* cp;
	while (i < argc)
	{
		cp = argv[i];
		while (*cp != '\0' && *cp != '>')
		{
			cp++;
		}

		if (*cp == '>')
		{
			break;
		}
		i++;
	}

	if (i == argc || *cp != '>')
	{
		return NULL;
	}

	fout = malloc(sizeof(char) * MaxLine);
	if (fout == NULL)
	{
		printf("error:wish.c: could not allocate memory\n");
	}
	// case "[...]> file"
	if (*(cp + 1) == '\0')
	{
		if (i+1 == argc-1)
		{
			strcpy(fout, argv[i+1]);
		}
		//either too many or too few arguments
		else
		{
			free(fout);
			Error();
			exit(0);
		}
	}
	// case "[...]>file"
	else
	{
		strcpy(fout, cp+1);
		if (i+1 < argc)
		{
			free(fout);
			Error();
			exit(0);
		}
	}

	// terminate argv with NULL
	// based on whether '>' is first character of string
	if (cp == argv[i])
	{
		argv[i] = NULL;
	}
	else
	{
		//terminate argv[i] before the '>'
		*cp = '\0';
		argv[i+1] = NULL;
	}

	return fout;
}

int redirectOutput(char* fout)
{
	int fd;

	if (fout != NULL)
	{
		fd = open(fout, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR);
		if (fd == -1 
			|| dup2(fd, 1) == -1 
			|| dup2(fd, 2) == -1)
		{
			return 1;
		}
	}

	return 0;
}

int addToProcessList(int pid)
{
	struct node* current = (struct node*) malloc(sizeof(struct node*));
	if (current == NULL)
	{
		printf("error:wish.c: could not allocate memory\n");
		return 1;
	}
	current->pid = pid;
	current->next = _backgroundProcessHead;
	_backgroundProcessHead = current;
	return 0;
}

void clean()
{
	struct node* last;
	struct node* current = _backgroundProcessHead;
	while (current != NULL)
	{
		kill(current->pid, SIGKILL);
		last = current;
		current = current->next;
		free(last);
	}

	free(_workingDirectory);
}

