/*
*   This file is part of TSGBAStringFetcher
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

#ifndef _TSGBA_STRING_FETCHER_HPP
#define _TSGBA_STRING_FETCHER_HPP

#include <memory> // std::unique_ptr because i like those instead of raw pointers.
#include <string> // Default include, it's always in my projects.


class TSGBAStringFetcher {
public:
	/* All available Languages can be found on this enum class. */
	enum class Languages : uint8_t { English = 0, Dutch = 1, French = 2, German = 3, Italian = 4, Spanish = 5 };
	enum class Games : uint8_t { BustinOut = 0, Urbz = 1, Sims2 = 2, Invalid = 3 };
	static constexpr uint8_t LanguageAmount = 0x6;

	TSGBAStringFetcher(const std::string &ROMPath);
	std::string Fetch(const uint16_t StringID, const Languages Language = Languages::English);

	Games DetectedGame() const { return this->ActiveGame; };
	bool SupportedGame() const { return this->ActiveGame != Games::Invalid; };
	uint16_t GetMaxStringID() const {
		switch(this->ActiveGame) {
			case Games::BustinOut:
				return 0x1A02;

			case Games::Urbz:
				return 0x1AFD;

			case Games::Sims2:
				return 0xD85;

			case Games::Invalid:
			default:
				return 0x0;
		}
	};
private:
	Games ActiveGame = Games::Invalid;
	std::unique_ptr<uint8_t[]> ROMData = nullptr;
	static constexpr uint8_t TIDs[3][4] = { 
		{ 0x41, 0x53, 0x49, 0x45 }, // The Sims Bustin' Out.
		{ 0x42, 0x4F, 0x43, 0x45 }, // The Urbz - Sims in the City.
		{ 0x42, 0x34, 0x36, 0x45 } // The Sims 2.
	};

	/* Bustin' Out is 16 MB, while The Urbz and The Sims 2 are 32 MB. */
	static constexpr uint32_t MinROMSize = 0x1000000;
	static constexpr uint32_t MaxROMSize = 0x2000000;

	/*
		Language String related Locations.
		
		Address1: The start location where the language is stored(?) also related to the ShiftingAddress.
		Address2: Related to the (StringID * 0x4) read value for the ShiftingAddress.
		Address3: Related to the 0x400 / 0x3FE thing.
	*/
	struct StringLocs {
		uint32_t Address1 = 0x0;
		uint32_t Address2 = 0x0;
		uint32_t Address3 = 0x0;
	};

	static constexpr StringLocs UrbzStrings[6] = {
		{ 0xE4F820, 0xE4F9B0, 0xE4F824 }, // English.
		{ 0xE93ECC, 0xE94074, 0xE93ED0 }, // Dutch.
		{ 0xEDA9AC, 0xEDAB60, 0xEDA9B0 }, // French.
		{ 0xF26B40, 0xF26CD8, 0xF26B44 }, // German.
		{ 0xF733B4, 0xF73560, 0xF733B8 }, // Italian.
		{ 0xFBA2AC, 0xFBA460, 0xFBA2B0 }  // Spanish.
	};

	static constexpr StringLocs BustinOutStrings[6] = {
		{ 0x98D488, 0x98D5FC, 0x98D48C }, // English.
		{ 0x9C1A7C, 0x9C1C00, 0x9C1A80 }, // Dutch.
		{ 0x9F5294, 0x9F5438, 0x9F5298 }, // French.
		{ 0xA2FE48, 0xA2FFD4, 0xA2FE4C }, // German.
		{ 0xA5ECF0, 0xA5EE7C, 0xA5ECF4 }, // Italian.
		{ 0xA94E60, 0xA9500C, 0xA94E64 }  // Spanish.
	};

	static constexpr StringLocs Sims2Strings[6] = {
		{ 0x019B4990, 0x019B4B20, 0x019B4994 }, // English.
		{ 0x019D7784, 0x019D7924, 0x019D7788 }, // Dutch.
		{ 0x019FAF9C, 0x019FB154, 0x019FAFA0 }, // French.
		{ 0x01A1F7E0, 0x01A1F98C, 0x01A1F7E4 }, // German.
		{ 0x01A460A0, 0x01A46254, 0x01A460A4 }, // Italian.
		{ 0x01A697C0, 0x01A69978, 0x01A697C4 }  // Spanish.
	};

	uint8_t LangIdx(const Languages Language) const {
		switch(Language) {
			case Languages::English:
			case Languages::Dutch:
			case Languages::French:
			case Languages::German:
			case Languages::Italian:
			case Languages::Spanish:
				return (uint8_t)Language;

			default:
				return 0;
		}
	};

	StringLocs GetLocForGame(const Languages Language) const {
		switch(this->ActiveGame) {
			case Games::BustinOut:
				return this->BustinOutStrings[this->LangIdx(Language)];

			case Games::Urbz:
				return this->UrbzStrings[this->LangIdx(Language)];

			case Games::Sims2:
				return this->Sims2Strings[this->LangIdx(Language)];

			case Games::Invalid:
			default:
				return { 0x0, 0x0, 0x0 };
		}
	};


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