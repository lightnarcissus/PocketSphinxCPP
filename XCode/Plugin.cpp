/*
	This is a simple plugin, a bunch of functions that do simple things.
*/

#include "Plugin.pch"
#define MODELDIR "/Users/anshpatel/Desktop/PocketSphinx/project/pocketsphinx/model"
#include <pocketsphinx.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sstream>
#include <fstream>
using namespace std;

extern "C" int ps_start_utt(ps_decoder_t *ps);
extern "C" int cmd_ln_free_r(cmd_ln_t *cmdln);
extern "C" void ps_default_search_args(cmd_ln_t *);
extern "C" int ps_end_utt(ps_decoder_t *ps);
extern "C" ps_decoder_t *ps_init(cmd_ln_t *config);
extern "C" int ps_free(ps_decoder_t *ps);
extern "C" char const *cmd_ln_str_r(cmd_ln_t *cmdln, char const *name);
extern "C" cmd_ln_t *cmd_ln_parse_file_r(cmd_ln_t *inout_cmdln, /**< In/Out: Previous command-line to update,
                                                                  or NULL to create a new one. */
                                         arg_t const *defn,   /**< In: Array of argument name definitions*/
                                         char const *filename,/**< In: A file that contains all
                                                               the arguments */
                                         int32 strict         /**< In: Fail on duplicate or unknown
                                                               arguments, or no arguments? */
);

extern "C" string audioPath="";
char* logPath="";
char* audioExtension=".wav";
char* textExtension=".txt";
//const char* dirPath="/Users/anshpatel/Desktop/Unity Projects/YC_CoinTask/TextFiles/TH1/Test/session_0/audio/";

static const arg_t cont_args_def[] = {
    POCKETSPHINX_OPTIONS,
    {"-argfile",
        ARG_STRING,
        NULL,
        "Argument file giving extra arguments."},
    {"-adcdev",
        ARG_STRING,
        NULL,
        "Name of audio device to use for input."},
    {"-infile",
        ARG_STRING,
        NULL,
        "Audio file to transcribe."},
    {"-inmic",
        ARG_BOOLEAN,
        "no",
        "Transcribe audio from microphone."},
    {"-time",
        ARG_BOOLEAN,
        "no",
        "Print word times in file transcription."},
    CMDLN_EMPTY_OPTION
};


extern "C" void CreateLogFilePath(char* log)
{
    char* logName="sphinxLog.log";
    size_t logLength=strlen(logName);
    size_t audioPathLength=strlen(log);
    int totalLength=audioPathLength+logLength;
    char* logFile=new char[totalLength];
    strcpy(logFile,log);
    strcat(logFile, logName);
    logPath=logFile;
}

extern "C" void SetAudioPath(char* path)
{
    audioPath=path;
    CreateLogFilePath(path);
}


extern "C" char* GetAudioPath()
{
    
    const char* audioChar=audioPath.c_str();
    size_t length=strlen(audioChar);
    char* objChar=new char[length];
    strcpy(objChar,audioChar);
    return objChar;
}

char* createFile(int trialNumber, int recallNumber,char* extension)
{
    
    stringstream trialStr;
    stringstream recallStr;
    trialStr << trialNumber;
    recallStr << recallNumber;
    
    string str1=trialStr.str();
    string str2=recallStr.str();
    
    const char* trialChar=str1.c_str();
    const char* recallChar=str2.c_str();
    size_t trialLength=strlen(trialChar);
    size_t recallLength=strlen(recallChar);
    
    const char* audioChar= audioPath.c_str();
    //if(audioPath!=NULL)
    size_t dirLength=strlen(audioChar);
    
    char* midbit="_";
    char* finalBit=extension;
    
    //trial length + recall length + "_" + ".wav" (last two bits comprising of the right-most 4 operand
    size_t totalLength=dirLength+trialLength+recallLength+4;
    // printf("Total length %i \n",totalLength);
    char* fileName=new char[totalLength];
    //copy the directory path and then concatenate the rest
    strcpy(fileName,audioChar);
    strcat(fileName,trialChar);
    strcat(fileName,midbit);
    strcat(fileName,recallChar);
    strcat(fileName,finalBit);
    
    // printf("Current filename is: %s \n",fileName);
    //  printf("%i",trialNumber);
 //   char trial=trialNumber;
    //const char* recall=(char*)recallNumber;
    // cout << trialLength;
    return fileName;
    
}

char* readRecallObjectName(char* textFile)
{
    
    std::ifstream infile(textFile);
    string objName;
    infile >> objName;
    const char* constStr= objName.c_str();
    size_t length=strlen(constStr);
    char* objChar=new char[length];
    strcpy(objChar,constStr);
    return objChar;
}

char* SphinxRun(int trialNumber, int recallNumber,int threshold){
    
    ps_decoder_t *ps;
    cmd_ln_t *config;
    //create filename
    char* audioFile=createFile(trialNumber, recallNumber,audioExtension);
    //   printf("Current File is: %s \n",audioFile);
    char *currentTextFile= createFile(trialNumber,recallNumber,textExtension);
    char* currentRecallObject = readRecallObjectName(currentTextFile);
    
    stringstream kws_stream;
    kws_stream<<threshold;
    string threshold_string=kws_stream.str();
    const char* currentkws= threshold_string.c_str();
    size_t kwslength=strlen(currentkws);
    char* addedBit="1e-";
    int totLength=3+kwslength;
    char* actualkws=new char[totLength];
    strcpy(actualkws,addedBit);
    strcat(actualkws,currentkws);
    //  cout<< currentRecallObject;
    FILE *fh;
    char const *hyp;
    int16 buf[512];
    int rv;
    int32 score;
    char *argv[8];
    char* keyword=currentRecallObject;
    argv[0]="-infile";
    argv[1]=audioFile;
    argv[2]="-keyphrase";
    argv[3]=currentRecallObject;
    //   argv[3]="/Users/anshpatel/Desktop/TH Audio Pilot/TextFiles/Andrew_W/session_0/audio/kws.txt";
    argv[4]="-kws_threshold";
    argv[5]=actualkws;
    argv[6]="-logfn";
    argv[7]=logPath;
    argv[8]="-time";
    argv[9]="yes";
    config = cmd_ln_parse_r(NULL, cont_args_def, 10, argv, FALSE);    //keyword=objNames[trialNumber];
    /* Handle argument file as -argfile. */
//    if (config && (cfg = cmd_ln_str_r(config, "-argfile")) != NULL) {
//        config = cmd_ln_parse_file_r(config, cont_args_def, cfg, FALSE);
//    }
//    
//    if (config == NULL || (cmd_ln_str_r(config, "-infile") == NULL && cmd_ln_boolean_r(config, "-inmic") == FALSE)) {
//        //  E_INFO("Specify '-infile <file.wav>' to recognize from file or '-inmic yes' to recognize from microphone.\n");
//        cmd_ln_free_r(config);
//        //  return 1;
//    }
    
    
    ps_default_search_args(config);
    ps = ps_init(config);
    if (ps == NULL) {
        cmd_ln_free_r(config);
        //  return 1;
    }
    char cwd[1024];
    //  FOR THE FILE PATH
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        fprintf(stdout, "");
    
    // E_INFO("%s COMPILED ON: %s, AT: %s\n\n", argv[0], __DATE__, __TIME__);
    
//    if (cmd_ln_str_r(config, "-infile") != NULL) {
//        // printf("recognize from file");
//        // recognize_from_file();
//    } else if (cmd_ln_boolean_r(config, "-inmic")) {
//        //recognize_from_microphone();
//        // printf("recognize from mic");
//    }
    if(audioFile!=NULL)
    {
        fh = fopen(audioFile, "rb");
        if (fh == NULL) {
            fprintf(stderr, "Unable to open input file goforward.raw\n");
            //   return -1;
        }
        
        rv = ps_start_utt(ps);
        
        while (!feof(fh)) {
            size_t nsamp;
            nsamp = fread(buf, 2, 512, fh);
            rv = ps_process_raw(ps, buf, nsamp, FALSE, FALSE);
        }
        rv = ps_end_utt(ps);
        hyp = ps_get_hyp(ps, &score);
        //     printf("Recognized: %s with keyword: %s \n", hyp, keyword);
        
        //    if(strncmp(hyp,"television",9)==0)
        //        printf("television");
        
        fclose(fh);
        //ps_free(ps);
        //cmd_ln_free_r(config);
        //  const char* audioChar=audioPath.c_str();
        //const char* fin=cwd;
        //  cout << hyp;
        //  printf("%s and keyword %s",hyp,keyword);
        if(hyp!=NULL && strncmp(hyp,keyword,strlen(keyword))==0)
        {
            ps_free(ps);
            cmd_ln_free_r(config);
            return "found";
            
        }
        else
        {
            ps_free(ps);
            cmd_ln_free_r(config);
            return "not found";
        }
    }
    else
    {
        return "Error! File not found";
    }
}

int PrintANumber(){
	return 5;
}

int AddTwoIntegers(int a, int b) {
	return a + b;
}

float AddTwoFloats(float a, float b) {
	return a + b;
}



