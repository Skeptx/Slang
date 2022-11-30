# Slang

### Description

Slang is an alternative to Wordle created by Henry, Isaiah, and Patrick.

### Team Leader

Henry

### Assigned Tasks

Henry: CLIENT

Isaiah: LIBRARY/README

Patrick: SERVER

### How to Compile from "src" Directory

`make slang`

### How to Run the Server from "src" Directory

`make start`

or

`./slang-server [port]`

e.g.

`./slang-server 46257`

### How to Run the Client from "src" Directory

`make connect`

or

`./slang-client <host> [port]`

e.g.

`./slang-client acad.kutztown.edu 46257`

### Design Decisions

We used a static library for the program.  We chose to use a static library so it does not have to be shipped with the executables.

The client handles only alphabetical characters.  The client then wraps your guess, (e.g. "GUESS"), in parentheses, with a 'G' prefix (e.g. "G(GUESS)").  The first character specifies the type of message (and the length is known), the parentheses allow the receiving end to make sure that the entire message is read.  It will stop reading after that.

### Known Issues

- There is no error checking for `fprintf` calls to the client's log file.  We realized this without enough time to update it before the due date.
- Some terminals cannot access the full client experience due to incompatibilities with ANSI escape codes.  Because of this, user experience will vary.  This is not something that can be standardized by an application.
- `SlangLib` is vulnerable to being deleted without cleanup if the destructor is called before `killThreads`.
- There are no other client "themes" (as specified in the `--help` menu) currently implemented other than the default black theme.

### Communication

We communicated both in person and via a Discord chat server.  Using Discord, we are able to stream our screens to each other in real time.  This allowed us to collaborate properly.  We also used GitHub to post updated versions of our files so everybody can have the most updated version of the codebase.  We collaborated on most of the codebase.  We made sure to explain to each other how our code worked so that anybody modifying it would be sure not to break anything.