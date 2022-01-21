/*
*   This file is part of TSGBAJPNStringFetcher
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

#ifndef _TSGBA_JPN_STRING_FETCHER_HPP
#define _TSGBA_JPN_STRING_FETCHER_HPP

#include <memory> // std::unique_ptr because i like those instead of raw pointers.
#include <string> // Default include, it's always in my projects.
#include <vector> // std::vector for TSGBAJPNStringFetcher::Fetch.


class TSGBAJPNStringFetcher {
public:
	enum class Games : uint8_t { BustinOut = 0, Urbz = 1, Invalid = 2 };

	TSGBAJPNStringFetcher(const std::string &ROMPath);
	std::vector<uint8_t> Fetch(const uint16_t StringID);

	Games DetectedGame() const { return this->ActiveGame; };
	bool SupportedGame() const { return this->ActiveGame != Games::Invalid; };
	uint16_t GetMaxStringID() const {
		switch(this->ActiveGame) {
			case Games::BustinOut:
				return 0x1A02;

			case Games::Urbz:
				return 0x1AFD;

			case Games::Invalid:
			default:
				return 0x0;
		}
	};
private:
	Games ActiveGame = Games::Invalid;
	std::unique_ptr<uint8_t[]> ROMData = nullptr;
	static constexpr uint8_t TIDs[2][4] = {
		{ 0x42, 0x34, 0x50, 0x4A }, // The Sims Bustin' Out. JPN.
		{ 0x42, 0x4F, 0x43, 0x4A } // The Urbz - Sims in the City JPN.
	};

	/* Bustin' Out is 16 MB, while The Urbz is 32 MB. */
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

	StringLocs GetLocForGame() const {
		switch(this->ActiveGame) {
			case Games::BustinOut:
				return { 0x9A732C, 0x9A7730, 0x9A7330 };

			case Games::Urbz:
				return { 0xE7EDE4, 0xE7F1E8, 0xE7EDE8 };

			case Games::Invalid:
			default:
				return { 0x0, 0x0, 0x0 };
		}
	};
};

#endif