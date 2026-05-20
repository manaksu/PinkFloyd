#include <pebble.h>
#include <string.h>

/*
 * Pink Floyd Watch
 * Lyrics fill the screen, time words highlighted bold
 * Header: dark bar bleeding left, PINK FLOYD left-aligned
 * Top right: Month Day / DOW
 *
 * Hour + quarter past / half past / quarter to
 */

/* ── lyric entry ── */
typedef struct {
  const char *text;  /* full lyric, space-separated uppercase words */
  const char *h1;    /* hour word to highlight */
} Lyric;

static const Lyric s_lyrics[12][3] = {
  /* 1 */
  {{"AND THEN ONE WORLD AND ONE DAY YOU FIND JUST ONE LIFE ONE LOVE", "ONE"},
   {"ONE SLIP AND DOWN THE HOLE WE FALL IT TAKES NO TIME TO FALL AT ALL", "ONE"},
   {"ONE OF THESE DAYS IM GOING TO CUT YOU INTO LITTLE PIECES", "ONE"}},
  /* 2 */
  {{"TWO LOST SOULS SWIMMING IN A FISH BOWL YEAR AFTER YEAR RUNNING OVER THE SAME OLD GROUND", "TWO"},
   {"AND YOU RUN PAST THE SUN WITH THE TWO SINKING RACING AROUND", "TWO"},
   {"IS THERE ANYBODY OUT THERE AFTER TWO LONG YEARS", "TWO"}},
  /* 3 */
  {{"BREATHE BREATHE IN THE AIR DONT BE AFRAID TO CARE LEAVE BUT DONT LEAVE ME", "THREE"},
   {"THE MEMORIES OF A MAN IN HIS OLD AGE ARE THE DEEDS OF A MAN IN HIS PRIME", "THREE"},
   {"IS THERE ANYBODY OUT THERE THREE LONG YEARS OF SILENCE", "THREE"}},
  /* 4 */
  {{"WE DONT NEED NO EDUCATION WE DONT NEED NO THOUGHT CONTROL", "FOUR"},
   {"HEY YOU OUT THERE IN THE COLD GETTING LONELY GETTING OLD CAN YOU FEEL ME", "FOUR"},
   {"FOUR WALLS OF JERICHO CRUMBLE WHEN THE MUSIC DIES", "FOUR"}},
  /* 5 */
  {{"SHINE ON YOU CRAZY DIAMOND REMEMBER WHEN YOU WERE YOUNG YOU SHONE LIKE THE SUN", "FIVE"},
   {"FIVE MILES OUT FROM THE WIRE AND FALLING FAST", "FIVE"},
   {"REMEMBER WHEN YOU WERE YOUNG AT FIVE YOU SHONE LIKE THE SUN", "FIVE"}},
  /* 6 */
  {{"MONEY GET AWAY GET A GOOD JOB WITH MORE PAY AND YOURE OKAY", "SIX"},
   {"THE LUNATIC IS ON THE GRASS SIX FEET FROM THE EDGE", "SIX"},
   {"SIX OCLOCK IN THE MORNING THE LIGHT IS CALLING ME HOME", "SIX"}},
  /* 7 */
  {{"WELCOME MY SON WELCOME TO THE MACHINE WHERE HAVE YOU BEEN", "SEVEN"},
   {"YOU ARE YOUNG AND LIFE IS LONG AND THERE IS TIME TO KILL TODAY", "SEVEN"},
   {"SEVEN SEAS OF RHYE WILL I SEE YOU AGAIN", "SEVEN"}},
  /* 8 */
  {{"HELLO IS THERE ANYBODY IN THERE JUST NOD IF YOU CAN HEAR ME", "EIGHT"},
   {"COMFORTABLY NUMB JUST A LITTLE PINPRICK THERE WILL BE NO MORE AAAAAH", "EIGHT"},
   {"EIGHT MILES HIGH AND FALLING FAST SHE RODE OUT ON A CAROUSEL", "EIGHT"}},
  /* 9 */
  {{"ON THE TURNING AWAY FROM THE PALE AND DOWNTRODDEN AND THE WORDS THEY SAY", "NINE"},
   {"THE TIDE IS TURNING ROSA NINE DAYS IN THE VALLEY OF SHADOWS", "NINE"},
   {"TICKING AWAY AT NINE THE MOMENTS THAT MAKE UP A DULL DAY", "NINE"}},
  /* 10 */
  {{"AND THEN ONE DAY YOU FIND TEN YEARS HAVE GOT BEHIND YOU NO ONE TOLD YOU WHEN TO RUN", "TEN"},
   {"TICKING AWAY THE MOMENTS THAT MAKE UP A DULL DAY FRITTER AND WASTE THE HOURS", "TEN"},
   {"YOU MISSED THE STARTING GUN TEN SECONDS TO COMPLY", "TEN"}},
  /* 11 */
  {{"NOBODY HOME I GOT A LITTLE BLACK BOOK WITH MY POEMS IN AT ELEVEN", "ELEVEN"},
   {"HEY HEY RISE UP ELEVEN TIMES THE FLAME BURNS BRIGHTER", "ELEVEN"},
   {"THE SHOW MUST GO ON ELEVEN CURTAIN CALLS AND STILL THEY CHEER", "ELEVEN"}},
  /* 12 */
  {{"THE SUN IS THE SAME IN A RELATIVE WAY BUT YOURE OLDER SHORTER OF BREATH", "TWELVE"},
   {"AND ONE DAY CLOSER TO DEATH THE CLOCK STRIKES TWELVE AND REMINDS US", "TWELVE"},
   {"TWELVE NOON THE SUN IS HIGH THE CLOCK TICKS BY AND WE GROW OLD", "TWELVE"}},
};

static const char *minute_label(int min) {
  if (min >=  8 && min <= 22) return "QUARTER PAST";
  if (min >= 23 && min <= 36) return "HALF PAST";
  if (min >= 37 && min <= 51) return "QUARTER TO";
  return NULL;
}

static Window      *s_window;
static BitmapLayer *s_bg_layer;
static GBitmap     *s_bg_bitmap;
static Layer       *s_canvas;

static GFont s_font_sm;  /* FONT_SE_14 */
static GFont s_font_lg;  /* FONT_SE_18 */

static int  s_hour, s_min;
static char s_month_day[10];  /* "May 17" */
static char s_dow[5];         /* "MON" */

/* ── draw word-wrapped lyric with inline highlights ── */
static void draw_lyric(GContext *ctx, const char *text, const char *h1,
                        const char *min_lbl, int start_y) {
  char buf[256];
  /* combine lyric + minute label */
  if (min_lbl) {
    snprintf(buf, sizeof(buf), "%s %s", text, min_lbl);
  } else {
    strncpy(buf, text, sizeof(buf)-1);
    buf[sizeof(buf)-1] = '\0';
  }

  /* split by space, highlight h1 and minute words */
  const int MARGIN_L = 4;
  const int MARGIN_R = 140;
  int x = MARGIN_L, y = start_y;
  int line_h = 16;

  char *word = strtok(buf, " ");
  while (word) {
    bool hi = (h1 && strcmp(word, h1)==0)
           || (min_lbl && strstr(min_lbl, word) != NULL);
    GFont fnt  = hi ? s_font_lg : s_font_sm;
    GColor col = hi ? GColorBlack : GColorDarkGray;

    GSize wsz = graphics_text_layout_get_content_size(
      word, fnt, GRect(0,0,200,40),
      GTextOverflowModeWordWrap, GTextAlignmentLeft);

    if (x + wsz.w > MARGIN_R && x > MARGIN_L) {
      y += line_h + 1;
      x = MARGIN_L;
      line_h = 16;
    }

    if (y > 162) break;

    graphics_context_set_text_color(ctx, col);
    graphics_draw_text(ctx, word, fnt,
      GRect(x, y, MARGIN_R - x, 40),
      GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

    x += wsz.w + 3;
    if (hi && line_h < 20) line_h = 20;
    word = strtok(NULL, " ");
  }
}

/* ── canvas ── */
static void canvas_draw(Layer *layer, GContext *ctx) {
  GFont sys14 = fonts_get_system_font(FONT_KEY_GOTHIC_14);

  /* ── PINK FLOYD header bar ── */
  /* measure text width to know where bar ends */
  GSize hdr_sz = graphics_text_layout_get_content_size(
    "PINK FLOYD", s_font_sm, GRect(0,0,200,20),
    GTextOverflowModeWordWrap, GTextAlignmentLeft);
  int bar_right = 6 + hdr_sz.w + 10;

  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(0, 3, bar_right, 22), 2, GCornersRight);

  graphics_context_set_text_color(ctx, GColorLightGray);
  graphics_draw_text(ctx, "PINK FLOYD", s_font_sm,
    GRect(6, 5, hdr_sz.w + 4, 16),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  /* ── date top right ── */
  graphics_context_set_text_color(ctx, GColorDarkGray);
  graphics_draw_text(ctx, s_month_day, sys14,
    GRect(0, 4, 140, 14),
    GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
  graphics_draw_text(ctx, s_dow, sys14,
    GRect(0, 16, 140, 14),
    GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);

  /* ── lyric ── */
  int hidx    = s_hour % 12;
  int variant = (s_min / 5) % 3;
  const Lyric *l = &s_lyrics[hidx][variant];
  const char  *suffix = minute_label(s_min);

  draw_lyric(ctx, l->text, l->h1, suffix, 28);
}

static void update_time(struct tm *t) {
  s_hour = t->tm_hour;
  s_min  = t->tm_min;
  strftime(s_month_day, sizeof(s_month_day), "%b %d", t);
  strftime(s_dow,       sizeof(s_dow),       "%a",    t);
  layer_mark_dirty(s_canvas);
}

static void tick_handler(struct tm *tick_time, TimeUnits u) {
  update_time(tick_time);
}

static void window_load(Window *window) {
  Layer *root = window_get_root_layer(window);
  GRect  bounds = layer_get_bounds(root);

  s_font_sm = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_font_lg = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);

  s_bg_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PAPER_BG);
  s_bg_layer  = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_bg_layer, s_bg_bitmap);
  bitmap_layer_set_compositing_mode(s_bg_layer, GCompOpAssign);
  layer_add_child(root, bitmap_layer_get_layer(s_bg_layer));

  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, canvas_draw);
  layer_add_child(root, s_canvas);

  time_t now = time(NULL);
  update_time(localtime(&now));
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  bitmap_layer_destroy(s_bg_layer);
  gbitmap_destroy(s_bg_bitmap);
}

static void init(void) {
  s_window = window_create();
  window_set_background_color(s_window, GColorWhite);
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load, .unload = window_unload
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) { init(); app_event_loop(); deinit(); }
