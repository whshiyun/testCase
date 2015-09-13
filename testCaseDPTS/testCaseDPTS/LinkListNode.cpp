#include "StdAfx.h"
#include "LinkListNode.h"


LinkListNode::LinkListNode(void)
{
	this->pNext = this;
	this->pPre = this;
}


LinkListNode::~LinkListNode(void)
{
}
