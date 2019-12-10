//main.cpp
#include "ListInterface.h"
#include "LinkedList.h"
#include "StackInterface.h"
#include "ArrayStack.h"
#include "Node.h"
#include "Point.h"
#include "PrecondViolatedExcept.h"
#include "Editor.h"
#include "Snapshot.h"
#include "BinaryNode.h"
#include "BinaryNodeTree.h"
#include "BinarySearchTree.h"
#include "BinaryTreeInterface.h"
#include "NotFoundException.h"
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char *argv[])
{
	Editor xEditor;
	xEditor.readfile("sample.txt", "keywords.txt");
	xEditor.run();

	return 0;
}