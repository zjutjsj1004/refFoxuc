#include "StdAfx.h"
#include "GameLogic.h"

//////////////////////////////////////////////////////////////////////////
//宏定义

//索引定义
#define INDEX_OUT					0									//出牌索引
#define INDEX_HAND					1									//手牌索引
#define INDEX_FIRST					2									//首出索引

//////////////////////////////////////////////////////////////////////////
//静态变量

//常主数值
const BYTE	CGameLogic::m_cbNTValue=VALUE_ERROR;						//常主数值

//扑克数据
const BYTE	CGameLogic::m_cbCardData[CELL_PACK]=
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//方块 A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//梅花 A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//红桃 A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//黑桃 A - K
	0x4E,0x4F,
};

//////////////////////////////////////////////////////////////////////////

//构造函数
CGameLogic::CGameLogic()
{
	//属性变量
	m_cbPackCount=0;
	m_cbDispatchCount=0;

	//逻辑变量
	m_cbMainValue=VALUE_ERROR;
	m_cbMainColor=COLOR_ERROR;

	//辅助变量
	for (BYTE i=0;i<CountArray(m_cbSortRight);i++) m_cbSortRight[i]=i*COLOR_RIGHT;

	return;
}

//析构函数
CGameLogic::~CGameLogic()
{
}

//设置副数
bool CGameLogic::SetPackCount(BYTE cbPackCount)
{
	//效验参数
	ASSERT((cbPackCount>0)&&(cbPackCount<=PACK_COUNT));
	if ((cbPackCount==0)||(cbPackCount>PACK_COUNT)) return false;

	//副数数目
	m_cbPackCount=cbPackCount;

	//派发数目
	BYTE cbConcealCount=((m_cbPackCount%2)==0)?8:6;
	m_cbDispatchCount=((m_cbPackCount*CELL_PACK)-cbConcealCount)/4;

	return false;
}

//主牌花色
bool CGameLogic::SetMainColor(BYTE cbMainColor)
{
	//效验参数
	ASSERT((cbMainColor==COLOR_ERROR)||(cbMainColor<=COLOR_NT));
	if ((cbMainColor!=COLOR_ERROR)&&(cbMainColor>COLOR_NT)) return false;

	//设置变量
	m_cbMainColor=cbMainColor;

	//更新权位
	UpdateSortRight();

	return true;
}

//主牌数值
bool CGameLogic::SetMainValue(BYTE cbMainValue)
{
	//效验参数
	ASSERT((cbMainValue==VALUE_ERROR)||(cbMainValue<=0x0D));
	if ((cbMainValue!=VALUE_ERROR)&&(cbMainValue>0x0D)) return false;

	//设置变量
	m_cbMainValue=cbMainValue;

	//更新权位
	UpdateSortRight();

	return true;
}

//获取类型
BYTE CGameLogic::GetCardType(const BYTE cbCardData[], BYTE cbCardCount)
{
	//状态效验
	ASSERT(m_cbMainColor!=COLOR_ERROR);
	ASSERT(m_cbMainValue!=VALUE_ERROR);

	//数目判断
	ASSERT((cbCardCount>0)&&(cbCardCount<=MAX_COUNT));
	if ((cbCardCount==0)||(cbCardCount>MAX_COUNT)) return CT_ERROR;

	//同花判断
	if (GetCardLogicColor(cbCardData,cbCardCount)==COLOR_ERROR) return CT_ERROR;

	//同牌判断
	BYTE cbSameCount=1;
	for (BYTE i=1;i<cbCardCount;i++) 
	{
		if (cbCardData[i]==cbCardData[0]) cbSameCount++;
		else break;
	}
	if (cbSameCount==cbCardCount) return CT_SINGLE+cbSameCount-1;

	//拖牌判断
	if ((cbSameCount>=2)&&(cbCardCount%cbSameCount)==0)
	{
		//变量定义
		BYTE cbBlockCount=cbCardCount/cbSameCount;

		//单元判断
		for (BYTE i=1;i<cbBlockCount;i++)
		{
			//变量定义
			BYTE cbIndex=i*cbSameCount;
			BYTE cbFirstValue=GetCardValue(cbCardData[0]);

			//相同分析
			for (BYTE j=0;j<cbSameCount;j++)
			{
				if (cbCardData[cbIndex+j]!=cbCardData[cbIndex]) break;
			}
			if (j!=cbSameCount) break;

			//相连分析
			if (IsLineValue(cbCardData[cbIndex-cbSameCount],cbCardData[cbIndex])==false) break;
		}

		//成功判断
		if (i==cbBlockCount) return CT_TRACKOR_2+cbSameCount-2;
	}

	return CT_THROW_CARD;
}

//排列扑克
void CGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//转换数值
	BYTE cbSortValue[MAX_COUNT];
	for (BYTE i=0;i<cbCardCount;i++) cbSortValue[i]=GetCardSortOrder(cbCardData[i]);	

	//排序操作
	bool bSorted=true;
	BYTE bTempData,bLast=cbCardCount-1;
	do
	{
		bSorted=true;
		for (BYTE i=0;i<bLast;i++)
		{
			if ((cbSortValue[i]<cbSortValue[i+1])||
				((cbSortValue[i]==cbSortValue[i+1])&&(cbCardData[i]<cbCardData[i+1])))
			{
				//交换位置
				bTempData=cbCardData[i];
				cbCardData[i]=cbCardData[i+1];
				cbCardData[i+1]=bTempData;
				bTempData=cbSortValue[i];
				cbSortValue[i]=cbSortValue[i+1];
				cbSortValue[i+1]=bTempData;
				bSorted=false;
			}	
		}
		bLast--;
	} while(bSorted==false);

	return;
}

//混乱扑克
void CGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//混乱准备
	BYTE cbCardData[CELL_PACK*PACK_COUNT];
	for (BYTE i=0;i<PACK_COUNT;i++) CopyMemory(&cbCardData[CELL_PACK*i],m_cbCardData,sizeof(m_cbCardData));

	//混乱扑克
	BYTE cbRandCount=0,cbPosition=0;
	do
	{
		cbPosition=rand()%(cbBufferCount-cbRandCount);
		cbCardBuffer[cbRandCount++]=cbCardData[cbPosition];
		cbCardData[cbPosition]=cbCardData[cbBufferCount-cbRandCount];
	} while (cbRandCount<cbBufferCount);

	return;
}

//删除扑克
bool CGameLogic::RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount)
{
	//检验数据
	ASSERT(cbRemoveCount<=cbCardCount);

	//定义变量
	BYTE cbDeleteCount=0,cbTempCardData[MAX_COUNT];
	if (cbCardCount>CountArray(cbTempCardData)) return false;
	CopyMemory(cbTempCardData,cbCardData,cbCardCount*sizeof(cbCardData[0]));

	//置零扑克
	for (BYTE i=0;i<cbRemoveCount;i++)
	{
		for (BYTE j=0;j<cbCardCount;j++)
		{
			if (cbRemoveCard[i]==cbTempCardData[j])
			{
				cbDeleteCount++;
				cbTempCardData[j]=0;
				break;
			}
		}
	}
	if (cbDeleteCount!=cbRemoveCount) return false;

	//清理扑克
	BYTE cbCardPos=0;
	for (BYTE i=0;i<cbCardCount;i++)
	{
		if (cbTempCardData[i]!=0) cbCardData[cbCardPos++]=cbTempCardData[i];
	}

	return true;
}

//获取积分
WORD CGameLogic::GetCardScore(const BYTE cbCardData[], BYTE cbCardCount)
{
	//变量定义
	WORD wCardScore=0;

	//扑克累计
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//获取数值
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//累计积分
		if (cbCardValue==5) 
		{
			wCardScore+=5;
			continue;
		}

		//累计积分
		if ((cbCardValue==10)||(cbCardValue==13)) 
		{
			wCardScore+=10;
			continue;
		}
	}

	return wCardScore;
}

//获取积分
BYTE CGameLogic::GetScoreCard(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbScoreCard[], BYTE cbMaxCount)
{
	//变量定义
	BYTE cbPostion=0;

	//扑克累计
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//获取数值
		BYTE cbCardValue=GetCardValue(cbCardData[i]);

		//累计扑克
		if ((cbCardValue==5)||(cbCardValue==10)||(cbCardValue==13))
		{
			ASSERT(cbPostion<cbMaxCount);
			cbScoreCard[cbPostion++]=cbCardData[i];
		}
	}

	return cbPostion;
}

//效验甩牌
bool CGameLogic::EfficacyThrowCard(const BYTE cbOutCardData[], BYTE cbOutCardCount, WORD wOutCardIndex, const BYTE cbHandCardData[GAME_PLAYER][MAX_COUNT], BYTE cbHandCardCount, tagOutCardResult & OutCardResult)
{
	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalyseCardData(cbOutCardData,cbOutCardCount,AnalyseResult);

	//循环比较
	for (WORD i=0;i<GAME_PLAYER-1;i++)
	{
		//获取数据
		WORD wUserIndex=(wOutCardIndex+i+1)%GAME_PLAYER;

		//提取扑克
		BYTE cbDistillCardHand[MAX_COUNT];
		BYTE cbDistillCountHand=DistillCardByColor(cbHandCardData[wUserIndex],cbHandCardCount,AnalyseResult.cbCardColor,cbDistillCardHand);

		//判断过滤
		if (cbDistillCountHand==0) continue;

		//牌型判断
		for (BYTE j=PACK_COUNT;j>=2;j--)
		{
			//变量定义
			tagSameDataInfo * pSameDataInfo=&AnalyseResult.SameDataInfo[j-1];
			tagTractorDataInfo * pTractorDataInfo=&AnalyseResult.TractorDataInfo[j-2];

			//拖牌判断
			if (pTractorDataInfo->cbTractorCount>0)
			{
				//变量定义
				BYTE cbUserTractorIndex=0;

				//提取拖牌
				tagTractorDataInfo TractorDataInfo;
				DistillTrackorByCount(cbDistillCardHand,cbDistillCountHand,j,TractorDataInfo);

				//循环对比
				for (BYTE cbUserRound=0;cbUserRound<TractorDataInfo.cbTractorCount;cbUserRound++)
				{
					//变量定义
					BYTE cbOutTractorIndex=0;
					//获取属性
					BYTE cbLineCountUser=TractorDataInfo.cbTractorData[cbUserTractorIndex];
					BYTE cbSameCountUser=TractorDataInfo.cbTractorData[cbUserTractorIndex+1];

					//对比拖牌
					for (BYTE cbOutRound=0;cbOutRound<pTractorDataInfo->cbTractorCount;cbOutRound++)
					{
						//获取属性
						BYTE cbLineCountOut=pTractorDataInfo->cbTractorData[cbOutTractorIndex];
						BYTE cbSameCountOut=pTractorDataInfo->cbTractorData[cbOutTractorIndex+1];

						//拖牌对比
						if (cbLineCountUser>=cbLineCountOut)
						{
							//扑克定义
							BYTE cbNextCardData=TractorDataInfo.cbTractorData[cbUserTractorIndex+2];
							BYTE cbFisrtCardData=pTractorDataInfo->cbTractorData[cbOutTractorIndex+2];

							//对比大小
							if (CompareCardData(cbFisrtCardData,cbNextCardData)==true)
							{
								OutCardResult.cbCardCount=cbLineCountOut*cbSameCountOut;
								CopyMemory(OutCardResult.cbResultCard,&pTractorDataInfo->cbTractorData[cbOutTractorIndex+2],sizeof(BYTE)*OutCardResult.cbCardCount);

								return false;
							}
						}

						//设置索引
						cbOutTractorIndex+=(2+cbLineCountOut*cbSameCountOut);
					}

					//设置索引
					cbUserTractorIndex+=(2+cbLineCountUser*cbSameCountUser);
				}
			}

			//同牌判断
			if (pSameDataInfo->cbBlockCount>0)
			{
				//提取同牌
				tagSameDataInfo SameDataInfo;
				DistillCardByCount(cbDistillCardHand,cbDistillCountHand,j,SameDataInfo);

				//同牌判断
				if (SameDataInfo.cbBlockCount>0)
				{
					//扑克定义
					BYTE cbLessSameIndex=pSameDataInfo->cbCardCount-j;
					BYTE cbFisrtCardData=pSameDataInfo->cbSameData[cbLessSameIndex];

					//对比大小
					if (CompareCardData(cbFisrtCardData,SameDataInfo.cbSameData[0])==true)
					{
						OutCardResult.cbCardCount=j;
						CopyMemory(OutCardResult.cbResultCard,&pSameDataInfo->cbSameData[cbLessSameIndex],sizeof(BYTE)*OutCardResult.cbCardCount);

						return false;
					}
				}
			}
		}

		//单牌判断
		if (AnalyseResult.SameDataInfo[0].cbCardCount!=0)
		{
			//变量定义
			BYTE cbCardCount=AnalyseResult.SameDataInfo[0].cbCardCount;
			BYTE cbLessCardData=AnalyseResult.SameDataInfo[0].cbSameData[cbCardCount-1];

			//对比扑克
			if (CompareCardData(cbLessCardData,cbDistillCardHand[0])==true)
			{
				OutCardResult.cbCardCount=1;
				OutCardResult.cbResultCard[0]=cbLessCardData;

				return false;
			}
		}
	}

	return true;
}

//对比扑克
bool CGameLogic::CompareCardData(BYTE cbFirstCardData, BYTE cbNextCardData)
{
	//获取花色
	BYTE cbLogicColorNext=GetCardLogicColor(cbNextCardData);
	BYTE cbLogicColorFirst=GetCardLogicColor(cbFirstCardData);

	//对比大小
	if ((cbLogicColorNext==COLOR_NT)||(cbLogicColorNext==cbLogicColorFirst))
	{
		//获取大小
		BYTE cbLogicValueNext=GetCardLogicValue(cbNextCardData);
		BYTE cbLogicValueFirst=GetCardLogicValue(cbFirstCardData);

		//大小判断
		if (cbLogicValueNext>cbLogicValueFirst) return true;
	}

	return false;
}

//对比扑克
bool CGameLogic::CompareCardResult(const tagAnalyseResult WinnerResult, const tagAnalyseResult UserResult)
{
	//对比拖牌
	for (BYTE i=PACK_COUNT;i>=2;i--)
	{
		//变量定义
		const tagTractorDataInfo * pTractorDataInfoUser=&UserResult.TractorDataInfo[i-2];
		const tagTractorDataInfo * pTractorDataInfoWinner=&WinnerResult.TractorDataInfo[i-2];

		//对比拖牌
		if ((pTractorDataInfoWinner->cbTractorCount>0)&&(pTractorDataInfoUser->cbTractorCount>0))
		{
			//获取大小
			BYTE cbNextCardData=pTractorDataInfoUser->cbTractorData[2];
			BYTE cbFisrtCardData=pTractorDataInfoWinner->cbTractorData[2];

			//对比大小
			if (CompareCardData(cbFisrtCardData,cbNextCardData)==true) return true;
		}
	}

	//对比同牌
	for (BYTE i=PACK_COUNT;i>=1;i--)
	{
		//变量定义
		const tagSameDataInfo * pSameDataInfoUser=&UserResult.SameDataInfo[i-1];
		const tagSameDataInfo * pSameDataInfoWinner=&WinnerResult.SameDataInfo[i-1];

		//对比同牌
		if ((pSameDataInfoWinner->cbCardCount>0)&&(pSameDataInfoUser->cbCardCount>0))
		{
			//获取大小
			BYTE cbNextCardData=pSameDataInfoUser->cbSameData[0];
			BYTE cbFisrtCardData=pSameDataInfoWinner->cbSameData[0];

			//对比大小
			if (CompareCardData(cbFisrtCardData,cbNextCardData)==true) return true;
		}
	}

	return false;
}

//对比扑克
WORD CGameLogic::CompareCardArray(const BYTE cbOutCardData[GAME_PLAYER][MAX_COUNT], BYTE cbCardCount, WORD wFirstIndex)
{
	//变量定义
	WORD wWinnerIndex=wFirstIndex;
	BYTE cbCardType=GetCardType(cbOutCardData[wFirstIndex],cbCardCount);

	//对比扑克
	switch (cbCardType)
	{
	case CT_SINGLE:		//单牌类型
	case CT_SAME_2:		//对牌类型
	case CT_SAME_3:		//三牌类型
	case CT_SAME_4:		//四牌类型
	case CT_TRACKOR_2:	//拖拉机型
	case CT_TRACKOR_3:	//拖拉机型
	case CT_TRACKOR_4:	//拖拉机型
		{
			//循环比较
			for (WORD i=0;i<GAME_PLAYER-1;i++)
			{
				//用户索引
				WORD wUserIndex=(wFirstIndex+i+1)%GAME_PLAYER;

				//牌型判断
				if (GetCardType(cbOutCardData[wUserIndex],cbCardCount)!=cbCardType) continue;

				//对比大小
				if (CompareCardData(cbOutCardData[wWinnerIndex][0],cbOutCardData[wUserIndex][0])==true) 
				{
					wWinnerIndex=wUserIndex;
					continue;
				}
			}

			return wWinnerIndex;
		}
	case CT_THROW_CARD:	//甩牌类型
		{
			//花色判断
			if (GetCardLogicColor(cbOutCardData[wWinnerIndex][0])==COLOR_NT) return wFirstIndex;

			//分析牌型
			tagAnalyseResult WinnerResult;
			AnalyseCardData(cbOutCardData[wFirstIndex],cbCardCount,WinnerResult);

			//循环比较
			for (WORD i=0;i<GAME_PLAYER-1;i++)
			{
				//规则判断
				WORD wUserIndex=(wFirstIndex+i+1)%GAME_PLAYER;
				if (GetCardLogicColor(cbOutCardData[wUserIndex],cbCardCount)!=COLOR_NT) continue;

				//构造牌型
				tagAnalyseResult UserResult;
				if (RectifyCardWeave(cbOutCardData[wUserIndex],cbCardCount,WinnerResult,UserResult)==false) continue;

				//对比结果
				if (CompareCardResult(WinnerResult,UserResult)==true)
				{
					wWinnerIndex=wUserIndex;
					CopyMemory(&WinnerResult,&UserResult,sizeof(WinnerResult));
				}
			}

			return wWinnerIndex;
		}
	}

	//错误断言
	ASSERT(FALSE);

	return wFirstIndex;
}

//分析扑克
bool CGameLogic::AnalyseCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult)
{
	//设置结果
	ZeroMemory(&AnalyseResult,sizeof(AnalyseResult));

	//花色判断
	AnalyseResult.cbCardColor=GetCardLogicColor(cbCardData,cbCardCount);
	if (AnalyseResult.cbCardColor==COLOR_ERROR) return false;

	//变量定义
	BYTE cbAnalyseCard[MAX_COUNT];
	BYTE cbAnalyseCount=cbCardCount;
	CopyMemory(cbAnalyseCard,cbCardData,cbCardCount*sizeof(BYTE));

	//提取扑克
	for (BYTE i=PACK_COUNT;i>=2;i--)
	{
		//提取拖牌
		tagTractorDataInfo * pTractorDataInfo=&AnalyseResult.TractorDataInfo[i-2];
		DistillTrackorByCount(cbAnalyseCard,cbAnalyseCount,i,AnalyseResult.TractorDataInfo[i-2]);

		//拖牌处理
		if (pTractorDataInfo->cbTractorCount>0)
		{
			BYTE cbIndex=0,cbRemoveCount=0;
			for (BYTE j=0;j<pTractorDataInfo->cbTractorCount;j++)
			{
				//变量计算
				cbRemoveCount=pTractorDataInfo->cbTractorData[cbIndex]*i;

				//删除扑克
				cbAnalyseCount-=cbRemoveCount;
				RemoveCard(&pTractorDataInfo->cbTractorData[cbIndex+2],cbRemoveCount,cbAnalyseCard,cbAnalyseCount+cbRemoveCount);

				//设置变量
				cbIndex+=cbRemoveCount+2;
			}
		}

		//提取同牌
		tagSameDataInfo * pSameDataInfo=&AnalyseResult.SameDataInfo[i-1];
		DistillCardByCount(cbAnalyseCard,cbAnalyseCount,i,AnalyseResult.SameDataInfo[i-1]);

		//删除扑克
		if (pSameDataInfo->cbBlockCount>0)
		{
			cbAnalyseCount-=pSameDataInfo->cbCardCount;
			RemoveCard(pSameDataInfo->cbSameData,pSameDataInfo->cbCardCount,cbAnalyseCard,cbAnalyseCount+pSameDataInfo->cbCardCount);
		}
	}

	//提取单牌
	AnalyseResult.SameDataInfo[0].cbCardCount=cbAnalyseCount;
	AnalyseResult.SameDataInfo[0].cbBlockCount=cbAnalyseCount;
	CopyMemory(AnalyseResult.SameDataInfo[0].cbSameData,cbAnalyseCard,cbAnalyseCount*sizeof(BYTE));

	return true;
}

//出牌判断
bool CGameLogic::SearchOutCard(const BYTE cbHandCardData[], BYTE cbHandCardCount,const  BYTE cbTurnCardData[], BYTE cbTurnCardCount, tagOutCardResult & OutCardResult)
{
	//设置变量
	ZeroMemory(&OutCardResult,sizeof(OutCardResult));

	//首出用户
	if (cbTurnCardCount==0)
	{
		for (BYTE i=PACK_COUNT;i>=2;i--)
		{
			//提取拖牌
			tagTractorDataInfo TractorDataInfo;
			DistillTrackorByCount(cbHandCardData,cbHandCardCount,i,TractorDataInfo);

			//拖牌判断
			if (TractorDataInfo.cbTractorCount>0)
			{
				//获取属性
				BYTE cbLineCount=TractorDataInfo.cbTractorData[0];
				BYTE cbSameCount=TractorDataInfo.cbTractorData[1];
				
				//构造结果
				OutCardResult.cbCardCount=cbLineCount*cbSameCount;
				CopyMemory(OutCardResult.cbResultCard,&TractorDataInfo.cbTractorData[2],sizeof(BYTE)*OutCardResult.cbCardCount);

				return true;
			}

			//提取同牌
			tagSameDataInfo SameDataInfo;
			DistillCardByCount(cbHandCardData,cbHandCardCount,i,SameDataInfo);

			//同牌判断
			if (SameDataInfo.cbCardCount>0)
			{
				//构造结果
				OutCardResult.cbCardCount=i;
				CopyMemory(OutCardResult.cbResultCard,&SameDataInfo.cbSameData[0],sizeof(BYTE)*i);

				return true;
			}
		}

		//提取单牌
		OutCardResult.cbCardCount=1;
		OutCardResult.cbResultCard[0]=cbHandCardData[0];

		return true;
	}

	//分析扑克
	tagAnalyseResult AnalyseResult;
	AnalyseCardData(cbTurnCardData,cbTurnCardCount,AnalyseResult);

	//提取扑克
	BYTE cbAnalyseCard[MAX_COUNT];
	BYTE cbAnalyseCardCount=DistillCardByColor(cbHandCardData,cbHandCardCount,GetCardLogicColor(cbTurnCardData[0]),cbAnalyseCard);

	//数目判断
	if (cbTurnCardCount>=cbAnalyseCardCount)
	{
		//构造扑克
		OutCardResult.cbCardCount=cbAnalyseCardCount;
		CopyMemory(OutCardResult.cbResultCard,cbAnalyseCard,sizeof(BYTE)*cbAnalyseCardCount);

		//剩余扑克
		if (cbTurnCardCount>cbAnalyseCardCount)
		{
			//变量定义
			BYTE cbSourceCount=OutCardResult.cbCardCount;
			BYTE cbLeaveCount=(cbTurnCardCount-cbAnalyseCardCount);

			//构造扑克
			BYTE cbCardDataTemp[MAX_COUNT];
			BYTE cbCardCountTemp=cbHandCardCount;
			CopyMemory(cbCardDataTemp,cbHandCardData,sizeof(BYTE)*cbHandCardCount);

			//删除扑克
			cbCardCountTemp-=cbAnalyseCardCount;
			RemoveCard(cbAnalyseCard,cbAnalyseCardCount,cbCardDataTemp,cbCardCountTemp+cbAnalyseCardCount);

			//构造结果
			OutCardResult.cbCardCount+=cbLeaveCount;
			CopyMemory(&OutCardResult.cbResultCard[cbSourceCount],&cbCardDataTemp[cbCardCountTemp-cbLeaveCount],sizeof(BYTE)*cbLeaveCount);
		}

		return true;
	}

	//需求变量
	tagDemandInfo DemandInfo;
	ZeroMemory(&DemandInfo,sizeof(DemandInfo));

	//需求分析
	for (BYTE i=PACK_COUNT;i>=2;i--)
	{
		//变量定义
		tagSameDataInfo * pSameDataInfo=&AnalyseResult.SameDataInfo[i-1];
		tagTractorDataInfo * pTractorDataInfo=&AnalyseResult.TractorDataInfo[i-2];

		//拖牌需求
		if (pTractorDataInfo->cbTractorCount>0)
		{
			DemandInfo.cbTractorCardCount[i-2]=pTractorDataInfo->cbCardCount;
			DemandInfo.cbMaxTractor[i-2]=pTractorDataInfo->cbTractorMaxLength;
		}

		//同牌需求
		if (pSameDataInfo->cbBlockCount>0) DemandInfo.cbSameCardCount[i-1]=pSameDataInfo->cbCardCount;
	}

	//单牌需求
	if (AnalyseResult.SameDataInfo[0].cbCardCount>0) DemandInfo.cbSameCardCount[0]=AnalyseResult.SameDataInfo[0].cbCardCount;

	//出牌搜索
	for (BYTE i=PACK_COUNT;i>=2;i--)
	{
		//拖牌判断
		if (DemandInfo.cbTractorCardCount[i-2]>0)
		{
			//提取拖牌
			tagTractorDataInfo TractorDataInfo;
			DistillTrackorByCount(cbAnalyseCard,cbAnalyseCardCount,i,TractorDataInfo);

			//变量定义
			BYTE cbIndex=0;
			BYTE cbLeaveCount=DemandInfo.cbTractorCardCount[i-2];

			//提取扑克
			for (BYTE j=0;j<TractorDataInfo.cbTractorCount;j++)
			{
				//调整索引
				if (j==0) cbIndex=TractorDataInfo.cbTractorMaxIndex;
				if ((j==1)&&(TractorDataInfo.cbTractorMaxIndex!=0)) cbIndex=0;

				//调整索引
				if ((j!=0)&&(cbIndex==TractorDataInfo.cbTractorMaxIndex))
				{
					//获取属性
					BYTE cbLineCount=TractorDataInfo.cbTractorData[cbIndex];
					BYTE cbSameCount=TractorDataInfo.cbTractorData[cbIndex+1];

					//设置索引
					cbIndex+=cbLineCount*cbSameCount+2;
				}

				//获取属性
				BYTE cbLineCount=TractorDataInfo.cbTractorData[cbIndex];
				BYTE cbSameCount=TractorDataInfo.cbTractorData[cbIndex+1];

				//提取变量
				BYTE cbTractorCardCount=cbLineCount*cbSameCount;
				BYTE cbTakeCardCount=__min(cbTractorCardCount,cbLeaveCount);

				//提取扑克
				OutCardResult.cbCardCount+=cbTakeCardCount;
				CopyMemory(&OutCardResult.cbResultCard[OutCardResult.cbCardCount-cbTakeCardCount],
					&TractorDataInfo.cbTractorData[cbIndex+2+cbTractorCardCount-cbTakeCardCount],sizeof(BYTE)*cbTakeCardCount);

				//删除扑克
				cbLeaveCount-=cbTakeCardCount;
				cbAnalyseCardCount-=cbTakeCardCount;
				RemoveCard(&TractorDataInfo.cbTractorData[cbIndex+2+cbTractorCardCount-cbTakeCardCount],cbTakeCardCount,
					cbAnalyseCard,cbAnalyseCardCount+cbTakeCardCount);

				//设置索引
				cbIndex+=cbLineCount*cbSameCount+2;
			}

			//设置需求
			DemandInfo.cbSameCardCount[i-1]+=cbLeaveCount;
		}

		//同牌判断
		BYTE cbTakeCardCount = 0;
		if (DemandInfo.cbSameCardCount[i-1]>=i-1)
		{
			//提取同牌
			tagSameDataInfo SameDataInfo;
			DistillCardByCount(cbAnalyseCard,cbAnalyseCardCount,i,SameDataInfo);

			//提取分析
			cbTakeCardCount=__min(SameDataInfo.cbCardCount,DemandInfo.cbSameCardCount[i-1]);
			cbTakeCardCount=cbTakeCardCount-cbTakeCardCount%i;

			//构造结果
			OutCardResult.cbCardCount+=cbTakeCardCount;
			CopyMemory(&OutCardResult.cbResultCard[OutCardResult.cbCardCount-cbTakeCardCount],
				&SameDataInfo.cbSameData[SameDataInfo.cbCardCount-cbTakeCardCount],sizeof(BYTE)*cbTakeCardCount);

			//删除扑克
			cbAnalyseCardCount-=cbTakeCardCount;
			RemoveCard(&SameDataInfo.cbSameData[SameDataInfo.cbCardCount-cbTakeCardCount],cbTakeCardCount,cbAnalyseCard,cbAnalyseCardCount+cbTakeCardCount);
		}			
		//剩余数目
		BYTE cbLeaveCount = DemandInfo.cbSameCardCount[i-1]-cbTakeCardCount;
		//设置需求
		if( cbLeaveCount >= (i-1)*2 && i >= 3 )
			DemandInfo.cbTractorCardCount[i-3] += cbLeaveCount;
		else DemandInfo.cbSameCardCount[i-2]+=cbLeaveCount;
	}

	//提取单牌
	if (DemandInfo.cbSameCardCount[0]>0)
	{
		//变量定义
		BYTE cbSourceCount=OutCardResult.cbCardCount;
		BYTE cbLeaveCount=DemandInfo.cbSameCardCount[0];

		//构造结果
		OutCardResult.cbCardCount+=cbLeaveCount;
		CopyMemory(&OutCardResult.cbResultCard[cbSourceCount],&cbAnalyseCard[cbAnalyseCardCount-cbLeaveCount],sizeof(BYTE)*cbLeaveCount);
	}

	return true;
}

//效验出牌
bool CGameLogic::EfficacyOutCard(const BYTE cbOutCardData[], BYTE cbOutCardCount, const BYTE cbFirstCardData[], BYTE cbFirstCardCount, const BYTE cbHandCardData[], BYTE cbHandCardCount)
{
	//状态效验
	ASSERT(m_cbMainColor!=COLOR_ERROR);
	ASSERT(m_cbMainValue!=VALUE_ERROR);

	//数目判断
	if (cbOutCardCount!=cbFirstCardCount) return false;

	//扑克类型
	BYTE cbOutCardType=GetCardType(cbOutCardData,cbOutCardCount);
	BYTE cbFirstCardType=GetCardType(cbFirstCardData,cbFirstCardCount);

	//逻辑花色
	BYTE cbOutLogicColor=GetCardLogicColor(cbOutCardData,cbOutCardCount);
	BYTE cbFirstLogicColor=GetCardLogicColor(cbFirstCardData,cbFirstCardCount);

	//同型判断
	ASSERT(cbFirstCardType!=CT_ERROR);
	if ((cbFirstCardType!=CT_THROW_CARD)&&(cbOutLogicColor==cbFirstLogicColor)&&(cbOutCardType==cbFirstCardType)) return true;

	//提取扑克
	BYTE cbDistillCardOut[MAX_COUNT];
	BYTE cbDistillCardHand[MAX_COUNT];
	BYTE cbDistillCountOut=DistillCardByColor(cbOutCardData,cbOutCardCount,cbFirstLogicColor,cbDistillCardOut);
	BYTE cbDistillCountHand=DistillCardByColor(cbHandCardData,cbHandCardCount,cbFirstLogicColor,cbDistillCardHand);

	//花色判断
	if (cbFirstLogicColor!=cbOutLogicColor) return (cbDistillCountOut==cbDistillCountHand);

	//变量定义
	tagDemandInfo DemandInfo;
	ZeroMemory(&DemandInfo,sizeof(DemandInfo));
	tagAnalyseResult AnalyseResultFirst;
	AnalyseCardData(cbFirstCardData,cbFirstCardCount,AnalyseResultFirst);
	//分析需求
	for( BYTE i = PACK_COUNT; i >= 2; i-- )
	{
		//拖牌
		if( AnalyseResultFirst.TractorDataInfo[i-2].cbCardCount > 0 )
		{
			DemandInfo.cbMaxTractor[i-2] = AnalyseResultFirst.TractorDataInfo[i-2].cbTractorMaxLength;
			DemandInfo.cbTractorCardCount[i-2] = AnalyseResultFirst.TractorDataInfo[i-2].cbCardCount;
		}
		//相同牌
		if( AnalyseResultFirst.SameDataInfo[i-1].cbBlockCount > 0 )
			DemandInfo.cbSameCardCount[i-1] = AnalyseResultFirst.SameDataInfo[i-1].cbCardCount;
	}

	//匹配牌型
	tagTractorDataInfo TractorInfoOut,TractorInfoHand;
	tagSameDataInfo	SameInfoOut,SameInfoHand;
	BYTE cbRemoveCount,cbRemoveCard[MAX_COUNT];
	for( i = PACK_COUNT; i >= 2; i-- )
	{
		//拖牌
		if( DemandInfo.cbTractorCardCount[i-2] > 0 )
		{
			BYTE cbNeedCount = DemandInfo.cbTractorCardCount[i-2];
			//寻找匹配
			for( BYTE j = i; j >= 2; j-- )
			{
				//搜索拖牌
				if( cbNeedCount >= j*2 && cbDistillCountOut >= j*2 )
				{
					DistillTrackorByCount(cbDistillCardHand,cbDistillCountHand,j,TractorInfoHand);
					DistillTrackorByCount(cbDistillCardOut,cbDistillCountOut,j,TractorInfoOut);
					//提取出牌
					cbRemoveCount = 0;
					BYTE cbIndex = 0;
					if( TractorInfoHand.cbCardCount > TractorInfoOut.cbCardCount )
					{
						//数目判断
						if( cbNeedCount > TractorInfoOut.cbCardCount+j ) return false;
						else if( cbNeedCount == TractorInfoOut.cbCardCount + j &&
							TractorInfoHand.cbTractorMaxLength*j >= cbNeedCount )
							return false;
					}
					//提取匹配牌
					BYTE cbTempNeedCount = cbNeedCount-cbNeedCount%j;
					for( BYTE k = 0; k < TractorInfoOut.cbTractorCount; k++ )
					{
						BYTE cbLineCount = TractorInfoOut.cbTractorData[cbIndex];
						BYTE cbSameCount = TractorInfoOut.cbTractorData[cbIndex+1];
						BYTE cbCopyCount = __min(cbTempNeedCount,cbLineCount*cbSameCount);
						CopyMemory(&cbRemoveCard[cbRemoveCount],&TractorInfoOut.cbTractorData[cbIndex+2],
							sizeof(BYTE)*cbCopyCount);
						cbRemoveCount += cbCopyCount;
						cbTempNeedCount -= cbCopyCount;
						if( cbTempNeedCount == 0 ) break;
					}
					//匹配数目
					cbNeedCount -= cbRemoveCount;
					//删除匹配到的拖牌,包括出牌和手上牌
					VERIFY( RemoveCard(cbRemoveCard,cbRemoveCount,cbDistillCardOut,cbDistillCountOut) );
					cbDistillCountOut -= cbRemoveCount;
					VERIFY( RemoveCard(cbRemoveCard,cbRemoveCount,cbDistillCardHand,cbDistillCountHand) );
					cbDistillCountHand -= cbRemoveCount;
				}

				//搜索相同牌
				if( cbNeedCount >= j && cbDistillCountOut >= j )
				{
					//提取同牌
					DistillCardByCount(cbDistillCardHand,cbDistillCountHand,j,SameInfoHand);
					DistillCardByCount(cbDistillCardOut,cbDistillCountOut,j,SameInfoOut);
					//效验
					if( cbNeedCount/j > SameInfoOut.cbBlockCount && 
						SameInfoHand.cbBlockCount > SameInfoOut.cbBlockCount )
						return false;
					//匹配数目
					cbNeedCount -= SameInfoOut.cbCardCount;
					//删除匹配到的相同牌,包括出牌和手上牌
					VERIFY( RemoveCard(SameInfoOut.cbSameData,SameInfoOut.cbCardCount,
						cbDistillCardOut,cbDistillCountOut) );
					cbDistillCountOut -= SameInfoOut.cbCardCount;
					VERIFY( RemoveCard(SameInfoOut.cbSameData,SameInfoOut.cbCardCount,
						cbDistillCardHand,cbDistillCountHand) );
					cbDistillCountHand -= SameInfoOut.cbCardCount;
				}
			}
		}
		//相同牌
		if( DemandInfo.cbSameCardCount[i-1] > 0 )
		{
			BYTE cbNeedCount = DemandInfo.cbSameCardCount[i-1];
			//寻找匹配
			for( BYTE j = i; j >= 2; j-- )
			{
				//搜索相同牌
				if( cbNeedCount >= j && cbDistillCountOut >= j )
				{
					//提取同牌
					DistillCardByCount(cbDistillCardHand,cbDistillCountHand,j,SameInfoHand);
					DistillCardByCount(cbDistillCardOut,cbDistillCountOut,j,SameInfoOut);
					//效验
					if( cbNeedCount/j > SameInfoOut.cbBlockCount && 
						SameInfoHand.cbBlockCount > SameInfoOut.cbBlockCount )
						return false;
					//匹配数目
					cbNeedCount -= SameInfoOut.cbCardCount;
					//删除匹配到的相同牌,包括出牌和手上牌
					VERIFY( RemoveCard(SameInfoOut.cbSameData,SameInfoOut.cbCardCount,
						cbDistillCardOut,cbDistillCountOut) );
					cbDistillCountOut -= SameInfoOut.cbCardCount;
					VERIFY( RemoveCard(SameInfoOut.cbSameData,SameInfoOut.cbCardCount,
						cbDistillCardHand,cbDistillCountHand) );
					cbDistillCountHand -= SameInfoOut.cbCardCount;
				}
				//搜索拖牌
				if( j == 2 ) continue;
				if( cbNeedCount >= (j-1)*2 && cbDistillCountOut >= (j-1)*2 )
				{
					DistillTrackorByCount(cbDistillCardHand,cbDistillCountHand,(j-1),TractorInfoHand);
					DistillTrackorByCount(cbDistillCardOut,cbDistillCountOut,(j-1),TractorInfoOut);
					//提取出牌
					cbRemoveCount = 0;
					BYTE cbIndex = 0;
					if( TractorInfoHand.cbCardCount > TractorInfoOut.cbCardCount )
					{
						//数目判断
						if( cbNeedCount > TractorInfoOut.cbCardCount+(j-1) ) return false;
						else if( cbNeedCount == TractorInfoOut.cbCardCount + j - 1 && 
							TractorInfoHand.cbTractorMaxLength*(j-1) >= cbNeedCount )
							return false;
					}
					//提取匹配牌
					BYTE cbTempNeedCount = cbNeedCount-cbNeedCount%j;
					for( BYTE k = 0; k < TractorInfoOut.cbTractorCount; k++ )
					{
						BYTE cbLineCount = TractorInfoOut.cbTractorData[cbIndex];
						BYTE cbSameCount = TractorInfoOut.cbTractorData[cbIndex+1];
						BYTE cbCopyCount = __min(cbTempNeedCount,cbLineCount*cbSameCount);
						CopyMemory(&cbRemoveCard[cbRemoveCount],&TractorInfoOut.cbTractorData[cbIndex+2],
							sizeof(BYTE)*cbCopyCount);
						cbRemoveCount += cbCopyCount;
						cbTempNeedCount -= cbCopyCount;
						if( cbTempNeedCount == 0 ) break;
					}
					//匹配数目
					cbNeedCount -= cbRemoveCount;
					//删除匹配到的拖牌,包括出牌和手上牌
					VERIFY( RemoveCard(cbRemoveCard,cbRemoveCount,cbDistillCardOut,cbDistillCountOut) );
					cbDistillCountOut -= cbRemoveCount;
					VERIFY( RemoveCard(cbRemoveCard,cbRemoveCount,cbDistillCardHand,cbDistillCountHand) );
					cbDistillCountHand -= cbRemoveCount;
				}
			}
		}
	}
	
	return true;
}

//有效判断
bool CGameLogic::IsValidCard(BYTE cbCardData)
{
	//获取属性
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//有效判断
	if ((cbCardData==0x4E)||(cbCardData==0x4F)) return true;
	if ((cbCardColor<=0x30)&&(cbCardValue>=0x01)&&(cbCardValue<=0x0D)) return true;

	return false;
}

//排序等级
BYTE CGameLogic::GetCardSortOrder(BYTE cbCardData)
{
	//逻辑数值
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//鬼牌过滤
	if ((cbCardData==0x4E)||(cbCardData==0x4F))
	{
		BYTE cbSortValue=cbCardValue+11;
		return cbSortValue+m_cbSortRight[4];
	}

	//常主过滤
	if (cbCardValue==m_cbNTValue)
	{
		BYTE cbSortValue=(cbCardColor>>4)+15;
		if (cbCardColor==m_cbMainColor) cbSortValue=19;
		return cbSortValue+m_cbSortRight[4];
	}

	//花色变换
	BYTE cbSortColor=cbCardColor;
	if ((cbCardColor==m_cbMainColor)||(cbCardValue==m_cbMainValue)) cbSortColor=0x40;

	//数值变换
	BYTE cbSortValue=cbCardValue;
	if (cbCardValue==1) cbSortValue+=13;

	//主牌变换
	if (cbCardValue==m_cbMainValue)
	{
		if (cbCardColor==m_cbMainColor) cbSortValue=24;
		else cbSortValue=(cbCardColor>>4)+20;
	}

	return cbSortValue+m_cbSortRight[cbSortColor>>4];
}

//逻辑数值
BYTE CGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//逻辑数值
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);
	BYTE cbNTRight=((m_cbNTValue==VALUE_ERROR)||(m_cbNTValue==m_cbMainValue))?0:2;

	//鬼牌过滤
	if ((cbCardData==0x4E)||(cbCardData==0x4F))
	{
		BYTE cbLogicValue=cbCardValue+3+cbNTRight;
		BYTE cbDecreaseOrder=(m_cbMainColor==COLOR_NT)?1:0;
		return cbLogicValue+m_cbSortRight[4]-cbDecreaseOrder;
	}

	//常主过滤
	if (cbCardValue==m_cbNTValue)
	{
		BYTE cbLogicValue=(cbCardColor==m_cbMainColor)?16:15;
		return cbLogicValue+m_cbSortRight[4];
	}

	//花色变换
	BYTE cbLogicColor=cbCardColor;
	if ((cbCardColor==m_cbMainColor)||(cbCardValue==m_cbMainValue)) cbLogicColor=0x40;

	//转换变换
	BYTE cbLogicValue=cbCardValue;
	if (cbCardValue==1) cbLogicValue+=13;

	//主牌变换
	if (cbCardValue==m_cbMainValue)
	{
		if (cbCardColor==m_cbMainColor) cbLogicValue=16+cbNTRight;
		else cbLogicValue=15+cbNTRight;
	}

	//紧凑处理
	if ((cbCardValue!=1)&&(cbCardValue!=m_cbMainValue)&&(cbCardValue!=m_cbNTValue))
	{
		if (cbCardValue<m_cbMainValue) cbLogicValue++;
		if ((m_cbNTValue!=VALUE_ERROR)&&(m_cbNTValue!=m_cbMainValue)&&(cbCardValue<m_cbNTValue)) cbLogicValue++;
	}

	return cbLogicValue+m_cbSortRight[cbLogicColor>>4];
}

//逻辑花色
BYTE CGameLogic::GetCardLogicColor(BYTE cbCardData)
{
	//逻辑数值
	BYTE cbCardColor=GetCardColor(cbCardData);
	BYTE cbCardValue=GetCardValue(cbCardData);

	//花色判断
	if (cbCardColor==COLOR_NT) return COLOR_NT;
	if (cbCardColor==m_cbMainColor) return COLOR_NT;

	//数值判断
	if (cbCardValue==m_cbNTValue) return COLOR_NT;
	if (cbCardValue==m_cbMainValue) return COLOR_NT;

	return cbCardColor;
}

//牌列花色
BYTE CGameLogic::GetCardLogicColor(const BYTE cbCardData[], BYTE cbCardCount)
{
	//获取花色
	BYTE cbFirstColor=GetCardLogicColor(cbCardData[0]);
	BYTE cbSecondColor=GetCardLogicColor(cbCardData[cbCardCount-1]);

	return (cbFirstColor==cbSecondColor)?cbFirstColor:COLOR_ERROR;
}

//是否相连
bool CGameLogic::IsLineValue(BYTE cbFirstCard, BYTE cbSecondCard)
{
	//获取数值
	BYTE cbFirstLogicValue=GetCardLogicValue(cbFirstCard);
	BYTE cbSecondLogicValue=GetCardLogicValue(cbSecondCard);

	//相连判断
	return (cbFirstLogicValue==(cbSecondLogicValue+1));
}

//是否同花
bool CGameLogic::IsSameColor(BYTE cbFirstCard, BYTE cbSecondCard)
{
	//获取花色
	BYTE cbFirstLogicColor=GetCardLogicColor(cbFirstCard);
	BYTE cbSecondLogicColor=GetCardLogicColor(cbSecondCard);

	//同花判断
	return (cbFirstLogicColor==cbSecondLogicColor);
}

//提取扑克
BYTE CGameLogic::DistillCardByColor(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardColor, BYTE cbResultCard[])
{
	//变量定义
	BYTE cbResultCount=0;

	//提取扑克
	for (BYTE i=0;i<cbCardCount;i++)
	{
		//花色判断
		if (GetCardLogicColor(cbCardData[i])==cbCardColor) 
		{
			cbResultCard[cbResultCount++]=cbCardData[i];
			continue;
		}

		//终止判断
		if (cbResultCount>0) break;
	}

	return cbResultCount;
}

//提取扑克
BYTE CGameLogic::DistillCardByCount(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbSameCount, tagSameDataInfo & SameDataInfo)
{
	//参数效验
	ASSERT((cbSameCount>=2)&&(cbSameCount<=PACK_COUNT));
	if ((cbSameCount<2)||(cbSameCount>PACK_COUNT)) return 0;

	//设置变量
	ZeroMemory(&SameDataInfo,sizeof(SameDataInfo));

	//提取判断
	if (cbCardCount<cbSameCount) return 0;

	//提取扑克
	for (BYTE i=0;i<cbCardCount-cbSameCount+1;i++)
	{
		//数目判断
		for (BYTE j=1;j<cbSameCount;j++)
		{
			if (cbCardData[i]!=cbCardData[i+j]) break;
		}

		//成功处理
		if (j==cbSameCount)
		{
			//同牌属性
			SameDataInfo.cbBlockCount++;

			//扑克数据
			for (BYTE k=0;k<cbSameCount;k++)
			{
				SameDataInfo.cbSameData[SameDataInfo.cbCardCount++]=cbCardData[i+k];
			}
		}

		//递增处理
		i+=j-1;
	}

	return SameDataInfo.cbBlockCount;
}

//提取扑克
BYTE CGameLogic::DistillTrackorByCount(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbSameCount, tagTractorDataInfo & TractorDataInfo)
{
	//参数效验
	ASSERT((cbSameCount>=2)&&(cbSameCount<=PACK_COUNT));
	if ((cbSameCount<2)||(cbSameCount>PACK_COUNT)) return 0;

	//设置变量
	ZeroMemory(&TractorDataInfo,sizeof(TractorDataInfo));

	//提取同牌
	tagSameDataInfo SameDataInfo;
	DistillCardByCount(cbCardData,cbCardCount,cbSameCount,SameDataInfo);

	//牌段判断
	if (SameDataInfo.cbBlockCount<2) return 0;

	//分析处理
	BYTE cbResultIndex=0;
	for (BYTE i=0;i<SameDataInfo.cbBlockCount-1;i++)
	{
		//分析过滤
		if (SameDataInfo.cbSameData[i*cbSameCount]==0) continue;

		//变量定义
		BYTE cbLineCount=1;
		BYTE cbTrackorIndex[MAX_TRACKOR]={i*cbSameCount};

		//连牌判断
		for (BYTE j=i;j<SameDataInfo.cbBlockCount-1;j++)
		{
			//分析过滤
			if (SameDataInfo.cbSameData[(j+1)*cbSameCount]==0) continue;

			//变量定义
			BYTE cbSecondIndex=(j+1)*cbSameCount;
			BYTE cbFirstIndex=cbTrackorIndex[cbLineCount-1];
			BYTE cbFirstCardData=SameDataInfo.cbSameData[cbFirstIndex];
			BYTE cbSecondCardData=SameDataInfo.cbSameData[cbSecondIndex];

			//连牌判断
			if (IsLineValue(cbFirstCardData,cbSecondCardData)==true)
			{
				ASSERT(cbLineCount<MAX_TRACKOR);
				cbTrackorIndex[cbLineCount++]=cbSecondIndex;
				continue;
			}

			//副主间隔
			if (cbFirstCardData==cbSecondCardData) continue;
			if (GetCardValue(cbSecondCardData)==m_cbNTValue) continue;
			if (GetCardValue(cbSecondCardData)==m_cbMainValue) continue;

			break;
		}

		//成功判断
		if (cbLineCount>=2)
		{
			//最大长度
			if (cbLineCount>TractorDataInfo.cbTractorMaxLength)
			{
				TractorDataInfo.cbTractorMaxLength=cbLineCount;
				TractorDataInfo.cbTractorMaxIndex=cbResultIndex;
			}

			//设置结果
			TractorDataInfo.cbTractorCount++;
			TractorDataInfo.cbCardCount+=cbLineCount*cbSameCount;
			TractorDataInfo.cbTractorData[cbResultIndex++]=cbLineCount;
			TractorDataInfo.cbTractorData[cbResultIndex++]=cbSameCount;

			//设置数据
			for (BYTE k=0;k<cbLineCount;k++)
			{
				BYTE cbIndex=cbTrackorIndex[k];
				for (BYTE l=0;l<cbSameCount;l++)
				{
					TractorDataInfo.cbTractorData[cbResultIndex++]=SameDataInfo.cbSameData[cbIndex+l];
					SameDataInfo.cbSameData[cbIndex+l]=0;
				}
			}
		}
	}

	return TractorDataInfo.cbTractorCount;
}

//更新权位
void CGameLogic::UpdateSortRight()
{
	//设置权位
	for (BYTE i=0;i<CountArray(m_cbSortRight);i++) m_cbSortRight[i]=i*COLOR_RIGHT;

	//主牌权位
	if ((m_cbMainColor!=COLOR_ERROR)&&(m_cbMainColor!=COLOR_NT)) m_cbSortRight[m_cbMainColor>>4]=4*COLOR_RIGHT;

	//调整权位
	switch (m_cbMainColor)
	{
	case COLOR_MEI_HUA:		//梅花
		{
			m_cbSortRight[COLOR_HEI_TAO>>4]=COLOR_RIGHT;
			break;
		}
	case COLOR_HONG_TAO:	//红桃
		{
			m_cbSortRight[COLOR_FANG_KUAI>>4]=2*COLOR_RIGHT;
			break;
		}
	}

	return;
}

//调整扑克
bool CGameLogic::RectifyCardWeave(const BYTE cbCardData[], BYTE cbCardCount, const tagAnalyseResult & TargetResult, tagAnalyseResult & RectifyResult)
{
	//设置变量
	ZeroMemory(&RectifyResult,sizeof(RectifyResult));

	//花色判断
	RectifyResult.cbCardColor=GetCardLogicColor(cbCardData,cbCardCount);
	if (RectifyResult.cbCardColor==COLOR_ERROR) return false;

	//构造扑克
	BYTE cbTempCardData[MAX_COUNT];
	BYTE cbTempCardCount=cbCardCount;
	CopyMemory(cbTempCardData,cbCardData,sizeof(BYTE)*cbCardCount);

	//循环调整
	for (BYTE i=PACK_COUNT;i>=2;i--)
	{
		//调整变量
		tagSameDataInfo * pSameDataInfoRectify=&RectifyResult.SameDataInfo[i-1];
		tagTractorDataInfo * pTractorDataInfoRectify=&RectifyResult.TractorDataInfo[i-2];

		//目标变量
		const tagSameDataInfo * pSameDataInfoTarget=&TargetResult.SameDataInfo[i-1];
		const tagTractorDataInfo * pTractorDataInfoTarget=&TargetResult.TractorDataInfo[i-2];

		//调整拖牌
		if (pTractorDataInfoTarget->cbTractorCount>0)
		{
			//提取拖牌
			tagTractorDataInfo TractorDataInfo;
			DistillTrackorByCount(cbTempCardData,cbTempCardCount,i,TractorDataInfo);

			//数据判断
			if (pTractorDataInfoTarget->cbCardCount>TractorDataInfo.cbCardCount) return false;
			if (pTractorDataInfoTarget->cbTractorMaxLength>TractorDataInfo.cbTractorMaxLength) return false;

			//变量定义
			BYTE cbCollectCount=0;
			BYTE cbCollectCardData[MAX_COUNT];

			//设置属性
			pTractorDataInfoRectify->cbTractorMaxIndex=255;
			pTractorDataInfoRectify->cbTractorMaxLength=pTractorDataInfoTarget->cbTractorMaxLength;

			//设置结果
			BYTE cbIndex=0;
			for (BYTE j=0;j<TractorDataInfo.cbTractorCount;j++)
			{
				//变量定义
				BYTE cbRectifyLineCount=TractorDataInfo.cbTractorData[cbIndex];
				BYTE cbResidualLineCount=(pTractorDataInfoTarget->cbCardCount-cbCollectCount)/i;

				//属性定义
				BYTE cbSameCount=i;
				BYTE cbLineCount=__min(cbRectifyLineCount,cbResidualLineCount);

				//设置属性
				pTractorDataInfoRectify->cbTractorCount++;
				pTractorDataInfoRectify->cbCardCount+=cbLineCount*cbSameCount;

				//设置属性
				pTractorDataInfoRectify->cbTractorData[cbIndex]=cbLineCount;
				pTractorDataInfoRectify->cbTractorData[cbIndex+1]=cbSameCount;

				//设置扑克
				BYTE cbInsertCount=cbLineCount*cbSameCount;
				CopyMemory(&cbCollectCardData[cbCollectCount],&TractorDataInfo.cbTractorData[cbIndex+2],cbInsertCount*sizeof(BYTE));
				CopyMemory(&pTractorDataInfoRectify->cbTractorData[cbIndex+2],&TractorDataInfo.cbTractorData[cbIndex+2],cbInsertCount*sizeof(BYTE));

				//设置变量
				cbIndex+=(cbInsertCount+2);
				cbCollectCount+=cbInsertCount;

				//完成判断
				if (cbCollectCount>=pTractorDataInfoTarget->cbCardCount) break;
			}

			//删除扑克
			cbTempCardCount-=cbCollectCount;
			RemoveCard(cbCollectCardData,cbCollectCount,cbTempCardData,cbTempCardCount+cbCollectCount);
		}

		//调整同牌
		if (pSameDataInfoTarget->cbCardCount>0)
		{
			//提取同牌
			tagSameDataInfo SameDataInfo;
			DistillCardByCount(cbTempCardData,cbTempCardCount,i,SameDataInfo);

			//数据判断
			if (pSameDataInfoTarget->cbCardCount>SameDataInfo.cbCardCount) return false;
			if (pSameDataInfoTarget->cbBlockCount>SameDataInfo.cbBlockCount) return false;

			//设置扑克
			pSameDataInfoRectify->cbCardCount=pSameDataInfoTarget->cbCardCount;
			pSameDataInfoRectify->cbBlockCount=pSameDataInfoTarget->cbBlockCount;
			CopyMemory(pSameDataInfoRectify->cbSameData,SameDataInfo.cbSameData,sizeof(BYTE)*pSameDataInfoRectify->cbCardCount);

			//变量定义
			BYTE cbRemoveCount=pSameDataInfoRectify->cbCardCount;
			LPBYTE pcbRemoveCard=&pSameDataInfoRectify->cbSameData[0];

			//删除扑克
			cbTempCardCount-=cbRemoveCount;
			RemoveCard(pcbRemoveCard,cbRemoveCount,cbTempCardData,cbTempCardCount+cbRemoveCount);
		}
	}

	//设置单牌
	if (cbTempCardCount>0)
	{
		RectifyResult.SameDataInfo[0].cbCardCount=cbTempCardCount;
		RectifyResult.SameDataInfo[0].cbBlockCount=cbTempCardCount;
		CopyMemory(RectifyResult.SameDataInfo[0].cbSameData,cbTempCardData,cbTempCardCount);
	}

	return true;
}

//交集数目
BYTE CGameLogic::GetIntersectionCount(const BYTE cbCardData1[], BYTE cbCardCount1, const BYTE cbCardData2[], BYTE cbCardCount2,
									  BYTE *cbResultCard)
{
	//变量定义
	BYTE cbAnalyseCard[MAX_COUNT];
	CopyMemory(cbAnalyseCard,cbCardData2,sizeof(BYTE)*cbCardCount2);

	//分析判断
	BYTE cbIntersectionCount=0;
	for (BYTE i=0;i<cbCardCount1;i++)
	{
		for (BYTE j=0;j<cbCardCount2;j++)
		{
			if (cbCardData1[i]==cbAnalyseCard[j])
			{
				//设置变量
				if( NULL != cbResultCard )
					cbResultCard[cbIntersectionCount] = cbAnalyseCard[j];
				cbIntersectionCount++;
				cbAnalyseCard[j]=0;
				break;
			}
		}
	}

	return cbIntersectionCount;
}

//////////////////////////////////////////////////////////////////////////
