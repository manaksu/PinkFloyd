# Pink Floyd Time Watchface

A Pebble Time Steel (Basalt) watchface that displays random Pink Floyd lyrics with the current time highlighted within the lyrics.

## Features

- **Random Pink Floyd Lyrics**: Displays 4 random lines from a corpus of Pink Floyd song lyrics
- **Time Highlighting**: Words representing the current time are highlighted in yellow with black text
- **Word Clock Format**: Time is displayed in natural language (e.g., "TEN PAST TWO" for 10:02)
- **Dynamic Updates**: Updates every minute with new random lyrics

## How It Works

The watchface uses a word clock format to display time:
- Hours: ONE through TWELVE
- Minutes: FIVE, TEN, QUARTER, TWENTY, TWENTYFIVE, HALF
- Prepositions: PAST (for minutes 1-30) or TO (for minutes 31-59)

For example:
- 10:02 → "TEN PAST TWO"
- 3:15 → "THREE QUARTER PAST THREE"
- 7:45 → "QUARTER TO EIGHT"

The watchface randomly selects 4 lines of Pink Floyd lyrics and highlights any words that match the current time expression.

## Building

### Prerequisites

- Pebble SDK 3.x
- Python 2.7 (for build tools)
- Pebble Tool CLI

### Build Instructions

1. Install the Pebble SDK if you haven't already:
   ```bash
   # Follow instructions from https://developer.pebble.com/sdk/
   ```

2. Navigate to the project directory:
   ```bash
   cd /Users/manaksu/CascadeProjects/pink-floyd-watchface
   ```

3. Build the project:
   ```bash
   pebble build
   ```

4. Install to your Pebble Time Steel:
   ```bash
   pebble install --phone <your-phone-ip>
   ```

   Or use the Pebble mobile app to install the `.pbw` file from the `build/` directory.

## Project Structure

```
pink-floyd-watchface/
├── appinfo.json          # App metadata and configuration
├── wscript              # Build configuration
├── package.json         # Node.js package info
├── README.md            # This file
└── src/
    └── main.c           # Main watchface code
```

## Customization

### Adding More Lyrics

Edit the `lyrics` array in `src/main.c` to add more Pink Floyd lyrics:

```c
static const char *lyrics[] = {
  "YOUR NEW LYRIC HERE",
  "ANOTHER LYRIC LINE",
  // ... existing lyrics
};
```

### Changing Highlight Color

Modify the highlight color in `lyrics_layer_update_callback()` in `src/main.c`:

```c
// Change GColorYellow to any Pebble color
graphics_context_set_fill_color(ctx, GColorYellow);
```

Available colors include: `GColorRed`, `GColorGreen`, `GColorBlue`, `GColorYellow`, `GColorCyan`, `GColorMagenta`, etc.

### Adjusting Font Size

Change the font in `main_window_load()`:

```c
// Change FONT_KEY_GOTHIC_14_BOLD to other system fonts
s_font = fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD);
```

Available fonts: `FONT_KEY_GOTHIC_14`, `FONT_KEY_GOTHIC_14_BOLD`, `FONT_KEY_GOTHIC_18`, `FONT_KEY_GOTHIC_18_BOLD`, `FONT_KEY_GOTHIC_24`, `FONT_KEY_GOTHIC_24_BOLD`, `FONT_KEY_GOTHIC_28`, `FONT_KEY_BITHAM_30_BLACK`, `FONT_KEY_BITHAM_42_BOLD`, etc.

## License

This project is open source and available for personal use.

## Credits

- Inspired by the concept of word clocks
- Lyrics from various Pink Floyd songs
- Built with Pebble SDK for Basalt (Pebble Time Steel)
