# TSGBAStringFetcher

***TSGBAStringFetcher*** is a tool written by SuperSaiyajinStackZ to "extract" in-game strings from The Sims Game Boy Advance games.

## NOTE: Only untrimmed versions are supported as of now.


# Usage with -D_DIRECT_USE

> `-i <PathToROM> -l <Language see on the Parameters section below> -id <Hexadecimal ID of the string>`

## Parameters

- `-i` or `-input`: The path to the GBA ROM of the Game (required).
- `-l` or `-language`: The language of the string to fetch. See below for valid languages (required).
- `-id`: The Hexadecimal ID of the String to fetch. See below for valid ID ranges (required).

***Languages***

- `English`: english *or* e
- `Dutch`: dutch *or* d
- `French`: french *or* f
- `German` german *or* g
- `Italian`: italian *or* i
- `Spanish`: spanish *or* s

***ID Ranges***
You can find the proper min and max values of the String IDs in the table below.

| Game                        | Min | Max    |
| --------------------------- | --- | ------ |
| The Sims Bustin' Out        | 0x0 | 0x1A02 |
| The Urbz - Sims in the City | 0x0 | 0x1AFD |
| The Sims 2                  | 0x0 | 0xD85  |


# How it works

***TODO, I still need to come up with a good explanation way.***