# 4061-proj1
The first project for CSCI 4061 which replicates fork, exec, and wait functionality native to UNIX.

The description for this project can be found [here.](https://canvas.umn.edu/courses/98151/assignments/497913)

|Members|x500    |
|-------|--------|
|Chase  |joh13266|
|Clayton|joh13124|
|Modou  |jawxx001|

### Contributions:

* Chase-> Spearheaded and completed most of the coding
* Clayton-> wrote the README
* Modou-> Ran exhaustive tests

### Known Bugs:

### Test Cases Used:

*********************************************************************
```C
./myBashProj -p /bin/cat/ file1.txt "&" /bin/cat/ file2.txt "&" /bin/cat/ file3.txt

./myBashProj -p /bin/mkdir dir "&" /bin/cat/ file2.txt "&" whatis mkdir  "&" ./ans.o 5

./myBashProj /bin/cat/ file1.txt file2.txt "&" ./ans.o 10 "&" echo "Hello"

./myBashProj /bin/cat/ file1.txt file2.txt "&" ./ans.o 10 "&&" echo "Hello"

./myBashProj /bin/cat file1.txt file2.txt "&&" ./ans.o "&" /bin/echo "hello"

./myBashProj -p /bin/cat file1.txt "&" ./ans.o 5 "&" /bin/echo "hello"

./myBashProj grep text file1.txt "&&" /bin/cat/ file2.txt && cd NewFolder

./myBashProj /bin/cat/ file1.txt "&" /bin/cat/ file2.txt "&" /bin/cat/ test3.txt

./myBashProj /bin/mkdir newdir "&&" /bin/cat/ file2.txt "&" ls

./myBashProj /bin/mkdir newdir "&&" /bin/cat/ file2.txt "&" whatis ls "&" ./ans.o 5

./myBashProj /bin/mkdir newdir "&&" /bin/cat/ file2.txt "&" whatis /bin/cat/ "&&" /bin/cat/ test3.txt

./myBashProj -p /bin/cat file1.txt file2.txt  "&" ./ans.o 5 "&" /bin/echo “hello”

./myBashProj /bin/cat/ file1.txt file2.txt "&&" ./ans.o "&" echo "hello"
```
****************************************************************************************

### Extra Credit Attempted:

False

### Instructions For Running Code:

The users runs the program from the command line, passing in arguments.

Start by typing: `./myBashProj`

A user can then pass in arguments, which are other executable and their arguments. The path to the
executable must be included if it isn't in the current directory. Otherwise, using
`./(executable file name).o` will work. If a user wishes to execute more than one executable, they may
separate them by typing "&" after an executables arguments.

A user may also specify if they want their executables to run in parallel, or sequentially. By default,
the executables will run sequentially. To specify to execute in parallel, the modifier `-p` is added after
`./myBashProj`, but before the arguments. Below are examples (given by the project description found above).

#### Sequentially:

`./myBashProj /bin/cat file1 file2 "&" ./btree.o 10 "&" /bin/echo "Hello!"`

#### Parallel:

`bash> ./myBashProj -p /bin/cat file1 file2 "&" ./btree.o 10 "&" /bin/echo "Hello!"`
