#include "tickit.h"
#include "taplib.h"
#include "taplib-mockterm.h"

int main(int argc, char *argv[])
{
  TickitTerm *tt = make_term(25, 80);
  TickitRenderBuffer *rb;

  rb = tickit_renderbuffer_new(10, 20);

  TickitRect mask = {.top = 3, .left = 5, .lines = 4, .cols = 6};

  // Mask over text
  {
    tickit_renderbuffer_mask(rb, &mask);

    tickit_renderbuffer_text_at(rb, 3, 2, "ABCDEFG", NULL);      // before
    tickit_renderbuffer_text_at(rb, 4, 6, "HI", NULL);           // inside
    tickit_renderbuffer_text_at(rb, 5, 8, "JKLMN", NULL);        // after
    tickit_renderbuffer_text_at(rb, 6, 2, "OPQRSTUVWXYZ", NULL); // spanning

    tickit_renderbuffer_flush_to_term(rb, tt);
    is_termlog("RenderBuffer masking around text",
        GOTO(3, 2), SETPEN(), PRINT("ABC"),
        GOTO(5,11), SETPEN(), PRINT("MN"),
        GOTO(6, 2), SETPEN(), PRINT("OPQ"),
          GOTO(6,11), SETPEN(), PRINT("XYZ"),
        NULL);
  }

  // Mask over erase
  {
    tickit_renderbuffer_mask(rb, &mask);

    tickit_renderbuffer_erase_at(rb, 3, 2,  6, NULL); // before
    tickit_renderbuffer_erase_at(rb, 4, 6,  2, NULL); // inside
    tickit_renderbuffer_erase_at(rb, 5, 8,  5, NULL); // after
    tickit_renderbuffer_erase_at(rb, 6, 2, 12, NULL); // spanning

    tickit_renderbuffer_flush_to_term(rb, tt);
    is_termlog("RenderBuffer masking around erasech",
        GOTO(3, 2), SETPEN(), ERASECH(3,-1),
        GOTO(5,11), SETPEN(), ERASECH(2,-1),
        GOTO(6, 2), SETPEN(), ERASECH(3,-1),
          GOTO(6,11), SETPEN(), ERASECH(3,-1),
        NULL);
  }

  // Mask over lines
  {
    tickit_renderbuffer_mask(rb, &mask);

    tickit_renderbuffer_hline_at(rb, 3, 2,  8, TICKIT_LINE_SINGLE, NULL, 0);
    tickit_renderbuffer_hline_at(rb, 4, 6,  8, TICKIT_LINE_SINGLE, NULL, 0);
    tickit_renderbuffer_hline_at(rb, 5, 8, 13, TICKIT_LINE_SINGLE, NULL, 0);
    tickit_renderbuffer_hline_at(rb, 6, 2, 14, TICKIT_LINE_SINGLE, NULL, 0);

    tickit_renderbuffer_flush_to_term(rb, tt);
    is_termlog("RenderBuffer masking around lines",
        GOTO(3, 2), SETPEN(), PRINT("╶──"),
        GOTO(5,11), SETPEN(), PRINT("──╴"),
        GOTO(6, 2), SETPEN(), PRINT("╶──"),
          GOTO(6,11), SETPEN(), PRINT("───╴"),
        NULL);
  }

  // Restore removes masks
  {
    tickit_renderbuffer_save(rb);
    {
      tickit_renderbuffer_mask(rb, &mask);

      tickit_renderbuffer_text_at(rb, 3, 0, "AAAAAAAAAAAAAAAAAAAA", NULL);
    }
    tickit_renderbuffer_restore(rb);

    tickit_renderbuffer_text_at(rb, 4, 0, "BBBBBBBBBBBBBBBBBBBB", NULL);

    tickit_renderbuffer_flush_to_term(rb, tt);
    is_termlog("RenderBuffer save/restore removes mask",
        GOTO(3, 0), SETPEN(), PRINT("AAAAA"),
          GOTO(3,11), SETPEN(), PRINT("AAAAAAAAA"),
        GOTO(4, 0), SETPEN(), PRINT("BBBBBBBBBBBBBBBBBBBB"),
        NULL);
  }

  // translate over mask
  {
    tickit_renderbuffer_mask(rb, &(TickitRect){.top = 2, .left = 2, .lines = 1, .cols = 1});

    {
      tickit_renderbuffer_save(rb);
      tickit_renderbuffer_translate(rb, 0, 0);
      tickit_renderbuffer_text_at(rb, 0, 0, "A", NULL);
      tickit_renderbuffer_restore(rb);
    }
    {
      tickit_renderbuffer_save(rb);
      tickit_renderbuffer_translate(rb, 0, 2);
      tickit_renderbuffer_text_at(rb, 0, 0, "B", NULL);
      tickit_renderbuffer_restore(rb);
    }
    {
      tickit_renderbuffer_save(rb);
      tickit_renderbuffer_translate(rb, 2, 0);
      tickit_renderbuffer_text_at(rb, 0, 0, "C", NULL);
      tickit_renderbuffer_restore(rb);
    }
    {
      tickit_renderbuffer_save(rb);
      tickit_renderbuffer_translate(rb, 2, 2);
      tickit_renderbuffer_text_at(rb, 0, 0, "D", NULL);
      tickit_renderbuffer_restore(rb);
    }

    tickit_renderbuffer_flush_to_term(rb, tt);
    is_termlog("RenderBuffer translate over mask",
        GOTO(0,0), SETPEN(), PRINT("A"),
        GOTO(0,2), SETPEN(), PRINT("B"),
        GOTO(2,0), SETPEN(), PRINT("C"),
        // D was masked
        NULL);
  }

  // Mask out of limits doesn't SEGV
  {
    tickit_renderbuffer_save(rb);

    tickit_renderbuffer_mask(rb, &(TickitRect){.top = 0, .left = 0, .lines = 50, .cols = 200});

    tickit_renderbuffer_mask(rb, &(TickitRect){.top = -10, .left = -20, .lines = 5, .cols = 20});

    tickit_renderbuffer_restore(rb);
  }

  tickit_renderbuffer_destroy(rb);

  return exit_status();
}
