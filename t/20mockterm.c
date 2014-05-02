#include "tickit.h"
#include "tickit-mockterm.h"
#include "taplib.h"
#include "taplib-mockterm.h"

#include <string.h>

#define streq(a,b) (strcmp(a,b)==0)

static TickitTerm *tt;

static void is_display_text(char *name, ...)
{
  va_list args;
  va_start(args, name);

  int lines, cols;
  tickit_term_get_size(tt, &lines, &cols);

  for(int line = 0; line < lines; line++) {
    const char *expect = va_arg(args, char *);
    size_t len = tickit_mockterm_get_display_text((TickitMockTerm *)tt, NULL, 0, line, 0, cols);
    char *got = malloc(len + 1);
    tickit_mockterm_get_display_text((TickitMockTerm *)tt, got, len, line, 0, cols);

    if(streq(expect, got)) {
      free(got);
      continue;
    }

    fail(name);
    diag("Got line % 2d |%s|", line, got);
    diag("Expected    |%s|", expect);

    free(got);
    return;
  }

  pass(name);
}

static void fillterm(TickitTerm *tt)
{
  tickit_term_goto(tt, 0, 0);
  tickit_term_print(tt, "0000000000");
  tickit_term_goto(tt, 1, 0);
  tickit_term_print(tt, "1111111111");
  tickit_term_goto(tt, 2, 0);
  tickit_term_print(tt, "2222222222");
}

int main(int argc, char *argv[])
{
  tt = make_term(3, 10);

  is_termlog("Termlog initially",
      NULL);
  is_display_text("Display initially",
      "          ",
      "          ",
      "          ");

  tickit_term_goto(tt, 1, 5);
  is_termlog("Termlog after goto",
      GOTO(1,5),
      NULL);

  tickit_term_print(tt, "foo");

  is_termlog("Termlog after print",
      PRINT("foo"),
      NULL);
  is_display_text("Display after print",
      "          ",
      "     foo  ",
      "          ");

  char c = 'l';
  tickit_term_printn(tt, &c, 1);

  is_termlog("Termlog after printn 1",
      PRINT("l"),
      NULL);
  is_display_text("Display after printn 1",
      "          ",
      "     fool ",
      "          ");

  tickit_term_goto(tt, 2, 0);
  tickit_term_print(tt, "Ĉu vi?");

  is_termlog("Termlog after print UTF-8",
      GOTO(2,0), PRINT("Ĉu vi?"),
      NULL);
  is_display_text("Display after print UTF-8",
      "          ",
      "     fool ",
      "Ĉu vi?    ");

  // U+FF10 = Fullwidth digit zero = EF BC 90
  tickit_term_print(tt, "\xef\xbc\x90");

  is_termlog("Termlog after print UTF-8 fullwidth",
      PRINT("０"),
      NULL);
  is_display_text("Display after print UTF-8 fullwidth",
      "          ",
      "     fool ",
      "Ĉu vi?０  ");

  tickit_term_clear(tt);

  is_termlog("Termlog after clear",
      CLEAR(),
      NULL);
  is_display_text("Display after clear",
      "          ",
      "          ",
      "          ");

  TickitPen *fg_pen = tickit_pen_new_attrs(TICKIT_PEN_FG, 3, -1);

  tickit_term_setpen(tt, fg_pen);

  is_termlog("Termlog after setpen",
      SETPEN(.fg=3),
      NULL);

  TickitPen *bg_pen = tickit_pen_new_attrs(TICKIT_PEN_BG, 6, -1);

  tickit_term_chpen(tt, bg_pen);

  is_termlog("Termlog after chpen",
      SETPEN(.fg=3, .bg=6),
      NULL);

  // Now some test content for scrolling
  fillterm(tt);
  tickit_mockterm_clearlog((TickitMockTerm *)tt);

  is_display_text("Display after scroll fill",
      "0000000000",
      "1111111111",
      "2222222222");

  ok(tickit_term_scrollrect(tt, 0,0,3,10, +1,0), "Scrollrect down OK");
  is_termlog("Termlog after scroll 1 down",
      SCROLLRECT(0,0,3,10, +1,0),
      NULL);
  is_display_text("Display after scroll 1 down",
      "1111111111",
      "2222222222",
      "          ");

  ok(tickit_term_scrollrect(tt, 0,0,3,10, -1,0), "Scrollrect up OK");
  is_termlog("Termlog after scroll 1 up",
      SCROLLRECT(0,0,3,10, -1,0),
      NULL);
  is_display_text("Display after scroll 1 up",
      "          ",
      "1111111111",
      "2222222222");

  fillterm(tt);
  tickit_mockterm_clearlog((TickitMockTerm *)tt);

  tickit_term_scrollrect(tt, 0,0,2,10, +1,0);
  is_termlog("Termlog after scroll partial 1 down",
      SCROLLRECT(0,0,2,10, +1,0),
      NULL);
  is_display_text("Display after scroll partial 1 down",
      "1111111111",
      "          ",
      "2222222222");

  tickit_term_scrollrect(tt, 0,0,2,10, -1,0);
  is_termlog("Termlog after scroll partial 1 up",
      SCROLLRECT(0,0,2,10, -1,0),
      NULL);
  is_display_text("Display after scroll partial 1 up",
      "          ",
      "1111111111",
      "2222222222");

  for(int line = 0; line < 3; line++)
    tickit_term_goto(tt, line, 0), tickit_term_print(tt, "ABCDEFGHIJ");
  tickit_mockterm_clearlog((TickitMockTerm *)tt);

  tickit_term_scrollrect(tt, 0,5,1,5, 0,2);
  is_termlog("Termlog after scroll right",
      SCROLLRECT(0,5,1,5, 0,+2),
      NULL);
  is_display_text("Display after scroll right",
      "ABCDEHIJ  ",
      "ABCDEFGHIJ",
      "ABCDEFGHIJ");

  tickit_term_scrollrect(tt, 1,5,1,5, 0,-3);
  is_termlog("Termlog after scroll left",
      SCROLLRECT(1,5,1,5, 0,-3),
      NULL);
  is_display_text("Display after scroll left",
      "ABCDEHIJ  ",
      "ABCDE   FG",
      "ABCDEFGHIJ");

  tickit_term_goto(tt, 2, 3);
  tickit_term_erasech(tt, 5, -1);

  is_termlog("Termlog after erasech",
      GOTO(2,3),
      ERASECH(5, -1),
      NULL);
  is_display_text("Display after erasech",
      "ABCDEHIJ  ",
      "ABCDE   FG",
      "ABC     IJ");

  return exit_status();
}
