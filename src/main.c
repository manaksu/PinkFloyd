#include <pebble.h>
#include <string.h>

#define KEY_FONT   0
#define FONT_SYSTEM  0
#define FONT_CUSTOM  1

static int s_font_mode = FONT_CUSTOM;
static Window      *s_window;
static BitmapLayer *s_bg_layer;
static GBitmap     *s_bg_bitmap;
static Layer       *s_canvas;

static int  s_hour, s_min;
static GFont s_font_sm;
static GFont s_font_lg;
static GFont s_font_sm;
static GFont s_font_lg;
static char s_month_day[10];
static char s_dow[5];

static const char * const s_lyrics[12][3] = {
  /* 1 */
  {"AND THEN ONE DAY YOU FIND ONE LIFE IS ALL WE HAVE NO ONE TOLD YOU WHEN TO RUN YOU MISSED THE STARTING GUN AND YOU RUN AND YOU RUN",
   "ONE SLIP AND DOWN THE HOLE WE FALL IT TAKES NO TIME TO FALL AT ALL ONE STEP AND THEN WE ALL RETURN TO DUST AND THE MEMORIES FADE",
   "ONE OF THESE DAYS IM GOING TO CUT YOU INTO LITTLE PIECES ONE BY ONE THE VOICES GROWING LOUDER IN THE DARKNESS OF MY MIND"},
  /* 2 */
  {"TWO LOST SOULS SWIMMING IN A FISH BOWL YEAR AFTER YEAR RUNNING OVER THE SAME OLD GROUND WHAT HAVE WE FOUND THE SAME OLD FEARS",
   "AND YOU RUN PAST THE SUN WITH THE TWO SINKING RACING AROUND TO COME UP BEHIND YOU AGAIN THE SUN IS THE SAME IN A RELATIVE WAY",
   "IS THERE ANYBODY OUT THERE AFTER TWO LONG YEARS THE SILENCE GROWS THE WALL GROWS TALLER AND THE LIGHT GROWS EVER DIMMER NOW"},
  /* 3 */
  {"BREATHE BREATHE IN THE AIR DONT BE AFRAID TO CARE LEAVE BUT DONT LEAVE ME LOOK AROUND AND CHOOSE YOUR OWN GROUND THREE STEPS",
   "THE MEMORIES OF A MAN THREE YEARS GONE IN HIS OLD AGE ARE THE DEEDS OF A MAN IN HIS PRIME AND THE YEARS GO BY LIKE BROKEN DREAMS",
   "IS THERE ANYBODY OUT THERE THREE LONG YEARS OF SILENCE AND THE WALL STILL STANDS TALL BETWEEN US AND THE WORLD OUTSIDE"},
  /* 4 */
  {"WE DONT NEED NO EDUCATION WE DONT NEED NO THOUGHT CONTROL FOUR WALLS OF DARKNESS CLOSING IN AND ALL IN ALL WE ARE JUST BRICKS",
   "HEY YOU OUT THERE IN THE COLD GETTING LONELY GETTING OLD CAN YOU FEEL ME AT FOUR IN THE COLD DARK MORNING ALL ALONE AND LOST",
   "FOUR MINUTES TO MIDNIGHT THE WALL COMES DOWN ALL IN ALL YOU ARE JUST ANOTHER BRICK IN THE WALL ANOTHER FACE BEHIND THE GLASS"},
  /* 5 */
  {"SHINE ON YOU CRAZY DIAMOND REMEMBER WHEN YOU WERE YOUNG FIVE YEARS YOU SHONE LIKE THE SUN NOW THERES A LOOK IN YOUR EYES",
   "REMEMBER WHEN YOU WERE YOUNG AT FIVE YOU SHONE LIKE THE SUN NOW THERES A LOOK IN YOUR EYES LIKE BLACK HOLES IN THE SKY",
   "FIVE MILES OUT FROM THE WIRE AND FALLING FAST THE SUNS GOING DOWN AND THE WIND CARRIES DUST AND THE MEMORIES OF BETTER DAYS"},
  /* 6 */
  {"MONEY GET AWAY SIX DAYS A WEEK GET A GOOD JOB WITH MORE PAY AND YOURE OKAY MONEY ITS A GAS GRAB THAT CASH WITH BOTH HANDS",
   "THE LUNATIC IS ON THE GRASS SIX FEET FROM THE EDGE REMEMBERING GAMES AND DAISY CHAINS AND LAUGHS GOT TO KEEP THE LOONIES ON",
   "SIX OCLOCK IN THE MORNING THE LIGHT IS CALLING ME HOME AND THE PAPERS HAVE ALL GONE TO BED ONLY THE LONELY WAIT FOR DAWN"},
  /* 7 */
  {"WELCOME MY SON WELCOME TO THE MACHINE WHERE HAVE YOU BEEN SEVEN YEARS BEHIND THE GLASS AND WHAT DID YOU DREAM IT DOESNT MATTER",
   "YOU ARE YOUNG AND LIFE IS LONG AND THERE IS TIME TO KILL TODAY SEVEN DAYS A WEEK AND THEN ONE DAY YOU FIND TEN YEARS BEHIND",
   "SEVEN SEAS OF RHYE WILL I SEE YOU AGAIN FEAR ME YOU LORDS AND LADY PREACHERS I DESCEND UPON YOUR EARTH AND SWALLOW IT WHOLE"},
  /* 8 */
  {"HELLO IS THERE ANYBODY IN THERE JUST NOD IF YOU CAN HEAR ME IS THERE ANYONE AT HOME EIGHT YEARS HAVE PASSED IN SILENCE NOW",
   "COMFORTABLY NUMB EIGHT MILES FROM THE STARTING LINE JUST A LITTLE PINPRICK THERE WILL BE NO MORE AAAH BUT YOU MAY FEEL SICK",
   "EIGHT MILES HIGH AND FALLING FAST SHE RODE OUT ON A CAROUSEL AND SLIPPED INTO MY POCKET WITH THE CARNIVAL AND ALL THE CLOWNS"},
  /* 9 */
  {"ON THE TURNING AWAY FROM THE PALE AND DOWNTRODDEN NINE DAYS IN THE VALLEY AND THE WORDS THEY SAY WHICH WE WON'T UNDERSTAND",
   "THE TIDE IS TURNING NINE TIMES THE SUN HAS SET AND STILL WE WAIT FOR SOMETHING THAT WILL NEVER COME AGAIN LIKE IT DID BEFORE",
   "TICKING AWAY AT NINE THE MOMENTS THAT MAKE UP A DULL DAY FRITTER AND WASTE THE HOURS IN AN OFFHAND WAY DIGGING AROUND"},
  /* 10 */
  {"AND THEN ONE DAY YOU FIND TEN YEARS HAVE GOT BEHIND YOU NO ONE TOLD YOU WHEN TO RUN YOU MISSED THE STARTING GUN AND YOU RUN",
   "TICKING AWAY FRITTER AND WASTE TEN HOURS IN AN OFFHAND WAY YOU MISSED THE STARTING GUN AND YOU RUN AND YOU RUN TO CATCH THE SUN",
   "YOU MISSED THE STARTING GUN TEN SECONDS TO COMPLY AND THE SUN IS THE SAME IN A RELATIVE WAY BUT YOURE OLDER SHORTER OF BREATH"},
  /* 11 */
  {"NOBODY HOME I GOT A LITTLE BLACK BOOK WITH MY POEMS IN GOT A BAG WITH A TOOTHBRUSH AND A COMB IN MY ROOM AT ELEVEN OCLOCK",
   "HEY HEY RISE UP ELEVEN TIMES THE FLAME BURNS BRIGHTER THAN BEFORE AND THE DARKNESS FADES AND THE MORNING BREAKS THROUGH COLD",
   "THE SHOW MUST GO ON ELEVEN CURTAIN CALLS AND STILL THEY CHEER WHILE DEEP INSIDE THE EMPTINESS GROWS IN THE SPACE BETWEEN US"},
  /* 12 */
  {"THE SUN IS THE SAME IN A RELATIVE WAY BUT YOURE OLDER SHORTER OF BREATH AND TWELVE YEARS CLOSER TO DEATH AND ONE DAY MORE",
   "AND ONE DAY CLOSER TO DEATH THE CLOCK STRIKES TWELVE AND REMINDS US THAT THE TIME WE HAVE IS RUNNING OUT LIKE SAND THROUGH GLASS",
   "TWELVE NOON THE SUN IS HIGH THE CLOCK TICKS BY AND WE GROW OLD AND THE YEARS PASS LIKE MOMENTS IN THE WIND AND WE ARE GONE"},
};

static const char * const s_hour_words[12] = {
  "ONE","TWO","THREE","FOUR","FIVE","SIX",
  "SEVEN","EIGHT","NINE","TEN","ELEVEN","TWELVE"
};

static const char *minute_label(int min) {
  if (min >=  8 && min <= 22) return "QUARTER PAST";
  if (min >= 23 && min <= 36) return "HALF PAST";
  if (min >= 37 && min <= 51) return "QUARTER TO";
  return NULL;
}

static void draw_lyric(GContext *ctx, const char *text,
                       const char *hw, const char *suffix) {
  GFont sm = s_font_sm;
  GFont lg = s_font_lg;

  /* build mutable buffer */
  char buf[280];
  if (suffix) {
    snprintf(buf, sizeof(buf), "%s %s", text, suffix);
  } else {
    snprintf(buf, sizeof(buf), "%s", text);
  }

  int x = 4, y = 26, lh = 16;
  int bi = 0;
  char word[32];

  while (buf[bi] != '\0') {
    while (buf[bi] == ' ') bi++;
    if (buf[bi] == '\0') break;
    int wi = 0;
    while (buf[bi] != ' ' && buf[bi] != '\0' && wi < 30)
      word[wi++] = buf[bi++];
    word[wi] = '\0';
    if (wi == 0) break;

    bool hi = (hw && strcmp(word, hw) == 0);
    if (!hi && suffix) {
      if (strcmp(word,"QUARTER")==0 || strcmp(word,"HALF")==0 ||
          strcmp(word,"PAST")==0    || strcmp(word,"TO")==0)
        hi = true;
    }

    GFont fnt  = hi ? lg : sm;
    GColor col = hi ? GColorBlack : GColorDarkGray;

    GSize wsz = graphics_text_layout_get_content_size(
      word, fnt, GRect(0,0,200,40),
      GTextOverflowModeWordWrap, GTextAlignmentLeft);

    if (x + wsz.w > 140 && x > 4) {
      y += lh + 1;
      x = 4;
      lh = 16;
    }
    if (y > 162) break;

    graphics_context_set_text_color(ctx, col);
    graphics_draw_text(ctx, word, fnt,
      GRect(x, y, 140-x, 24),
      GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

    x += wsz.w + 3;
    if (hi && lh < 20) lh = 20;
  }
}


/* ── Dark Side prism battery indicator ── */
static void draw_prism_battery(GContext *ctx) {
  BatteryChargeState batt = battery_state_service_peek();
  int pct = (int)batt.charge_percent;

  /* prism coordinates — small, bottom-right corner */
  const int ox=108, oy=140, pw=28, ph=24;
  int tx=ox+pw/2, ty=oy;
  int lx=ox,      ly=oy+ph;
  int rx=ox+pw,   ry=oy+ph;

  /* entry left face, exit right face */
  int entry_x=(tx+lx)/2, entry_y=(ty+ly)/2;
  int exit_x=(tx+rx)/2,  exit_y=(ty+ry)/2 - 2;

  /* incoming rays start */
  int in_sx  = entry_x - 18;
  int in_s1y = 160;
  int in_s2y = 164;

  /* 3 outgoing ray endpoints */
  const int NUM_RAYS = 3;
  int out_ex = 144;
  int out_ey[3] = {exit_y, exit_y+5, exit_y+11};

  int filled = (pct * (NUM_RAYS-1)) / 100;
  if (pct > 0 && filled == 0) filled = 1;

  /* fill triangle left to right */
  int fill_w = pw * pct / 100;
  for (int x = ox; x <= ox + fill_w; x++) {
    int y_top;
    if (x <= tx) {
      y_top = ly + (ty - ly) * (x - lx) / (tx - lx);
    } else {
      y_top = ty + (ry - ty) * (x - tx) / (rx - tx);
    }
    graphics_context_set_stroke_color(ctx, GColorLightGray);
    graphics_draw_line(ctx, GPoint(x, y_top), GPoint(x, ly));
  }

  /* fill between incoming rays */
  if (pct > 0) {
    int steps_l = entry_x - in_sx;
    if (steps_l > 0) {
      for (int x = in_sx; x <= entry_x; x++) {
        int t_num = x - in_sx;
        int ya = in_s1y + (entry_y - 2 - in_s1y) * t_num / steps_l;
        int yb = in_s2y + (entry_y + 2 - in_s2y) * t_num / steps_l;
        if (ya > yb) { int tmp=ya; ya=yb; yb=tmp; }
        graphics_context_set_stroke_color(ctx, GColorLightGray);
        graphics_draw_line(ctx, GPoint(x, ya), GPoint(x, yb));
      }
    }
  }

  /* fill outgoing ray gaps */
  int steps_r = out_ex - exit_x;
  if (steps_r > 0) {
    for (int i = 0; i < NUM_RAYS - 1; i++) {
      int ri = NUM_RAYS - 2 - i;
      if (i < filled) {
        for (int x = exit_x; x <= out_ex; x++) {
          int t_num = x - exit_x;
          int ya = exit_y + (out_ey[ri+1] - exit_y) * t_num / steps_r;
          int yb = exit_y + (out_ey[ri]   - exit_y) * t_num / steps_r;
          if (ya > yb) { int tmp=ya; ya=yb; yb=tmp; }
          graphics_context_set_stroke_color(ctx, GColorLightGray);
          graphics_draw_line(ctx, GPoint(x, ya), GPoint(x, yb));
        }
      }
    }
  }

  /* outgoing ray lines */
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_stroke_width(ctx, 1);
  for (int i = 0; i < NUM_RAYS; i++) {
    graphics_draw_line(ctx, GPoint(exit_x, exit_y), GPoint(out_ex, out_ey[i]));
  }

  /* 2 incoming rays */
  graphics_draw_line(ctx, GPoint(in_sx, in_s1y), GPoint(entry_x, entry_y-2));
  graphics_draw_line(ctx, GPoint(in_sx, in_s2y), GPoint(entry_x, entry_y+2));

  /* prism outline */
  graphics_draw_line(ctx, GPoint(tx,ty), GPoint(lx,ly));
  graphics_draw_line(ctx, GPoint(lx,ly), GPoint(rx,ry));
  graphics_draw_line(ctx, GPoint(rx,ry), GPoint(tx,ty));
}

static void canvas_draw(Layer *layer, GContext *ctx) {
  GFont sm = s_font_sm;

  /* header bar — flush top-left, bleeds left edge, ends after FLOYD */
  GSize hdr = graphics_text_layout_get_content_size(
    "PINK FLOYD", sm, GRect(0,0,200,20),
    GTextOverflowModeWordWrap, GTextAlignmentLeft);
  int bar_r = 4 + hdr.w + 8;
  /* semi-transparent effect — dark overlay on paper bg */
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, GRect(0, 0, bar_r, 20), 0, GCornerNone);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, "PINK FLOYD", sm,
    GRect(4, 1, hdr.w + 4, 18),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  /* prism battery */
  draw_prism_battery(ctx);

  /* date top right */
  graphics_context_set_text_color(ctx, GColorDarkGray);
  graphics_draw_text(ctx, s_month_day, sm,
    GRect(0, 1, 140, 14), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);
  graphics_draw_text(ctx, s_dow, sm,
    GRect(0, 13, 140, 14), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);

  /* lyric */
  int hidx    = s_hour % 12;
  int variant = (s_min / 5) % 3;
  draw_lyric(ctx, s_lyrics[hidx][variant],
             s_hour_words[hidx], minute_label(s_min));
}

static void update_time(struct tm *t) {
  s_hour = t->tm_hour;
  s_min  = t->tm_min;
  strftime(s_month_day, sizeof(s_month_day), "%b %d", t);
  strftime(s_dow,       sizeof(s_dow),       "%a",    t);
  layer_mark_dirty(s_canvas);
}

static void inbox_received(DictionaryIterator *iter, void *ctx) {
  Tuple *t = dict_find(iter, KEY_FONT);
  if (t) {
    s_font_mode = (int)t->value->int32;
    persist_write_int(KEY_FONT, s_font_mode);
    /* reload window to swap fonts */
    window_stack_pop(true);
    window_stack_push(s_window, true);
  }
}

static void tick_handler(struct tm *t, TimeUnits u) { update_time(t); }

static void window_load(Window *window) {
  Layer *root   = window_get_root_layer(window);
  GRect  bounds = layer_get_bounds(root);
  s_bg_bitmap = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_PAPER_BG);
  s_bg_layer  = bitmap_layer_create(bounds);
  bitmap_layer_set_bitmap(s_bg_layer, s_bg_bitmap);
  bitmap_layer_set_compositing_mode(s_bg_layer, GCompOpAssign);
  layer_add_child(root, bitmap_layer_get_layer(s_bg_layer));
  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, canvas_draw);
  layer_add_child(root, s_canvas);
  s_font_sm = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  s_font_lg = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  /* load fonts based on setting */
  if (s_font_mode == FONT_CUSTOM) {
    s_font_sm = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SCP_14));
    s_font_lg = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SCP_18));
  } else {
    s_font_sm = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    s_font_lg = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  }
  time_t now = time(NULL);
  update_time(localtime(&now));
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  bitmap_layer_destroy(s_bg_layer);
  gbitmap_destroy(s_bg_bitmap);
  if (s_font_mode == FONT_CUSTOM) {
    fonts_unload_custom_font(s_font_sm);
    fonts_unload_custom_font(s_font_lg);
  }
}

static void init(void) {
  s_font_mode = persist_exists(KEY_FONT) ? persist_read_int(KEY_FONT) : FONT_SYSTEM;
  s_window = window_create();
  window_set_window_handlers(s_window, (WindowHandlers){
    .load = window_load, .unload = window_unload
  });
  window_stack_push(s_window, true);
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
  app_message_open(64, 64);
  app_message_register_inbox_received(inbox_received);
}

static void deinit(void) {
  tick_timer_service_unsubscribe();
  window_destroy(s_window);
}

int main(void) { init(); app_event_loop(); deinit(); }
