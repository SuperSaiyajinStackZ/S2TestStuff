/*
*   This file is part of TS2GBAMenuAddr
*   Copyright (C) 2021-2022 SuperSaiyajinStackZ
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
	The Sims 2 Game Boy Advance ROM Tools
	------------------------------------
	File: TS2GBAMenuAddr.cpp | TS2GBAMenuAddr.hpp
	Authors: SuperSaiyajinStackZ
	Version: 0.1.0
	Purpose: Get a Menu's function pointer addresses of The Sims 2 GBA.
	Category: ROM Tools
	Last Updated: 17 December 2021
	------------------------------------

	To compile this, run:
	g++ -D_DIRECT_USE -std=c++20 TS2GBAMenuAddr.cpp -o TS2GBAMenuAddr.exe
	^ If you want to use your own int main, then don't include -D_DIRECT_USE.
*/


#include "TS2GBAMenuAddr.hpp" // Header of this file.
#include <cstring> // memcmp to compare the ROM's TID.
#include <unistd.h> // access() to check for ROM File existence.



/*
	Constructor for TS2GBAMenuAddr, which checks the passed ROM for validation and loads it into RAM.

	const std::string &ROMPath: The path to the ROM to load.
*/
TS2GBAMenuAddr::TS2GBAMenuAddr(const std::string &ROMPath) {
	if (access(ROMPath.c_str(), F_OK) != 0) return;

	FILE *In = fopen(ROMPath.c_str(), "rb");

	if (In) {
		fseek(In, 0, SEEK_END);
		const uint32_t Size = ftell(In);
		fseek(In, 0, SEEK_SET);

		if (Size == this->ROMSize) {
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

				/* Check the Title ID of the ROM. */
				if (memcmp(&this->TID, &IDFromROM, 0x4) == 0) {
					this->ROMData = std::make_unique<uint8_t[]>(Size);
					fread(this->ROMData.get(), 0x1, Size, In);
					this->ROMValid = true;
				}
			}
		}

		fclose(In);
	}
};


/*
	Get a Menu's function pointer address of the prepare function or the logic.

	const uint32_t MenuID: The Menu ID to get the addresses from. ( 0x0 - 0x27 is valid ).
	const bool OnPrepare: If getting the prepare function address (true) or the logic (false).
*/
uint32_t TS2GBAMenuAddr::GetMenuAddress(const uint32_t MenuID, const bool OnPrepare) const {
	if (!this->GetValid() || !this->ROMData || !this->ROMData.get() || MenuID >= this->GetMenuAmount()) return 0;

	const uint32_t Addr =  *reinterpret_cast<uint32_t *>(this->ROMData.get() + (OnPrepare ? 0x064F84 : 0x064F88) + (MenuID * 12));
	if (Addr >= 0x08000001) return Addr - 0x08000000 - 1; // The -1 at the end, because it actually would be 1 byte *after* the function.
	else return 0;
};


/* Direct Use would include this int main. */
#ifdef _DIRECT_USE

	int AbortMain(const std::string &Msg) {
		printf("%s\n", Msg.c_str());
		return 0;
	};

	int main(int Argc, char *Argv[]) {
		if (Argc > 1) {
			bool Provided[2] = { false, false };

			std::string ROMPath = "";
			uint32_t MenuID = 0x0;

			/* Go through all Arguments. */
			for (size_t Idx = 1; Idx < (size_t)Argc; Idx++) {
				const std::string ARG = Argv[Idx];

				/* -i => Input. */
				if (ARG == "-i" || ARG == "-input") {
					if (Idx + 1 >= Argc) return AbortMain("No argument provided after '-i'.");
					ROMPath = Argv[Idx + 1];

					Provided[0] = true;
					Idx++;
					continue;

				/* -id => Menu ID in hexadecimal. */
				} else if (ARG == "-id") {
					if (Idx + 1 >= Argc) return AbortMain("No argument provided after '-id'.");
					
					MenuID = strtoul(Argv[Idx + 1], nullptr, 16);

					Provided[1] = true;
					Idx++;
					continue;

				} else {
					return AbortMain("Not a valid parameter provided.");
				}
			}

			/* Ensure all needed parameters have been provided to work on. */
			for (int8_t Idx = 0; Idx < 2; Idx++) {
				if (!Provided[Idx]) return AbortMain("Not all needed parameters have been provided.");
			}

			/* The actual action. */
			std::unique_ptr<TS2GBAMenuAddr> Getter = std::make_unique<TS2GBAMenuAddr>(ROMPath);
			if (Getter && Getter->GetValid()) {
				if (MenuID >= Getter->GetMenuAmount()) return AbortMain("The Menu ID is too high");

				uint32_t MenuAddr = Getter->GetMenuAddress(MenuID, true);
				if (MenuAddr > 0x0) {
					printf("The function pointer address for the prepare function of ID %X is: %08X or when running: %08X.\n", MenuID, MenuAddr, MenuAddr + 0x08000000);
				}

				MenuAddr = Getter->GetMenuAddress(MenuID, false);
				if (MenuAddr > 0x0) {
					printf("The function pointer address for the logic function of ID %X is: %08X or when running: %08X.\n", MenuID, MenuAddr, MenuAddr + 0x08000000);
				}

			} else {
				return AbortMain("The provided ROM is either not supported, trimmed or doesn't exist.");
			}

		/* No arguments provided => Show info. */
		} else {
			printf(
				"TS2GBAMenuAddr v0.1.0 by SuperSaiyajinStackZ, © 2021-2022.\n" \
				"Purpose: Get a Menu's function pointer addresses of The Sims 2 GBA.\n\n" \
				"Usage: -i <PathToROM> -id <Hexadecimal ID of the Menu>\n\n" \
				"Use -i or -input and then the path to the ROM to provide it as the ROM Source.\n" \
				"Use -id to provide the ID of the menu in hexadecimal format you want to get the addresses from.\n"
			);
		}

		return 0;
	};

#endif