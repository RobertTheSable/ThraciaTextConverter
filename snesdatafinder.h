/*
 *  Thracia 776 Text Inserter
 *
 *  Copyright (C) 2016 Robert the Sable
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 3
 *  as published by the Free Software Foundation
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  <Description> For those of you not familiar with the GNU GPL system,
 *  the license is in the file named "COPYING".
 */
#ifndef SNESDATAFINDER_H
#define SNESDATAFINDER_H

#include <iostream>
#include <fstream>
#include <string>

class SNESRom
{
private:
	std::string m_FileName;
	//std::fstream m_ROMFile;
public:
	SNESRom(const char*);
	static int LoROMToPC(int, bool);
	void readData(unsigned char**, int, int);
	void writeData(char **buffer, int addr, int size);
};

#endif // SNESDATAFINDER_H
