/* ________________________________________________________ *\
  |                                                        |
  |          Author: Henry Morales                         |
  |          Majors: Computer Science                      |
  |                  Game Development                      |
  |   Creation Date: October 24th, 2022                    |
  |        Due Date: November 29th, 2022                   |
  |          Course: CSC 328                               |
  |  Professor Name: Dr. Frye                              |
  |      Assignment: Wordle                                |
  |        Filename: client.cpp                            |
  |         Purpose: Interface with the user to play the   |
  |                    game "Slang", a Wordle lookalike.   |
  |        Language: C++14                                 |
  |________________________________________________________|
\*                                                          */

// C libraries
#include <ncurses.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

// C++ libraries
#include <climits>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

// Slang library
#include "slanglib.h"

// Standard namespace
using namespace std;

// Alphabetical functions
unsigned short int check_opts(int argc, char **argv, char **host_name);
char check_opts_long(int argc, char **argv, short int i);
void check_opts_short(int argc, char **argv, short int i);
void clean(int signo);
void init();
void intro();
void print(int signo);
void print_key(short int position_x, short int position_y, short int color_b, short int color_f, short int character, short int current_width);
void print_message();
void print_with_gui();
void print_without_gui();
char server_read(char *buffer, char const * const expected);
char server_write(char *buffer, char const * const expected);
char set_theme(char *arg);
void set_winsize();
void usage(char **argv);

// Global variables
// Note: These variables are global because they are used by signal handlers
FILE *file;
WINDOW *window;
mmask_t mouse_history;
SlangLib *slang = NULL;
int *theme = NULL;
char history[36];
char keyboard[52];
char results[35];
char words[35];
char const *output[7];
struct termios terminal_history;
short int opts;
short int highlight = 36;
short int letter = 0;
short int origin_x = 0;
short int origin_y = 0;
short int status = 0;
short int width = 0;
short int word = 0;

/* _____________________________________________________ *\
  |                                                     |
  |  Function Name: main                                |
  |    Description: Entry point for the application     |
  |     Parameters: int argc - The number of arguments  |
  |                 char **argv - The arguments         |
  |   Return Value: int - The exit status code          |
  |_____________________________________________________|
\*                                                       */

int main(int argc, char **argv) {
	char *host_name = NULL;
	unsigned short int port_number;
	if (argc == 1) {
		opts = 0;
	} else {
		opts = -1;
		port_number = check_opts(argc, argv, &host_name);
	}
	if (opts > -1 || !host_name) {
		usage(argv);
	}
	file = fopen("logs.txt", "w");
	if (!file) {
		perror("Error: fopen failed");
		return EXIT_FAILURE;
	}
	fprintf(file, "Server: %s:%d\r\n", host_name, port_number);
	fflush(stdout);
	slang = new SlangLib(port_number, host_name);
	char buffer[14];
	if (server_read(buffer, "5(HELLO)")) {
		fprintf(stderr, "\r\nError: \"5(HELLO)\" expected (got \"%s\")\r\n", buffer);
		clean(-4);
	}
	init();
	fputs("\e[?25l", stdout);
	print(-2);
	MEVENT mouse_event;
	if (opts % 8 > 3) {
		intro();
	}
	int input = 89;
	while (input == 89) {
		strcpy(buffer, "5(READY)");
		if (server_write(buffer, "5(START)")) {
			fprintf(stderr, "\r\nError: \"5(START)\" expected (got \"%s\")\r\n", buffer);
			clean(-3);
		}
		status = 1;
		memset(keyboard + 26, 6, 26);
		memset(results, 0, 30);
		memset(results + 30, 16, 5);
		memset(history, 32, 30);
		memset(words, 32, 30);
		strncpy(words + 30, "SLANG", 5);
		word = 0;
		while (word < 6) {
			memset(results + word * 5, 6, 5);
			letter = 0;
			print(-1);
			fflush(stdin);
			input = opts % 2 ? wgetch(window) : getc(stdin);
			while (input != 27 && (input != 13 && input != 10 || letter > -5)) {
				if (input == KEY_MOUSE && getmouse(&mouse_event) == OK && mouse_event.bstate & BUTTON1_PRESSED) {
					mouse_event.y -= origin_y - 1;
					int v = word * 5 - letter;
					if (mouse_event.y > 5 && mouse_event.y < 9) {
						mouse_event.x -= origin_x + 37;
						if (mouse_event.x > -1 && mouse_event.x < 68 && mouse_event.x % 7 < 5 && letter > -5) {
							--letter;
							words[v] = keyboard[mouse_event.x / 7];
							if (history[v] != words[v] && v < 30) {
								memset(history + v + 1, 32, 30 - v);
							}
							history[v] = words[v];
						}
					} else if (mouse_event.y > 9 && mouse_event.y < 13) {
						mouse_event.x -= origin_x + 39;
						if (mouse_event.x > -1 && mouse_event.x < 61 && mouse_event.x % 7 < 5 && letter > -5) {
							--letter;
							words[v] = keyboard[mouse_event.x / 7 + 10];
							if (history[v] != words[v] && v < 30) {
								memset(history + v + 1, 32, 30 - v);
							}
							history[v] = words[v];
						}
					} else if (mouse_event.y > 13 && mouse_event.y < 17) {
						mouse_event.x -= origin_x + 41;
						if (mouse_event.x > -1 && mouse_event.x < 47 && mouse_event.x % 7 < 5 && letter > -5) {
							--letter;
							words[v] = keyboard[mouse_event.x / 7 + 19];
							if (history[v] != words[v] && v < 30) {
								memset(history + v + 1, 32, 30 - v);
							}
							history[v] = words[v];
						} else if (mouse_event.x > 48 && mouse_event.x < 64 && letter < 0) {
							words[word * 5 - ++letter] = ' ';
						}
					} else {
						if (mouse_event.y > 17 && mouse_event.y < 21 && mouse_event.x > origin_x + 42 && mouse_event.x < origin_x + 100 && letter == -5) {
							input = 10;
						} else {
							input = opts % 2 ? wgetch(window) : getc(stdin);
						}
						continue;
					}
					print(-1);
					input = opts % 2 ? wgetch(window) : getc(stdin);
					continue;
				} else if ((input == 8 || input == 127 || input == KEY_BACKSPACE || input == KEY_LEFT) && letter < 0) {
					words[word * 5 - ++letter] = ' ';
					print(-1);
				} else if (letter > -5) {
					int v = word * 5 - letter;
					if (input == KEY_RIGHT && history[v] != ' ') {
						--letter;
						words[v] = history[v];
					} else {
						if (input > 96) {
							input -= 32;
						}
						if (input > 64 && input < 91) {
							--letter;
							words[v] = input;
							if (history[v] != input && v < 30) {
								memset(history + v + 1, 32, 30 - v);
							}
							history[v] = input;
						}
					}
					print(-1);
				}
				input = opts % 2 ? wgetch(window) : getc(stdin);
			}
			if (input == 27) {
				break;
			}
			strcpy(buffer, "G(     )");
			for (int i = 2; i < 7; ++i) {
				buffer[i] = words[word * 5 - 2 + i];
			}
			status = 3;
			print(-1);
			char err = server_write(buffer, "5(WRONG)");
			if (err) {
				char result = buffer[2] - 48;
				char key = 26 + strchr(keyboard, words[word * 5]) - keyboard;
				char correct = result;
				results[word * 5] = result;
				if (result > keyboard[key] || keyboard[key] == 6) {
					keyboard[key] = result ? result + 6 : 0;
				}
				print(-1);
				for (int i = 3; i < 7; ++i) {
					usleep(100000);
					result = buffer[i] - 48;
					key = 26 + strchr(keyboard, words[word * 5 - 2 + i]) - keyboard;
					correct += result;
					results[word * 5 - 2 + i] = result;
					if (result > keyboard[key] || keyboard[key] == 6) {
						keyboard[key] = result ? result + 6 : 0;
					}
					print(-1);
				}
				if (buffer[0] == 'A') {
					strncpy(history + 30, buffer + 7, 5);
					memset(keyboard + 26, 0, 26);
					status = 5;
					for (int i = 7; i < 11; ++i) {
						usleep(100000);
						words[23 + i] = buffer[i];
						results[23 + i] = 13;
						print(-1);
					}
					words[34] = buffer[11];
					results[34] = 13;
					print(-1);
					break;
				}
				if (correct == 20) {
					strncpy(history + 30, buffer + 7, 5);
					memset(keyboard + 26, 0, 26);
					status = 4;
					for (int i = 0; i < 4; ++i) {
						usleep(100000);
						words[30 + i] = words[word * 5 + i];
						results[30 + i] = 10;
						print(-1);
					}
					words[34] = words[word * 5 + 4];
					results[34] = 10;
					print(-1);
					break;
				}
				++word;
				status = 1;
			} else {
				memset(words + word * 5, 32, 5);
				memset(history + word * 5, 32, 5);
				status = 2;
			}
		}
		while (input != 27 && input != 78 && input != 89) {
			input = opts % 2 ? wgetch(window) : getc(stdin);
			if (input > 96) {
				input -= 32;
			}
		}
	}
	clean(-1);
	return EXIT_SUCCESS;
}

/* _________________________________________________________ *\
  |                                                         |
  |  Function Name: check_opts                              |
  |    Description: Checks optional command line arguments  |
  |     Parameters: int argc - The number of arguments      |
  |                 char **argv - The arguments             |
  |   Return Value: unsigned short int - The port number    |
  |_________________________________________________________|
\*                                                           */

unsigned short int check_opts(int argc, char **argv, char **host_name) {
	unsigned short int port_number = 0;
	for (short int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			if (argv[i][1] == '-') {
				if (check_opts_long(argc, argv, i)) {
					break;
				}
			} else {
				check_opts_short(argc, argv, i);
			}
			continue;
		} else if (!(*host_name)) {
			*host_name = argv[i];
			continue;
		} else if (!port_number) {
			port_number = atoi(argv[i]);
			if (port_number > 1023 && port_number < 65536) {
				continue;
			}
		}
		opts = i;
		break;
	}
	if (port_number) {
		return port_number;
	} else {
		return 46257;
	}
}

/* ________________________________________________________ *\
  |                                                        |
  |  Function Name: check_opts_long                        |
  |    Description: Checks a --long command line argument  |
  |     Parameters: int argc - The number of arguments     |
  |                 char **argv - The arguments            |
  |                 short int i - The argument's index     |
  |   Return Value: char - Success (0) or failure (1)      |
  |________________________________________________________|
\*                                                          */

char check_opts_long(int argc, char **argv, short int i) {
	size_t len = strlen(argv[i]);
	char lower[len + 1];
	lower[len] = 0;
	for (short int j = 0; j < len; ++j) {
		lower[j] = tolower(argv[i][j]);
	}
	if (len == 6 && !strcmp(lower, "--usage")) {
		opts = 0;
	} else if (len != 11 || strcmp(lower, "--no-curses")) {
		if (len != 8 || strcmp(lower, "--no-gui")) {
			if (len != 10 || strcmp(lower, "--no-intro")) {
				if (len != 8 || strcmp(lower, "--no-key")) {
					if (len != 7 || strcmp(lower, "--theme") || i + 1 == argc || theme || set_theme(argv[++i])) {
						opts = i;
						return 1;
					}
				} else if (opts < 0 && opts > -8) {
					opts -= 8;
				}
			} else if (opts < 0 && (opts + 16) % 8 > 3) {
				opts -= 4;
			}
		} else if (opts < 0 && (opts + 16) % 4 > 1) {
			opts -= 2;
		}
	} else if (opts < 0 && opts % 2) {
		opts -= 1;
	}
	return 0;
}

/* ________________________________________________________ *\
  |                                                        |
  |  Function Name: check_opts_short                       |
  |    Description: Checks a -short command line argument  |
  |     Parameters: int argc - The number of arguments     |
  |                 char **argv - The arguments            |
  |                 short int i - The argument's index     |
  |   Return Value: char - Success (0) or failure (1)      |
  |________________________________________________________|
\*                                                          */

void check_opts_short(int argc, char **argv, short int i) {
	size_t len = strlen(argv[i]);
	for (short int j = 1; j < len; ++j) {
		switch (tolower(argv[i][j])) {
		case 'h':
			opts = 0;
			break;
		case 'c':
			if (opts < 0 && opts % 2) {
				opts -= 1;
			}
			break;
		case 'g':
			if (opts < 0 && (opts + 16) % 4 > 1) {
				opts -= 2;
			}
			break;
		case 'i':
			if (opts < 0 && (opts + 16) % 8 > 3) {
				opts -= 4;
			}
			break;
		case 'k':
			if (opts < 0 && opts > -8) {
				opts -= 8;
			}
			break;
		case 't':
			if (j + 1 == len) {
				if (i + 1 == argc || theme || set_theme(argv[++i])) {
					opts = i;
					break;
				}
			} else if (theme || set_theme(argv[i] + j + 1)) {
				opts = i;
			}
			j = SHRT_MAX;
			break;
		default:
			opts = i;
			j = SHRT_MAX;
			break;
		}
	}
}

/* _____________________________________________________________ *\
  |                                                             |
  |  Function Name: clean                                       |
  |    Description: Cleans up the process, including sockets,   |
  |                   files, and dynamically allocated memory,  |
  |     Parameters: int signo - The signal number, if handled   |
  |_____________________________________________________________|
\*                                                               */

void clean(int signo) {
	if (theme) {
		signal(SIGINT, SIG_DFL);
		signal(SIGWINCH, SIG_DFL);
		signal(SIGKILL, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		signal(SIGTERM, SIG_DFL);
		signal(SIGTSTP, SIG_DFL);
		if (opts % 2) {
			mousemask(mouse_history, NULL);
			echo();
			if (endwin() == ERR) {
				fprintf(file, "Error: endwin failed\r\n", strerror(errno));
				system("reset");
			}
		} else if (signo > -4 && tcsetattr(STDIN_FILENO, TCSANOW, &terminal_history)) {
			perror("\r\nError: tcsetattr failed");
			fprintf(file, "Error: tcsetattr failed: %s\r\n", strerror(errno));
		}
		fputs("\e[m\e[2J\e[?25h\e[H", stdout);
		free(theme);
	}
	if (signo == -2) {
		fputs("Server shutdown\r\n", file);
	} else {
		fputs("Write: \"4(QUIT)\"\r\n", file);
		SlangWrite(slang->getSocket(), "4(QUIT)");
		if (errno == ENOTCONN) {
			fprintf(file, "Error: write failed: %s\r\n", strerror(errno));
			clean(-3);
		}
	}
	if (close(slang->getSocket())) {
		perror("Error: close failed");
		fprintf(file, "Error: close failed: %s\r\n", strerror(errno));
		signo = -3;
	}
	delete slang;
	fputs(signo < -2 ? "Exit: Failure" : "Exit: Success", file);
	fflush(stdout);
	fflush(file);
	fclose(file);
	exit(signo < -2 ? EXIT_FAILURE : EXIT_SUCCESS);
}

/* ____________________________________________________________ *\
  |                                                            |
  |  Function Name: init                                       |
  |    Description: Initializes important information such as  |
  |                   dynamically allocated memory, signal     |
  |                   handlers, default data, constant data,   |
  |                   and handling the terminal                |
  |____________________________________________________________|
\*                                                              */

void init() {
	opts += 16;
	if (!theme) {
		set_theme(NULL);
	}
	signal(SIGINT, print);
	signal(SIGWINCH, print);
	signal(SIGKILL, clean);
	signal(SIGQUIT, clean);
	signal(SIGTERM, clean);
	signal(SIGTSTP, clean);
	output[0] = "Type a 5-letter word and press enter.";
	output[1] = "Checking your guess...";
	output[2] = "You must guess a real word!\r\n";
	output[3] = "Would you like to play again?\r\n";
	output[4] = "Congratulations, you guessed \"%s\"!\r\n";
	output[5] = "You lose, the word was \"%s\".\r\n";
	output[6] = "Please type \"Y\" or \"N\".";
	strcpy(keyboard, "QWERTYUIOPASDFGHJKLZXCVBNM");
	memset(keyboard + 26, 6, 26);
	memset(results, 0, 30);
	memset(results + 30, 16, 5);
	history[35] = 0;
	memset(words, 32, 30);
	strncpy(words + 30, "SLANG", 5);
	struct termios terminal;
	if (opts % 2) {
		window = initscr();
		cbreak();
		keypad(stdscr, 1);
		noecho();
		mousemask(BUTTON1_PRESSED | REPORT_MOUSE_POSITION, &mouse_history);
		print(-2);
	} else {
		if (tcgetattr(STDIN_FILENO, &terminal_history)) {
			perror("\r\nError: tcgetattr failed");
			fprintf(file, "Error: tcgetattr failed: %s\r\n", strerror(errno));
			clean(-4);
		}
		terminal = terminal_history;
		terminal.c_lflag &= ~(ICANON + ECHO);
		if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &terminal)) {
			perror("\r\nError: tcsetattr failed");
			fprintf(file, "Error: tcsetattr failed: %s\r\n", strerror(errno));
			clean(-4);
		}
	}
}

/* _________________________________________________ *\
  |                                                 |
  |  Function Name: intro                           |
  |    Description: Plays an introductory tutorial  |
  |                   using predefined words        |
  |_________________________________________________|
\*                                                   */

void intro() {
	char intro[31] = "WRITEFIXEDWIDTHWORDSENJOYSLANG";
	for (word = 0; word < 6; ++word) {
		memset(results + word * 5, 6, 5);
		letter = 0;
		print(-1);
		usleep(600000);
		while (letter > -5) {
			words[word * 5 - letter] = intro[word * 5 - letter];
			--letter;
			print(-1);
			usleep(200000);
		}
		for (highlight = word * 5; highlight < word * 5 + 5; ++highlight) {
			usleep(100000);
			if (word == 5) {
				results[highlight] = 4;
			} else if (word == 4 && highlight == 21) {
				results[highlight] = 2;
			} else {
				results[highlight] = 0;
			}
			print(-1);
		}
	}
	sleep(1);
}

/* ____________________________________________________________ *\
  |                                                            |
  |  Function Name: print                                      |
  |    Description: Refreshes the terminal, given the current  |
  |                   state of the game                        |
  |     Parameters: int signo - The signal number, if handled  |
  |____________________________________________________________|
\*                                                              */

void print(int signo) {
	if (origin_x == -1 && signo != -2) {
		return;
	}
	if (signo == SIGWINCH || signo == -2) {
		set_winsize();
	}
	printf("\e[m\e[1m\e[48;2;%d;%d;%dm\e[H\e[2J", (theme[21] >> 16) % 256, (theme[21] >> 8) % 256, theme[21] % 256);
	if (origin_x > 0 && opts % 4 > 1) {
		print_with_gui();
	} else {
		print_without_gui();
	}
	fflush(stdout);
	if (opts % 2 && !isendwin()) {
		wrefresh(window);
	}
}

/* ___________________________________________________________ *\
  |                                                           |
  |  Function Name: print_key                                 |
  |    Description: Prints a single key to the GUI keyboard,  |
  |                   which can be clicked in some terminals  |
  |     Parameters: short int position_x - The X position of  |
  |                   the key, assuming 5 characters wide,    |
  |                   relative to the origin                  |
  |                 short int position_y - The Y position of  |
  |                   the key, relative to the origin         |
  |                 short int color_b - The background color  |
  |                 short int color_f - The foreground color  |
  |                 short int current_width - The inverted,   |
  |                   modified width of the key               |
  |___________________________________________________________|
\*                                                             */

void print_key(short int position_x, short int position_y, short int color_b, short int color_f, short int character, short int current_width) {
	printf("\e[48;2;%d;%d;%dm", (theme[color_b] >> 16) % 256, (theme[color_b] >> 8) % 256, theme[color_b] % 256);
	printf("\e[38;2;%d;%d;%dm", (theme[color_f] >> 16) % 256, (theme[color_f] >> 8) % 256, theme[color_f] % 256);
	for (short int row = 0; row < 3; ++row) {
		printf("\e[%d;%dH", position_y + row, position_x + current_width);
		for (short int column = current_width; column < 5 - current_width; ++column) {
			if (row == 1 && column == 2) {
				if (character == '_') {
					fputs("\e[5m_\e[25m", stdout);
				} else {
					fputc(character, stdout);
				}
			} else {
				fputc(' ', stdout);
			}
		}
	}
}

/* ________________________________________________ *\
  |                                                |
  |  Function Name: print_key                      |
  |    Description: Prints a message box to the    |
  |                   GUI with the active message  |
  |________________________________________________|
\*                                                  */

void print_message() {
	if (opts < 8) {
		origin_x -= 8;
		origin_y -= 16;
	}
	short int color_b;
	if (status == 5) {
		color_b = 13;
	} else if (status == 4) {
		color_b = 10;
	} else {
		color_b = 16;
	}
	printf("\e[48;2;%d;%d;%dm", (theme[color_b] >> 16) % 256, (theme[color_b] >> 8) % 256, theme[color_b] % 256);
	printf("\e[38;2;%d;%d;%dm", (theme[19] >> 16) % 256, (theme[19] >> 8) % 256, theme[19] % 256);
	for (int i = 23; i < 28; ++i) {
		printf("\e[%d;%dH                                                    ", origin_y + i, origin_x + 46);
	}
	printf("\e[%d;%dH", origin_y + 24, origin_x + 50);
	switch(status) {
	case 5:
	case 4:
		printf(output[status], history + 30);
		for (int i = 1; i < 3; ++i) {
			printf("\e[%d;%dH", origin_y + 24 + i, origin_x + 50);
			fputs(output[3 * i], stdout);
		}
		break;
	case 3:
		fputs(output[1], stdout);
		break;
	case 2:
		fputs(output[2], stdout);
		printf("\e[%d;%dH", origin_y + 25, origin_x + 50);
	default:
		fputs(output[0], stdout);
	}
	++color_b;
	printf("\e[48;2;%d;%d;%dm", (theme[color_b] >> 16) % 256, (theme[color_b] >> 8) % 256, theme[color_b] % 256);
	printf("\e[%d;%dH            ", origin_y + 22, origin_x + 48);
	printf("\e[%d;%dH  ", origin_y + 23, origin_x + 46);
	printf("\e[%d;%dH  ", origin_y + 27, origin_x + 46);
	printf("\e[%d;%dH                                    ", origin_y + 28, origin_x + 48);
	++color_b;
	printf("\e[48;2;%d;%d;%dm", (theme[color_b] >> 16) % 256, (theme[color_b] >> 8) % 256, theme[color_b] % 256);
	printf("\e[%d;%dH                                    ", origin_y + 22, origin_x + 60);
	printf("\e[%d;%dH  ", origin_y + 27, origin_x + 96);
	printf("\e[%d;%dH  ", origin_y + 23, origin_x + 96);
	printf("\e[%d;%dH            ", origin_y + 28, origin_x + 84);
	if (opts < 8) {
		origin_x += 8;
		origin_y += 16;
	}
}

/* _________________________________________ *\
  |                                         |
  |  Function Name: print_with_gui          |
  |    Description: Handles all GUI output  |
  |_________________________________________|
\*                                           */

void print_with_gui() {
	for (short int i = 0; i < 7; ++i) {
		short int position = i * 5;
		for (short int j = 0; j < 5; ++j) {
			short int color_b = results[position + j] + (i + j) % 2;
			short int color_f;
			if (i == 6 || color_b > 1 && status < 4) {
				color_f = 19;
				if (color_b < 6) {
					color_b += 6;
				}
			} else {
				color_f = 20;
			}
			short int current_width = position + j == 5 * word - letter ? width : 0;
			print_key(origin_x + 7 * j, i == 6 ? origin_y : origin_y + position - i + 6, color_b, color_f, position + j == 5 * word - letter && letter > -5 && words[position + j] == ' ' && status < 3 ? '_' : words[position + j], current_width);
		}
	}
	if (status > 0) {
		if (opts > 7) {
			for (short int i = 0; i < 10; ++i) {
				print_key(origin_x + 38 + 7 * i, origin_y + 6, keyboard[i + 26] + i % 2, keyboard[i + 26] ? 19 : 20, keyboard[i], 0);
			}
			for (short int i = 10; i < 19; ++i) {
				print_key(origin_x - 30 + 7 * i, origin_y + 10, keyboard[i + 26] + (i + 1) % 2, keyboard[i + 26] ? 19 : 20, keyboard[i], 0);
			}
			for (short int i = 19; i < 26; ++i) {
				print_key(origin_x - 91 + 7 * i, origin_y + 14, keyboard[i + 26] + (i + 1) % 2, keyboard[i + 26] ? 19 : 20, keyboard[i], 0);
			}
			print_key(origin_x + 96, origin_y + 14, letter == 0 || status > 3 ? 0 : 16, letter == 0 || status > 3 ? 20 : 19, 'S', -5);
			printf("\e[%d;%dHBACKSPACE", origin_y + 15, origin_x + 94);
			print_key(origin_x + 70, origin_y + 18, letter == -5 && status < 4 ? 16 : 1, letter == -5 && status < 4 ? 19 : 20, 'T', -26);
			printf("\e[%d;%dHENTER\e[m\e[H", origin_y + 19, origin_x + 70);
		}
		print_message();
	}
}

/* _____________________________________________ *\
  |                                             |
  |  Function Name: print_without_gui           |
  |    Description: Handles all non-GUI output  |
  |_____________________________________________|
\*                                               */

void print_without_gui() {
	for (int i = 0; i < 6; ++i) {
		for (int j = 0; j < 5; ++j) {
			short int position = i * 5 + j;
			short int color_b = results[position] + (i + j) % 2;
			short int color_f;
			if (color_b > 1 && status < 4) {
				color_f = 19;
				if (color_b < 6) {
					color_b += 6;
				}
			} else {
				color_f = 20;
			}
			printf("\e[48;2;%d;%d;%dm", (theme[color_b] >> 16) % 256, (theme[color_b] >> 8) % 256, theme[color_b] % 256);
			printf("\e[38;2;%d;%d;%dm", (theme[color_f] >> 16) % 256, (theme[color_f] >> 8) % 256, theme[color_f] % 256);
			printf("%c\e[m", words[position]);
		}
		fputs("\r\n", stdout);
	}
	fputs("\r\n", stdout);
	switch(status) {
	case 5:
	case 4:
		printf(output[status], history + 30);
		fputs(output[3], stdout);
		fputs(output[6], stdout);
		break;
	case 3:
		fputs(output[1], stdout);
		break;
	case 2:
		fputs(output[2], stdout);
	case 1:
		fputs(output[0], stdout);
	}
	if (opts % 4 > 1) {
		printf("\r\n\r\nEnlarge your terminal to at least 30 rows by %d columns to view the user interface.\e[H", opts > 7 ? 107 : 91);
	}
}

/* ___________________________________________________ *\
  |                                                   |
  |  Function Name: server_read                       |
  |    Description: Reads a message from the server,  |
  |                   handling certain messages       |
  |     Parameters: char *buffer - The buffer that    |
  |                   the message will read into      |
  |                 char const * const expected - An  |
  |                   expected response (may differ)  |
  |___________________________________________________|
\*                                                     */

char server_read(char *buffer, char const * const expected) {
	SlangRead(slang->getSocket(), buffer);
	if (errno == ENOTCONN) {
		fprintf(file, "Error: read failed: %s\r\n", strerror(errno));
		clean(-3);
	}
	fprintf(file, "Read: %s expected (got \"%s\")\r\n", expected, buffer);
	if (strcmp(buffer, expected)) {
		if (buffer[0] == 'A' && buffer[1] == '(' && buffer[12] == ')') {
			if (word < 5 || strcmp(expected, "5(WRONG)")) {
				fprintf(stderr, "\r\nError: \"%s\" expected (got \"%s\")\r\n", expected, buffer);
				clean(-3);
			}
			return 1;
		} else if (buffer[1] == '(' && buffer[7] == ')') {
			if (buffer[0] == '5') {
				if (!strcmp(buffer, "5(REPLY)")) {
					return 2;
				} else if (!strcmp(buffer, "5(DEATH)")) {
					fputs("\r\nServer shutdown\r\n", stderr);
					clean(-2);
				} else {
					fprintf(stderr, "\r\nError: \"%s\" expected (got \"%s\")\r\n", expected, buffer);
					clean(-3);
				}
				clean(-3);
			}
			if (strcmp(expected, "5(WRONG)")) {
				fprintf(stderr, "\r\nError: \"%s\" expected (got \"%s\")\r\n", expected, buffer);
				clean(-3);
			}
			if (word == 5) {
				
			}
			return 1;
		}
		return 3;
	}
	return 0;
}

/* ___________________________________________________ *\
  |                                                   |
  |  Function Name: server_write                      |
  |    Description: Writes a message to the server,   |
  |                   handling certain responses      |
  |     Parameters: char *buffer - The message that   |
  |                   will be sent to the server and  |
  |                   the buffer that the response    |
  |                   will be read into               |
  |                 char const * const expected - An  |
  |                   expected response (may differ)  |
  |___________________________________________________|
\*                                                     */

char server_write(char *buffer, char const * const expected) {
	char original[9];
	strcpy(original, buffer);
	fprintf(file, "Write: \"%s\"\r\n", buffer);
	SlangWrite(slang->getSocket(), buffer);
	if (errno == ENOTCONN) {
		fprintf(file, "Error: write failed: %s\r\n", strerror(errno));
		clean(-3);
	}
	char result = server_read(buffer, expected);
	char attempts = 1;
	while (result > 1 && ++attempts < 5) {
		fprintf(file, "Unexpected response, write attempt #%d: \"%s\"\r\n", attempts, original);
		if (result == 2) {
			SlangWrite(slang->getSocket(), original);
		} else {
			SlangWrite(slang->getSocket(), "5(REPLY)");
		}
		if (errno == ENOTCONN) {
			fprintf(file, "Error: write failed: %s\r\n", strerror(errno));
			clean(-3);
		}
		result = server_read(buffer, expected);
	}
	if (attempts == 4) {
		fprintf(stderr, "\r\nError: \"%s\" expected (got \"%s\")\r\n", expected, buffer);
		clean(-3);
	}
	return result;
}

/* __________________________________________________ *\
  |                                                  |
  |  Function Name: set_theme                        |
  |    Description: Sets the color theme of the app  |
  |     Parameters: char *arg - The theme, or NULL   |
  |                   for the default black theme    |
  |__________________________________________________|
\*                                                    */

char set_theme(char *arg) {
	char lower[6];
	if (arg) {
		size_t len = strlen(arg);
		if (theme || len > 5) {
			return 1;
		}
		for (short int i = 0; i < len; ++i) {
			lower[i] = arg[i] > 96 ? arg[i] - 32 : arg[i];
		}
		lower[len] = 0;
	} else {
		lower[0] = 'b';
		lower[1] = 0;
	}
	theme = (int *)malloc(sizeof(int) * 22);
	if (lower[0] == 'b' && strstr("black", lower)) {
		theme[0] = 1315860;
		theme[1] = 986895;
		theme[2] = 3289625;
		theme[3] = 2631700;
		theme[4] = 1651225;
		theme[5] = 1320980;
		theme[6] = 4934475;
		theme[7] = 4276545;
		theme[8] = 9868850;
		theme[9] = 8881965;
		theme[10] = 3315250;
		theme[11] = 2983725;
		theme[12] = 3646775;
		theme[13] = 9843250;
		theme[14] = 8858925;
		theme[15] = 10827575;
		theme[16] = 3302550;
		theme[17] = 2972295;
		theme[18] = 3632805;
		theme[19] = 16777215;
		theme[20] = 5592405;
		theme[21] = 0;
	/*} else if (lower[0] == 'd' && strstr("dark", lower)) {
	} else if (lower[0] == 'l' && strstr("light", lower)) {
	} else if (lower[0] == 'w' && strstr("white", lower)) {*/
	} else {
		return 1;
	}
	return 0;
}

/* _____________________________________________________ *\
  |                                                     |
  |  Function Name: set_winsize                         |
  |    Description: Changes the origin values based on  |
  |                   the size of the terminal window   |
  |                   initially or handling SIGWINCH.   |
  |_____________________________________________________|
\*                                                       */

void set_winsize() {
	struct winsize winsize;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize);
	if (opts > 7) {
		origin_x = (winsize.ws_col - 107) / 2 + 1;
		origin_y = (winsize.ws_row - 30) / 2 + 1;
	} else {
		origin_x = (winsize.ws_col - 91) / 2 + 1;
		origin_y = (winsize.ws_row - 30) / 2 + 1;
	}
	if (origin_x < 0 || origin_y <= 0) {
		origin_x = 0;
	}
}

/* ______________________________________________ *\
  |                                              |
  |  Function Name: usage                        |
  |    Description: Displays a usage message     |
  |     Parameters: char **argv - The arguments  |
  |______________________________________________|
\*                                                */

void usage(char **argv) {
	printf("\nUsage: %s <host> [port] [optional_arguments]\n\n", argv[0]);
	puts("Optional arguments:");
	puts("-h | --usage      : Show this usage menu again");
	puts("-c | --no-curses : Disables curses functions for defective terminals");
	puts("-g | --no-gui    : Disables the GUI for the console");
	puts("-i | --no-intro  : Disables the animated introduction");
	puts("-k | --no-keys   : Disables the keyboard that shows all input");
	puts("-t | --theme <s> : Change the theme to \e[9mwhite, light, dark, or\e[29m black (default)\n");
	puts("Notes: The host must be a fully qualified domain name.");
	puts("       The port must be between 1024 and 65535 (inclusive).");
	puts("       It is recommended to use a port between 1024 and 49151 (inclusive).");
	puts("       If a port is not specified, it defaults to 46257.");
	puts("       Optional arguments are case-insensitive.");
	puts("       You can pass any prefix of the theme (e.g. \"Da\" becomes \"dark\").\n");
	if (opts) {
		printf("Invalid argument: %s\n\n", argv[opts]);
	}
	if (theme) {
		free(theme);
	}
	exit(EXIT_SUCCESS);
}