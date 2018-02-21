//============================================================================
// Name        : test.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include "define.h"
#include "example.h"

//#define DEBUG

int main() {

//	epollTest();
	int a[] = {3,2,1,4,5};
	quickSort1(a, 0, 4);
	printArray(a, 5);

	return 0;
}
