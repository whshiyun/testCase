// testCaseDPTS.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <stdlib.h>
#include "TimeSlot.h"
#include "netCycle.h"
#include "planEmptyTS.h"

#define  MAX_NODE_NUM		1
#define  MAX_TTS_NUM		4

void inti() {
	TimeSlot::maxMergeNum = 8;
}

bool runOnce(netCycle *cycle[MAX_NODE_NUM]) {
	
	int i = 0;
	for(; i<MAX_NODE_NUM; i++) {
		if(false == cycle[i]->goNextSituation()) {
			cycle[i]->restore();
			if(cycle[i]->TSNum >= MAX_TTS_NUM) {
				cycle[i]->resetNetCycle();
				continue;
			} else {
				cycle[i]->addOneTS();
				cycle[i]->pBaseTS = NULL;
				cycle[i]->pShitfTS = cycle[i]->pHeadTS;
				return true;
			}
		} else {
			return true;
		}
	}
	return false;
}
extern "C" {
int _tmain(int argc, _TCHAR* argv[])
{

	PLAN_INPUT_UNIT in[2];
	PLAN_OUTPUT_UNIT out[2];

	in[0].TSNum = 3;
	in[0].totalTTSNum = 7;
	in[0].pTTSNumInTS = (unsigned char *)malloc(in[0].TSNum);
	in[0].pTTSNumInTS[0] = 2;
	in[0].pTTSNumInTS[1] = 2;
	in[0].pTTSNumInTS[2] = 3;

	in[1].TSNum = 3;
	in[1].totalTTSNum = 9;
	in[1].pTTSNumInTS = (unsigned char *)malloc(in[0].TSNum);
	in[1].pTTSNumInTS[0] = 3;
	in[1].pTTSNumInTS[1] = 5;
	in[1].pTTSNumInTS[2] = 1;

	extern int planEmptyTS(
	unsigned int TSNumPerLine, 
	unsigned int protectTSNum, 
	unsigned int leastEmptyTSNum, 
	unsigned int inNum, 
	P_PLAN_INPUT_UNIT pIn, 
	P_PLAN_OUTPUT_UNIT pOut);

	planEmptyTS(32, 3, 2, 2, in, out);

	return 0;

	inti();
	netCycle *cycle[MAX_NODE_NUM];

	unsigned long long cnt = 0;
	unsigned int i = 0;
	for(i=0; i<MAX_NODE_NUM; i++) {
		cycle[i] = new netCycle();
		cycle[i]->resetNetCycle();
		cycle[i]->printCycle();
	}

	printf("%llu \r\n", ++cnt);
	while(true == runOnce(cycle)) {
#if 1
		for(i=0; i<MAX_NODE_NUM; i++) {
			cycle[i]->printCycle();
		}
#endif
		printf("%llu \r\n", ++cnt);
		if(cnt == 463) {
			cnt ++;
			cnt --;
		}
	}

#if 0
	netCycle *pCycle = new netCycle();

	pCycle->addOneTS();
	pCycle->addOneTS();
	pCycle->addOneTS();
	pCycle->addOneTS();
	pCycle->addOneTS();
	pCycle->addOneTS();
	pCycle->addOneTS();
	pCycle->addOneTS();
	pCycle->addOneTS();
	pCycle->addOneTS();
	pCycle->addOneTS();

	pCycle->resetNetCycle();
	pCycle->printCycle();
	int i = 0;
	while (1) {
		//if(i == 127)
		i++;
		if(true == pCycle->goNextSituation()) {
			//printf("%d \r\n", i);
			pCycle->printCycle();
		}
		else {
			printf("total cnt = %d \r\n", i);
			printf("error !!! \r\n");
			return 0;
		}
	}

	delete(pCycle);
#endif
	return 0;
}

}


