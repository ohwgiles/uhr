#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iconv.h>
#include <string.h>

#define RECOG_MEM_SIZE 2048
#define RECOG_MAX_CANDIDATES 10
#define CHAR_CODE_BYTES 2
#define UTF8_STR_LEN 10

extern unsigned char _binary_WTPDA_GB2312_lib_start;
extern unsigned char _binary_WTPDA_GB2312_lib_end;
extern unsigned char _binary_WTPDA_GB2312_lib_size;

extern short WTRecognizeInit(char *RamAddress, size_t RamSize, unsigned char *LibStartAddress);	
extern short WTRecognize(char *PointBuf, short PointsNumber, unsigned short *CandidateResult);
extern short WTSetRange(short Range);
extern short WTRecognizeEnd();

int main(int argc, char **argv) {
	(void) argc;
	(void) argv;
	
	// Initialisation
	iconv_t ic = iconv_open("UTF-8", "GB18030");
	if(ic == (iconv_t)-1) {
		perror("iconv_open");
		return 1;
	}

	char* recognise_memblock = 0;
	if((recognise_memblock = (char *)malloc(RECOG_MEM_SIZE)) == NULL)
		return perror("malloc"), 1;
	if(WTRecognizeInit(recognise_memblock, 5000, &_binary_WTPDA_GB2312_lib_start) != 0)
		return perror("RecognizeInit"), 1;

	// simplified
	WTSetRange(0x0001);
	// traditional
	//	WTSetRange(0x0007);

	// Main Loop
	char* input = 0;
	char candidates[RECOG_MAX_CANDIDATES][CHAR_CODE_BYTES];
	char result[UTF8_STR_LEN+2];
	short nInput, nLastInput = 0;
	while(1) {
		// handle EOF
		if(read(STDIN_FILENO, &nInput, sizeof(short)) == 0)
			break;
		// get length of message
		if(nInput > nLastInput && (input = realloc(input, nInput+2)) == NULL)
			return perror("realloc"), 1;
		// read strokes
		int nRead;
		if((nRead = read(STDIN_FILENO, input, nInput)) != nInput) {
			fprintf(stderr, "Error: wanted %d bytes but got %d\n", nInput, nRead);
			return 1;
		}
		// replace 0xffff with 0xff00
		for(int i=1; i<nInput; i+=2) {
			if(input[i] == (char)0xff && input[i-1] == (char)0xff)
				input[i] = 0x00;
		}
		//terminate input array with 0xffff
		input[nInput] = 0xff;
		input[nInput+1] = 0xff;
		// recognise
		memset(candidates, 0, sizeof(candidates));
		WTRecognize(input, nInput, (unsigned short*)candidates);
		candidates[RECOG_MAX_CANDIDATES-1][0] = 0; // terminate array
	
		char* inp = (char*)candidates;
		char* p = &result[sizeof(short)];
		for(int i=1; i<RECOG_MAX_CANDIDATES; ++i) {
			size_t nIn = CHAR_CODE_BYTES;
			size_t nOut = UTF8_STR_LEN;
			if(iconv(ic, &inp, &nIn, &p, &nOut) == (size_t)-1)
				return perror("iconv"), 1;
		}
		int msgbytes = p - result;
		((short*)result)[0] = msgbytes - sizeof(short);
		write(STDOUT_FILENO, result, msgbytes);
		//fprintf(stdout, "%s\n", result_utf8);
		nLastInput = nInput;
	}
	
	// Clean up
	iconv_close(ic);
	WTRecognizeEnd();
	free(recognise_memblock);
	return 0;
}
