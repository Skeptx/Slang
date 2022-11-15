slang: library server client client-no-curses

../int/library.a: library/*.cpp library/*.h
	rm -f *.o
	g++ library/*.cpp -c $(flags)
	rm -f ../int/*.o
	mv *.o ../int
	ar rcs ../int/library.a ../int/*.o

server: ../int/library.a server/server.cpp
	g++ server/server.cpp -c -o ../int/server.o $(flags)
	g++ ../int/server.o ../int/library.a -o ../dev/server $(flags) -L ../int -l library
	cp ../dev/server ../dev/server.out

client: ../int/library.a client/client.cpp
	g++ client/client.cpp -c -o ../int/client.o -lcurses $(flags)
	g++ ../int/client.o ../int/library.a -o ../dev/client -lcurses $(flags) -L ../int -l library
	cp ../dev/client ../dev/client.out

client-no-curses: ../int/library.a client/client.cpp
	g++ client/client.cpp -c -o ../int/client-no-curses.o -DNO_CURSES_H $(flags)
	g++ ../int/client-no-curses.o ../int/library.a -o ../dev/client-no-curses $(flags) -L ../int -l library
	cp ../dev/client-no-curses ../dev/client-no-curses.out