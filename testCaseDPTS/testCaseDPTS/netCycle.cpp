#include "StdAfx.h"
#include "netCycle.h"

netCycle::netCycle(void)
{

	this->pHeadTS = new TimeSlot(1);
	this->pEndTS = this->pHeadTS;
	this->TSNum = 1;

	this->addOneTS();

	this->pBaseTS = this->pHeadTS;
	this->pShitfTS = this->pBaseTS->pNext;
	//this->pPreBaseTS = this->pBaseTS->pPre;
	this->stackPos = 0;
	this->push(this->pBaseTS->pPre);
}


netCycle::~netCycle(void)
{
}

TimeSlot *netCycle::pop() {
	if(0 == this->stackPos)
		return NULL;
	
	this->stackPos --;
	return this->pPreBaseTSBuf[this->stackPos];
}

bool netCycle::push(TimeSlot *pPreBaseTS) {
	if(this->stackPos >= MAX_STACK_DEPP) {
		printf("stack over flow !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\r\n");
		while(1);
		return false;
	}

	this->pPreBaseTSBuf[this->stackPos] = pPreBaseTS;
	this->stackPos ++;
}

TimeSlot *netCycle::stackGet() {

	if(0 == this->stackPos)
		return NULL;

	return this->pPreBaseTSBuf[this->stackPos - 1];
}

void netCycle::addOneTS() {
	TimeSlot *pTS = new TimeSlot(this->TSNum+1);
	pTS->add(this->pEndTS);
	this->pEndTS = pTS;
	this->TSNum ++;
}

void netCycle::resetNetCycle(){

	//this->pBaseTS = this->pHeadTS;
	//this->pShitfTS = this->pBaseTS->pNext;

	while(this->pEndTS->pPre != this->pHeadTS) {
		this->pEndTS = this->pEndTS->pPre;
		delete(this->pEndTS->pNext);
		this->TSNum --;
	}
	this->pEndTS->pNext = NULL;

	this->pBaseTS = this->pHeadTS;
	this->pBaseTS->mergeSubTS(1);
	if(NULL == this->pBaseTS->pNext)
		this->pEndTS = this->pBaseTS;
	this->pShitfTS = this->pBaseTS->pNext;
}

void netCycle::restore() {
	this->pShitfTS = this->pEndTS;
	while(NULL != this->pShitfTS) {
		this->pShitfTS->releaseSubTS();
		this->pShitfTS = this->pShitfTS->pPre;
	}
	this->pBaseTS = this->pHeadTS;
	this->pShitfTS = this->pBaseTS->pNext;
}

void netCycle::del(TimeSlot *pPre, TimeSlot *pNext) {
	if(NULL != pPre)
		pPre->pNext = pNext;
	if(NULL != pNext)
		pNext->pPre = pPre;
}

void netCycle::insert(TimeSlot *pPre, TimeSlot *pNext, TimeSlot *pTS) {

	if((NULL == pPre) && (NULL == pNext))
		return ;

	if((pPre == pTS) || (pNext == pTS))
		return ;

	if(pTS == this->pEndTS)
		this->pEndTS = pTS->pPre;
	if(pTS == this->pHeadTS)
		this->pHeadTS = pTS->pNext;

	if(pNext == this->pHeadTS)
		this->pHeadTS = pTS;
	if(pPre == this->pEndTS)
		this->pEndTS = pTS;

	this->del(pTS->pPre, pTS->pNext);
	/*
	if(NULL != pTS->pNext) {
		pTS->pNext->pPre = pTS->pPre;
	}

	if(NULL != pTS->pPre) {
		pTS->pPre->pNext = pTS->pNext;
	}
	*/
	if(NULL != pPre) {
		pPre->pNext = pTS;
	}

	if(NULL != pNext) {
		pNext->pPre = pTS;
	}

	pTS->pPre = pPre;
	pTS->pNext = pNext;
}

bool netCycle::shiftRightOneTTS(TimeSlot *pTS){

	if(NULL == pTS->pNext)
		return false;

	if(1 != pTS->pNext->TTS_num) {
		printf("pTS->pNext->TTS_num = %d \r\n", pTS->pNext->TTS_num);
		return false;
	}

	insert(pTS->pNext, pTS->pNext->pNext, pTS);

	return true;
}

bool netCycle::goNextSituation() {

	//if(NULL == this->pBaseTS)
		//return false;

	if(NULL == this->pShitfTS) {

		if(NULL == this->stackGet())
			this->insert(NULL, this->pHeadTS, this->pBaseTS);
		else
			this->insert(this->stackGet(), this->stackGet()->pNext, this->pBaseTS);

		if(true == this->pBaseTS->mergeSubTS(1)) {
			if(NULL == this->pBaseTS->pNext)
				this->pEndTS = this->pBaseTS;

			this->pShitfTS = this->pBaseTS->pNext;
			return true;
		} else {
			//this->pBaseTS->releaseSubTS();
			
			if(this->pBaseTS == this->pEndTS)
				this->pEndTS = this->pBaseTS->releaseSubTS();
			else
				this->pBaseTS->releaseSubTS();

			if(NULL == this->stackGet())
				return false;
			this->pBaseTS = this->stackGet();
			//this->stackGet()->pPre = this->stackGet();
			this->pop();
			if(true == this->shiftRightOneTTS(this->pBaseTS)) {
				this->pShitfTS = this->pBaseTS->pNext;
				return true;
			} else {
				return this->goNextSituation();
			}
		}
	}

	if(true == this->pShitfTS->mergeSubTS(1)) {
		if(NULL == this->pShitfTS->pNext)
			this->pEndTS = this->pShitfTS;

		this->push(this->pBaseTS);
		this->pBaseTS = this->pShitfTS;
		this->pShitfTS = this->pShitfTS->pNext;
	} else {
		this->push(this->pBaseTS);
		this->pBaseTS = this->pShitfTS;
		this->pShitfTS = this->pShitfTS->pNext;

		return this->goNextSituation();
	}

	return true;

}

void netCycle::printCycle() {

	//printf("TTSNum = %d \r\n", this->TSNum);

	TimeSlot *pTemp = this->pHeadTS;
	unsigned int i = 0, cnt = 0;
	while(NULL != pTemp) {
		cnt ++;
		for(i=0; i<pTemp->TTS_num; i++)
			printf("%02d ", cnt);

		pTemp = pTemp->pNext;
	}
	printf("\r\n");
}