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
	const char* SpeechRecognitionLM(char*, char*, char*, char*);
	int PrintANumber();
	int AddTwoIntegers(int, int);
	float AddTwoFloats(float, float);

}
