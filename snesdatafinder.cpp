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
#include "snesdatafinder.h"
#include <cstdlib>

using namespace std;

SNESRom::SNESRom(const char* filename)
{
	m_FileName = filename;
}

//Returns size of data after decompression.
void SNESRom::readData(unsigned char **buffer, int addr, int size)
{
	ifstream m_ROMFile(m_FileName.c_str(), ios_base::binary|ios_base::in);
	unsigned char* dataBuffer = (unsigned char*)malloc(size);
	m_ROMFile.seekg(addr, ios_base::beg);
	for(int i = 0; i < size; i++)
	{
		char data;
		m_ROMFile.get(data);
		dataBuffer[i] = (unsigned char) data;
	}
	*buffer = dataBuffer;
	m_ROMFile.close();
}
void SNESRom::writeData(char **buffer, int addr, int size)
{
	ofstream m_ROMFile(m_FileName.c_str(), ios_base::binary|ios_base::out|ios_base::in);
	m_ROMFile.seekp(addr, ios_base::beg);
	m_ROMFile.write(*buffer, size);
	m_ROMFile.close();
}

int SNESRom::LoROMToPC(int addr , bool header)
{
	if (addr<0 || addr>0xFFFFFF ||//not 24bit
                (addr&0xFE0000)==0x7E0000 ||//wram
                (addr&0x408000)==0x000000)//hardware regs
			return -1;
	addr=((addr&0x7F0000)>>1|(addr&0x7FFF));
	if (header) addr+=512;
	return addr;
}
