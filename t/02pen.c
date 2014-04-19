#include "tickit.h"
#include "taplib.h"

static int changed = 0;

void on_changed(TickitPen *pen, TickitEventType ev, TickitEvent *args, void *data)
{
  changed++;
}

int main(int argc, char *argv[])
{
  TickitPen *pen, *pen2;
  TickitPenAttr attr;

  pen = tickit_pen_new();

  ok(!!pen, "tickit_pen_new");

  tickit_pen_bind_event(pen, TICKIT_EV_CHANGE, on_changed, NULL);

  is_int(tickit_pen_attrtype(TICKIT_PEN_BOLD), TICKIT_PENTYPE_BOOL, "bold is a boolean attribute");

  is_int(tickit_pen_lookup_attr("b"), TICKIT_PEN_BOLD, "lookup_attr \"b\" gives bold");
  is_str(tickit_pen_attrname(TICKIT_PEN_BOLD), "b", "pen_attrname bold gives \"b\"");

  is_int(changed, 0, "change counter 0 initially");

  ok(!tickit_pen_has_attr(pen, TICKIT_PEN_BOLD), "pen lacks bold initially");
  ok(!tickit_pen_nondefault_attr(pen, TICKIT_PEN_BOLD), "pen bold attr is default initially");
  is_int(tickit_pen_get_bool_attr(pen, TICKIT_PEN_BOLD), 0, "bold 0 initially");

  ok(!tickit_pen_is_nonempty(pen), "pen initially empty");
  ok(!tickit_pen_is_nondefault(pen), "pen initially default");

  tickit_pen_set_bool_attr(pen, TICKIT_PEN_BOLD, 1);

  ok(tickit_pen_has_attr(pen, TICKIT_PEN_BOLD), "pen has bold after set");
  ok(tickit_pen_nondefault_attr(pen, TICKIT_PEN_BOLD), "pen bold attr is nondefault after set");
  is_int(tickit_pen_get_bool_attr(pen, TICKIT_PEN_BOLD), 1, "bold 1 after set");

  ok(tickit_pen_is_nonempty(pen), "pen non-empty after set bold on");
  ok(tickit_pen_is_nondefault(pen), "pen non-default after set bold on");

  is_int(changed, 1, "change counter 1 after set bold on");

  tickit_pen_set_bool_attr(pen, TICKIT_PEN_BOLD, 0);

  ok(!tickit_pen_nondefault_attr(pen, TICKIT_PEN_BOLD), "pen bold attr is default after set bold off");

  ok(tickit_pen_is_nonempty(pen), "pen non-empty after set bold off");
  ok(!tickit_pen_is_nondefault(pen), "pen default after set bold off");

  is_int(changed, 2, "change counter 2 after set bold off");

  tickit_pen_clear_attr(pen, TICKIT_PEN_BOLD);

  ok(!tickit_pen_has_attr(pen, TICKIT_PEN_BOLD), "pen lacks bold after clear");
  is_int(tickit_pen_get_bool_attr(pen, TICKIT_PEN_BOLD), 0, "bold 0 after clear");

  is_int(changed, 3, "change counter 3 after clear bold");

  is_int(tickit_pen_attrtype(TICKIT_PEN_FG), TICKIT_PENTYPE_COLOUR, "foreground is a colour attribute");

  ok(!tickit_pen_has_attr(pen, TICKIT_PEN_FG), "pen lacks foreground initially");
  is_int(tickit_pen_get_colour_attr(pen, TICKIT_PEN_FG), -1, "foreground -1 initially");

  tickit_pen_set_colour_attr(pen, TICKIT_PEN_FG, 4);

  ok(tickit_pen_has_attr(pen, TICKIT_PEN_FG), "pen has foreground after set");
  is_int(tickit_pen_get_colour_attr(pen, TICKIT_PEN_FG), 4, "foreground 4 after set");

  ok(tickit_pen_set_colour_attr_desc(pen, TICKIT_PEN_FG, "12"), "pen set foreground '12'");
  is_int(tickit_pen_get_colour_attr(pen, TICKIT_PEN_FG), 12, "foreground 12 after set '12'");

  ok(tickit_pen_set_colour_attr_desc(pen, TICKIT_PEN_FG, "green"), "pen set foreground 'green'");
  is_int(tickit_pen_get_colour_attr(pen, TICKIT_PEN_FG), 2, "foreground 2 after set 'green'");

  ok(tickit_pen_set_colour_attr_desc(pen, TICKIT_PEN_FG, "hi-red"), "pen set foreground 'hi-red'");
  is_int(tickit_pen_get_colour_attr(pen, TICKIT_PEN_FG), 8+1, "foreground 8+1 after set 'hi-red'");

  tickit_pen_clear_attr(pen, TICKIT_PEN_FG);

  ok(!tickit_pen_has_attr(pen, TICKIT_PEN_FG), "pen lacks foreground after clear");
  is_int(tickit_pen_get_colour_attr(pen, TICKIT_PEN_FG), -1, "foreground -1 after clear");

  pen2 = tickit_pen_new();

  ok(tickit_pen_equiv_attr(pen, pen2, TICKIT_PEN_BOLD), "pens have equiv bold attribute initially");

  tickit_pen_set_bool_attr(pen, TICKIT_PEN_BOLD, 1);

  ok(!tickit_pen_equiv_attr(pen, pen2, TICKIT_PEN_BOLD), "pens have unequiv bold attribute after set");

  ok(tickit_pen_equiv_attr(pen, pen2, TICKIT_PEN_ITALIC), "pens have equiv italic attribute");

  tickit_pen_set_bool_attr(pen, TICKIT_PEN_ITALIC, 0);
  ok(tickit_pen_equiv_attr(pen, pen2, TICKIT_PEN_ITALIC), "pens have equiv italic attribute after set 0");

  tickit_pen_copy_attr(pen2, pen, TICKIT_PEN_BOLD);
  ok(tickit_pen_equiv_attr(pen, pen2, TICKIT_PEN_BOLD), "pens have equiv bold attribute after copy attr");

  tickit_pen_clear_attr(pen2, TICKIT_PEN_BOLD);
  tickit_pen_copy(pen2, pen, 1);

  ok(tickit_pen_equiv_attr(pen, pen2, TICKIT_PEN_BOLD), "pens have equiv bold attribute after copy");

  tickit_pen_set_bool_attr(pen2, TICKIT_PEN_BOLD, 0);
  tickit_pen_copy(pen2, pen, 0);

  ok(!tickit_pen_equiv_attr(pen, pen2, TICKIT_PEN_BOLD), "pens have non-equiv bold attribute after copy no overwrite");

  tickit_pen_set_bool_attr(pen, TICKIT_PEN_UNDER, 0);
  tickit_pen_clear_attr(pen2, TICKIT_PEN_UNDER);
  tickit_pen_copy(pen2, pen, 1);

  ok(tickit_pen_has_attr(pen2, TICKIT_PEN_UNDER), "pen copy still copies present but default-value attributes");

  tickit_pen_destroy(pen);
  tickit_pen_destroy(pen2);

  pass("tickit_pen_destroy");

  return exit_status();
}
