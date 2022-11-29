#include <ncurses.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <climits>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <string>

#include "slanglib.h"

using namespace std;

void clean(int signo);
void print(int signo);
void print_key(short int position_x, short int position_y, short int color_b, short int color_f, short int character, short int current_width);
char set_theme(char *arg);
char read_from_server(char *buffer, char const * const expected);
char write_to_server(char *buffer, char const * const expected);

FILE *file;
WINDOW *window;
mmask_t mouse_history;
struct termios terminal_history;
short int highlight = 36;
short int status = 0;
short int width = 0;
short int word = 0;
short int letter = 0;
short int origin_x = 0;
short int origin_y = 0;
short int opts;
char const **output;
char *keyboard;
char *history;
char *results;
char *words;
int *theme = NULL;
SlangLib *slang = NULL;

int main(int argc, char **argv) {
	opts = argc == 1 ? 0 : -1;
	char *host_name = NULL;
	int port_number = 0;
	for (short int i = 1; i < argc; ++i) {
		if (argv[i][0] == '-') {
			size_t len = strlen(argv[i]);
			if (argv[i][1] == '-') {
				char *lower = (char *)malloc(len + 1);
				lower[len] = 0;
				for (short int j = 0; j < len; ++j) {
					lower[j] = tolower(argv[i][j]);
				}
				if (len == 6 && !strcmp(lower, "--help")) {
					opts = 0;
				} else if (len != 11 || strcmp(lower, "--no-curses")) {
					if (len != 8 || strcmp(lower, "--no-gui")) {
						if (len != 10 || strcmp(lower, "--no-intro")) {
							if (len != 8 || strcmp(lower, "--no-key")) {
								if (len != 7 || strcmp(lower, "--theme") || i + 1 == argc || theme || set_theme(argv[++i])) {
									free(lower);
									opts = i;
									break;
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
				free(lower);
			} else {
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
			continue;
		} else if (!host_name) {
			host_name = argv[i];
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
	if (opts > -1 || !host_name) {
		printf("\r\nUsage: %s <host> [port] [optional_arguments]\r\n\r\n", argv[0]);
		puts("Optional arguments:");
		puts("-h | --help      : Show this help menu again");
		puts("-c | --no-curses : Disables curses functions for defective terminals");
		puts("-g | --no-gui    : Disables the gui for the console");
		puts("-i | --no-intro  : Disables the animated introduction");
		puts("-k | --no-keys   : Disables the keyboard that shows all input");
		puts("-t | --theme <1> : Change the theme to white, light, dark, or black (default)\r\n");
		puts("Notes: The host must be a fully qualified domain name.");
		puts("       The port must be between 1024 and 65535 (inclusive).");
		puts("       It is recommended to use a port between 1024 and 49151 (inclusive).");
		puts("       If a port is not specified, it defaults to 46257.");
		puts("       Optional arguments are case-insensitive.");
		puts("       You can pass any prefix of the theme (e.g. \"Da\" becomes \"dark\").\r\n");
		if (opts) {
			printf("Invalid argument: %s\r\n\r\n", argv[opts]);
		}
		if (theme) {
			free(theme);
		}
		return EXIT_SUCCESS;
	}
	if (!port_number) {
		port_number = 46257;
	}
	if (!theme) {
		set_theme(NULL);
	}
	file = fopen("logs.txt", "w");
	if (!file) {
		perror("Error: fopen failed");
		free(theme);
	}
	printf("\r\nConnecting to %s:%d . . .\r\n", host_name, port_number);
	fprintf(file, "Server: %s:%d\r\n", host_name, port_number);
	fflush(stdout);
	slang = new SlangLib(port_number, host_name);
	char buffer[14];
	opts += 16;
	output = (const char **)malloc(7 * sizeof(char *));
	keyboard = (char *)malloc(52);
	history = (char *)malloc(30);
	results = (char *)calloc(35, 1);
	words = (char *)malloc(35);
	signal(SIGKILL, clean);
	signal(SIGQUIT, clean);
	signal(SIGTERM, clean);
	signal(SIGTSTP, clean);
	signal(SIGINT, print);
	signal(SIGWINCH, print);
	output[0] = "Type a 5-letter word and press enter.\r\n";
	output[1] = "Checking your guess...";
	output[2] = "You must guess a real word!";
	output[3] = "Would you like to play again?\r\n";
	output[4] = "Congratulations, you win!\r\n";
	output[5] = "Sorry, you lose.\r\n";
	output[6] = "Please type \"Y\" or \"N\".";
	strcpy(keyboard, "QWERTYUIOPASDFGHJKLZXCVBNM");
	memset(keyboard + 26, 6, 26);
	memset(results + 30, 16, 5);
	memset(history, 32, 30);
	memset(words, 32, 30);
	words[30] = 'S';
	words[31] = 'L';
	words[32] = 'A';
	words[33] = 'N';
	words[34] = 'G';
	if (read_from_server(buffer, "5(HELLO)")) {
		fprintf(stderr, "\r\nError: \"5(HELLO)\" expected (got \"%s\")\r\n", buffer);
		clean(-4);
	}
	struct termios terminal;
	MEVENT mouse_event;
	mmask_t mouse = BUTTON1_PRESSED | REPORT_MOUSE_POSITION;
	if (opts % 2) {
		window = initscr();
		cbreak();
		keypad(stdscr, 1);
		noecho();
		mousemask(mouse, &mouse_history);
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
	fputs("\e[?25l\e[?1003h\n", stdout);
	print(-2);
	/*if (opts % 8 < 4) {
		char intro[31] = "WRITEFIXEDWIDTHWORDSRANGESLANG";
		for (word = 0; word < 6; ++word) {
			letter = 0;
			print(-1);
			sleep(1);
			while (letter > -5) {
				words[word * 5 - letter] = intro[word * 5 - letter];
				--letter;
				print(-1);
				usleep(200000);
			}
			usleep(400000);
			for (highlight = word * 5; highlight < word * 5 + 5; ++highlight) {
				usleep(100000);
				if (word == 5) {
					results[highlight] = 4;
				} else if (word == 4 && (highlight - 1) % 5 < 3) {
					results[highlight] = 2;
				}
				print(-1);
			}
		}
	}*/
	int input = 0;
	while (input != 27) {
		strcpy(buffer, "5(READY)");
		if (write_to_server(buffer, "5(START)")) {
			fprintf(stderr, "\r\nError: \"5(START)\" expected (got \"%s\")\r\n", buffer);
			clean(-3);
		}
		status = 1;
		memset(keyboard + 26, 6, 26);
		memset(results, 0, 30);
		memset(results + 30, 16, 5);
		memset(history, 32, 30);
		memset(words, 32, 30);
		words[30] = 'S';
		words[31] = 'L';
		words[32] = 'A';
		words[33] = 'N';
		words[34] = 'G';
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
					} else if (mouse_event.y > 17 && mouse_event.y < 21 && mouse_event.x > origin_x + 42 && mouse_event.x < origin_x + 100 && letter == -5) {
						input = 10;
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
						if (input > 96 && input < 123) {
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
			
			
			char err = write_to_server(buffer, "5(WRONG)");
			
			
			if (err) {
				results[word * 5] = buffer[2] - 48;
				print(-1);
				short int correct = buffer[2];
				for (int i = 3; i < 7; ++i) {
					correct += buffer[i];
					usleep(100000);
					results[word * 5 - 2 + i] = buffer[i] - 48;
					print(-1);
				}
				if (buffer[0] == 'A') {
					status = 5;
					for (int i = 7; i < 11; ++i) {
						words[23 + i] = buffer[i];
						results[23 + i] = 13;
						print(-1);
						usleep(100000);
					}
					words[34] = buffer[11];
					results[34] = 13;
					print(-1);
					break;
				}
				if (correct == 260) {
					status = 4;
					for (int i = 0; i < 4; ++i) {
						words[30 + i] = words[word * 5 + i];
						results[30 + i] = 10;
						print(-1);
						usleep(100000);
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
		opts % 2 ? wgetch(window) : getc(stdin);
		// WAIT FOR NEXT INPUT OR SOME SHIT
	}
	clean(-1);
	return EXIT_SUCCESS;
}

void clean(int signo) {
	signal(SIGKILL, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
	signal(SIGTERM, SIG_DFL);
	signal(SIGTSTP, SIG_DFL);
	signal(SIGINT, SIG_DFL);
	signal(SIGWINCH, SIG_DFL);
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
	fputs("\e[m\e[H\e[2J\e[?25h\e[?10031\n", stdout);
	free(keyboard);
	free(history);
	free(results);
	free(words);
	free(theme);
	theme = NULL;
	if (signo == -2) {
		fputs("Server shutdown\r\n", file);
	} else {
		SlangWrite(slang->getSocket(), "4(QUIT)");
		fputs("Write: \"4(QUIT)\"\r\n", file);
	}
	close(slang->getSocket());
	delete slang;
	fputs(signo < -2 ? "Exit: Failure" : "Exit: Success", file);
	fflush(stdout);
	fflush(file);
	fclose(file);
	exit(signo < -2 ? EXIT_FAILURE : EXIT_SUCCESS);
}

void print(int signo) {
	if (origin_x == -1 && signo != -2) {
		return;
	}
	if (signo == SIGWINCH || signo == -2) {
		struct winsize winsize;
		ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize);
		origin_x = (winsize.ws_col - 106) / 2 + 1;
		origin_y = (winsize.ws_row - 29) / 2 + 1;
		if (origin_x < 0 || origin_y <= 0) {
			origin_x = 0;
		}
	}
	printf("\e[m\e[1m\e[48;2;%d;%d;%dm\e[H\e[2J", (theme[21] >> 16) % 256, (theme[21] >> 8) % 256, theme[21] % 256);
	if (origin_x > 0 && opts % 4 > 1) {
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
			for (short int i = 0; i < 10; ++i) {
				print_key(origin_x + 38 + 7 * i, origin_y + 6, keyboard[i + 26] + i % 2, keyboard[i + 26] ? 19 : 20, keyboard[i], 0);
			}
			for (short int i = 10; i < 19; ++i) {
				print_key(origin_x - 30 + 7 * i, origin_y + 10, keyboard[i + 26] + (i + 1) % 2, keyboard[i + 26] ? 19 : 20, keyboard[i], 0);
			}
			for (short int i = 19; i < 26; ++i) {
				print_key(origin_x - 91 + 7 * i, origin_y + 14, keyboard[i + 26] + (i + 1) % 2, keyboard[i + 26] ? 19 : 20, keyboard[i], 0);
			}
			print_key(origin_x + 96, origin_y + 14, letter == 0 ? 0 : 16, letter == 0 ? 20 : 19, 'S', -5);
			printf("\e[%d;%dHBACKSPACE", origin_y + 15, origin_x + 94);
			print_key(origin_x + 70, origin_y + 18, letter == -5 ? 16 : 1, letter == -5 ? 19 : 20, 'T', -26);
			printf("\e[%d;%dHENTER\e[m\e[H", origin_y + 19, origin_x + 70);
		}
	} else {
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
		switch(status) {
		case 1:
			fputs(output[0], stdout);
		case 2:
			fputs(output[2], stdout);
			break;
		case 3:
			fputs(output[1], stdout);
			break;
		case 4:
		case 5:
			fputs(output[status], stdout);
			fputs(output[3], stdout);
			fputs(output[6], stdout);
			break;
		};
		fputs("\e[H", stdout);
	}
	fflush(stdout);
	if (opts % 2 && !isendwin()) {
		wrefresh(window);
	}
}

void print_key(short int position_x, short int position_y, short int color_b, short int color_f, short int character, short int current_width) {
	printf("\e[48;2;%d;%d;%dm", (theme[color_b] >> 16) % 256, (theme[color_b] >> 8) % 256, theme[color_b] % 256);
	printf("\e[38;2;%d;%d;%dm", (theme[color_f] >> 16) % 256, (theme[color_f] >> 8) % 256, theme[color_f] % 256);
	for (short int row = 0; row < 3; ++row) {
		printf("\e[%d;%dH", position_y + row, position_x + current_width);
		for (short int column = current_width; column < 5 - current_width; ++column) {
			if (row == 1 && column == 2) {
				fputc(character, stdout);
			} else {
				fputc(' ', stdout);
			}
		}
	}
}

char set_theme(char *arg) {
	char lower[6];
	if (arg) {
		size_t len = strlen(arg);
		if (theme || len > 5) {
			return 1;
		}
		for (short int i = 0; i < len; i++) {
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
		theme[6] = 3289650;
		theme[7] = 2631720;
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

char write_to_server(char *buffer, char const * const expected) {
	char original[9];
	strcpy(original, buffer);
	SlangWrite(slang->getSocket(), buffer);
	fprintf(file, "Write: \"%s\"\r\n", buffer);
	char result = read_from_server(buffer, expected);
	char attempts = 1;
	while (result > 1 && ++attempts < 5) {
		fprintf(file, "Unexpected response, write attempt #%d: \"%s\"\r\n", attempts, original);
		if (result == 2) {
			SlangWrite(slang->getSocket(), original);
		} else {
			SlangWrite(slang->getSocket(), "5(REPLY)");
		}
		result = read_from_server(buffer, expected);
	}
	if (attempts == 4) {
		fprintf(stderr, "\r\nError: \"%s\" expected (got \"%s\")\r\n", expected, buffer);
		clean(-3);
	}
	return result;
}

char read_from_server(char *buffer, char const * const expected) {
	SlangRead(slang->getSocket(), buffer);
	fprintf(file, "Read: \"%s\"\r\n", buffer);
	if (strcmp(buffer, expected)) {
		if (buffer[0] == 'A' && buffer[1] == '(' && buffer[12] == ')') {
			if (strcmp(expected, "5(WRONG)")) {
				fprintf(stderr, "\r\nError: \"%s\" expected (got \"%s\")\r\n", expected, buffer);
				clean(-3);
			}
			return 1;
		} else if (buffer[1] == '(' && buffer[7] == ')') {
			if (buffer[0] == '5') {
				if (!strcmp(buffer, "5(HELLO)")) {
					fprintf(stderr, "\r\nError: \"%s\" expected (got \"5(HELLO)\")\r\n", expected);
					clean(-3);
				} else if (!strcmp(buffer, "5(START)")) {
					fprintf(stderr, "\r\nError: \"%s\" expected (got \"5(START)\")\r\n", expected);
					clean(-3);
				} else if (!strcmp(buffer, "5(WRONG)")) {
					fprintf(stderr, "\r\nError: \"%s\" expected (got \"5(WRONG)\")\r\n", expected);
					clean(-3);
				} else if (!strcmp(buffer, "5(RESET)")) {
					fprintf(stderr, "\r\nError: \"%s\" expected (got \"5(RESET)\")\r\n", expected);
					clean(-3);
				} else if (!strcmp(buffer, "5(REPLY)")) {
					return 2;
				} else if (!strcmp(buffer, "5(DEATH)")) {
					fputs("\r\nServer shutdown\r\n", stderr);
					clean(-2);
				}
				clean(-3);
			}
			if (strcmp(expected, "5(WRONG)")) {
				fprintf(stderr, "\r\nError: \"%s\" expected (got \"%s\")\r\n", expected, buffer);
				clean(-3);
			}
			return 1;
		}
		return 3;
	}
	return 0;
}