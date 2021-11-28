/*
*   This file is part of TS2GBAStringFetcher
*   Copyright (C) 2021 SuperSaiyajinStackZ
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/


/*
	-------------------------------------
	The Sims 2 Game Boy Advance ROM Tools
	-------------------------------------
	File: TS2GBAStringFetcher.cpp | TS2GBAStringFetcher.hpp
	Authors: SuperSaiyajinStackZ
	Version: 0.1
	Purpose: "Extract" in-game strings from The Sims 2 Game Boy Advance.
	Category: ROM Tools
	Last Updated: 28 November 2021
	-------------------------------------

	To compile this, run:
	g++ -D_DIRECT_USE -std=c++20 TS2GBAStringFetcher.cpp -o TS2GBAStringFetcher.exe
	^ If you want to use your own int main, then don't include -D_DIRECT_USE.
*/


#include "TS2GBAStringFetcher.hpp" // Header of this file.
#include <cstring> // memcmp to compare the ROM's TID.
#include <memory> // std::unique_ptr because i like those instead of raw pointers.
#include <string> // Default include, it's always in my projects.
#include <unistd.h> // access() to check for ROM File existence.



/*
	Constructor for TS2GBAStringFetcher, which checks the passed ROM for validation and loads it into RAM.

	const std::string &ROMPath: The path to the ROM to load.
*/
TS2GBAStringFetcher::TS2GBAStringFetcher(const std::string &ROMPath) {
	if (access(ROMPath.c_str(), F_OK) != 0) return;

	FILE *In = fopen(ROMPath.c_str(), "rb");

	if (In) {
		fseek(In, 0, SEEK_END);
		const uint32_t Size = ftell(In);
		fseek(In, 0, SEEK_SET);

		if (Size == this->ROMSize) {
			uint8_t IDFromROM[4] = { '\0' };
			fseek(In, 0xAC, SEEK_SET);
			fread(&IDFromROM, 0x1, 0x4, In);
			fseek(In, 0, SEEK_SET);

			/* Check the Title ID of the ROM. */
			if (memcmp(&this->TID, &IDFromROM, 0x4) == 0) {
				this->ROMData = std::make_unique<uint8_t[]>(this->ROMSize);
				fread(this->ROMData.get(), 0x1, this->ROMSize, In);
				this->ROMValid = true;
			}
		}

		fclose(In);
	}
};



/*
	Fetches a string from the ROM.

	const uint16_t StringID: The ID of the string to fetch ( 0x0 - 0xD85 is valid ).
	const Languages Language: The language to fetch.

	Returns a std::string with the wanted string.
*/
std::string TS2GBAStringFetcher::Fetch(const uint16_t StringID, const TS2GBAStringFetcher::Languages Language) {
	std::string TempStr         =  "";
	uint8_t     WantedLanguage  = 0x0;
	uint8_t     Counter         = 0x0;
	uint16_t    Character       = 0x0;
	uint32_t    ShiftVal        = 0x0;
	uint32_t    ShiftAddr       = 0x0;

	/* Ensure the data are valid and the ID is in proper range before we do it. */
	if (this->ROMValid && this->ROMData && this->ROMData.get() && StringID < this->StringAmount) {
		/* Ensure a valid language has been provided. */
		switch(Language) {
			case TS2GBAStringFetcher::Languages::English:
			case TS2GBAStringFetcher::Languages::Dutch:
			case TS2GBAStringFetcher::Languages::French:
			case TS2GBAStringFetcher::Languages::German:
			case TS2GBAStringFetcher::Languages::Italian:
			case TS2GBAStringFetcher::Languages::Spanish:
				WantedLanguage = (uint8_t)Language;
				break;

			default:
				return ""; // Invalid check for out of range things.
		}

		ShiftAddr = (this->StringLocs[WantedLanguage][0] + *reinterpret_cast<uint32_t *>(this->ROMData.get() + (StringID * 0x4) + this->StringLocs[WantedLanguage][1]));
		ShiftVal = *reinterpret_cast<uint32_t *>(this->ROMData.get() + ShiftAddr);

		do {
			Character = 0x100;

			do {
				Character = *reinterpret_cast<uint16_t *>(
					this->ROMData.get() + (Character * 0x4) + this->StringLocs[WantedLanguage][2] - (((ShiftVal >> Counter) % 0x2) == 0 ? 0x400 : 0x3FE)
				);

				Counter++;

				if (Counter == 0x8) {
					Counter = 0x0;
					ShiftAddr++;
					ShiftVal = *reinterpret_cast<uint32_t *>(this->ROMData.get() + ShiftAddr);
				}
			} while (0xFF < Character);

			TempStr.push_back((uint8_t)Character);
		} while (Character != 0x0);
	}

	return this->Decode(TempStr); // Return the decoded string.
};



/*
	Decodes the passed string with the encoding / decoding table to a new string and returns it.
	Maybe a better way should be there? For now that way works prolly fine.

	const std::string &StringToDecode: The string to decode.
*/
std::string TS2GBAStringFetcher::Decode(const std::string &StringToDecode) const {
	if (StringToDecode.empty()) return ""; // Do nothing as it's empty.
	std::string NewString = "";

	for (size_t CurIdx = 0; CurIdx < StringToDecode.size(); CurIdx++) {
		const uint8_t CurChar = (uint8_t)StringToDecode[CurIdx];

		/* 0x1 - 0x9: Invalid below range;        0xB - 0x1F: Invalid below range too;    0xBC+: Invalid above range. */
		if ((CurChar >= 0x1 && CurChar <= 0x9) || (CurChar >= 0xB && CurChar <= 0x1F) || (CurChar >= 0xBC)) continue;
		
		/* 0x7B up to 0xBA are the sign / letter things which aren't like real ASCII. 0xBB seems to be literally blank or so and 0xBC+ is invalid. */
		if (CurChar >= 0x7B && CurChar <= 0xBA) {
			NewString += (this->DecodingTable[CurChar - 0x7B]);

		/* Otherwise seems to be normal ASCII or whatever it is and push it normally back. */
		} else {
			NewString += StringToDecode[CurIdx];
		}
	}

	return NewString;
};



/* Direct Use would include this int main. */
#ifdef _DIRECT_USE

	int AbortMain(const std::string &Msg) {
		printf("%s.\n", Msg.c_str());
		return 0;
	};

	int main(int Argc, char *Argv[]) {
		if (Argc > 1) {
			bool Provided[3] = { false, false, false };

			std::string ROMPath = "";
			TS2GBAStringFetcher::Languages WantedLang = TS2GBAStringFetcher::Languages::English;
			uint16_t StringID = 0x0;

			/* Go through all Arguments. */
			for (size_t Idx = 1; Idx < (size_t)Argc; Idx++) {
				const std::string ARG = Argv[Idx];

				/* -i => Input. */
				if (ARG == "-i" || ARG == "-input") {
					if (Idx + 1 >= Argc) return AbortMain("No argument provided after '-i'");
					ROMPath = Argv[Idx + 1];

					Provided[0] = true;
					Idx++;
					continue;

				/* -l => Language. */
				} else if (ARG == "-l" || ARG == "-language") {
					if (Idx + 1 >= Argc) return AbortMain("No argument provided after '-l'");

					const std::string Lang = Argv[Idx + 1];

					if (Lang == "english" || Lang == "e") {
						WantedLang = TS2GBAStringFetcher::Languages::English;

					} else if (Lang == "dutch" || Lang == "d") {
						WantedLang = TS2GBAStringFetcher::Languages::Dutch;

					} else if (Lang == "french" || Lang == "f") {
						WantedLang = TS2GBAStringFetcher::Languages::French;

					} else if (Lang == "german" || Lang == "g") {
						WantedLang = TS2GBAStringFetcher::Languages::German;

					} else if (Lang == "italian" || Lang == "i") {
						WantedLang = TS2GBAStringFetcher::Languages::Italian;

					} else if (Lang == "spanish" || Lang == "s") {
						WantedLang = TS2GBAStringFetcher::Languages::Spanish;

					} else {
						return AbortMain("No valid language has been provided with the parameter '-l'");
					}

					Provided[1] = true;
					Idx++;
					continue;

				/* -id => String ID in hexadecimal. */
				} else if (ARG == "-id") {
					if (Idx + 1 >= Argc) return AbortMain("No argument provided after '-id'");
					
					StringID = strtoul(Argv[Idx + 1], nullptr, 16);
					if (StringID >= TS2GBAStringFetcher::StringAmount) return AbortMain("The provided ID is out of range. Valid ID range: 0x0 - 0xD85");

					Provided[2] = true;
					Idx++;
					continue;

				} else {
					return AbortMain("Not a valid parameter provided");
				}
			}

			/* Ensure all needed parameters have been provided to work on. */
			for (int Idx = 0; Idx < 3; Idx++) {
				if (!Provided[Idx]) return AbortMain("Not all needed parameters have been provided");
			}

			/* The actual action. */
			std::unique_ptr<TS2GBAStringFetcher> Fetcher = std::make_unique<TS2GBAStringFetcher>(ROMPath);
			if (Fetcher && Fetcher->ROMValid) {
				std::string Fetched = Fetcher->Fetch(StringID, WantedLang);
				printf("Your wanted string is:\n%s\n", Fetched.c_str());

			} else {
				return AbortMain("The provided ROM is either not correct, trimmed or doesn't exist");
			}

		/* No arguments provided => Show info. */
		} else {
			printf(
				"TS2GBAStringFetcher v0.1.0 by SuperSaiyajinStackZ, © 2021.\n" \
				"Purpose: 'Extract' in-game strings from The Sims 2 Game Boy Advance.\n\n" \
				"Usage: -i <PathToROM> -l <Language see below> -id <Hexadecimal ID between 0 and D85>\n\n" \
				"Use -i or -input and then the path to the ROM to provide it as the ROM Source.\n" \
				"Use -l or -language to provide the language you want the string to be.\n" \
				"Use -id to provide the ID of the string in hexadecimal format you want to fetch.\n\n" \
				"Valid Languages:\nenglish or e\ndutch or d\nfrench or f\ngerman or g\nitalian or i\nspanish or s\n"
			);
		}

		return 0;
	};

#endif