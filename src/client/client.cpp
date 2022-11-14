#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

using namespace std;

void print(int signum);
void printLine(int word);

int status;
char *history;
int *results;
char *words;
WINDOW *window;

int main(int argc, char **argv) {
	/*if (argc == 2) {
		
	} else if (argc == 3) {
		
	} else {
		printf("\r\n\r\nUsage: %s <hostname> [port] [args]\r\nArgs:\r\n-c\t: Removes the curses library and SIGWINCH signal handler\r\n", argv[0]);
	}*/
	status = 0;
	history = (char *)malloc(sizeof(char) * 30);
	results = (int *)malloc(sizeof(int) * 35);
	words = (char *)malloc(sizeof(char) * 40);
	memset(history, 32, 30);
	bzero(results, 35);
	memset(words, 32, 40);
	words[30] = 'S';
	words[31] = 'L';
	words[32] = 'A';
	words[33] = 'N';
	words[34] = 'G';
	window = initscr();
	cbreak();
	keypad(stdscr, true);
	noecho();
	signal(SIGWINCH, print);
	print(0);
	int input = 0;
	for (bool start = true; start; start = input != 27) {
		for (int word = 0; word < 6; word++) {
			int length = 0;
			print(0);
			input = wgetch(window);
			while (input != 10 && input != 13 && input != 27) {
				if ((input == KEY_BACKSPACE || input == KEY_LEFT) && length > 0) {
					words[word * 5 + --length] = ' ';
				} else if (length < 5) {
					int v = word * 5 + length;
					if (input == KEY_RIGHT && history[v] != 32) {
						length++;
						words[v] = history[v];
					} else {
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
					}
				}
				print(0);
				input = wgetch(window);
			}
			if (input == 27) {
				break;
			}
		}
	}
	signal(SIGWINCH, SIG_DFL);
	echo();
	int err = endwin();
	if (err == ERR) {
		printf("Error: %i", err);
	}
	print(0);
	printf("THANK YOU FOR PLAYING SLANG\r\n\r\n");
	free(history);
	free(results);
	free(words);
	return 0;
}

void print(int signo) {
	printf("\033[2J\033[1;1H  _____ _____ _____ _____ _____");
	printLine(30);
	printf("\r\n\r\n   SLANG, A WORDLE ALTERNATIVE\r\n  _____ _____ _____ _____ _____");
	for (int word = 0; word < 30; word += 5) {
		printLine(word);
	}
	if (status < 3) {
		printf("\r\n\r\n   TYPE A WORD AND PRESS ENTER\r\n\r\n   OR HIT ESCAPE TO QUIT SLANG\r\n\r\n");
		if (status == 1) {
			printf("   WAITING FOR SERVER RESPONSE\r\n\r\n");
		} else if (status == 2) {
			printf("   YOUR WORD IS NOT RECOGNIZED\r\n\r\n");
		}
	} else if (status < 7) {
		if (status > 4) {
			printf("\r\n\r\n   OH NO, THE WORD WAS \"%c%c%c%c%c\"", words[35], words[36], words[37], words[38], words[39]);
		} else if (status > 2) {
			printf("\r\n\r\n   CONGRATULATIONS, YOU GOT IT");
		}
		printf("\r\n\r\n   DO YOU WANT TO START AGAIN?\r\n\r\n   TYPE \"YES\" TO RESTART SLANG\r\n   TYPE \"NO\" IF YOU'RE CHICKEN\r\n\r\n");
		if (status % 2) {
			printf("   PLEASE RESPOND \"YES\" / \"NO\"\r\n\r\n");
		}
	}
	printf("   ");
	fflush(stdout);
	if (!isendwin()) {
		wrefresh(window);
	}
}

void printLine(int offset) {
	printf("\r\n |");
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

/*
  _____ _____ _____ _____ _____
 |     |     |     |     |     |
 |  S  |  L  |  A  |  N  |  G  |
 |_____|_____|_____|_____|_____|

   SLANG, A WORDLE ALTERNATIVE
  _____ _____ _____ _____ _____
 |     |     |     | \ / |  ^  |
 |  S  |  L  |  A  |--N--|< G >|
 |_____|_____|_____|_/_\_|__v__|
 |     |     |     |     |     |
 |     |     |     |     |     |
 |_____|_____|_____|_____|_____|
 |     |     |     |     |     |
 |     |     |     |     |     |
 |_____|_____|_____|_____|_____|
 |     |     |     |     |     |
 |     |     |     |     |     |
 |_____|_____|_____|_____|_____|
 |     |     |     |     |     |
 |     |     |     |     |     |
 |_____|_____|_____|_____|_____|
 |     |     |     |     |     |
 |     |     |     |     |     |
 |_____|_____|_____|_____|_____|

   TYPE A WORD AND PRESS ENTER

   OR HIT ESCAPE TO QUIT SLANG

   WAITING FOR SERVER RESPONSE
   YOUR WORD IS NOT RECOGNIZED
   ___________________________

   CONGRATULATIONS, YOU GOT IT

   DO YOU WANT TO START AGAIN?

   TYPE "YES" TO RESTART SLANG
   TYPE "NO" IF YOU'RE CHICKEN
   
   PLEASE RESPOND "YES" / "NO"
   ___________________________

   OH NO, THE WORD WAS "SLANG"

   DO YOU WANT TO START AGAIN?

   TYPE "YES" TO RESTART SLANG
   TYPE "NO" IF YOU'RE CHICKEN

*/