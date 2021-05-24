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
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtCore>
#include <QMessageBox>
#include <QFileDialog>
#include <QLayout>
#include <string>
#include <queue>
#include <sstream>
#include <cstdlib>
#include "snesdatafinder.h"
#include "thraciatextconverter.h"
#include "scriptparser.h"

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    enable1 = false;
    enable2 = false;
    QString logpath = QDir::toNativeSeparators(QDir::currentPath()) + QDir::separator() + "insertion.log";
    ui->logDirectory->setText(logpath);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_InsertScriptButton_clicked()
{
    bool header = ui->checkBox->isChecked();
    ScriptParser sc(ui->scriptDirEdit->text());
    TextConverter* t = TextConverter::getInstance();
    SNESRom romFile(ui->romPathEdit->text().toStdString().c_str());
    std::vector<std::string> fileList;
    QMessageBox msgBox;
    QString lastFile = "";
    bool success = true;
    try{
        while(sc.hasNext())
        {
            int romAddr = QDir(sc.getCurrentDir()).dirName().toInt(0, 16);
            int pcAddr = SNESRom::LoROMToPC(romAddr, header);
            if(pcAddr != -1)
            {
                QString file = sc.getnextFileInDir();
                while(file != "")
                {
                    std::queue<std::string> convertText;
                    std::ifstream inFile(file.toStdString().c_str());
                    std::string inLine = "";
                    while(getline(inFile, inLine))
                    {
                        convertText.push(inLine);
                    }
                    std::queue<char> convertedText;
                    try
                    {
                        convertedText  = t->convertText(convertText);
                    }
                    catch(textexception &e)
                    {
                        e.filename = file.toStdString();
                        throw;
                    }
                    int size  = convertedText.size();
                    char* dataArray = (char*)malloc(size);
                    int i = 0;
                    while(!convertedText.empty())
                    {
                        dataArray[i] = convertedText.front();
                        convertedText.pop();
                        i++;
                    }
                    romFile.writeData(&dataArray, pcAddr, size);
                    std::ostringstream oss;
                    oss << "Wrote file " << file.toStdString() << " to $" << std::hex << romAddr << std::endl;
                    lastFile = file;
                    fileList.push_back(oss.str());
                    romAddr += size;
                    pcAddr += size;
                    file = sc.getnextFileInDir();
                }

            }
            sc.getNextDir();
        }
    }
    catch(textexception& e){
        //qDebug() << e.what();
        msgBox.setText(QString(e.what()));
        msgBox.setIcon(QMessageBox::Warning);
        success = false;
    }

    if(success)
    {
        QString logPath = ui->logDirectory->text();
        QFile logFile(logPath);

        if(!logFile.open(QFile::WriteOnly | QFile::Text))
        {
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("Could not write log file.");
        }
        else if(fileList.empty())
        {
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.setText("No files inserted.");
        }
        else
        {

            QTextStream out(&logFile);
            for(std::vector<std::string>::iterator it = fileList.begin(); it != fileList.end(); ++it)
            {
                out << it->c_str();
            }
            QString message = "Inserion Succesful. Log file written to: \n" + logPath;
            msgBox.setText(message);
            msgBox.setIcon(QMessageBox::Information);
        }
    }
    msgBox.exec();
}

void MainWindow::on_chooseDirButton_clicked()
{
    QString path = ui->scriptDirEdit->text();
    if(path == "")
    {
        path = QDir::homePath();
    }
    QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Script Directory"), path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if(dir != "")
    {
        if(QDir(dir).exists())
        {
            ui->scriptDirEdit->setText(QDir::toNativeSeparators(dir));
            enable1 = true;
        }
        else
        {
            enable1 = false;
        }
        this->ui->InsertScriptButton->setEnabled(enable1&enable2);
    }
}

void MainWindow::on_chooseROMButton_clicked()
{
    QString path = QFileInfo(ui->romPathEdit->text()).absolutePath();
    if(path == "" || !QDir(path).exists() )
    {
        path = QDir::homePath();
    }
    QString selfilter = tr("SNES Rom files (*.sfc *.smc)");
    QString outFile = QFileDialog::getOpenFileName(
                this,
                tr("Choose ROM File"),
                path,
                tr("SNES Rom files (*.sfc *.smc)" ),
                &selfilter );
    if(outFile != "")
    {
        QFileInfo checkFile(outFile);
        if(checkFile.exists() && checkFile.isFile())
        {
            ui->romPathEdit->setText(QDir::toNativeSeparators(outFile));
            enable2 = true;
        }
        else
        {
            enable2 = false;
        }
        this->ui->InsertScriptButton->setEnabled(enable1&enable2);
        this->ui->checkBox->setEnabled(enable2);
    }
}

void MainWindow::on_scriptDirEdit_textChanged(const QString &arg1)
{
    if(arg1 != ""&&QDir(arg1).exists())
    {
        enable1 = true;
    }
    else
    {
        enable1 = false;
    }
    this->ui->InsertScriptButton->setEnabled(enable1&enable2);
}

void MainWindow::on_romPathEdit_textChanged(const QString &arg1)
{
    QFileInfo checkFile(arg1);
    if(arg1 != ""&&checkFile.exists() && checkFile.isFile())
    {
        enable2 = true;
    }
    else
    {
        enable2 = false;
    }
    this->ui->InsertScriptButton->setEnabled(enable1&enable2);
    this->ui->checkBox->setEnabled(enable2);
}

void MainWindow::on_ChooseLogLocation_clicked()
{
  QString path = ui->scriptDirEdit->text();
  if(path == "")
  {
      path = QDir::homePath();
  }
  QString dir = QFileDialog::getExistingDirectory(this, tr("Choose Log Location"), path, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
  if(dir != "")
  {
      if(QDir(dir).exists())
      {
          ui->logDirectory->setText(QDir::toNativeSeparators(dir) + QDir::separator() + "insertion.log");
      }
  }
}
