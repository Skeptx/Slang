host = acad.kutztown.edu
port = 46257

.PHONY: slang
slang: ../int slang-server slang-client

.PHONY: restart
restart: stop start

.PHONY: start
start: slang-server
	./slang-server $(port) &

.PHONY: connect
connect: slang-client
	./slang-client $(host) $(port) $(args)

.PHONY: stop
stop:
	pkill -9 slang-server 2>/dev/null || :
	pkill -9 slang-server 2>/dev/null || :

../int:
	mkdir -p ../int

../int/libslang.a: slanglib.cpp slanglib.h
	g++ slanglib.cpp -c -o ../int/slanglib.o -l curses -pthread $(flags)
	ar rcs ../int/libslang.a ../int/slanglib.o

slang-server: ../int/libslang.a server.cpp
	g++ server.cpp -c -o ../int/server.o -pthread $(flags)
	g++ ../int/server.o -o slang-server -L ../int -l slang -pthread $(flags)

slang-client: ../int/libslang.a client.cpp
	g++ client.cpp -c -o ../int/client.o -l curses -pthread $(flags)
	g++ ../int/client.o -o slang-client -l curses -L ../int -l slang -pthread $(flags)