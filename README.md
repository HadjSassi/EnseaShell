# Synthesys Practical Work Ensea in the Shell

## Question 8

we've made a condition if we find this symbol ```'|'``` then we want to run the 
code, but unfortunately we couldn't make the detection of the symbol in the input
by this line, after debuging.
```char *token = strtok(inputCopy, PIPE_SEPARATOR);```


## Question 7

In this question to redirect ```stdin``` and ```stdout```, first we need to 
make sure the presence of the symbols ```<``` or ```>```.

For Handling Input Redirection (<):

    We use open() to open the input file in read-only mode.
    We use dup2() to duplicate the file descriptor and redirect stdin to the input file.

For Handling Output Redirection (>):

    We use open() to open the output file in write-only mode, creating it if it doesn't exist, and truncating it if it does.
    We use dup2() to redirect stdout to the output file.

![img.png](img/Q7.png)

## Question 6

So to send multiple arguments to execute a command, we need to extract the 
arguments separately and then we put in the table to send it to the execvp 
that can accept multiple arguments.

In the example below, we can see that we executed multiple commands with multiple
arguments.
But for the last command example, we got No fortunes found, and that's not the
desired behaviour requested, yet this what we found even in our terminals 

![img.png](img/Q6.png)


## Question 5

Now to calculate the execution time for a process, we save the time before
creating the new process and we save the finishing time after the wait of that
process. And then we do the calculation and we print it.

![img.png](img/Q5.png)


## Question 4

Now to capture signals, we've tested this example, 
we run gedit which is continuous process, and then we've 
located its PID from a new terminal by writing ```ps -e``` command

Then we've sent a signal to this process gedit and the signal is kill
which has a signal code 15 so either we send kill or kill -15 the same 
result we'll get.

![img.png](img/Q4_1.png)

After hitting the enter button, we can see that we've captured the signal 15

![img_1.png](img/Q4_2.png)

## Question3
as you can see from the previous question, when we write exit, 
our shell is printing command not found, so we added a verification
condition to verify is the command is not exit :

```if (strcmp(inputBuffer, EXIT_KEY_WORD) != 0 && fork() == CHILD_SELF_PID) {```

but we're facing a little problem, when we execute the code and write 
exit command it shows the correct behaviour like this 

![img.png](img/Q3_1.png)

But when we press the <CTRL-D> command, it's not going on, maybe 
because it's interrupted by the process who executed our bash, before
our bash execute it.

![img.png](img/Q3_2.png)

but we know we should put the displaying of the message here in the main when interrogating commands!

```
if (!read_input(inputBuffer, BUFSIZE)) {
            display_message(FAREWELL_MSG);
            break;
        }
```

or here in the read_input function in the io.c file: 

```
int read_input(char *buffer, size_t size) {
    ssize_t bytesRead = read(STDIN_FILENO, buffer, size - 1);
    if (bytesRead > 0) {
        buffer[bytesRead - 1] = '\0';
        return 1;
    }
    display_message(FAREWELL_MSG);
    return 0;
}
```

## Question2 

When we execute the code: 
we got this behaviour ! 

![img.png](img/Q2.png)


## Question 1
When we execute the code : 
and write anything always getting the same ouput

![img.png](img/Q1.png)


BY ELLOUMI Molka & HADJ SASSI Mahdi