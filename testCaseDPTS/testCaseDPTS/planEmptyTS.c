#include "stdLib.h"
#include "planEmptyTS.h"

#define USED_TS_FLAG					0x00
#define EMPTY_TS_FLAG					0x01

#define MAX_DELAY_TS_NUM_PER_CYCLE		2

static int disperseEmptyTS(unsigned int extraTSNum, unsigned int cycleNum, unsigned char *pPerCycleEmptyTSNum) {
	
	memset(pPerCycleEmptyTSNum, 0, cycleNum);
	pPerCycleEmptyTSNum[cycleNum-1] = 2;

	unsigned int i = cycleNum;
	while(extraTSNum > 0) {
		if(0 == i)
			i = cycleNum;
		i --;

		if(0 == cycleNum-1) {
			if(pPerCycleEmptyTSNum[i] <= pPerCycleEmptyTSNum[0]) {
				pPerCycleEmptyTSNum[i] ++;
				extraTSNum --;
			}
		} else {
			if(pPerCycleEmptyTSNum[i] <= pPerCycleEmptyTSNum[i+1]) {
				pPerCycleEmptyTSNum[i] ++;
				extraTSNum --;
			}
		}
	}

	pPerCycleEmptyTSNum[cycleNum-1] -= 2;

	return 0;
}

void expressionEmptyTS(unsigned int cycleLen, unsigned int cycleNum, unsigned char *pPerCycleEmptyTSNum, unsigned char *pEmptyPos) {
	unsigned int i = 0, j = 0, k = 0, pos = 0;
	for(i=0; i<cycleNum; i++) {
		pos += cycleLen;
		for(j=0; j<pPerCycleEmptyTSNum[i], j++) {
			pos ++;
			pEmptyPos[k] = pos;
			k ++;
		}
	}
}

static int trySyncEmptyTSPos(unsigned int pos, P_PLAN_INPUT_UNIT *pCycle, unsigned int totalEmptyTSNum, unsigned char *pEmptyPos) {
	return 0;
}

int planEmptyTS(
	unsigned int TSNumPerLine,
	unsigned int protectTSNum,
	unsigned int leastEmptyTSNum, 
	unsigned int inNum, 
	P_PLAN_INPUT_UNIT pIn, 
	P_PLAN_OUTPUT_UNIT pOut) {

	if((2*TSNumPerLine) >= TSNumPerLine) 
		return -1;

	//unsigned char extraTSNum[inNum];
	unsigned char cycleNum[inNum];
	unsigned char perCycleEmptyTSNum[inNum][TSNumPerLine];
	unsigned int i = 0;

	for(i=0; i<inNum; i++) {
		pOut[i].emptyTSNum = TSNumPerLine % pIn[i].totalTTSNum;
		cycleNum[i] = TSNumPerLine / pIn[i].totalTTSNum;
		if(0 == pOut[i].emptyTSNum) {
			pOut[i].emptyTSNum += pIn[i].totalTTSNum;
			cycleNum[i] --;
		}
		disperseEmptyTS(pOut[i].emptyTSNum, cycleNum[i], perCycleEmptyTSNum[i]);
		expressionEmptyTS(pIn[i].totalTTSNum, cycleNum[i], perCycleEmptyTSNum[i], pOut[i].emptyTSPos);
		memset(pOut[i].emptyTSAttr, EMPTY_TS_ATTR_ASYNC, pOut[i].emptyTSNum);
	}

	unsigned int TTSSchedule[inNum];
	unsigned char delayTSNum[inNum][TSNumPerLine-(2*protectTSNum)];
	
	return 0;
}