Project 0 - VM setup, simple C programs
====================================================

> The goal of this assignment is to setup the development environment for this course, to interact with GitHub Classroom, and to ensure than students can implement, compile, test, and submit a program written in C.

The assignment has three parts, described in the section below:

-   setting up a virtual machine
-   setting up Github classroom
-   implementing a simple cipher (in C)
-   writing a linked queue

This assignment must be done individually.

Part 1 -- Setting up the Virtual Machine
----------------------------------------

In this course, all of our programming assignments will be implemented in the C programming language and will be assessed on a Linux machine running **Ubuntu 20.04 LTS** (LTS refers to *long-term support* which means that this is the current stable release of Ubuntu). We recommend that you develop in this environment to ensure that your assignments all work as expected for the graders. To this end, this first part of the assignment, you are asked to build a virtual machine with the development environment expected for this class.

A *virtual machine* is an emulation of a *target* computer system that is running on top of a *host*. There are several available programs that make this possible but we are going to recommend that you download and install [VirtualBox](https://www.virtualbox.org/), which runs on Windows, Linux, and MacOS and has been successfully used in this class for many years.

First, you need to download VirtualBox from [their download page](https://www.virtualbox.org/wiki/Downloads). Be sure to download the package that is appropriate for your system. Once it has downloaded, install it by double clicking on the installer and following the prompts. The default settings for installation are generally the right ones.

Next you need to download the target operating system -- Ubuntu 20.04 LTS by going to <https://ubuntu.com/download/desktop>. This will download an .ISO file to your computer. An ISO file is an image of the data on an optical disc, and it will appear to the virtual machine as a (virtual) CD/DVD-ROM drive.

The system requirements for Ubuntu 20.04 are on the download page but include:

-   2 GHz dual-core processor or better
-   4 GB system memory
-   25 GB of free hard drive space

Finally, follow along with the following video to setup and configuration of an Ubuntu virtual machine. You will need to pause the video as you go along.

[VIDEO](https://northeastern.hosted.panopto.com/Panopto/Pages/Viewer.aspx?id=b820a4c5-c987-4580-b1aa-ac2a00cbbd10&start=undefined)

By the end of this video, you will have correctly created and setup an Ubuntu virtual machine for use in this class and written a simple "Hello World" program.

Part 2 -- Setting up Github Classroom
-------------------------------------

We are using Github Classroom to manage assignments in this class. Northeastern has single-sign on for Github. By connecting your account, or creating a new account with your Northeastern credentials, you will be able to use the links in this class without signing on. The first time you use one of the links you will be asked to associate your Github account with Northeastern and your name. This is important so that we know who you are and you get credit for your assignments.

Your assignment will be submitted by pushing your solution to the repository that is automatically created for you when you accept the assignment on GitHub Classroom. Each assignment will contain a link that you will use to accept the assignment. You can find that link at the top of this section.

Use this repository to save your work often by committing and pushing your work. You must do this at **end of every day that you work on it** or (preferably) more frequently. **Include a git message which describes what you have done.** (If you forget to add a message, you can add one with the `git commit --amend` command; if you have already pushed your commit you need to use `git push --force` to push the amended comment.)

**If you do not push your code sufficiently frequently, or provide comments describing your work, you may lose points.** Note that code similarity across submissions, a lack of understanding of your submitted code (as determined from commit comments), or evidence of variable changed by search-and-replace may be considered evidence of academic dishonesty. (If you decide that you really hate the names that you put in your code, change them, but be sure to include that information in your commit message.)

Part 3 -- Ciphers
-----------------

Encryption is a method by which information is converted into secret code that hides the information's true meaning. The science of encrypting and decrypting information is called *cryptography*. In computing, unencrypted data is also known as *plaintext* while encrypted data is called *ciphertext*. The formulas used to encode and decode messages are called *encryption algorithms* or *ciphers*.

To be effective, a cipher includes a variable as part of the algorithm. The variable, which is called a *key*, is what makes a cipher's output unique. When an encrypted messages is intercepted by an unauthorized entity, the intruder has to guess which cipher the sender used to encrypt the message, as well as what key(s) were used as variables. The time it takes to guess this information is what makes encryption such a valuable security tool.

The [Caesar Cipher](https://www.braingle.com/brainteasers/codes/caesar.php#form) is one of the earliest and simplest of ciphers. It takes any integer value as a *key* and replaces each letter of the secret message with a different letter of the alphabet which is *key* positions further away in the alphabet. For example, if *key = 3*, then A is encoded by D, B is encoded by E, ... X is encoded by A, Y is encoded by B, and Z is encoded by C.

### What to do

**File:** caesar.c

-   Write a function `encode(const char *plaintext, int key)` that encodes the string `plaintext` using the Caesar cipher by shifting characters by `key` positions. It must produce results that are in all upper case.
-   Write a function `decode(const char *ciphertext, int key)` that decodes the `ciphertext` string using the Caesar cipher by shifting the characters back by `key` positions. It must produce results that are in all upper case.
-   Write a `main` function that exercises all four of these functions to demonstrate that your code works correctly.

You have been provided with a "Makefile" to compile your solution; using it is described below.

Part 4 -- Linked Queue
----------------------

In operating systems, queues are used frequently. For example, processes are represented using a specific data structure that contains all the information related to the process (identifier, etc). The scheduler keeps track of processes to schedule using a linked queue.

#### What to do:

**Files:** queue.h, queue.c

-   Define a type `process_t` that represents a process that has both an identifier (`int`) and a name (`char *`). We will be testing our queue using items of this type, though we will keep the implementation of the queue more generic.
-   Define a type `queue_t` that represents a queue that can hold anything (hint, the data in the node must be of type `void *`).
-   Define a function `enqueue(queue_t *queue, void *element)` that can adds `element` to the end of the `queue`.
-   Define a function `void * dequeue(queue_t *queue)` that removes and returns the element at the front of the `queue`.
-   Write a `main` function that exercises the linked queue by using `process_t` elements. It must create and add several `process_t` elements to thoroughly exercise the queue. In order to show this, you must print a one-line description of what must be in the queue along with the contents of the queue in between steps.

Several things to keep in mind:

-   C does not have classes, it is a procedural language, but it does have `struct` for grouping together several data items into a new type.
-   Types can be given new names using `typedef`. By convention, they are usually named ending with a `_t`.
-   Your implementation may need other structures or functions.

How to compile your code
------------------------

You are given a Makefile with rules for compiling your files:

- `make caesar` will compile `caesar.c` to the executable `caesar`
- `make queue` will compile  `queue.c` to `queue`
- `make all` (or just `make`) will compile both
- `make clean` will delete any compiler output

To run the executables, use the commands `./caesar` and `./queue` respectively.

What to submit
--------------

There is no need to do anything special to submit your assignment. Simply create, add, commit and push your `caesar.c`, `queue.h`, and `queue.c` files into your repository before the deadline. We will grade whatever is in your repository at the time of the deadline.

Grading Criteria
----------------

You will be graded on:

-   The structure and organization of your code. If you have a preferred C-style for formatting syntax, then stick to that. Otherwise, I prefer a style as follows:

        void foo(int number) 
        {
            if (number > 3) {
                printf("number is %d\n", number);
            }
        }

    where the opening `{` is on the line after a function but on the same line as `if`/`then`/`while`/..., the indentation is 4 spaces, and there is a space surrounding operators, and after commas in parameter lists. Please be consistent in how you structure your code - take some pride in your work, and don't submit ugly code.

-   Whether or not your code can be easily understood - i.e. does it use sensible (descriptive) names for functions and variables? Is the logic reasonable, and not excessively complicated or unwieldy? Is your logic obscured by big blocks of commented-out code? (remember, comments are for comments, not for code...)

-   Correctness of your implementation - when we test it, does it perform the function you were asked to implement?

-  Compiler warnings. Most compiler warnings are because you're doing something incorrect; just because incorrect code gets the right answer doesn't mean it deserves full points.

