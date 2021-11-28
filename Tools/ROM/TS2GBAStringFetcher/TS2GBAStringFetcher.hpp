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

#ifndef _TS2GBA_STRING_FETCHER_HPP
#define _TS2GBA_STRING_FETCHER_HPP

#include <memory> // std::unique_ptr because i like those instead of raw pointers.
#include <string> // Default include, it's always in my projects.

class TS2GBAStringFetcher {
public:
	/* All available Languages can be found on this enum class. */
	enum class Languages : uint8_t { English = 0, Dutch = 1, French = 2, German = 3, Italian = 4, Spanish = 5 };

	TS2GBAStringFetcher(const std::string &ROMPath);

	std::string Fetch(const uint16_t StringID, const Languages Language = Languages::English);

	static constexpr uint16_t StringAmount = 0xD86;
	static constexpr uint8_t LanguageAmount = 0x6;
	bool ROMValid = false;
private:
	std::unique_ptr<uint8_t[]> ROMData = nullptr;

	/*
		Language String related Locations.
		
		[0]: The start location where the language is stored(?) also related to the ShiftingAddress.
		[1]: Related to the (StringID * 0x4) read value for the ShiftingAddress.
		[2]: Related to the 0x400 / 0x3FE thing.
	*/
	static constexpr uint32_t StringLocs[6][3] = {
		{ 0x019B4990, 0x019B4B20, 0x019B4994 }, // English.
		{ 0x019D7784, 0x019D7924, 0x019D7788 }, // Dutch.
		{ 0x019FAF9C, 0x019FB154, 0x019FAFA0 }, // French.
		{ 0x01A1F7E0, 0x01A1F98C, 0x01A1F7E4 }, // German.
		{ 0x01A460A0, 0x01A46254, 0x01A460A4 }, // Italian.
		{ 0x01A697C0, 0x01A69978, 0x01A697C4 }  // Spanish.
	};


	static constexpr uint8_t TID[4] = { 0x42, 0x34, 0x36, 0x45 }; // Gamecode / Title ID Located at 0xAC, "B46E".
	static constexpr uint32_t ROMSize = 0x2000000;


	/*
		Decoding Section starting at 0x7B - 0xBA.

		I would assume the game only contains the really needed characters, so it uses ASCII or whatever it is and it's special encoding / decoding.
		Also 0x40 (@) has a special meaning in the game, it seems to be used for formatting such as '@1' for parameter 1 or so.
	*/
	const std::string DecodingTable[0x40] = {
		"©", "œ", "¡", "¿", "À", "Á", "Â", "Ã", "Ä", "Å", "Æ", "Ç", "È", "É", "Ê", "Ë",
		"Ì", "Í", "Î", "Ï", "Ñ", "Ò", "Ó", "Ô", "Õ", "Ö", "Ø", "Ù", "Ú", "Ü", "ß", "à",
		"á", "â", "ã", "ä", "å", "æ", "ç", "è", "é", "ê", "ë", "ì", "í", "î", "ï", "ñ",
		"ò", "ó", "ô", "õ", "ö", "ø", "ù", "ú", "û", "ü", "º", "ª", "…", "™", "", "®"
	};

	/*
		Decodes the passed string with the table above to a new string and returns it.
		Maybe a better way should be there? For now that way works prolly fine.

		const std::string &StringToDecode: The string to decode.
	*/
	std::string Decode(const std::string &StringToDecode) const;
};

#endif