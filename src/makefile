slang: library server client

../int/library.a: library/*.cpp library/*.h
	g++ library/library.cpp -c -o ../int/library.o $(flags)
	ar rcs ../int/library.a ../int/library.o

server: ../int/library.a server/server.cpp
	g++ server/server.cpp -c -o ../int/server.o $(flags)
	g++ ../int/server.o ../int/library.o -o ../dev/server $(flags)
	cp ../dev/server ../dev/server.out

client: ../int/library.a client/client.cpp
	g++ client/client.cpp -c -o ../int/client.o -lcurses $(flags)
	g++ ../int/client.o ../int/library.o -o ../dev/client -lcurses $(flags)
	cp ../dev/client ../dev/client.out
