#include <stdLib.h>
#include <stdio.h>
#include <string.h>
#include "planEmptyTS.h"

#define USED_TS_FLAG					0x00
#define EMPTY_TS_FLAG					0x01
#define SYNC_TS_FLAG					0x02

#define MAX_DELAY_TS_NUM_PER_CYCLE		2

static unsigned int adjHeap(unsigned int i, unsigned int bufLen, unsigned char *buf) {

	unsigned int temp = 0;

	if(i >= bufLen)
		return i;
	if((2*i + 1) >= bufLen)
		return i;
	if((2*i + 2) >= bufLen) {
		temp = 2*i + 1;
	} else {
		temp = buf[2*i + 1] > buf[2*i + 2] ? (2*i + 1) : (2*i + 2);
	}
	
	if(buf[temp] > buf[i]) {
		buf[i] ^= buf[temp] ^= buf[i] ^= buf[temp];
	}

	return temp;
}

static void neatemHeap(unsigned int i, unsigned int bufLen, unsigned char *buf) {

	unsigned int cur = i;
	cur = adjHeap(cur, bufLen, buf);
	if(cur != i)
		neatemHeap(cur, bufLen, buf);
	return ;
}

static void buildHeap(unsigned int bufLen, unsigned char *buf) {
	unsigned int i = 0, adjRet = 0;
	i = bufLen / 2;
	if(0 == bufLen % 2)
		i --;

	while(1) {
		adjRet = adjHeap(i, bufLen, buf);
		if(0 == i)
			break;
		i --;
	}

	if(0 != adjRet) {
		neatemHeap(adjRet, bufLen, buf);
	}
	
	return ;
}

static void sort(unsigned int num, unsigned char *buf) {

	unsigned int i = 0, j = 0;
	if(0 == num)
		return ;

	i = num - 1;
	buildHeap(num, buf);

	while(i > 0) {

		buf[0] ^= buf[i] ^= buf[0] ^= buf[i];

		neatemHeap(0, i, buf);
		i --;
	}
}

static int disperseEmptyTS(unsigned int extraTSNum, unsigned int cycleNum, unsigned char *pPerCycleEmptyTSNum) {
	
	unsigned int i = cycleNum;

	memset(pPerCycleEmptyTSNum, 0, cycleNum);
	pPerCycleEmptyTSNum[cycleNum-1] = 2;

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
		for(j=0; j<pPerCycleEmptyTSNum[i]; j++) {
			//pos ++;
			pEmptyPos[k] = pos;
			pos++;
			k ++;
		}
	}
}

static int trySyncEmptyTSPos(unsigned int pos, P_PLAN_INPUT_UNIT pCycle, unsigned int totalEmptyTSNum, unsigned char *pEmptyPos) {

	unsigned int i = 0, j = 0, k = 0, ret = 0;
	unsigned int variableEmptyTSNum = 0;
	unsigned char *line = (unsigned char *)malloc(pos);

	if(pos < 1) {
		free(line);
		return -2;
	}

	memset(line, USED_TS_FLAG, pos);
	sort(totalEmptyTSNum, pEmptyPos);

	for(i=0; i<totalEmptyTSNum; i++) {
		line[pEmptyPos[i]] = EMPTY_TS_FLAG;
		if(pEmptyPos[i] >= pos)
			variableEmptyTSNum ++;
	}

	if(0 == variableEmptyTSNum) {
		free(line);
		return -1;
	}

	if(EMPTY_TS_FLAG == line[pos-1])
		ret ++;
	for(i=(pos-1); i>0; i--) {
		if(EMPTY_TS_FLAG != line[i-1])
			break;
		pos --;
		ret ++;
	}

	for(i=0; i<pos; ) {

		if(EMPTY_TS_FLAG == line[i]) {
			i ++;
			continue;
		}

		if(j >= pCycle->TSNum)
			j = 0;

		if((i + pCycle->pTTSNumInTS[j]) > pos) {
			ret += (pos - i);
		} else {
			for(k=0; k<pCycle->pTTSNumInTS[j]; k++) {
				if(EMPTY_TS_FLAG == line[i+k]) {
					free(line);
					return -1;
				}
				line[i+k] = USED_TS_FLAG;
			}
			i += k;
		}
	}

	free(line);
	return ret;
}

int planEmptyTS(
	unsigned int TSNumPerLine,
	unsigned int protectTSNum,
	unsigned int leastEmptyTSNum, 
	unsigned int inNum, 
	P_PLAN_INPUT_UNIT pIn, 
	P_PLAN_OUTPUT_UNIT pOut) {

	//unsigned char extraTSNum[inNum];
	unsigned char *cycleNum = NULL;
	unsigned char **perCycleEmptyTSNum = NULL;
	unsigned int i = 0;

	if((2*TSNumPerLine) >= TSNumPerLine) 
		return -1;

	cycleNum = (unsigned char *)malloc(inNum);
	perCycleEmptyTSNum = (unsigned char **)malloc(inNum);
	for(i=0; i<inNum; i++) {
		perCycleEmptyTSNum[i] = (unsigned char *)malloc(TSNumPerLine);
	}

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

	//unsigned int TTSSchedule[inNum];
	//unsigned char delayTSNum[inNum][TSNumPerLine-(2*protectTSNum)];
	
	return 0;
}