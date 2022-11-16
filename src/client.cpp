#include <curses.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include "libslang.h"

void messageHandler(char *message);
void print(int signum);
void printLine(int word);

int bold;
int status;
int word;
char opts;
char *history;
char *results;
char *words;
WINDOW *window;

int main(int argc, char **argv) {
	opts = argc == 1 ? 0 : -1;
	char *host_name = NULL;
	int port_number = 0;
	for (int i = 1; i < argc; i++) {
		if (!(strcmp(argv[i], "-c") * strcmp(argv[i], "--no-curses"))) {
			opts -= 1;
			continue;
		} else if (!(strcmp(argv[i], "-i") * strcmp(argv[i], "--no-intro"))) {
			opts -= 2;
			continue;
		} else if (strcmp(argv[i], "-h") * strcmp(argv[i], "--help")) {
			if (!host_name) {
				host_name = argv[i];
				continue;
			} else if (!port_number) {
				int port_number = atoi(argv[i]);
				if (port_number > 1024 && port_number < 65536) {
					continue;
				}
			}
			opts = i;
			break;
		}
		opts = 0;
		break;
	}
	if (opts > -1 || !host_name) {
		printf("\r\nUsage: %s <host_name> [port_number] [optional_arguments]\r\n\r\nOptional arguments:\r\n-h | --help      : Show this help menu\r\n-c | --no-curses : Disables curses.h functionality for defective clients\r\n-i | --no-intro  : Disables the animated introduction\r\n\r\n", argv[0]);
		if (opts > 0) {
			printf("Invalid argument: %s\r\n\r\n", argv[opts]);
		}
		return EXIT_SUCCESS;
	}
	if (!port_number) {
		port_number = 46257;
	}
	// SlangLib slang('C', messageHandler);
	// slang.init(host_name, port_number);
	// exit(EXIT_SUCCESS); // TEMPORARY
	bold = 35;
	status = 0;
	word = 0;
	history = (char *)malloc(sizeof(char) * 30);
	results = (char *)malloc(sizeof(char) * 35);
	words = (char *)malloc(sizeof(char) * 40);
	static struct termios t0, t1;
	if (opts % 2) {
		window = initscr();
		cbreak();
		noecho();
		keypad(stdscr, true);
		signal(SIGWINCH, print);
		print(0);
	} else {
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
		fputs("\e[2J", stdout);
		fflush(stdout);
	}
	memset(history, 32, 30);
	memset(results, 0, 35);
	memset(words, 0, 35);
	words[30] = 'S';
	words[31] = 'L';
	words[32] = 'A';
	words[33] = 'N';
	words[34] = 'G';
	/*char intro[31] = "WRITEWORDSFIXEDCOUNTCHARSSLANG";
	for (int i = 0; i < 6; i++) {
		word = i;
		for (int j = 0; j < 5; j++) {
			words[i * 5 + j] = intro[i * 5 + j];
			print(j);
			sleep(.05);
		}
	}*/
	memset(results, 0, 35);
	memset(words, 32, 30);
	fflush(stdin);
	// results[1] = 1; // TEMP
	// results[2] = 2; // TEMP
	int input = 0;
	for (bool start = true; start; start = input != 27) {
		for (word = 0; word < 6; word++) {
			int length = 0;
			print(length);
			input = opts % 2 ? wgetch(window) : getc(stdin);
			while (input != 10 && input != 13 && input != 27) {
				if (opts % 2 && (input == KEY_BACKSPACE || input == KEY_LEFT) && length > 0) {
					words[word * 5 + --length] = ' ';
				} else if (length < 5) {
					int v = word * 5 + length;
					if (opts % 2 && input == KEY_RIGHT && history[v] != 32) {
						length++;
						words[v] = history[v];
					} else {
						if (input > 96 && input < 123) {
							input -= 32;
						}
						if (input > 64 && input < 91) {
							length++;
							words[v] = input;
							if (history[v] != input && v < 30) {
								memset(history + v + 1, 32, 30 - v);
							}
							history[v] = input;
						}
					}
				}
				print(length);
				input = opts % 2 ? wgetch(window) : getc(stdin);
			}
			if (input == 27) {
				break;
			}
		}
	}
	if (opts % 2) {
		signal(SIGWINCH, SIG_DFL);
		echo();
		if (endwin() == ERR) {
			system("reset");
		}
	} else if (tcsetattr(STDIN_FILENO, TCSANOW, &t0)) {
		perror("\r\nError: tcsetattr failed");
		return EXIT_FAILURE;
	}
	print(6);
	fputs("THANK YOU FOR PLAYING SLANG\r\n\r\n", stdout);
	free(history);
	free(results);
	free(words);
	return 0;
}

void messageHandler(char *message) {
	int len = strlen(message);
	if (len > 6 && message[1] == '(' && message[len - 1] == ')' && len == message[0] == '4' ? 7 : 8) {
		switch(message[0]) {
		case 'G':
			break;
		case 'R':
			break;
		case 'A':
			break;
		default:
			if (message[0] > 48 && message[0] < 58 && message[0] == len + 45) {
				
			}
		}
	}
	
}

void print(int length) {
	if (opts % 2) {
		fputs("\e[2J", stdout);
	}
	fputs("\e[1;1H  _____ _____ _____ _____ _____", stdout);
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
	} else {
		if (status > 4) {
			printf("\r\n\r\n   OH NO, THE WORD WAS \"%c%c%c%c%c\"", words[35], words[36], words[37], words[38], words[39]);
		} else {
			fputs("\r\n\r\n   CONGRATULATIONS, YOU GOT IT", stdout);
		}
		fputs("\r\n\r\n   DO YOU WANT TO START AGAIN?\r\n\r\n   TYPE \"Y\" TO PLAY A NEW GAME\r\n   TYPE \"N\" TO END THE PROGRAM", stdout);
		if (status % 2) {
			fputs("\r\n\r\n                              \e[32;31H", stdout); // CHECK IF THIS IS CORRECT
		} else {
			fputs("\r\n\r\n   PLEASE ONLY TYPE \"Y\" OR \"N\"", stdout);
		}
	}
	if (length == 6) {
		fputs("\r\n\r\n   \e[?25h", stdout);
	} else if (length == 5 || bold < 35) {
		fputs("\e[?25l", stdout);
	} else {
		printf("\e[%d;%dH\e[?25h", word * 3 + 9, length * 6 + 5);
	}
	fflush(stdout);
	if (opts % 2 && !isendwin()) {
		wrefresh(window);
	}
}

void printLine(int offset) {
	fputs("\r\n |", stdout);
	for (int letter = offset; letter < offset + 5; letter++) {
		printf(" \e[%dm%c%c%c\e[m |", results[letter] == 2 ? 32 : 33, results[letter] == 2 ? '\\' : ' ', results[letter] == 1 ? '^' : ' ', results[letter] == 2 ? '/' : ' ');
	}
	printf("\r\n |");
	for (int letter = offset; letter < offset + 5; letter++) {
		printf(letter == bold ? "\e[%dm%c \e[1m%c\e[m %c\e[m|" : "\e[%dm%c %c %c\e[m|", results[letter] == 2 ? 32 : results[letter] == 1 ? 33 : 0, results[letter] == 2 ? '-' : results[letter] == 1 ? '<' : ' ', words[letter], results[letter] == 2 ? '-' : results[letter] == 1 ? '>' : ' ');
	}
	printf("\r\n |");
	for (int letter = offset; letter < offset + 5; letter++) {
		printf("_%s%s%s_|", results[letter] == 2 ? "\e[32m/\e[m" : "_", results[letter] == 1 ? "\e[33mv\e[m" : "_", results[letter] == 2 ? "\e[32m\\\e[m" : "_");
	}
}
