# TS2GBAMenuAddr

***TS2GBAMenuAddr*** is a tool written by SuperSaiyajinStackZ to get the function pointer addresses for the Menus from **The Sims 2 Game Boy Advance**.

## NOTE: Only untrimmed versions are supported as of now.


# Usage with -D_DIRECT_USE

> `-i <PathToROM> -id <Hexadecimal ID of the Menu>`

## Parameters

- `-i` or `-input`: The path to the GBA ROM of the Game (required).
- `-id`: The Hexadecimal ID of the Menu to get the function pointer addresses from (required). ID Range: 0x0 - 0x27.

***Menu ID List***

You can find a list of the Menu IDs below. Menu's marked with `?? (Black screen)` still need to be figured out and some may black screen and eventually just freeze or crash the game if not used correctly.

| ID   | Menu                                                                           |
| ---- | ------------------------------------------------------------------------------ |
| 0x0  | ?? (Black screen)                                                              |
| 0x1  | In-Game                                                                        |
| 0x2  | The dialog menu with all the options you can do in-game (such as "What's up?") |
| 0x3  | Intro #1                                                                       |
| 0x4  | Intro #2                                                                       |
| 0x5  | Main Menu with the Create-a-Sim, Load-a-Sim, ... Options                       |
| 0x6  | The Slot Loading Menu                                                          |
| 0x7  | ?? (Black screen)                                                              |
| 0x8  | Multi Player Link Connection                                                   |
| 0x9  | In-Game SELECT Menu                                                            |
| 0xA  | Language Selection                                                             |
| 0xB  | Plot Points Menu                                                               |
| 0xC  | Cast Member Menu                                                               |
| 0xD  | Social Action / Conversation Menu                                              |
| 0xE  | Inventory Menu                                                                 |
| 0xF  | Episode Selection Menu                                                         |
| 0x10 | Shop Menu                                                                      |
| 0x11 | Map                                                                            |
| 0x12 | ?? (Black screen)                                                              |
| 0x13 | ?? (Black screen)                                                              |
| 0x14 | Bigfoot Love Chickens                                                          |
| 0x15 | Car Commercial                                                                 |
| 0x16 | Message Box (Black screens if not used correctly)                              |
| 0x17 | Create-a-Sim Menu                                                              |
| 0x18 | Minigame Level Selection                                                       |
| 0x19 | Minigame results                                                               |
| 0x1A | Keelhaulin' Cards                                                              |
| 0x1B | Cattle Cleanup                                                                 |
| 0x1C | Let's watch the show!                                                          |
| 0x1D | Goth Family Let's watch the show!                                              |
| 0x1E | Options Menu with selecting an Episode etc                                     |
| 0x1F | King Chug Chug                                                                 |
| 0x20 | Canyon Jumping                                                                 |
| 0x21 | Credits                                                                        |
| 0x22 | Chop Shop                                                                      |
| 0x23 | Skills Menu                                                                    |
| 0x24 | Testing Menu(?)                                                                |
| 0x25 | Dusty Hogg motorbike sharkpool cutscene                                        |
| 0x26 | Game Ending cutscene                                                           |
| 0x27 | Minigames Menu                                                                 |


# How it works

TODO, maybe again in a blog like TSGBAStringFetcher.