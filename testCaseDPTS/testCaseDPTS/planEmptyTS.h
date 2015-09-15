#ifndef __PLAN_EMPTY_TS_H__
#define __PLAN_EMPTY_TS_H__

#define MAX_EMPTY_TS_NUM		128
#define EMPTY_TS_ATTR_SYNC		0x01
#define EMPTY_TS_ATTR_ASYNC		0x00

typedef struct {
	unsigned int totalTTSNum;
	unsigned int TSNum;
	unsigned char *pTTSNumInTS;
}PLAN_INPUT_UNIT, *P_PLAN_INPUT_UNIT;

typedef struct {
	unsigned int emptyTSNum;
	unsigned char emptyTSPos[MAX_EMPTY_TS_NUM];
	unsigned char emptyTSAttr[MAX_EMPTY_TS_NUM];
}PLAN_OUTPUT_UNIT, *P_PLAN_OUTPUT_UNIT;

/*
int planEmptyTS(
	unsigned int TSNumPerLine, 
	unsigned int protectTSNum, 
	unsigned int leastEmptyTSNum, 
	unsigned int inNum, 
	P_PLAN_INPUT_UNIT pIn, 
	P_PLAN_OUTPUT_UNIT pOut);
	*/
	
#endif