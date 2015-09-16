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

static unsigned int findMax(unsigned int n, unsigned int *buf) {
	unsigned int ret = 0, i = 0;
	for(i=0; i<n; i++) {
		ret = (buf[i] > ret ? buf[i] : ret);
	}
	return ret;
}

static int disperseEmptyTS(unsigned int extraTSNum, unsigned int cycleNum, unsigned char *pPerCycleEmptyTSNum) {
	
	unsigned int i = cycleNum-1;

	memset(pPerCycleEmptyTSNum, 0, cycleNum);
	pPerCycleEmptyTSNum[cycleNum-1] = 2;

	while(extraTSNum > 0) {
		
		if(i == cycleNum-1) {
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

		//i --;
		if(0 == i)
			i = cycleNum-1;
		else 
			i--;
	}

	pPerCycleEmptyTSNum[cycleNum-1] -= 2;

	return 0;
}

unsigned int expressionEmptyTS(unsigned int cycleLen, unsigned int cycleNum, unsigned char *pPerCycleEmptyTSNum, unsigned char *pEmptyPos) {
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
	return k;
}

static int trySyncEmptyTSPos(unsigned int pos, unsigned int endPos, P_PLAN_INPUT_UNIT pCycle, unsigned int totalEmptyTSNum, unsigned char *pEmptyPos) {

	unsigned int i = 0, j = 0, k = 0, ret = 0;
	unsigned int variableEmptyTSNum = 0;
	unsigned char *line = NULL;

	if(pos < 1) 
		return -2;

	if((endPos - pos) < 3)
		return -2;

	line = (unsigned char *)malloc(pos);
	memset(line, USED_TS_FLAG, pos);
	//sort(totalEmptyTSNum, pEmptyPos);

	for(i=0; i<totalEmptyTSNum; i++) {
		if(pEmptyPos[i] < pos)
			line[pEmptyPos[i]] = EMPTY_TS_FLAG;
		if((pEmptyPos[i] >= pos) && (pEmptyPos[i] < endPos))
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
			if((pos - i + 1) > variableEmptyTSNum) {
				free(line);
				return -1;
			}
			ret += (pos - i + 1);
			break;
		} else {
			for(k=0; k<pCycle->pTTSNumInTS[j]; k++) {
				if(EMPTY_TS_FLAG == line[i+k]) {
					free(line);
					return -2;
				}
				line[i+k] = USED_TS_FLAG;
			}
			i += k;
		}

		j ++;
	}

	if(i == pos)
		ret ++;

	free(line);
	return ret;
}

static int setSyncEmptyTSPos(unsigned int pos, unsigned int endPos, P_PLAN_INPUT_UNIT pCycle, unsigned int totalEmptyTSNum, unsigned char *pEmptyPos) {

	unsigned int i = 0, j = 0, k = 0, ret = 0, firstVariableEmptyTSSN = 0;
	unsigned int variableEmptyTSNum = 0;
	unsigned char *line = NULL;

	if(pos < 1) 
		return -2;

	if((endPos - pos) < 3)
		return -2;

	line = (unsigned char *)malloc(pos);
	memset(line, USED_TS_FLAG, pos);
	sort(totalEmptyTSNum, pEmptyPos);

	for(i=0; i<totalEmptyTSNum; i++) {
		if(pEmptyPos[i] < pos)
			line[pEmptyPos[i]] = EMPTY_TS_FLAG;
		if((pEmptyPos[i] >= pos) && (pEmptyPos[i] < endPos))
			variableEmptyTSNum ++;
	}

	for(i=0; i<totalEmptyTSNum; i++) {
		if((pEmptyPos[i] >= pos) && (pEmptyPos[i] < endPos)) {
			firstVariableEmptyTSSN = i;
			break;
		}
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
			if((pos - i + 1) > variableEmptyTSNum) {
				free(line);
				return -1;
			}
			for(k=0; k<(pos - i + 1); k++) {
				pEmptyPos[firstVariableEmptyTSSN + k] = (i + k);
			}
			ret += (pos - i + 1);
			break;
		} else {
			for(k=0; k<pCycle->pTTSNumInTS[j]; k++) {
				if(EMPTY_TS_FLAG == line[i+k]) {
					free(line);
					return -2;
				}
				line[i+k] = USED_TS_FLAG;
			}
			i += k;
		}
	}

	if(i == pos) {
		ret ++;
		pEmptyPos[firstVariableEmptyTSSN] = pos;
	}

	free(line);
	return ret;
}

static int findInsertPos(unsigned int startPos, unsigned int endPos, unsigned int inNum, P_PLAN_INPUT_UNIT pIn, P_PLAN_OUTPUT_UNIT pOut, unsigned int *emptyTSNum) {

	int val = 0;
	unsigned int i = 0, j = 0, init_i, ret = 0;
	unsigned int *pTemp = NULL;
	unsigned char *errorRecord = NULL;
	unsigned char **insertPosDelayTSNum = NULL;

	(*emptyTSNum) = 255;

	pTemp = (unsigned int *)malloc(inNum*sizeof(unsigned int));
	memset(pTemp, 255, inNum*sizeof(unsigned int));
	errorRecord = (unsigned char *)malloc(inNum);
	memset(errorRecord, 0, inNum);
	insertPosDelayTSNum = (unsigned char **)malloc(inNum*sizeof(char *));
	for(i=0; i<inNum; i++) {
		insertPosDelayTSNum[i] = (unsigned char *)malloc(endPos);
	}

	init_i = pIn[0].pTTSNumInTS[0];
	for(i=1; i<inNum; i++) {
		if(pIn[i].pTTSNumInTS[0] < init_i)
			init_i = pIn[i].pTTSNumInTS[0];
	}
	for(i=startPos+init_i; i<endPos; i++) {/* 前面空几个 */
		for(j=0; j<inNum; j++) {
			val = trySyncEmptyTSPos(i, endPos, &pIn[j], pOut[j].emptyTSNum, pOut[j].emptyTSPos);
			if(-1 == val) {
				errorRecord[j] ++;
				if(errorRecord[j] == (endPos - 1)) {
					ret = (-j);
					goto findInsertPos_end;
				}
				break;
			}
			else if(val < 0) {
				if(i == init_i) {
					ret = (-inNum);
					goto findInsertPos_end;
				}
				goto findInsertPos_end;
			}
			insertPosDelayTSNum[j][i] = val;
			pTemp[j] = insertPosDelayTSNum[j][i];
		}
		if((*emptyTSNum) > findMax(inNum, pTemp)) {
			(*emptyTSNum) = findMax(inNum, pTemp);
			ret = i;
		}
	}

findInsertPos_end :
	for(i=0; i<inNum; i++) {
		free(insertPosDelayTSNum[i]);
	}
	free(insertPosDelayTSNum);
	free(pTemp);
	free(errorRecord);

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

	//unsigned int curInsertNum = 0;
	unsigned char *insertPos = NULL;

	unsigned int i = 0, j = 0, k = 0, startPos = 0, endPos = 0, epmtyTSNum = 255, tempEptTSNum = 255;

	int tempVal = 0;

	if((2*protectTSNum) >= TSNumPerLine) 
		return -1;

	cycleNum = (unsigned char *)malloc(inNum);

	perCycleEmptyTSNum = (unsigned char **)malloc(inNum*sizeof(char *));
	for(i=0; i<inNum; i++) {
		perCycleEmptyTSNum[i] = (unsigned char *)malloc(TSNumPerLine);
	}

	insertPos = (unsigned char *)malloc(leastEmptyTSNum);

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



PLAN_EMPTY_TS :

	memset(insertPos, TSNumPerLine-2, leastEmptyTSNum);
	for(i=0; i<leastEmptyTSNum; i++) {

		sort(i, insertPos);
		epmtyTSNum = 255;
		tempEptTSNum = 255;
		tempVal = 0;
		for(j=0; j<i+1; j++) {
			if(0 == j)
				startPos = 0;
			else
				startPos = insertPos[j-1];

			endPos = insertPos[j];

			tempVal = findInsertPos(startPos, endPos, inNum, pIn, pOut, &tempEptTSNum);
			if(tempVal > 0) {
				if(tempEptTSNum < epmtyTSNum) {
					epmtyTSNum = tempEptTSNum;
					insertPos[i] = tempVal;
				}
			} else {
				if(abs(tempVal) >= inNum) {
					//return -1;
					continue;
				}
				else {
					pOut[abs(tempVal)].emptyTSNum += pIn[abs(tempVal)].totalTTSNum;
					cycleNum[abs(tempVal)] --;
					for(k=0; k<inNum; k++) {
						disperseEmptyTS(pOut[k].emptyTSNum, cycleNum[k], perCycleEmptyTSNum[k]);
						expressionEmptyTS(pIn[k].totalTTSNum, cycleNum[k], perCycleEmptyTSNum[k], pOut[k].emptyTSPos);
						memset(pOut[k].emptyTSAttr, EMPTY_TS_ATTR_ASYNC, pOut[k].emptyTSNum);
						goto PLAN_EMPTY_TS;
					}
				}
			}
		}

		for(k=0; k<i; k++) {
			if(insertPos[k] > insertPos[i]) {
				endPos = insertPos[k];
				break;
			} else {
				endPos = (TSNumPerLine-protectTSNum);
			}
		}

		for(k=0; k<inNum; k++) {
			setSyncEmptyTSPos(insertPos[i], endPos, &pIn[k], pOut[k].emptyTSNum, pOut[k].emptyTSPos);
		}
	}

	
	for(i=0; i<inNum; i++) {
		for(j=0; j<TSNumPerLine; j++) {
			for(k=0; k<pOut[i].emptyTSNum; k++) {
				if(j == pOut[i].emptyTSPos[k])
					break;
			}
			if(k == pOut[i].emptyTSNum)
				printf("0 ");
			else 
				printf("1 ");
		}
		printf("\r\n");
	}



	free(cycleNum);

	for(i=0; i<inNum; i++) {
		free(perCycleEmptyTSNum[i]);
	}
	free(perCycleEmptyTSNum);

	free(insertPos);

	return 0;
}