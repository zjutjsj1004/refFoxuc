#ifndef GAME_LOGIC_HEAD_FILE
#define GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"

//////////////////////////////////////////////////////////////////////////	
//调试函数
//static void MyMsgBox(LPCSTR lpszFormat, ...)
//{
//	va_list args;
//	int		nBuf;
//	TCHAR	szBuffer[512];
//
//	va_start(args, lpszFormat);
//	nBuf = _vsntprintf(szBuffer, sizeof(szBuffer)-sizeof(TCHAR), lpszFormat, args);
//#ifdef _DEBUG
//	AfxMessageBox(szBuffer);
//#else
//	TCHAR	szFormat[512];
//	_stprintf(szFormat, _T("[JJ]: %s"), szBuffer);
//	OutputDebugString(szFormat);
//#endif
//
//	va_end(args);
//}
//
//static void MyDebug(LPCSTR lpszFormat, ...)
//{
//	va_list args;
//	int		nBuf;
//	TCHAR	szBuffer[512];
//	TCHAR	szFormat[512];
//
//	va_start(args, lpszFormat);
//	nBuf = _vsntprintf(szBuffer, sizeof(szBuffer)-sizeof(TCHAR), lpszFormat, args);
//	_stprintf(szFormat, _T("[JJ]: %s"), szBuffer);
//	OutputDebugString(szFormat);
//
//	va_end(args);
//}
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

//用于财神的转换，如果有牌可以代替财神本身牌使用，则设为该牌索引，否则设为MAX_INDEX. 注:如果替换牌是序数牌,将出错.
#define	INDEX_REPLACE_CARD					33

//////////////////////////////////////////////////////////////////////////
//逻辑掩码

#define	MASK_COLOR					0xF0								//花色掩码
#define	MASK_VALUE					0x0F								//数值掩码

//////////////////////////////////////////////////////////////////////////
//动作定义

//动作标志
#define WIK_NULL					0x00								//没有类型
#define WIK_LEFT					0x01								//左吃类型
#define WIK_CENTER					0x02								//中吃类型
#define WIK_RIGHT					0x04								//右吃类型
#define WIK_PENG					0x08								//碰牌类型
#define WIK_GANG					0x10								//杠牌类型
#define WIK_LISTEN					0x20								//吃牌类型
#define WIK_CHI_HU					0x40								//吃胡类型
#define WIK_REPLACE					0x80								//花牌替换

//////////////////////////////////////////////////////////////////////////
//胡牌定义

/*
//	牌型分
*/
#define CHK_NULL					0x0000								//非胡
#define CHR_ZI_MO					0x0001								//自摸
#define CHR_PENG_PENG_HU			0x0002								//碰碰胡
#define CHR_BA_DUI					0x0004								//八对
#define CHR_DI_HU					0x0008								//地胡
#define CHR_TIAN_HU					0x0010								//天胡
#define CHR_DAN_DIAO				0x0020								//单吊
#define CHR_THREE_KING				0x0040								//3金倒
#define CHR_RUAN_HU					0x0080								//软胡
#define CHR_YING_HU					0x0100								//硬胡
#define CHR_SHUANG_FAN				0x0200								//双翻	
#define CHR_QING_YI_SE				0x0400								//清一色
#define CHR_BA_DUI_YING				0x1000								//硬八对	

//////////////////////////////////////////////////////////////////////////

//类型子项
struct tagKindItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbValidIndex[3];					//实际扑克索引
};

//组合子项
struct tagWeaveItem
{
	BYTE							cbWeaveKind;						//组合类型
	BYTE							cbCenterCard;						//中心扑克
	BYTE							cbPublicCard;						//公开标志
	WORD							wProvideUser;						//供应用户
	BYTE							cbCardData[4];						//
};

//杠牌结果
struct tagGangCardResult
{
	BYTE							cbCardCount;						//扑克数目
	BYTE							cbCardData[MAX_WEAVE];				//扑克数据
};

//分析子项
struct tagAnalyseItem
{
	BYTE							cbCardEye;							//牌眼扑克
	bool                            bMagicEye;                          //牌眼是否是王霸
	BYTE							cbWeaveKind[MAX_WEAVE];				//组合类型
	BYTE							cbCenterCard[MAX_WEAVE];			//中心扑克
	BYTE                            cbCardData[MAX_WEAVE][4];           //实际扑克
};

//////////////////////////////////////////////////////////////////////////


#define MASK_CHI_HU_RIGHT			0x0fffffff

/*
//	权位类。
//  注意，在操作仅位时最好只操作单个权位.例如
//  CChiHuRight chr;
//  chr |= (chr_zi_mo|chr_peng_peng)，这样结果是无定义的。
//  只能单个操作:
//  chr |= chr_zi_mo;
//  chr |= chr_peng_peng;
*/
class CChiHuRight
{	
	//静态变量
private:
	static bool						m_bInit;
	static DWORD					m_dwRightMask[MAX_RIGHT_COUNT];

	//权位变量
private:
	DWORD							m_dwRight[MAX_RIGHT_COUNT];

public:
	//构造函数
	CChiHuRight();

	//运算符重载
public:
	//赋值符
	CChiHuRight & operator = ( DWORD dwRight );

	//与等于
	CChiHuRight & operator &= ( DWORD dwRight );
	//或等于
	CChiHuRight & operator |= ( DWORD dwRight );

	//与
	CChiHuRight operator & ( DWORD dwRight );
	CChiHuRight operator & ( DWORD dwRight ) const;

	//或
	CChiHuRight operator | ( DWORD dwRight );
	CChiHuRight operator | ( DWORD dwRight ) const;

	//相等
	bool operator == ( DWORD dwRight ) const;
	bool operator == ( const CChiHuRight chr ) const;

	//不相等
	bool operator != ( DWORD dwRight ) const;
	bool operator != ( const CChiHuRight chr ) const;

	//功能函数
public:
	//是否权位为空
	bool IsEmpty();

	//设置权位为空
	void SetEmpty();

	//获取权位数值
	BYTE GetRightData( DWORD dwRight[], BYTE cbMaxCount );

	//设置权位数值
	bool SetRightData( const DWORD dwRight[], BYTE cbRightCount );

private:
	//检查权位是否正确
	bool IsValidRight( DWORD dwRight );
};

//////////////////////////////////////////////////////////////////////////

//数组说明
typedef CWHArray<tagAnalyseItem,tagAnalyseItem &> CAnalyseItemArray;

//游戏逻辑类
class CGameLogic
{
	//变量定义
protected:
	static const BYTE				m_cbCardDataArray[MAX_REPERTORY];	//扑克数据
	BYTE							m_cbMagicIndex;						//钻牌索引

	//函数定义
public:
	//构造函数
	CGameLogic();
	//析构函数
	virtual ~CGameLogic();

	//控制函数
public:
	//混乱扑克
	void RandCardData(BYTE cbCardData[], BYTE cbMaxCount);
	//混乱扑克
	void RandCardData(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbRandData[]);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], BYTE cbRemoveCard);
	//删除扑克
	bool RemoveCard(BYTE cbCardIndex[MAX_INDEX], const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//删除扑克
	bool RemoveCard(BYTE cbCardData[], BYTE cbCardCount, const BYTE cbRemoveCard[], BYTE cbRemoveCount);
	//设置财神
	void SetMagicIndex( BYTE cbMagicIndex ) { m_cbMagicIndex = cbMagicIndex; }
	//财神判断
	inline bool IsMagicCard( BYTE cbCardData, bool bIsIndex/* = false */)
	{
		BYTE cbCardIndex = cbCardData;
		if (!bIsIndex) cbCardIndex = SwitchToCardIndex(cbCardData);

		if( m_cbMagicIndex != MAX_INDEX )
		{
			if (m_cbMagicIndex >= 34 && m_cbMagicIndex <= 37)
				return (cbCardIndex >= 34 && (cbCardIndex <= 37));
			else if (m_cbMagicIndex >= 38 && m_cbMagicIndex <= 41)
				return ((cbCardIndex >= 38) && (cbCardIndex <= 41));
			else
				return (cbCardIndex==m_cbMagicIndex);
		}
		return false;
	}
	//花牌判断
	bool IsHuaCard(BYTE cbCardData);
	//脱牌判断
	bool IsTuoPai(BYTE cbOutCard, tagWeaveItem * pWeaveItem);

	//辅助函数
public:
	//有效判断
	bool IsValidCard(BYTE cbCardData);
	//扑克数目
	BYTE GetCardCount(const BYTE cbCardIndex[MAX_INDEX]);
	//组合扑克
	BYTE GetWeaveCard(BYTE cbWeaveKind, BYTE cbCenterCard, BYTE cbCardBuffer[4]);
	//花牌数目
	BYTE GetMagicCardCount(const BYTE cbCardIndex[MAX_INDEX]);

	//等级函数
public:
	//动作等级
	BYTE GetUserActionRank(BYTE cbUserAction);
	//胡牌等级
	BYTE GetChiHuActionRank(const CChiHuRight & ChiHuRight, bool bCheckFan = false);

	//动作判断
public:
	//吃牌判断
	BYTE EstimateEatCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//碰牌判断
	BYTE EstimatePengCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);
	//杠牌判断
	BYTE EstimateGangCard(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCurrentCard);

	//动作判断
public:
	//杠牌分析
	BYTE AnalyseGangCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, tagGangCardResult & GangCardResult);
	//吃胡分析
	BYTE AnalyseChiHuCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, BYTE cbCurrentCard, CChiHuRight &ChiHuRight);
	//听牌分析
	BYTE AnalyseTingCard( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount );

	//转换函数
public:
	//扑克转换
	BYTE SwitchToCardData(BYTE cbCardIndex);
	//扑克转换
	BYTE SwitchToCardIndex(BYTE cbCardData);
	//扑克转换
	BYTE SwitchToCardData(const BYTE cbCardIndex[MAX_INDEX], BYTE cbCardData[MAX_COUNT]);
	//扑克转换
	BYTE SwitchToCardIndex(const BYTE cbCardData[], BYTE cbCardCount, BYTE cbCardIndex[MAX_INDEX]);

	//胡法分析
protected:
	//单吊
	bool IsDanDiaoJiang( const tagAnalyseItem *pAnalyseItem, BYTE cbCurrentCard );
	//八对
	bool IsBaDui( const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbWeaveCount, bool & bYingBaDui );
	//碰碰和
	bool IsPengPeng( const tagAnalyseItem *pAnalyseItem );
	//硬胡
	bool IsYingHu( const tagAnalyseItem *pAnalyseItem, BYTE cbWeaveCount );
	//三金倒
	bool IsSanJinDao(const BYTE cbCardIndex[MAX_INDEX]);

	//内部函数
private:
	//分析扑克
	bool AnalyseCard(const BYTE cbCardIndex[MAX_INDEX], const tagWeaveItem WeaveItem[], BYTE cbItemCount, CAnalyseItemArray & AnalyseItemArray);
	//排序,根据牌值排序
	bool SortCardList( BYTE cbCardData[MAX_COUNT], BYTE cbCardCount );
};

//////////////////////////////////////////////////////////////////////////

#endif
