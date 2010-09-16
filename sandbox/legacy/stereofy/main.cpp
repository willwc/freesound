#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include <cctype> // for toupper
#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>

#include "sndfile.h"
#include "anyoption.h"

#define BLOCKSIZE 2048

int main(int argc, char* argv[])
{
	AnyOption *opt = new AnyOption();

	opt->setOption("input",'i');
	opt->setOption("output",'o');
	
	opt->addUsage( "Usage: stereofy --input wavefile.wav --output.wav" );
	
	opt->processCommandArgs( argc, argv );

	std::string inputFilename;
	std::string outputFilename;
	
	if( opt->getValue("input") != NULL )
	{
		inputFilename = opt->getValue("input");
	}
	else
	{
		opt->printUsage();
		return 1;
	}

	if( opt->getValue("output") != NULL )
	{
		outputFilename = opt->getValue("output");
	}
	else
	{
		opt->printUsage();
		return 1;
	}

	SNDFILE *fileIn = NULL;
	SF_INFO sfinfoIn;
	
	// open the soundfile
	if(!(fileIn = sf_open (inputFilename.c_str(), SFM_READ, &sfinfoIn)))
	{
		std::cout << "stereofy: failed to open file" << std::endl;
		return  1;
	}
	else
	{
		std::cout << "stereofy: file opened" << std::endl;
	}

	if(sfinfoIn.frames == 0 || sfinfoIn.channels == 0)
	{
		sf_close(fileIn);
		std::cout << "stereofy: sfinfoIn.frames = 0 or sfinfoIn.channels = 0" << std::endl;
		return 1;
	}
	else
	{
		std::cout << "#channels " << sfinfoIn.channels << std::endl;
		std::cout << "#samplerate " << sfinfoIn.samplerate << std::endl;
		std::cout << "#duration " << double(sfinfoIn.frames)/double(sfinfoIn.samplerate) << std::endl;
	}


	SNDFILE *fileOut = NULL;
	SF_INFO sfinfoOut;

	sfinfoOut.format = SF_FORMAT_PCM_16;
	sfinfoOut.samplerate = sfinfoIn.samplerate;
	sfinfoOut.channels = sfinfoIn.channels < 2 ? sfinfoIn.channels : 2;

	if(!(fileOut = sf_open( outputFilename.c_str(), SFM_WRITE, &sfinfoOut)))
	{
		std::cout << "totally borked dude, opening output file failed!" << std::endl;
		std::cout << "error :: " << sf_strerror(fileOut);
		return 1;
	}

	// sampling data
	double *dataIn = new double[sfinfoIn.channels * BLOCKSIZE];

	double *dataOut = new double[sfinfoOut.channels * BLOCKSIZE];
		
	long readCount = 0;

	while((readCount = sf_read_double(fileIn, dataIn, sfinfoIn.channels * BLOCKSIZE)))
	{
		long j = 0;
		
		for(long i=0; i<readCount; i++)
		{
			if(i % sfinfoIn.channels < sfinfoOut.channels)
				dataOut[j++] = dataIn[i];
		}

		if(j != 0)
			sf_write_double(fileOut, dataOut, j);
	}

	sf_close (fileIn);
	sf_close (fileOut);
	
	delete dataIn;
	delete dataOut;
	
	return 0;
}
