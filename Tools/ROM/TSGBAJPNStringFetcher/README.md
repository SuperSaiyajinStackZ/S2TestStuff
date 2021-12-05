# TSGBAJPNStringFetcher

***TSGBAJPNStringFetcher*** is a tool written by SuperSaiyajinStackZ to "extract" in-game strings from The Sims Game Boy Advance Japanese games.

Right now it only outputs the raw bytes of the string, a TODO for that would be to decode the strings to proper strings.

## NOTE: Only untrimmed versions are supported as of now.


# Usage with -D_DIRECT_USE

> `-i <PathToROM> -id <Hexadecimal ID of the string>`

## Parameters

- `-i` or `-input`: The path to the GBA ROM of the Game (required).
- `-id`: The Hexadecimal ID of the String to fetch. See below for valid ID ranges (required).

***ID Ranges***
You can find the proper min and max values of the String IDs in the table below.

| Game                        | Min | Max    |
| --------------------------- | --- | ------ |
| The Sims Bustin' Out        | 0x0 | 0x1A02 |
| The Urbz - Sims in the City | 0x0 | 0x1AFD |


# How it works
***Section TODO.***