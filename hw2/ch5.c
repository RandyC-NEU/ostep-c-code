#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/wait.h>

void test_fork_change_x(void)
{
    int x = 100;
    int rc = fork();
    if (rc == 0)
    {
        printf("%s: (p:%d)c:%d\n", "Child process", getpid(), rc);
        printf("%s: %d\n", "val(x) in child on fork", x);
        x = 105;
        printf("%s: %d\n", "val(x) in child after set", x);
    }
    else if (rc > 0)
    {
        printf("%s: (p:%d)c:%d\n", "Parent process", getpid(), rc);
        printf("%s: %d\n", "val(x) in parent on fork", x);
        x = 501;
        printf("%s: %d\n", "val(x) in parent after set", x);
        //(void)waitpid(rc, NULL, 0);
    }
    else
    {
        fprintf(stderr, "Fork failed");
        exit(rc);
    }
    exit(0);
}

void test_fork_read_fds(void)
{
    int fd = open("scratch.txt", O_RDWR);
    int rc = fork();
    if (rc == 0)
    {
        printf("%s: (p:%d)c:%d\n", "Child process", getpid(), rc);
        char ch;
        ssize_t n_bytes = read(fd, &ch, 1);
        if (n_bytes > 0)
        {
            printf("%s: %c\n", "read one char from fd in child", ch);
        }
        n_bytes = write(fd, "W", 1);
        if (n_bytes > 0)
        {
            printf("%s\n", "wrote W from child");
        }
    }
    else if (rc > 0)
    {
        printf("%s: (p:%d)c:%d\n", "Parent process", getpid(), rc);
        char ch;
        ssize_t n_bytes = read(fd, &ch, 1);
        if (n_bytes > 0)
        {
            printf("%s: %c\n", "read one char from fd in parent", ch);
        }
        n_bytes = write(fd, "X", 1);
        if (n_bytes > 0)
        {
            printf("%s\n", "wrote X from parent");
        }
        // (void)waitpid(rc, NULL, 0);
    }
    else
    {
        fprintf(stderr, "Fork failed");
        exit(rc);
    }
    exit(0);
}

void print_goodbye(int _)
{
    printf("%s\n", "Goodbye!");
    exit(0);
}
void test_signal_spinlock(void)
{
    int rc = fork();
    if (rc == 0)
    {
        printf("%s\n", "Hello!");
        kill(getppid(), SIGUSR1);
    }
    else if (rc > 0)
    {
        signal(SIGUSR1, print_goodbye);
        while(1);
    }
    else
    {
        fprintf(stderr, "Fork failed");
        exit(rc);
    }
    exit(0);
}

void test_fork_exec(void)
{
    int rc = fork();
    if (rc == 0)
    {
        printf("%s\n", "Child becoming ls on cwd");
        int rc = execl("/bin/ls", "ls", (char*)NULL);
        if (rc < 0)
        {
            fprintf(stderr, "%s", "Exec(l) on child failed :(");
        }
    }
    else if (rc > 0)
    {
        printf("%s\n", "Parent becoming ls on /usr dir");
        char* args[3] = { "ls", "/usr", (char*)NULL };
        int rc = execv("/bin/ls", args);
        if (rc < 0)
        {
            fprintf(stderr, "%s", "Exec(v) on parent failed :(");
        }
    }
    else
    {
        fprintf(stderr, "Fork failed");
        exit(rc);
    }

    exit(0);
}

void test_fork_with_wait(void)
{
    int rc = fork();
    if (rc == 0)
    {
        printf("%s: (p:%d)c:%d\n", "Child process", getpid(), rc);
        int pid = wait(NULL);
        if ((pid == -1) && (errno == ECHILD))
        {
            printf("%s\n", "Expected error hit in child (ECHILD)");
        }
    }
    else if (rc > 0)
    {
        printf("%s: (p:%d)c:%d\n", "Parent process", getpid(), rc);
        int pid = wait(NULL);
        if (pid == rc)
        {
            printf("%s\n", "Parent recieved correct pid");
        }
    }
    else
    {
        fprintf(stderr, "Fork failed");
        exit(rc);
    }
    exit(0);

}

void test_fork_with_waitpid(void)
{
    int rc = fork();
    if (rc == 0)
    {
        printf("%s: (p:%d)c:%d\n", "Child process", getpid(), rc);
        int pid = waitpid(rc, NULL, 0);
        if ((pid == -1) && (errno == ECHILD))
        {
            printf("%s\n", "Expected error hit in child (ECHILD)");
        }
    }
    else if (rc > 0)
    {
        printf("%s: (p:%d)c:%d\n", "Parent process", getpid(), rc);
        int pid = waitpid(rc, NULL, 0);
        if (pid == rc)
        {
            printf("%s\n", "Parent recieved correct pid");
        }
    }
    else
    {
        fprintf(stderr, "Fork failed");
        exit(rc);
    }
    exit(0);
}

void test_fork_closing_stdout(void)
{
    int rc = fork();
    if (rc == 0)
    {
        close(STDOUT_FILENO);
        printf("%s: (p:%d)c:%d\n", "Child process", getpid(), rc);
    }
    else if (rc > 0)
    {
        (void)waitpid(rc, NULL, 0);
    }
    else
    {
        fprintf(stderr, "Fork failed");
        exit(rc);
    }
    exit(0);
}

void test_fork_pipe_two_children(void)
{
    int fds[2] = {-1, -1}; // 0 = read fd, 1 = write fd
    int rc = pipe(fds);

    if (rc < 0)
    {
        fprintf(stderr, "%s\n", "Could not open pipes :(");
    }

    int pid1 = fork();
    if (pid1 == 0)
    {
        const char* msg = "Hello child!";
        int n_bytes = write(fds[1], msg, strlen(msg));
        if (n_bytes > 0)
        {
            printf("Message sent on %d:  %s\n", getpid(), msg);
        }
        exit(0);

    }
    else if (pid1 > 0)
    {
        int pid2 = fork();
        if (pid2 == 0)
        {
            char msg_buf[64];
            int n_bytes = read(fds[0], &msg_buf[0], sizeof(msg_buf));
            if (n_bytes > 0)
            {
                msg_buf[n_bytes] = '\0';
                printf("Message recieved on %d: %s\n", getpid(), &msg_buf[0]);
            }
        }
        else if (pid2 > 0)
        {
            (void)waitpid(pid2, NULL, 0);
        }
        else
        {
            fprintf(stderr, "Fork failed");
            exit(pid2);
        }
        exit(0);
    }
    else
    {
        fprintf(stderr, "Fork failed");
        exit(pid1);
    }
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("%s\n", "Usage: ./ch5 <question_number{1-5}>");
        exit(-1);
    }
    else
    {
        long q = strtol(argv[1], NULL, 10);
        switch (q)
        {
            case 1:
            {
                test_fork_change_x();
                break;
            }
            case 2:
            {
                test_fork_read_fds();
                break;
            }
            case 3:
            {
                test_signal_spinlock();
                break;
            }
            case 4:
            {
                test_fork_exec();
                break;
            }
            case 5:
            {
                test_fork_with_wait();
                break;
            }
            case 6:
            {
                test_fork_with_waitpid();
                break;
            }
            case 7:
            {
                test_fork_closing_stdout();
                break;
            }
            case 8:
            {
                test_fork_pipe_two_children();
                break;
            }
            default:
            {
                fprintf(stderr, "%s: %s\n", "Invalid option [must be 1-5]:", argv[1]);
                exit(-1);
            }

        }
    }

    return 0;
}
