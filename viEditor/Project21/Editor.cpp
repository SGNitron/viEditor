#include "Editor.h"
#include "Snapshot.h"
#include "BinarySearchTree.h"
#include <fstream>
#include <string>
#include <iostream>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include "windows.h"
#include <conio.h> 
using namespace std;

void goToXY(int,int);
void colorText(int);

const int ESC = 27;
const int backspace = 8;
void placeCursorAt(Point coordinate) {
	COORD coord;
	coord.X = coordinate.getX();
	coord.Y = coordinate.getY();
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord);
}

void goToXY(int column, int line) {
	COORD coord;
	coord.X = column;
	coord.Y = line;
	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		coord);
}

void colorText(int value) {				//This method prints the text in buffer
										// It will print keywords in blue
	COORD coord;
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	FlushConsoleInputBuffer(hConsole);
	SetConsoleTextAttribute(hConsole, value);
}

Editor::Editor()
{
	Position.setX(0);
	Position.setY(0);
}

void Editor::readfile(string afile, string aKeyWord)
{
	int curPosition = 1;
	string aline;
	ifstream infile;
	ifstream anotherInFile;
	infile.open(afile);
	if (!infile)
	{
		cout << "Error!" << endl;
		system("pause");
	}

	while (!infile.eof())
	{
		getline(infile, aline);
		lines.insert(curPosition, aline);
		curPosition++;
	}

	infile.close();

	anotherInFile.open(aKeyWord);
	keywords = new BinarySearchTree<string>();
	string word;

	while (!anotherInFile.eof()) {
		anotherInFile >> word;
		keywords->add(word);
	}
}

void Editor::run()
{
	display();
	placeCursorAt(Position);      //Cursor at the origin (0,0)
	char Usercommand;

	do
	{
		Usercommand = _getch();
		switch (Usercommand)
		{
		case 'j':				//limits position Y not greater than the length of the list
			ActivateDown();
			break;
		case 'k':				//limits position Y not smaller than the length of the list
			ActivateUp();
			break;
		case 'h':				//limits position X not smaller than 0
			ActivateLeft();
			break;
		case 'l':				//limits position X not greater than the size of the entry
			ActivateRight();
			break;
		case 'd':				//double tap to delete	
			ActivateDelete();
			break;
		case 'x':				//Erase a single character 
			ActivateErase();
			break;
		case 'u':				//undo the most previous command
			ActivateUndo();
			break;
		case 'I':				//Insert characters at the begining of the entry
			ActivateCapitalInsert();
			break;
		case 'i':				//Insert characters anywhere within the entry
			ActivateSmallerInsert();
			break;
		default:
			break;
		}
	} while (Usercommand != 'E');
}

void Editor::display()const
{
	system("CLS");
	int i, j;
	string myLine;
	bool isKeyword;

	for (i = 1; i < lines.getLength() + 1; i++) {
		myLine = lines.getEntry(i);
		j = 0;
		for (int i = 0; i < myLine.size(); i++) {
			colorText(0XF0);  //default to black
			if (myLine[i] >= 'a' && myLine[i] <= 'z') {  //letter
				string currentWord;
				for (j = i; (myLine[j] >= 'a' && myLine[j] <= 'z'); j++) {
					currentWord += myLine[j];
				}
				isKeyword = keywords->contains(currentWord);
				if (isKeyword)
					colorText(FOREGROUND_BLUE | BACKGROUND_BLUE |

						BACKGROUND_GREEN | BACKGROUND_RED | 0X80);  //blue
				cout << currentWord;
				if (j != 0)
					i = j - 1;
			}
			else
				cout << myLine[i];
		}
		cout << endl;
	}
	placeCursorAt(Position);
}

int Editor::CurSize(const int Position)     //return the size of an entry within the linkedlist
{										//Parameter 'Position' is the Y coordinate
	string aEntry;
	int size;
	aEntry = lines.getEntry(Position + 1);
	size = aEntry.size();
	return size;
}

void Editor::ActivateDown()
{
	int preSize;							//record the size of an entry within the list before move up or down 
	if (Position.getY() < lines.getLength() - 1)		//position Y counts from 0, lines counts from 1, thus -1
	{
		preSize = Position.getX() + 1;;			//position X counts from 0, thus +1
		Position.down();
		if (preSize > CurSize(Position.getY()))		//compare lengths between two entries
		{
			Position.setX(CurSize(Position.getY()) - 1);
			placeCursorAt(Position);
		}
		else
			placeCursorAt(Position);
	}
}

void Editor::ActivateUp()
{
	int preSize;					//record the size of an entry within the list before move up or down 
	if (Position.getY() > 0)
	{
		preSize = Position.getX() + 1;
		Position.up();
		if (preSize > CurSize(Position.getY()))		//compare lengths between two entries
		{
			Position.setX(CurSize(Position.getY()) - 1);
			placeCursorAt(Position);
		}
		else
			placeCursorAt(Position);
	}
}

void Editor::ActivateLeft()
{
	if (Position.getX() > 0)
	{
		Position.left();
		placeCursorAt(Position);
	}
}

void Editor::ActivateRight()
{
	if (Position.getX() < CurSize(Position.getY()) - 1)	//limits position X not greater than the size of an entry
	{
		Position.right();
		placeCursorAt(Position);
	}
}

void Editor::ActivateDelete()
{
	char Usercommand;
	Snapshot copy;
	Usercommand = _getwch();					//double tap dd to successfully excute than delete function
	if (Usercommand == 'd')
	{
		copy.setStoredCommand("dd");									//copy the command
		copy.setStoredPosition(Position);							//copy the position	
		copy.setStoredValue(lines.getEntry(Position.getY() + 1));   //copy the entry
		undoStack.push(copy);										//push all three of them to a stack

		lines.remove(Position.getY() + 1);
		display();

	}
}

void Editor::ActivateErase()
{
	Snapshot copy;
	string EraseAChar;
	copy.setStoredCommand("x");										//copy the command
	copy.setStoredPosition(Position);								//copy the position
	copy.setStoredValue(lines.getEntry(Position.getY() + 1));		//copy the entry
	undoStack.push(copy);											//push all three of them to a stack

	EraseAChar = lines.getEntry(Position.getY() + 1);
	EraseAChar.erase(EraseAChar.begin() + Position.getX());		//erase a single character within an entry
	lines.replace(Position.getY() + 1, EraseAChar);			//replace the entry in the list by a modified 'EraseAChar'
	display();

}

void Editor::ActivateUndo()
{
	string precommand;
	if (!undoStack.isEmpty())			//check if there is any recorded command in the stack before undo
	{
		precommand = undoStack.peek().getStoredCommand();		//set precommand to be the most previous command
		if (precommand == "dd")			//insert the most previous entry in respond to the right position in the list

		{					//position of the most previous entry		   //value of the most previous entry
			lines.insert(undoStack.peek().getStoredPosition().getY() + 1, undoStack.peek().getStoredValue());
			Position = undoStack.peek().getStoredPosition();
			display();
			undoStack.pop();
		}
		else if (precommand == "x")	//replace the most previous entry in respond to the right position in the list

		{					//position of the most previous entry			//value of the most previous entry
			lines.replace(undoStack.peek().getStoredPosition().getY() + 1, undoStack.peek().getStoredValue());
			Position = undoStack.peek().getStoredPosition();
			display();
			undoStack.pop();
		}
		else if (precommand == "I")
		{
			for (int i = 0; i <= count; i++)
			{
				lines.remove(undoStack.peek().getStoredPosition().getY() + 1);
			}
			lines.insert(undoStack.peek().getStoredPosition().getY() + 1, undoStack.peek().getStoredValue());
			Position = undoStack.peek().getStoredPosition();
			display();
			undoStack.pop();
		}
		else if (precommand == "i")
		{
			for (int i = 0; i <= count; i++)
			{
				lines.remove(undoStack.peek().getStoredPosition().getY() + 1);
			}
			lines.insert(undoStack.peek().getStoredPosition().getY() + 1, undoStack.peek().getStoredValue());
			Position = undoStack.peek().getStoredPosition();
			display();
			undoStack.pop();
		}
	}
}

void Editor::ActivateCapitalInsert()
{
	count = 0;
	int UpdateX = 0;
	int UpdateY;
	char ch;
	string EraseAChar;
	string InsertAChar = "";
	string wholeline;
	string beforeCursor;
	string afterCursor;
	string str;
	Position.setX(0);							//Insert content at the begining of the entry
	Snapshot copy;
	copy.setStoredCommand("I");										//copy the command
	copy.setStoredPosition(Position);								//copy the position
	copy.setStoredValue(lines.getEntry(Position.getY() + 1));		//copy the entry
	undoStack.push(copy);											//push all three of them to a stack
	placeCursorAt(Position);

	do
	{
		ch = _getche();
		switch (ch)
		{
		case ESC:
			break;
		case backspace:
			if (Position.getX() == 0 && Position.getY() == 0 && CurSize(Position.getY()) > 0)
				//cursor at the origin (0,0)
			{
				EraseAChar = lines.getEntry(Position.getY() + 1);
				EraseAChar.erase(EraseAChar.begin() + Position.getX());
				lines.replace(Position.getY() + 1, EraseAChar);
				display();
			}
			else if (Position.getX() > 0 && Position.getY() >= 0)     //cursor at somewhere within an entry 
			{
				UpdateX = Position.getX();
				EraseAChar = lines.getEntry(Position.getY() + 1);
				EraseAChar.erase(EraseAChar.begin() + Position.getX() - 1);
				lines.replace(Position.getY() + 1, EraseAChar);
				UpdateX--;
				Position.setX(UpdateX);
				display();
			}
			else if (Position.getX() == 0 && Position.getY() > 0)       //cursor at the begining of the entry 
			{
				UpdateY = Position.getY();
				InsertAChar = lines.getEntry(Position.getY() + 1);
				lines.remove(Position.getY() + 1);
				UpdateY--;
				Position.setY(UpdateY);
				UpdateX = CurSize(Position.getY());
				Position.setX(UpdateX);
				wholeline = lines.getEntry(Position.getY() + 1);
				wholeline.insert(Position.getX(), InsertAChar);
				lines.replace(Position.getY() + 1, wholeline);
				display();
			}
			break;
		case '\r':
			UpdateY = Position.getY();
			beforeCursor = "";
			afterCursor = "";
			str = lines.getEntry(Position.getY() + 1);

			for (int i = 0; i < Position.getX(); i++)
			{
				beforeCursor += str[i];
			}
			for (int j = Position.getX(); j < CurSize(Position.getY()); j++)
			{
				afterCursor += str[j];
			}
			if (beforeCursor.size() == 0)
				beforeCursor = " ";
			lines.replace(Position.getY() + 1, beforeCursor);
			lines.insert(Position.getY() + 2, afterCursor);
			count++;
			UpdateY++;
			Position.setX(0);
			Position.setY(UpdateY);
			display();
			break;
		default:
			UpdateX = Position.getX();
			UpdateX++;
			InsertAChar = lines.getEntry(Position.getY() + 1);
			InsertAChar.insert(Position.getX(), 1, ch);			//string& insert(size_t pos, size_t n, char c);
			lines.replace(Position.getY() + 1, InsertAChar);
			Position.setX(UpdateX);
			display();
			break;
		}
	} while (ch != ESC);
}

void Editor::ActivateSmallerInsert()
{
	count = 0;
	int UpdateX;
	int UpdateY;
	char ch;
	string EraseAChar;
	string InsertAChar = "";
	string wholeline;
	string beforeCursor;
	string afterCursor;
	string str;
	UpdateX = Position.getX();

	Snapshot copy;
	copy.setStoredCommand("i");										//copy the command
	copy.setStoredPosition(Position);								//copy the position
	copy.setStoredValue(lines.getEntry(Position.getY() + 1));		//copy the entry
	undoStack.push(copy);											//push all three of them to a stack

	do
	{
		ch = _getche();
		switch (ch)
		{
		case ESC:
			break;
		case backspace:
			if (Position.getX() == 0 && Position.getY() == 0 && CurSize(Position.getY()) > 0)
				//cursor at the origin (0,0)
			{
				EraseAChar = lines.getEntry(Position.getY() + 1);
				EraseAChar.erase(EraseAChar.begin() + Position.getX());
				lines.replace(Position.getY() + 1, EraseAChar);
				display();
			}
			else if (Position.getX() > 0 && Position.getY() >= 0)     //cursor at somewhere within an entry 
			{
				EraseAChar = lines.getEntry(Position.getY() + 1);
				EraseAChar.erase(EraseAChar.begin() + Position.getX() - 1);
				lines.replace(Position.getY() + 1, EraseAChar);
				UpdateX--;
				Position.setX(UpdateX);
				display();
			}
			else if (Position.getX() == 0 && Position.getY() > 0)       //cursor at the begining of the entry 
			{
				UpdateY = Position.getY();
				InsertAChar = lines.getEntry(Position.getY() + 1);
				lines.remove(Position.getY() + 1);
				UpdateY--;
				Position.setY(UpdateY);
				UpdateX = CurSize(Position.getY());
				Position.setX(UpdateX);
				wholeline = lines.getEntry(Position.getY() + 1);
				wholeline.insert(Position.getX(), InsertAChar);
				lines.replace(Position.getY() + 1, wholeline);
				display();
			}
			break;
		case '\r':
			UpdateY = Position.getY();
			beforeCursor = "";
			afterCursor = "";
			str = lines.getEntry(Position.getY() + 1);

			for (int i = 0; i < Position.getX(); i++)
			{
				beforeCursor += str[i];
			}
			for (int j = Position.getX(); j < CurSize(Position.getY()); j++)
			{
				afterCursor += str[j];
			}
			if (beforeCursor.size() == 0)
				beforeCursor = " ";
			lines.replace(Position.getY() + 1, beforeCursor);
			lines.insert(Position.getY() + 2, afterCursor);
			count++;
			UpdateY++;
			Position.setX(0);
			Position.setY(UpdateY);
			display();
			break;
		default:
			UpdateX = Position.getX();
			UpdateX++;
			InsertAChar = lines.getEntry(Position.getY() + 1);
			InsertAChar.insert(Position.getX(), 1, ch);				//string& insert(size_t pos, size_t n, char c);
			lines.replace(Position.getY() + 1, InsertAChar);
			Position.setX(UpdateX);
			display();
			break;
		}
	} while (ch != ESC);
}