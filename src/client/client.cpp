#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
//#include "../library/library.h"

using namespace std;

void print(int signum);
void printLine(int word);

int status;
char *history;
int *results;
char *words;
int word;
#ifndef NO_CURSES_H
WINDOW *window;
#endif

int main(int argc, char **argv) {
	string host_name = "acad.kutztown.edu";
	int port_number = 46257;
	if (argc > 3) {
		printf("\r\nUsage: %s <host_name> [port_number]\r\n\r\n", argv[0]);
		return EXIT_SUCCESS;
	} else if (argv > 1) {
		hostname = argv[1];
		if (argv > 2) {
			port = atoi(argv[2]);
		}
	}
	SlangLib slang('c', port_number, host_name);
	status = 0;
	history = (char *)malloc(sizeof(char) * 30);
	results = (int *)malloc(sizeof(int) * 35);
	words = (char *)malloc(sizeof(char) * 40);
	memset(history, 32, 30);
	memset(results, 0, 35);
	memset(words, 32, 40);
	words[30] = 'S';
	words[31] = 'L';
	words[32] = 'A';
	words[33] = 'N';
	words[34] = 'G';
	#ifdef NO_CURSES_H
	static struct termios t0, t1;
	if (tcgetattr(STDIN_FILENO, &t0)) {
		perror("\r\nError: tcgetattr failed");
		return EXIT_FAILURE;
	}
	t1 = t0;
	t1.c_lflag &= ~(ICANON + ECHO);
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &t1)) {
		perror("\r\nError: tcsetattr failed");
		return EXIT_FAILURE;
	}
	fflush(stdout);
	#else
	window = initscr();
	cbreak();
	noecho();
	keypad(stdscr, true);
	signal(SIGWINCH, print);
	print(0);
	#endif
	int input = 0;
	for (bool start = true; start; start = input != 27) {
		for (word = 0; word < 6; word++) {
			int length = 0;
			print(length);
			#ifdef NO_CURSES_H
			input = getc(stdin);
			#else
			input = wgetch(window);
			#endif
			while (input != 10 && input != 13 && input != 27) {
				#ifdef NO_CURSES_H
				if (length < 5) {
				#else
				if ((input == KEY_BACKSPACE || input == KEY_LEFT) && length > 0) {
					words[word * 5 + --length] = ' ';
				} else if (length < 5) {
				#endif
					int v = word * 5 + length;
					#ifndef NO_CURSES_H
					if (input == KEY_RIGHT && history[v] != 32) {
						length++;
						words[v] = history[v];
					} else {
					#endif
						if (input > 96 && input < 123) {
							input -= 32;
						}
						if (input > 64 && input < 91) {
							length++;
							history[v] = input;
							words[v] = input;
							if (v < 30) {
								memset(history + v + 1, 32, 30 - v);
							}
						}
					#ifndef NO_CURSES_H
					}
					#endif
				}
				print(length);
				#ifdef NO_CURSES_H
				input = getc(stdin);
				#else
				input = wgetch(window);
				#endif
			}
			if (input == 27) {
				break;
			}
		}
	}
	#ifdef NO_CURSES_H
	if (tcsetattr(STDIN_FILENO, TCSANOW, &t0)) {
		perror("\r\nError: tcsetattr failed");
		return EXIT_FAILURE;
	}
	#else
	signal(SIGWINCH, SIG_DFL);
	echo();
	if (endwin() == ERR) {
		system("reset");
	}
	#endif
	print(5);
	fputs("THANK YOU FOR PLAYING SLANG\r\n\r\n", stdout);
	free(history);
	free(results);
	free(words);
	return 0;
}

void print(int length) {
	fputs("\033[2J\033[1;1H  _____ _____ _____ _____ _____", stdout);
	printLine(30);
	fputs("\r\n\r\n   SLANG, A WORDLE ALTERNATIVE\r\n  _____ _____ _____ _____ _____", stdout);
	for (int word = 0; word < 30; word += 5) {
		printLine(word);
	}
	if (status < 3) {
		fputs("\r\n\r\n   TYPE A WORD AND PRESS ENTER\r\n\r\n   OR HIT ESCAPE TO QUIT SLANG", stdout);
		if (status == 1) {
			fputs("\r\n\r\n   WAITING FOR SERVER RESPONSE", stdout);
		} else if (status == 2) {
			fputs("\r\n\r\n   YOUR WORD IS NOT RECOGNIZED", stdout);
		}
	} else if (status < 7) {
		if (status > 4) {
			printf("\r\n\r\n   OH NO, THE WORD WAS \"%c%c%c%c%c\"", words[35], words[36], words[37], words[38], words[39]);
		} else if (status > 2) {
			fputs("\r\n\r\n   CONGRATULATIONS, YOU GOT IT", stdout);
		}
		fputs("\r\n\r\n   DO YOU WANT TO START AGAIN?\r\n\r\n   TYPE \"YES\" TO RESTART SLANG\r\n   TYPE \"NO\" IF YOU'RE CHICKEN", stdout);
		if (status % 2) {
			fputs("\r\n\r\n   PLEASE RESPOND \"YES\" / \"NO\"", stdout);
		}
	}
	if (length == 5) {
		fputs("\r\n\r\n   ", stdout);
	} else {
		printf("\033[%i;%iH", word * 3 + 9, length * 6 + 5);
	}
	fflush(stdout);
	#ifndef NO_CURSES_H
	if (!isendwin()) {
		wrefresh(window);
	}
	#endif
}

void printLine(int offset) {
	fputs("\r\n |", stdout);
	for (int letter = offset; letter < offset + 5; letter++) {
		printf(" %c%c%c |", results[letter] == 2 ? '\\' : ' ', results[letter] == 1 ? '^' : ' ', results[letter] == 2 ? '/' : ' ');
	}
	printf("\r\n |");
	for (int letter = offset; letter < offset + 5; letter++) {
		printf("%c%c%c%c%c|", results[letter] == 1 ? '<' : results[letter] == 1 ? '-' : ' ', results[letter] == 1 ? '-' : ' ', words[letter], results[letter] == 1 ? '-' : ' ', results[letter] == 1 ? '>' : results[letter] == 1 ? '-' : ' ');
	}
	printf("\r\n |");
	for (int letter = offset; letter < offset + 5; letter++) {
		printf("_%c%c%c_|", results[letter] == 2 ? '/' : '_', results[letter] == 1 ? 'v' : '_', results[letter] == 2 ? '\\' : '_');
	}
}