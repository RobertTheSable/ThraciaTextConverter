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
#include <map>
#include <vector>
#include <string>
#include <queue>
#include <exception>

class textexception: public std::exception
{
public:
    int lineNumber;
    std::string filename;
};

enum controlCode {DEF, LFACE, RFACE, SHOWFACE, NEWLINE, INPUT, CLEARALL, HIDEFACE,
                  HIDEBOX, END, A, ELLIPSES, TAB, TAB2, CODE, WAIT, TITLE};


class TextConverter{
	public:
		std::queue<char> convertText(std::queue<std::string>);
        std::queue<char> convertMenuText(std::queue<std::string>);
		unsigned char convertCharacter(char);
		void processCommand(std::string);
		static TextConverter* getInstance();
	private:
        TextConverter();
		static TextConverter* s_instance;
		std::map<char, unsigned char> characterMap;
		std::map<char, std::map<char, unsigned char> > doubleCharMap;
        std::map<char, unsigned short> menuTextMap;
		std::map<std::string, controlCode> commandMap;
        std::map<std::string, std::vector<unsigned short>> menuCmdMap;
		std::queue<char> m_qDataQueue;
		unsigned char* convertedText;
		bool choice;
        bool title;
        bool overWorld;
};
