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
#include "thraciatextconverter.h"
TextConverter* TextConverter::s_instance = 0;
#include <cstdlib>
#include <sstream>
#include <iostream>
#include <fstream>


using namespace std;

class badcommandexception: public textexception
{
    virtual const char* what() const throw()
    {
        string message = "Error in " + filename + ": \nBadly formatted command at line " + to_string(lineNumber);
        return message.c_str();
    }
} badCmd;
class wrongcommandexception: public textexception
{
    virtual const char* what() const throw()
    {
        string message = "Error in " + filename + ": \nUnrecognized command \"" + cmdName + "\" at line " + to_string(lineNumber);
        return message.c_str();
    }
public:
    string cmdName;
} wrongCmd;

TextConverter::TextConverter(){
		characterMap['\''] = 0x51;
		characterMap[','] = 0x48;
		characterMap['.'] = 0x49;
		characterMap['!'] = 0x4A;
		characterMap['?'] = 0x4B;
		characterMap['\"'] = 0x4C;
		characterMap['('] = 0x46;
		characterMap[')'] = 0x47;
		characterMap['-'] = 0x50;
        characterMap[' '] = 0x4F;
		characterMap[0x92] = 0x51;//utf-8 apostraphe
		
		commandMap["Left"] = LFACE ;
		commandMap["Right"] = RFACE ;
        commandMap["ShowFace"] = SHOWFACE;
		commandMap["A"] = INPUT;
		commandMap["ClearAll"] = CLEARALL;
		commandMap["HideFace"] = HIDEFACE;
		commandMap["HideBox"] = HIDEBOX;
		commandMap["X"] = END;
		commandMap["."] = A;
		commandMap["..."] = ELLIPSES;
		commandMap["Tab"] = TAB;
		commandMap["Tab2"] = TAB2;
		commandMap["Code"] = CODE;
		commandMap["Wait"] = WAIT;
		commandMap["Title"] = TITLE;
		
		
		ifstream cfgFile("conversion.ini");
		string s = "";
		getline(cfgFile, s);
		char c, c2, c3;
		int lastIndex = 0;
		cfgFile >> c;
        bool menuText = false;
        bool menuCmd = false;
        while(!cfgFile.eof())
        {
            if(c == '[')
            {
                string tagname;
                while(cfgFile.get(c) && c != ']')
                {
                    tagname.push_back(c);
                }
                if(c == ']')
                {
                    if(tagname =="Menu Text")
                    {
                        menuText = true;
                    }
                    else if(tagname == "Menu Commands")
                    {
                        menuText = false;
                        menuCmd = true;
                    }
                }
                else
                {
                    break;
                }
            }
            else
            {
                if(menuText)
                {
                    unsigned short byte1, byte2;
                    cfgFile.ignore(256, ' ');
                    cfgFile >> hex >> byte1 >> byte2;
                    byte2 <<= 8;
                    byte1 |= byte2;
                    menuTextMap[c] = byte1;
                }
                else if(menuCmd)
                {
                    cfgFile.putback(c);
                    unsigned short byte1, byte2;
                    string command;
                    cfgFile >> command;
                    cfgFile >> c;
                    string line;
                    getline(cfgFile, line);
                    stringstream s(line);
                    while(s >> hex >> byte1 && !s.eof() )
                    {
                        s >> hex >> byte2;
                        byte2 <<= 8;
                        byte1 |= byte2;
                        menuCmdMap[command].push_back(byte1);
                    }
                }
                else
                {
                    cfgFile >> c2;
                    cfgFile >> c3;

                    if(c3 == '+')
                    {
                        lastIndex++;
                    }
                    else
                    {
                        cfgFile.get(c3);
                        cfgFile >> hex >> lastIndex;
                    }
                    doubleCharMap[c][c2] = lastIndex;
                }
            }
            cfgFile >> c;
		}
}
TextConverter* TextConverter::getInstance()
{
	if (!s_instance)
	{
		s_instance = new TextConverter();
	}
	return s_instance;
}
queue<char> TextConverter::convertText(queue<string> inputQueue)
{
	choice = false;
    title = false;
    overWorld = false;
	if(!m_qDataQueue.empty())
	{
		queue<char> empty;
		swap(m_qDataQueue, empty);
	}
	int size = inputQueue.size();
	bool writingText = false;
	for(int i = 0; i < size; i++)
	{
		string command = "";
		stringstream inputLine(inputQueue.front());
		char c;
		while(inputLine.get(c))
		{
			if( c == '[')
			{
				writingText = false;
				command = "";
				while(inputLine.get(c) && c!= ']')
				{
					command.push_back(c);
				}
                if(c != ']')
                {
                    badCmd.lineNumber = i+1;
                    throw badCmd;
                }
                if(command == "Menu")
                {
                    try{
                        return this->convertMenuText(inputQueue);
                    }
                    catch(exception &e){
                        throw;
                    }

                }
                else if(command == "Choice")
				{
					choice = true;
					m_qDataQueue.push(0);
					m_qDataQueue.push(0x2E);
					m_qDataQueue.push(0xBC);
					m_qDataQueue.push(0xBB);
					m_qDataQueue.push(0x8C);
				}
				else if(command == "Map")
				{
                    overWorld = true;
				}
				else
				{
                    try{
                        processCommand(command);
                    }
                    catch(wrongcommandexception &e)
                    {
                        wrongCmd.lineNumber = i+1;
                        throw wrongCmd;
                    }
				}
			}
			else
			{
				unsigned char gameVal;
				if(!writingText)
                {
					writingText = true;
				}
				char c2;
				if(inputLine.get(c2) && doubleCharMap[c][c2] != 0)
				{
					m_qDataQueue.push(doubleCharMap[c][c2]);
				}
				else
				{
					inputLine.putback(c2);
                    if( c == '\'')
					{
						char comma = c;
						if(inputLine.get(c) && (c == 's' || c == 't'))
						{
							comma = c;
                            if(comma == 's')
                            {
                                gameVal = 0x4D;
                            }
                            else if(comma == 't')
                            {
                                gameVal = 0x4E;
                            }
                            if(inputLine.get(c) && c != ' ')
							{
                                inputLine.putback(c);
                            }
							
						}
						else
						{
							gameVal = convertCharacter(comma);
							if(gameVal != 0)
							{
								m_qDataQueue.push(gameVal);
							}
							gameVal = convertCharacter(c);
						}
						if(gameVal != 0)
						{
							m_qDataQueue.push(gameVal);
						}
					}
					else if ( c == ',')
					{
						char comma = c;
						gameVal = convertCharacter(comma);
						if(inputLine.get(c) && c != ' ')
						{
							inputLine.putback(c);
						}
						m_qDataQueue.push(gameVal);
					}
                    else if( c == '.')
                    {
                        gameVal = convertCharacter(c);
                        char c2;
                        if(inputLine.get(c2) && c2 == '.')
                        {
                            char c3;
                            if(inputLine.get(c3) && c3 == '.')
                            {
                                gameVal = 0x52;
                            }
                            else
                            {
                                inputLine.putback(c3);
                                inputLine.putback(c2);
                            }
                        }
                        else
                        {
                            inputLine.putback(c2);
                        }
                        m_qDataQueue.push(gameVal);
                    }
					else
					{
						gameVal = convertCharacter(c);
						if(gameVal != 0)
						{
							m_qDataQueue.push(gameVal);
						}
                    }
				}
			}
		}
        inputQueue.pop();
		if(command != "X" && command != "ClearAll")
		{
            m_qDataQueue.push(2);
		}
	}
    return m_qDataQueue;
}

std::queue<char> TextConverter::convertMenuText(std::queue<string> inputQueue)
{
    if(!m_qDataQueue.empty())
    {
        queue<char> empty;
        swap(m_qDataQueue, empty);
    }
    int size = inputQueue.size();
    for(int i = 0; i < size; i++)
    {
        string command = "";
        stringstream inputLine(inputQueue.front());
        inputQueue.pop();
        char c;
        unsigned short gameVal = 0;
        while(inputLine.get(c))
        {
            if( c == '[')
            {
                command = "";
                while(inputLine.get(c) && c!= ']')
                {
                    command.push_back(c);
                }
                if(c != ']')
                {
                    badCmd.lineNumber = 1;
                    throw badCmd;
                    //break;
                }
                if(command != "Menu")
                {
                    if(!menuCmdMap[command].empty())
                    {
                        vector<unsigned short> sArr = menuCmdMap[command];
                        for(vector<unsigned short>::iterator it = sArr.begin(); it != sArr.end() ; ++it)
                        {
                            gameVal = *it;
                            char* cmdArr = reinterpret_cast<char*>(&gameVal);
                            m_qDataQueue.push(cmdArr[0]);
                            m_qDataQueue.push(cmdArr[1]);
                        }
                    }
                    else
                    {
                        gameVal = strtol(command.c_str(), NULL, 16);
                        char* cmdArr = reinterpret_cast<char*>(&gameVal);
                        m_qDataQueue.push(cmdArr[0]);
                        m_qDataQueue.push(cmdArr[1]);
                    }
                }
                else
                {
                    continue;
                }

            }
            else
            {
                gameVal = menuTextMap[c];
                char* cmdArr = reinterpret_cast<char*>(&gameVal);
                m_qDataQueue.push(cmdArr[0]);
                m_qDataQueue.push(cmdArr[1]);
            }

        }
    }
    return m_qDataQueue;
}
unsigned char TextConverter::convertCharacter(char c)
{
	unsigned short rVal = 0;
	if(c >= 'a' && c <= 'z')
	{
		
		rVal = c - 'a' + 0x2A;
	}
	else if(c >= 'A' && c <= 'Z')
	{
		rVal = (c - 'A' + 0x10);
		//convert uppercase
	}
	else if(c >= '0' && c <= '9')
	{
		rVal = (c - '0' + 0xB1);
		//convert number
	}
	else
	{//convert punctuation
		rVal =  characterMap[c];
	}
	return rVal;
}
void TextConverter::processCommand(string comString)
{
	static unsigned char sFace[2] = {0, 0x3A};
    static unsigned char hFace[2] = {0, 0x3B};
	static unsigned char hBox[2] = {0, 0x39};
	static unsigned char clear[2] = {0, 0x2A};
	
	controlCode command = commandMap[comString];
	
	switch(command)
	{
		case LFACE:
			m_qDataQueue.push('\6');
			break;
		case RFACE:
			m_qDataQueue.push('\7');
			break;
		case SHOWFACE:
            if(overWorld)
            {
                m_qDataQueue.push(0);
                m_qDataQueue.push(0x30);
                m_qDataQueue.push(0x9C);
                m_qDataQueue.push(0xBF);
                m_qDataQueue.push(0x8C);
            }
            else
            {
                m_qDataQueue.push(sFace[0]);
                m_qDataQueue.push(sFace[1]);
            }
			break;
		case INPUT:
			m_qDataQueue.push('\b');
			break;
		case CLEARALL:
            if(overWorld)
            {
                m_qDataQueue.push(4);
            }
            else
            {
                m_qDataQueue.push(clear[0]);
                m_qDataQueue.push(clear[1]);
            }
			break;
		case HIDEFACE:
            if(overWorld)
            {
                //00 2F 7A 93 82 01 00
                m_qDataQueue.push(0);
                m_qDataQueue.push(0x2F);
                m_qDataQueue.push(0x7A);
                m_qDataQueue.push(0x93);
                m_qDataQueue.push(0x82);
            }
            else
            {
                m_qDataQueue.push(hFace[0]);
                m_qDataQueue.push(hFace[1]);
            }
			break;
		case HIDEBOX:
			m_qDataQueue.push(hBox[0]);
			m_qDataQueue.push(hBox[1]);
			break;
		case END:
			if(choice)
			{
				choice = true;
				m_qDataQueue.push(0);
				m_qDataQueue.push(0x2E);
				m_qDataQueue.push(0xCC);
				m_qDataQueue.push(0xBB);
				m_qDataQueue.push(0x8C);
			}
            if(title)
            {
                m_qDataQueue.push(1);
            }
            else
            {
                m_qDataQueue.push(5);
            }
			break;
        case A:
            if(overWorld)
            {
                m_qDataQueue.push(8);
            }
            else
            {
                m_qDataQueue.push(0xA);
            }
			break;
		case ELLIPSES:
			m_qDataQueue.push(0x52);
			break;
        case TAB:
            m_qDataQueue.push(0xAF);
			break;
        case TAB2:
            m_qDataQueue.push(0x8C);
            break;
		case NEWLINE:
			break;
		case CODE:
			m_qDataQueue.push(0);
			m_qDataQueue.push(0x2E);
			break;
		case WAIT:
            m_qDataQueue.push(0x0f);
			break;
        case TITLE:
            m_qDataQueue.push(0x0A);
            m_qDataQueue.push(0x18);
            m_qDataQueue.push(0x17);
            m_qDataQueue.push(0x18);
            m_qDataQueue.push(0x17);
            m_qDataQueue.push(0x09);
            title = true;
            break;
		default:
            int num = strtol(comString.c_str(), NULL, 16);
            if(num == 0)
            {
                for(unsigned int i = 0; i < comString.length(); i++)
                {
                    if(comString.c_str()[i] != '0')
                    {
                        wrongCmd.cmdName = comString;
                        throw wrongCmd;
                    }
                }
            }
			char* numArr = reinterpret_cast<char*>(&num);
			if(comString.size() == 2)
			{
				m_qDataQueue.push(numArr[0]);
			}
			else if(comString.size() == 4)
			{
				m_qDataQueue.push(numArr[0]);
				m_qDataQueue.push(numArr[1]);
			}
			else if(comString.size() == 6)
			{
				m_qDataQueue.push(numArr[0]);
				m_qDataQueue.push(numArr[1]);
				m_qDataQueue.push(numArr[2]);
			}
			
			break;
		
	}
}
