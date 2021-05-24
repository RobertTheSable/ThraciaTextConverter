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
#include "scriptparser.h"

ScriptParser::ScriptParser(QString dir)
{
    QDirIterator it(dir, QDir::Dirs | QDir::NoDotAndDotDot);
    m_aDirList.append(dir);
    while(it.hasNext())
    {
        QString dir2 = it.next();
        m_aDirList.append(dir2);
        QDirIterator it2(dir2, QDir::Dirs | QDir::NoDotAndDotDot);
        while(it2.hasNext())
        {
             m_aDirList.append(it2.next());
        }
    }
    currentDir = 0;
    currentFile = 0;
}

QString ScriptParser::getNextDir()
{
    QString ret = "";
    if(currentDir < m_aDirList.size())
    {
        ret= m_aDirList.at(currentDir);
        currentDir++;
        currentFile = 0;
    }
    return ret;
}

QString ScriptParser::getCurrentDir()
{
    return m_aDirList.at(currentDir);
}

QString ScriptParser::getnextFileInDir()
{
    QString ret = "";
    if(hasNext())
    {
        QStringList nameFilter;
        nameFilter << "*.txt";
        QFileInfoList files = QDir(m_aDirList.at(currentDir)).entryInfoList(nameFilter, QDir::Files);
        if(!files.empty() && currentFile < files.size())
        {
            QFileInfo file = files[currentFile];
            ret = file.absoluteFilePath();
            currentFile++;
        }
    }
    return ret;
}

bool ScriptParser::hasNext()
{
    return currentDir < m_aDirList.size();
}
