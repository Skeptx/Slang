# Slang
Slang is an alternative to Wordle.
Henry: CLIENT
Isaiah: LIBRARY/README
Patrick: SERVER

To compile the program, there is a makefile provided that will provide executables
for both client and server. To compile, you need to use the command 'make'


To run the server first, you will use ./slang-server followed by the port #
example: ./slang-server 46257


To run the client after the server is up and running, you will use ./slang-client
followed by hostname and port #
example: ./slang-client acad.kutztown.edu 46257


We used a static library for our Slang Program. We chose static so the library does
not have to shift with the executable within the makefile


The client user interface handles that the only inputted alphabetical characters.
Then the client will take the word inputted for a guess and put it into a set of
parenthesis. The ending parenthesis ) will let the server know how long the message
is that the client is sending over. Once the Server reads until the end of the parenthesis
or vice versa with the client, it will know that is the end of the message and stop reading

example of message example G(Auxin)
G for guess (GUESSED WORD HERE) <- end of message indicated by )


// no known issues at the moment


We utilized Discord to get into calls and be able to share our screens and ideas.
This allowed us to work for hours and be able to collaborate. We also used GitHub
to allow people to send their version of their file/files in order for the other
team members to use the most up to date code that works. We asked each other questions
about how certain functions would work and also collaborated on some functions as a
collective.
