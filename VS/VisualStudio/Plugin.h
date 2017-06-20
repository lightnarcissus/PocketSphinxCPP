#pragma once
#include <iostream>
#include <stdio.h>
#include <errno.h>
#include <sstream>
#include <fstream>
using namespace std;

extern "C" {
	void SetAudioPath(char*);
	void CreateLogFilePath(char*);
	char* GetAudioPath();
	char* readRecallObjectName(char*);
	char* createFile(int, int, char*);
	const char* SphinxRun(int, int, int);
	int EXPORT_API PrintANumber();
	int EXPORT_APIAddTwoIntegers(int, int);
	float EXPORT_API AddTwoFloats(float, float);
}
