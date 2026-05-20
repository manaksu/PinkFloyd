# Pink Floyd Watch

A Pebble Time Steel watchface that tells the time through Pink Floyd lyrics.

## Concept
The screen fills with lyrics — time words (TEN, TWO, HALF, QUARTER) are highlighted bold and darker. Everything else fades to muted grey on cream paper.

## Time Format
- **On the hour** — hour word highlighted (e.g. TEN)
- **8-22 min** — QUARTER PAST highlighted
- **23-36 min** — HALF PAST highlighted
- **37-51 min** — QUARTER TO highlighted

## Songs used
Time, Wish You Were Here, Comfortably Numb, Money, Another Brick in the Wall, Shine On You Crazy Diamond, Welcome to the Machine, Hey You, One of These Days, On the Turning Away, and more.

## CloudPebble Setup
Add fonts twice in Resources:
| Name | File | Size |
|------|------|------|
| `FONT_SE_14` | `SpecialElite-Regular.ttf` | `14` |
| `FONT_SE_18` | `SpecialElite-Regular.ttf` | `18` |

## Layout
- **PINK FLOYD** — dark bar bleeding from left edge, right edge ends at D
- **Date** — top right, two lines: `May 17` / `MON`
- **Lyrics** — natural word-wrap, fills the screen
