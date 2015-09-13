#pragma once
class TimeSlot
{
public:
	TimeSlot(unsigned int id);
	~TimeSlot(void);

	unsigned int id;

	static unsigned int maxMergeNum;

	TimeSlot *pPre;
	TimeSlot *pNext;

	unsigned int TTS_num;
	TimeSlot *pSubTSHead;
	TimeSlot *pSubTSTail;

	void add(TimeSlot *pPre);
	bool mergeSubTS(unsigned int n);
	TimeSlot *releaseSubTS();
};

