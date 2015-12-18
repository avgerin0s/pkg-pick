#include <err.h>
#include <signal.h>
#include <stdlib.h>
#include <termios.h>

#ifdef HAVE_NCURSESW_H
#include <ncursesw/curses.h>
#include <ncursesw/term.h>
#else
#include <curses.h>
#include <term.h>
#endif

#include "tty.h"

#define EX_SIG 128
#define EX_SIGINT (EX_SIG + SIGINT)

#define ESCAPE 27

static int tty_getc();
static void tty_putp(const char *);
static int raw_tty_putc(int);
static void handle_interrupt();

FILE *tty_out;
FILE *tty_in;
struct termios original_attributes;
int using_alternate_screen;

void
tty_init(int use_alternate_screen)
{
	struct termios new_attributes;

	using_alternate_screen = use_alternate_screen;

	tty_in = fopen("/dev/tty", "r");
	if (tty_in == NULL) {
		err(1, "fopen");
	}

	tcgetattr(fileno(tty_in), &original_attributes);
	new_attributes = original_attributes;
	new_attributes.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(fileno(tty_in), TCSANOW, &new_attributes);

	tty_out = fopen("/dev/tty", "w");
	if (tty_out == NULL) {
		err(1, "fopen");
	}

	setupterm((char *)0, fileno(tty_out), (int *)0);

	if (using_alternate_screen) {
		tty_putp(enter_ca_mode);
	}

	tty_putp(clear_screen);

	signal(SIGINT, handle_interrupt);
}

void
tty_restore()
{
	tcsetattr(fileno(tty_in), TCSANOW, &original_attributes);
	fclose(tty_in);

	tty_putp(clear_screen);

	if (using_alternate_screen) {
		tty_putp(exit_ca_mode);
	}

	fclose(tty_out);
}

int
tty_getch()
{
	int ch;

	ch = tty_getc();

	if (ch == ESCAPE) {
		ch = tty_getc();

		if (ch == '\n') {
			return TTY_ALT_ENTER;
		}

		if (ch == '[' || ch == 'O') {
			ch = tty_getc();

			if (ch == 'A') {
				return TTY_UP;
			}

			if (ch == 'B') {
				return TTY_DOWN;
			}

			if (ch == 'C') {
				return TTY_RIGHT;
			}

			if (ch == 'D') {
				return TTY_LEFT;
			}
		}
	}

	return ch;
}

void
tty_putc(int c)
{
	if (raw_tty_putc(c) == EOF) {
		err(1, "raw_tty_putc");
	}
}

void
tty_show_cursor()
{
	tty_putp(cursor_normal);
}

void
tty_hide_cursor()
{
	tty_putp(cursor_invisible);
}

void
tty_enter_standout_mode()
{
	tty_putp(enter_standout_mode);
}

void
tty_exit_standout_mode()
{
	tty_putp(exit_standout_mode);
}

void
tty_move_cursor_to(int y, int x)
{
	tty_putp(tgoto(cursor_address, x, y));
}

int
tty_lines()
{
	return lines;
}

int
tty_columns()
{
	return columns;
}

int
tty_flush()
{
	return fflush(tty_out);
}

static int
tty_getc()
{
	int c;

	c = getc(tty_in);

	if (c == ERR) {
		err(1, "getc");
	}

	return c;
}

static void
tty_putp(const char *string)
{
	if (tputs(string, 1, raw_tty_putc) == ERR) {
		err(1, "tputs");
	}
}

static int
raw_tty_putc(int c)
{
    return putc(c, tty_out);
}

static void
handle_interrupt()
{
	tty_restore();
	exit(EX_SIGINT);
}
