/*
*   This file is part of TSGBAJPNStringFetcher
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
	------------------------------------
	The Sims Game Boy Advance ROM Tools
	------------------------------------
	File: TSGBAJPNStringFetcher.cpp | TSGBAJPNStringFetcher.hpp
	Authors: SuperSaiyajinStackZ
	Version: 0.2.0
	Purpose: "Extract" in-game strings as raw bytes from The Sims Game Boy Advance Japanese games.
	Category: ROM Tools
	Last Updated: 05 December 2021
	------------------------------------

	To compile this, run:
	g++ -D_DIRECT_USE -std=c++20 TSGBAJPNStringFetcher.cpp -o TSGBAJPNStringFetcher.exe
	^ If you want to use your own int main, then don't include -D_DIRECT_USE.
*/


#include "TSGBAJPNStringFetcher.hpp" // Header of this file.
#include <cstring> // memcmp to compare the ROM's TID.
#include <unistd.h> // access() to check for ROM File existence.



/*
	Constructor for TSGBAJPNStringFetcher, which checks the passed ROM for validation and loads it into RAM.

	const std::string &ROMPath: The path to the ROM to load.
*/
TSGBAJPNStringFetcher::TSGBAJPNStringFetcher(const std::string &ROMPath) {
	if (access(ROMPath.c_str(), F_OK) != 0) return;

	FILE *In = fopen(ROMPath.c_str(), "rb");

	if (In) {
		fseek(In, 0, SEEK_END);
		const uint32_t Size = ftell(In);
		fseek(In, 0, SEEK_SET);

		if (Size >= this->MinROMSize && Size <= this->MaxROMSize) {
			/* Check for the Magic Byte at 0xB2 that it's 0x96. */
			uint8_t MagicByte = 0x0;
			fseek(In, 0xB2, SEEK_SET);
			fread(&MagicByte, 0x1, 0x1, In);
			fseek(In, 0, SEEK_SET);

			if (MagicByte == 0x96) {
				uint8_t IDFromROM[4] = { '\0' };
				fseek(In, 0xAC, SEEK_SET);
				fread(&IDFromROM, 0x1, 0x4, In);
				fseek(In, 0, SEEK_SET);

				for (uint8_t Idx = 0; Idx < 2; Idx++) {
					/* Check the Title ID of the ROM. */
					if (memcmp(&this->TIDs[Idx], &IDFromROM, 0x4) == 0) {
						this->ActiveGame = (TSGBAJPNStringFetcher::Games)Idx;
						break;
					}
				}

				if (this->SupportedGame()) {
					this->ROMData = std::make_unique<uint8_t[]>(Size);
					fread(this->ROMData.get(), 0x1, Size, In);
				}
			}
		}

		fclose(In);
	}
};



/*
	Fetches a string from the ROM in a uint8_t vector.

	const uint16_t StringID: The ID of the string to fetch.

	Returns a std::vector<uint8_t> with the wanted string.
*/
std::vector<uint8_t> TSGBAJPNStringFetcher::Fetch(const uint16_t StringID) {
	std::vector<uint8_t> ResVec          = { };
	uint8_t              Counter         = 0x0;
	uint16_t             Character       = 0x0;
	uint32_t             ShiftVal        = 0x0;
	uint32_t             ShiftAddr       = 0x0;
	TSGBAJPNStringFetcher::StringLocs     Locs;

	/* Ensure the data are valid and the ID is in proper range before we do it. */
	if (this->SupportedGame() && this->ROMData && this->ROMData.get() && StringID <= this->GetMaxStringID()) {
		Locs = this->GetLocForGame();

		ShiftAddr = (Locs.Address1 + *reinterpret_cast<uint32_t *>(this->ROMData.get() + (StringID * 0x4) + Locs.Address2));
		ShiftVal = *reinterpret_cast<uint32_t *>(this->ROMData.get() + ShiftAddr);

		do {
			Character = 0x100;

			do {
				Character = *reinterpret_cast<uint16_t *>(this->ROMData.get() + (Character * 0x4) + Locs.Address3 - (((ShiftVal >> Counter) % 0x2) == 0 ? 0x400 : 0x3FE));
				Counter++;

				if (Counter == 0x8) {
					Counter = 0x0;
					ShiftAddr++;
					ShiftVal = *reinterpret_cast<uint32_t *>(this->ROMData.get() + ShiftAddr);
				}
			} while (0xFF < Character);

			ResVec.push_back((uint8_t)Character);
		} while (Character != 0x0);
	}

	return ResVec; // Return the uint8_t vector string.
};


/* Direct Use would include this int main. */
#ifdef _DIRECT_USE

	int AbortMain(const std::string &Msg) {
		printf("%s.\n", Msg.c_str());
		return 0;
	};

	int main(int Argc, char *Argv[]) {
		if (Argc > 1) {
			bool Provided[2] = { false, false };

			std::string ROMPath = "";
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

				/* -id => String ID in hexadecimal. */
				} else if (ARG == "-id") {
					if (Idx + 1 >= Argc) return AbortMain("No argument provided after '-id'");
					
					StringID = strtoul(Argv[Idx + 1], nullptr, 16);

					Provided[1] = true;
					Idx++;
					continue;

				} else {
					return AbortMain("Not a valid parameter provided");
				}
			}

			/* Ensure all needed parameters have been provided to work on. */
			for (int Idx = 0; Idx < 2; Idx++) {
				if (!Provided[Idx]) return AbortMain("Not all needed parameters have been provided");
			}

			/* The actual action. */
			std::unique_ptr<TSGBAJPNStringFetcher> Fetcher = std::make_unique<TSGBAJPNStringFetcher>(ROMPath);
			if (Fetcher && Fetcher->SupportedGame()) {
				if (StringID > Fetcher->GetMaxStringID()) return AbortMain("The String ID is too high");
				
				std::vector<uint8_t> Fetched = Fetcher->Fetch(StringID);

				printf("Your wanted string is:\n");
				for (size_t Idx = 0; Idx < Fetched.size(); Idx++) {
					if (Idx < Fetched.size() - 1) printf("%02X, ", Fetched[Idx]);
					else printf("%02X\n", Fetched[Idx]);
				}

			} else {
				return AbortMain("The provided ROM is either not supported, trimmed or doesn't exist");
			}

		/* No arguments provided => Show info. */
		} else {
			printf(
				"TSGBAJPNStringFetcher v0.2.0 by SuperSaiyajinStackZ, © 2021.\n" \
				"Purpose: 'Extract' in-game strings as raw bytes from The Sims Game Boy Advance Japanese games.\n\n" \
				"Usage: -i <PathToROM> -id <Hexadecimal ID of the string>\n\n" \
				"Use -i or -input and then the path to the ROM to provide it as the ROM Source.\n" \
				"Use -id to provide the ID of the string in hexadecimal format you want to fetch.\n"
			);
		}

		return 0;
	};

#endif