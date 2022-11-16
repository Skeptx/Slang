slang: int/libslang.a server client

int/libslang.a: libslang.cpp libslang.h
	g++ libslang.cpp -c -o int/libslang.o $(flags)
	ar rcs int/libslang.a int/libslang.o

server: int/libslang.a server.cpp
	g++ server.cpp -c -o int/server.o $(flags)
	g++ int/server.o -L int -l slang -o server $(flags)

client: int/libslang.a client.cpp
	g++ client.cpp -c -o int/client.o -lcurses $(flags)
	g++ int/client.o -L int -l slang -o client -lcurses $(flags)