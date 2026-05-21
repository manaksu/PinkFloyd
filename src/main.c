#include <pebble.h>
#include <string.h>

#define KEY_FONT     0
#define FONT_ROBOTO  0
#define FONT_SE      1
#define FONT_SCP     2
#define FONT_SYSTEM  3

static int s_font_mode = FONT_ROBOTO;
static Window      *s_window;
static Layer       *s_canvas;

static int  s_hour, s_min;
static GFont s_font_sm;
static GFont s_font_lg;
static GFont s_font_sm;
static GFont s_font_lg;
static char s_month_day[10];
static char s_dow[5];
static char s_date_line[32];

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

    GFont fnt  = sm;
    GColor col = hi ? GColorRed : GColorBlack;

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


/* ── song names matching lyrics ── */
static const char * const s_songs[12][3] = {
  /* 1 */  {"One Slip",        "One Slip",          "One of These Days"},
  /* 2 */  {"Wish You Were Here","Time",            "Echoes"},
  /* 3 */  {"Breathe",         "Free Four",         "Is There Anybody"},
  /* 4 */  {"Another Brick",   "Hey You",           "Another Brick"},
  /* 5 */  {"Shine On",        "Shine On",          "Five Miles Out"},
  /* 6 */  {"Money",           "Brain Damage",      "Time"},
  /* 7 */  {"Welcome to the Machine","Time",        "Seven Seas"},
  /* 8 */  {"Comfortably Numb","Comfortably Numb",  "Eight Miles High"},
  /* 9 */  {"On the Turning Away","A New Machine",  "Time"},
  /* 10 */ {"Time",            "Time",              "Time"},
  /* 11 */ {"Nobody Home",     "Rise Up",           "The Show Must Go On"},
  /* 12 */ {"Time",            "Time",              "Time"},
};
static void canvas_draw(Layer *layer, GContext *ctx) {
  GFont sm = s_font_sm;

  /* lyric first — so banner draws on top */
  int hidx    = s_hour % 12;
  int variant = (s_min / 5) % 3;
  draw_lyric(ctx, s_lyrics[hidx][variant],
             s_hour_words[hidx], minute_label(s_min));

  /* date — single line bottom, all caps */
  GFont date_font = fonts_get_system_font(FONT_KEY_GOTHIC_14);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(ctx, s_date_line, date_font,
    GRect(2, 152, 140, 16), GTextOverflowModeWordWrap, GTextAlignmentRight, NULL);

  /* battery bar bottom-left */
  BatteryChargeState batt = battery_state_service_peek();
  int bpct = batt.charge_percent;
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_rect(ctx, GRect(2, 157, 20, 8));
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, GRect(22, 159, 2, 4), 0, GCornerNone);
  GColor bfill = (bpct <= 20) ? GColorRed : GColorBlack;
  graphics_context_set_fill_color(ctx, bfill);
  graphics_fill_rect(ctx, GRect(3, 158, (18 * bpct / 100), 6), 0, GCornerNone);

  /* header bar — sized exactly to PINK FLOYD text width */
  GSize pf_sz = graphics_text_layout_get_content_size(
    "PINK FLOYD", sm, GRect(0,0,200,20),
    GTextOverflowModeWordWrap, GTextAlignmentLeft);
  graphics_context_set_fill_color(ctx, GColorDarkGray);
  graphics_fill_rect(ctx, GRect(0, 3, 4 + pf_sz.w + 6, 20), 0, GCornerNone);
  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_draw_text(ctx, "PINK FLOYD",
    sm,
    GRect(4, 5, pf_sz.w + 2, 16),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);

  /* song name — top right, pick font size by string length */
  int song_x = 4 + pf_sz.w + 10;
  const char *song = s_songs[s_hour%12][((s_min/5)%3)];
  GFont song_font = (strlen(song) <= 12)
    ? fonts_get_system_font(FONT_KEY_GOTHIC_14)
    : fonts_get_system_font(FONT_KEY_GOTHIC_09);
  graphics_context_set_text_color(ctx, GColorDarkGray);
  graphics_draw_text(ctx, song, song_font,
    GRect(song_x, 3, 144-song_x-2, 22),
    GTextOverflowModeWordWrap, GTextAlignmentLeft, NULL);
}

static void update_time(struct tm *t) {
  s_hour = t->tm_hour;
  s_min  = t->tm_min;
  strftime(s_month_day, sizeof(s_month_day), "%b %d", t);
  strftime(s_dow,       sizeof(s_dow),       "%a",    t);
  /* build single caps date line: "20 MAY, MONDAY" */
  char day_num[4], mon_name[5], full_day[12];
  strftime(day_num,  sizeof(day_num),  "%d",  t);
  strftime(mon_name, sizeof(mon_name), "%b",  t);
  strftime(full_day, sizeof(full_day), "%A",  t);
  snprintf(s_date_line, sizeof(s_date_line), "%s %s, %s", day_num, mon_name, full_day);
  for (int i=0; s_date_line[i]; i++)
    if (s_date_line[i]>='a' && s_date_line[i]<='z') s_date_line[i]-=32;
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
  window_set_background_color(s_window, GColorWhite);
  s_canvas = layer_create(bounds);
  layer_set_update_proc(s_canvas, canvas_draw);
  layer_add_child(root, s_canvas);
  /* load fonts based on setting */
  if (s_font_mode == FONT_ROBOTO) {
    s_font_sm = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_RB_14));
    s_font_lg = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_RB_18));
  } else if (s_font_mode == FONT_SE) {
    s_font_sm = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SE_14));
    s_font_lg = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SE_18));
  } else if (s_font_mode == FONT_SCP) {
    s_font_sm = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SCP_12));
    s_font_lg = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_SCP_16));
  } else {
    s_font_sm = fonts_get_system_font(FONT_KEY_GOTHIC_14);
    s_font_lg = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
  }
  time_t now = time(NULL);
  update_time(localtime(&now));
}

static void window_unload(Window *window) {
  layer_destroy(s_canvas);
  if (s_font_mode != FONT_SYSTEM) {
    fonts_unload_custom_font(s_font_sm);
    fonts_unload_custom_font(s_font_lg);
  }
}

static void init(void) {
  s_font_mode = persist_exists(KEY_FONT) ? persist_read_int(KEY_FONT) : FONT_ROBOTO;
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
