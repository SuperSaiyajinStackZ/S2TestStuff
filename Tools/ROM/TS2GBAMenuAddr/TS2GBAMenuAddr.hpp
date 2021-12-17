/*
*   This file is part of TS2GBAMenuAddr
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

#ifndef _TS2GBA_MENU_ADDR_HPP
#define _TS2GBA_MENU_ADDR_HPP

#include <memory> // std::unique_ptr because i like those instead of raw pointers.
#include <string> // Default include, it's always in my projects.


class TS2GBAMenuAddr {
public:
	TS2GBAMenuAddr(const std::string &ROMPath);
	uint32_t GetMenuAddress(const uint32_t MenuID, const bool OnPrepare) const;

	bool GetValid() const { return this->ROMValid; };
	uint32_t GetMenuAmount() const { return 0x28; };
private:
	std::unique_ptr<uint8_t[]> ROMData = nullptr;
	static constexpr uint8_t TID[4] = { 0x42, 0x34, 0x36, 0x45 };
	static constexpr uint32_t ROMSize = 0x2000000;
	bool ROMValid = false;
};

#endif