#pragma once
#include "LinkedList.h"
#include "Point.h"
#include "BinarySearchTree.h"
#include "ArrayStack.h"
#include "Snapshot.h"
#include <string>
using namespace std;
class Editor
{
public:
	Editor();
	void readfile(string, string);
	void run();
	void display()const;
	int CurSize(const int);    //return the size of an entry within the linkedlist
private:
	LinkedList<string> lines;
	BinarySearchTree<string> * keywords;
	Point Position;
	ArrayStack<Snapshot> undoStack;
	int count;
	void ActivateDown();
	void ActivateUp();
	void ActivateLeft();
	void ActivateRight();
	void ActivateDelete();
	void ActivateErase();
	void ActivateUndo();
	void ActivateCapitalInsert();
	void ActivateSmallerInsert();
};