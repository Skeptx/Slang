slang: library server client

../int/library.a: library/*.cpp library/*.h
	rm -f *.o
	g++ library/*.cpp -c $(flags)
	rm -f ../int/*.o
	mv *.o ../int
	ar rcs ../int/library.a ../int/*.o

server: ../int/library.a server/server.cpp
	g++ server/server.cpp -c -o ../int/server.o $(flags)
	g++ ../int/server.o ../int/library.o -o ../dev/server.out $(flags)

client: ../int/library.a client/client.cpp
	g++ client/client.cpp -c -o ../int/client.o -lcurses $(flags)
	g++ ../int/client.o ../int/library.o -o ../dev/client.out -lcurses $(flags)