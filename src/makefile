host = acad.kutztown.edu
port = 46257

slang: ../int ../int/slanglib.a server client

restart: stop start

start: slang-server
	./slang-server $(port) &

connect: slang-client
	./slang-client $(host) $(port) $(args)

stop:
	pkill -9 slang-server 2>/dev/null || :
	pkill -9 slang-server 2>/dev/null || :

../int:
	mkdir -p ../int

../int/slanglib.a: ../int slanglib.cpp slanglib.h
	g++ slanglib.cpp -c -o ../int/slanglib.o -l curses -pthread $(flags)
	ar rcs ../int/libslang.a ../int/slanglib.o

server: ../int ../int/slanglib.a server.cpp
	g++ server.cpp -c -o ../int/server.o -pthread $(flags)
	g++ ../int/server.o -o slang-server -L ../int -l slang -pthread $(flags)

client: ../int ../int/slanglib.a client.cpp
	g++ client.cpp -c -o ../int/client.o -l curses -pthread $(flags)
	g++ ../int/client.o -o slang-client -l curses -L ../int -l slang -pthread $(flags)