#include <pebble.h>
#include <string.h>

static Window *s_main_window;
static Layer *s_lyrics_layer;
static GFont s_font;

// Word structure for rendering
typedef struct {
  char text[32];
  bool highlight;
  GRect rect;
} WordInfo;

static WordInfo s_words[64];
static int s_word_count;

// Pink Floyd lyrics corpus
static const char *lyrics[] = {
  "AND THEN ONE DAY YOU FIND TEN YEARS HAVE GOT BEHIND YOU",
  "NO ONE TOLD YOU WHEN TO RUN YOU MISSED THE STARTING GUN",
  "AND YOU RUN AND YOU RUN TO CATCH UP WITH THE SUN BUT IT'S SINKING",
  "RACING AROUND TO COME UP BEHIND YOU AGAIN",
  "THE SUN IS THE SAME IN A RELATIVE WAY BUT YOU'RE OLDER",
  "SHORTER OF BREATH AND ONE DAY CLOSER TO DEATH",
  "EVERY YEAR IS GETTING SHORTER NEVER SEEM TO FIND THE TIME",
  "PLANS THAT EITHER COME TO NAUGHT OR HALF A PAGE OF SCRIBBLED LINES",
  "HANGING ON IN QUIET DESPERATION IS THE ENGLISH WAY",
  "THE TIME IS GONE THE SONG IS OVER THOUGHT I'D SOMETHING MORE TO SAY",
  "WELCOME TO THE MACHINE WE TOLD YOU WHAT TO DREAM",
  "YOU CHOSE A CAREER IN TELEVISION",
  "WHAT DID YOU DREAM IT'S ALRIGHT WE TOLD YOU WHAT TO DREAM",
  "SHINE ON YOU CRAZY DIAMOND",
  "NOBODY KNOWS WHERE YOU ARE TO HOW NEAR OR HOW FAR",
  "THERE'S NO WAY OUT OF HERE",
  "DANCE ON THE VAULTED CEILING",
  "TAKE YOUR WINGS AND FLY OUT OF HERE",
  "COMFORTABLY NUMB HELLO IS THERE ANYBODY IN THERE",
  "JUST NOD IF YOU CAN HEAR ME",
  "IS THERE ANYONE AT HOME",
  "I CAN FEEL IT COMING IN THE AIR TONIGHT",
  "WISH YOU WERE HERE SO SO YOU THINK YOU CAN TELL",
  "HEAVEN FROM HELL BLUE SKIES FROM PAIN",
  "DID YOU GET YOU TO TRADE YOUR HEROES FOR GHOSTS",
  "WE'RE JUST TWO LOST SOULS SWIMMING IN A FISH BOWL",
  "THE WALL ANOTHER BRICK IN THE WALL",
  "WE DON'T NEED NO EDUCATION",
  "HEY TEACHER LEAVE THEM KIDS ALONE",
  "ALL IN ALL IT'S JUST ANOTHER BRICK IN THE WALL",
  "MONEY GET AWAY GET A GOOD JOB WITH MORE PAY AND YOUR OKAY",
  "MONEY IT'S A GAS GRAB THAT CASH WITH BOTH HANDS",
  "SHARE IT FAIRLY BUT DON'T TAKE A SLICE OF MY PIE",
  "US AND THEM AND AFTER ALL WE'RE ONLY ORDINARY MEN",
  "ME AND YOU GOD ONLY KNOWS IT'S NOT WHAT WE WOULD CHOOSE TO DO",
  "FORWARD HE CRIED FROM THE REAR AND THE FRONT RANK DIED",
  "THE GENERAL SAT AND THE LINES ON THE MAP MOVED FROM SIDE TO SIDE",
  "BREATHE BREATHE IN THE AIR",
  "DON'T BE AFRAID TO CARE",
  "LEAVE BUT DON'T LEAVE ME",
  "LOOK AROUND AND CHOOSE YOUR OWN GROUND",
  "LONG YOU LIVE AND HIGH YOU FLY",
  "AND SMILES YOU'LL GIVE AND TEARS YOU'LL CRY",
  "AND ALL YOU TOUCH AND ALL YOU SEE",
  "IS ALL YOUR LIFE WILL EVER BE"
};

static const int num_lyrics = sizeof(lyrics) / sizeof(lyrics[0]);

// Time word mappings
static const char *hour_words[] = {
  "TWELVE", "ONE", "TWO", "THREE", "FOUR", "FIVE", "SIX",
  "SEVEN", "EIGHT", "NINE", "TEN", "ELEVEN"
};

static const char *minute_words[] = {
  "", "FIVE", "TEN", "QUARTER", "TWENTY", "TWENTYFIVE", "HALF"
};

static int s_current_hour;
static int s_current_minute;

// Convert string to uppercase
static void to_upper(char *str) {
  for (int i = 0; str[i]; i++) {
    str[i] = toupper(str[i]);
  }
}

// Check if a word is a time word and should be highlighted
static bool is_time_word(const char *word) {
  char upper_word[32];
  strncpy(upper_word, word, sizeof(upper_word) - 1);
  upper_word[sizeof(upper_word) - 1] = '\0';
  to_upper(upper_word);
  
  // Determine time words based on current time
  int display_hour = s_current_hour % 12;
  if (display_hour == 0) display_hour = 12;
  
  int next_hour = (s_current_hour % 12) + 1;
  if (next_hour == 13) next_hour = 1;
  
  bool use_past = s_current_minute <= 30;
  int display_minute = use_past ? s_current_minute : 60 - s_current_minute;
  int minute_index = display_minute / 5;
  if (minute_index > 6) minute_index = 6;
  
  // Check for hour words
  if (strcmp(upper_word, hour_words[display_hour - 1]) == 0) {
    return true;
  }
  
  // Check for next hour (when using "to")
  if (!use_past && strcmp(upper_word, hour_words[next_hour - 1]) == 0) {
    return true;
  }
  
  // Check for minute words
  if (s_current_minute > 0 && minute_index > 0 && minute_index <= 6) {
    if (strcmp(upper_word, minute_words[minute_index]) == 0) {
      return true;
    }
  }
  
  // Check for "PAST" or "TO"
  if (use_past && s_current_minute > 0 && strcmp(upper_word, "PAST") == 0) {
    return true;
  }
  if (!use_past && s_current_minute > 0 && strcmp(upper_word, "TO") == 0) {
    return true;
  }
  
  return false;
}

// Parse lyrics into words with highlighting info
static void parse_lyrics() {
  s_word_count = 0;
  
  // Select random lyrics (4 lines)
  int indices[4];
  for (int i = 0; i < 4; i++) {
    indices[i] = rand() % num_lyrics;
  }
  
  for (int line = 0; line < 4; line++) {
    const char *lyric = lyrics[indices[line]];
    char word[32];
    int word_pos = 0;
    
    for (int i = 0; lyric[i] != '\0' && s_word_count < 64; i++) {
      if (lyric[i] == ' ') {
        word[word_pos] = '\0';
        
        if (word_pos > 0) {
          strncpy(s_words[s_word_count].text, word, sizeof(s_words[s_word_count].text) - 1);
          s_words[s_word_count].text[sizeof(s_words[s_word_count].text) - 1] = '\0';
          s_words[s_word_count].highlight = is_time_word(word);
          s_word_count++;
        }
        word_pos = 0;
      } else {
        word[word_pos++] = lyric[i];
      }
    }
    
    // Handle last word
    if (word_pos > 0 && s_word_count < 64) {
      word[word_pos] = '\0';
      strncpy(s_words[s_word_count].text, word, sizeof(s_words[s_word_count].text) - 1);
      s_words[s_word_count].text[sizeof(s_words[s_word_count].text) - 1] = '\0';
      s_words[s_word_count].highlight = is_time_word(word);
      s_word_count++;
    }
  }
}

// Custom layer update callback for rendering lyrics
static void lyrics_layer_update_callback(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_bounds(layer);
  
  // Clear background
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);
  
  // Calculate word positions
  int x = 5;
  int y = 5;
  int line_height = 18;
  int max_width = bounds.size.w - 10;
  
  for (int i = 0; i < s_word_count; i++) {
    GSize word_size = graphics_text_layout_get_content_size(
      s_words[i].text,
      s_font,
      GRect(0, 0, max_width, line_height),
      GTextOverflowModeWordWrap,
      GTextAlignmentLeft
    );
    
    // Check if word fits on current line
    if (x + word_size.w > max_width) {
      x = 5;
      y += line_height;
    }
    
    s_words[i].rect = GRect(x, y, word_size.w, line_height);
    x += word_size.w + 5; // Add space between words
  }
  
  // Draw words
  for (int i = 0; i < s_word_count; i++) {
    if (s_words[i].highlight) {
      // Draw highlighted word with yellow background and black text
      graphics_context_set_fill_color(ctx, GColorYellow);
      graphics_fill_rect(ctx, s_words[i].rect, 2, GCornersAll);
      graphics_context_set_text_color(ctx, GColorBlack);
    } else {
      // Draw normal word with white text
      graphics_context_set_text_color(ctx, GColorWhite);
    }
    
    graphics_draw_text(
      ctx,
      s_words[i].text,
      s_font,
      s_words[i].rect,
      GTextOverflowModeTrailingEllipsis,
      GTextAlignmentLeft,
      NULL
    );
  }
}

static void update_time() {
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  
  s_current_hour = tick_time->tm_hour;
  s_current_minute = tick_time->tm_min;
  
  parse_lyrics();
  layer_mark_dirty(s_lyrics_layer);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  update_time();
}

static void main_window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  
  // Load custom font
  s_font = fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD);
  
  // Create custom lyrics layer
  s_lyrics_layer = layer_create(GRect(0, 0, bounds.size.w, bounds.size.h));
  layer_set_update_proc(s_lyrics_layer, lyrics_layer_update_callback);
  layer_add_child(window_layer, s_lyrics_layer);
  
  // Seed random number generator
  srand(time(NULL));
  
  update_time();
}

static void main_window_unload(Window *window) {
  layer_destroy(s_lyrics_layer);
}

static void init() {
  s_main_window = window_create();
  window_set_window_handlers(s_main_window, (WindowHandlers) {
    .load = main_window_load,
    .unload = main_window_unload
  });
  window_set_background_color(s_main_window, GColorBlack);
  window_stack_push(s_main_window, true);
  
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

static void deinit() {
  window_destroy(s_main_window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
