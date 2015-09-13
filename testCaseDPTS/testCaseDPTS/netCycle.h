#pragma once

#include "TimeSlot.h"

#define MAX_STACK_DEPP		(1024*1024*10)

class netCycle
{
public:
	netCycle(void);
	~netCycle(void);

	TimeSlot *pHeadTS;
	TimeSlot *pBaseTS;
	TimeSlot *pShitfTS;
	TimeSlot *pEndTS;

private :
		TimeSlot *pPreBaseTSBuf[MAX_STACK_DEPP];
		unsigned int stackPos;
		TimeSlot *pop();
		bool push(TimeSlot *pPreBaseTS);
		TimeSlot *stackGet();

public :
	unsigned int TSNum;

	void addOneTS();
	void resetNetCycle();
	void restore();
	void del(TimeSlot *pPre, TimeSlot *pNext);
	void insert(TimeSlot *pPre, TimeSlot *pNext, TimeSlot *pTS);
	bool shiftRightOneTTS(TimeSlot *pTS);

	bool goNextSituation();

	void printCycle();
};

