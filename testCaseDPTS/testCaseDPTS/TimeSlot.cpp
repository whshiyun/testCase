#include "StdAfx.h"
#include "TimeSlot.h"

unsigned int TimeSlot::maxMergeNum = 0;

TimeSlot::TimeSlot(unsigned int id)
{
	this->id = id;

	this->pPre = NULL;
	this->pNext = NULL;
	this->pSubTSHead = NULL;
	this->pSubTSTail = NULL;
	this->TTS_num = 1;
}


TimeSlot::~TimeSlot(void)
{
}

void TimeSlot::add(TimeSlot *pPre) {
	
	if(NULL != pPre->pNext) {
		pPre->pNext->pPre = this;
		this->pNext = pPre->pNext;
	}

	this->pPre = pPre;
	pPre->pNext = this;
}

bool TimeSlot::mergeSubTS(unsigned int n) {

	if((this->TTS_num + n) > TimeSlot::maxMergeNum)
		return false;

	unsigned int i = 0;
	TimeSlot *pEnd = NULL;
	for(; i<n; i++) {
		pEnd = this->pNext;
		if(NULL == pEnd)
			return false;
	}

	if(1 != this->pNext->TTS_num) {
		printf("1 != this->pNext->TTS_num");
		return false;
	}

	if(0 == n)
		return true;

	if(NULL == this->pSubTSHead) {
		this->pSubTSHead = this->pNext;
		if(NULL != this->pNext)
			this->pNext->pPre = this->pSubTSHead;

		this->pNext = pEnd->pNext;

		if(NULL != pEnd->pNext)
			pEnd->pNext->pPre = this;

		pEnd->pNext = NULL;

		this->pSubTSTail = pEnd;
	} else {
		this->pSubTSTail->pNext = this->pNext;
		if(NULL != this->pNext)
			this->pNext->pPre = this->pSubTSTail;

		this->pNext = pEnd->pNext;

		if(NULL != pEnd->pNext) {
			pEnd->pNext->pPre = this;

			pEnd->pNext = NULL;
		}

		this->pSubTSTail = pEnd;
	}

	this->TTS_num += n;

	return true;
}

TimeSlot *TimeSlot::releaseSubTS() {

	if(NULL == this->pSubTSHead)
		return this;

	this->pSubTSTail->pNext = this->pNext;
	if(NULL != this->pNext)
		this->pNext->pPre = this->pSubTSTail;

	this->pNext = this->pSubTSHead;
	this->pSubTSHead->pPre = this;

	TimeSlot *ret = this->pSubTSTail;

	this->pSubTSHead = NULL;
	this->pSubTSTail = NULL;
	this->TTS_num = 1;

	if(NULL == ret)
		return this;
	return ret;
}