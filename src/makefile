host = acad.kutztown.edu
port = 46257

slang: ../int ../int/libslang.a slang-server slang-client deploy

beta: ../int ../int/libslang.a slang-server slang-client

start: ../slang-server
	../slang-server &

connect: ../slang-server ../slang
	../slang $(host) $(port) $(args)

restart:
	pkill -9 slang-server 2>/dev/null || :
	pkill -9 slang-server 2>/dev/null || :

../int:
	mkdir -p ../int

../int/libslang.a: ../int libslang.cpp libslang.h
	g++ libslang.cpp -c -o ../int/libslang.o $(flags)
	ar rcs ../int/libslang.a ../int/libslang.o

slang-server: ../int ../int/libslang.a server.cpp
	g++ server.cpp -c -o ../int/server.o $(flags)
	g++ ../int/server.o -o slang-server -L ../int -l slang $(flags)

slang-client: ../int ../int/libslang.a client.cpp
	g++ client.cpp -c -o ../int/client.o -l curses $(flags)
	g++ ../int/client.o -o slang-client -l curses -L ../int -l slang $(flags)

deploy:
	cp -r slang-server ../slang-server 2>/dev/null || :
	cp -r slang-client slang 2>/dev/null || :
	mv -f slang ../slang