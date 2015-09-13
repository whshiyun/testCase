#ifndef __PLAN_EMPTY_TS_H__
#define __PLAN_EMPTY_TS_H__

#define MAX_EMPTY_TS_NUM	128

typedef struct {
	unsigned int TSNum;
	unsigned char *pTTSNumInTS;
}PLAN_INPUT_UNIT, *P_PLAN_INPUT_UNIT;

typedef struct {
	unsigned int num;
	unsigned char emptyTSPos[MAX_EMPTY_TS_NUM];
	unsigned char emptyTSAttr[MAX_EMPTY_TS_NUM];
}PLAN_OUTPUT, *P_PLAN_OUTPUT;

#endif