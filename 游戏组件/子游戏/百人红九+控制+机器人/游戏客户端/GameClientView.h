#ifndef GAME_CLIENT_VIEW_HEAD_FILE
#define GAME_CLIENT_VIEW_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "RgnButton.h"
#include "CardControl.h"
#include "RecordGameList.h"
#include "ApplyUserList.h"
#include "ClientControl.h"
#include "../游戏服务器/GameLogic.h"
#ifdef __BANKER___
#include "DlgBank.h"
#endif
//////////////////////////////////////////////////////////////////////////

//筹码定义
#define JETTON_COUNT				7									//筹码数目
#define JETTON_RADII				68									//筹码半径

//消息定义
#define IDM_PLACE_JETTON			WM_USER+200							//加住信息
#define IDM_APPLY_BANKER			WM_USER+201							//申请信息
#define IDM_CONTINUE_CARD			WM_USER+202							//继续发牌
#define IDM_AUTO_OPEN_CARD			WM_USER+203							//按钮标识
#define IDM_OPEN_CARD				WM_USER+204		

//索引定义
#define INDEX_PLAYER				0									//闲家索引
#define INDEX_BANKER				1									//庄家索引

//////////////////////////////////////////////////////////////////////////
//结构定义

//历史记录
#define MAX_SCORE_HISTORY			65									//历史个数
#define MAX_FALG_COUNT				13									//标识个数

//筹码信息
struct tagJettonInfo
{
	int								nXPos;								//筹码位置
	int								nYPos;								//筹码位置
	BYTE							cbJettonIndex;						//筹码索引
};

//操作结果
enum enOperateResult
{
	enOperateResult_NULL,
	enOperateResult_Win,
	enOperateResult_Lost
};

//记录信息
struct tagClientGameRecord
{
	enOperateResult					enOperateShunMen;					//操作标识
	enOperateResult					enOperateDuiMen;					//操作标识
	enOperateResult					enOperateDaoMen;					//操作标识
	bool							bWinShunMen;						//顺门胜利
	bool							bWinDuiMen;							//对门胜利
	bool							bWinDaoMen;							//倒门胜利
};

//发牌提示
enum enDispatchCardTip
{
	enDispatchCardTip_NULL,
	enDispatchCardTip_Continue,											//继续发牌
	enDispatchCardTip_Dispatch											//重新洗牌
};

//客户端机器人下分
struct tagAndroidBet
{
	BYTE							cbJettonArea;						//筹码区域
	LONGLONG						lJettonScore;						//加注数目
	WORD							wChairID;							//玩家位置
	int								nLeftTime;							//剩余时间 (100ms为单位)
};

//筹码数组
typedef CWHArray<tagJettonInfo,tagJettonInfo&> CJettonInfoArray;

//累声明
class CGameClientDlg;
//////////////////////////////////////////////////////////////////////////

//游戏视图
class CGameClientView : public CGameFrameViewGDI
{
	//限制信息
protected:
	LONGLONG						m_lMeMaxScore;						//最大下注
    LONGLONG						m_lAreaLimitScore;					//区域限制

	//下注信息
protected:
	LONGLONG						m_lUserJettonScore[AREA_COUNT+1];	//个人总注
	LONGLONG						m_lAllJettonScore[AREA_COUNT+1];	//全体总注

	//位置信息
protected:
	int								m_nWinFlagsExcursionX;				//偏移位置
	int								m_nWinFlagsExcursionY;				//偏移位置
	int								m_nScoreHead;						//成绩位置
	CRect							m_rcShunMen;						//闲家区域
	CRect							m_rcJiaoL;							//闲天王区域
	CRect							m_rcQiao;							//对子区域
	CRect							m_rcDuiMen;							//平区域
	CRect							m_rcDaoMen;							//同点平区域
	CRect							m_rcJiaoR;							//庄家区域
	int								m_OpenCardIndex;					//開牌順序
	int								m_PostCardIndex;					//發牌順序
	int								m_PostStartIndex;					//发牌起始位置

	//扑克信息
public:	
    BYTE							m_cbTableCardArray[4][2];			//桌面扑克
	bool							m_blMoveFinish;
	BYTE							m_bcfirstShowCard;
	BYTE							m_bcShowCount;
	bool							m_blAutoOpenCard;					//手自动
	bool							m_bWaitSkip;

	//历史信息
protected:
	LONGLONG						m_lMeStatisticScore;				//游戏成绩
	tagClientGameRecord				m_GameRecordArrary[MAX_SCORE_HISTORY];//游戏记录
	int								m_nRecordFirst;						//开始记录
	int								m_nRecordLast;						//最后记录

	//状态变量
public:
	WORD							m_wMeChairID;						//我的位置
	BYTE							m_cbAreaFlash;						//胜利玩家
	LONGLONG						m_lCurrentJetton;					//当前筹码
	bool							m_bShowChangeBanker;				//轮换庄家
	bool							m_bNeedSetGameRecord;				//完成设置
	bool							m_bWinShunMen;						//胜利标识
	bool							m_bWinDuiMen;						//胜利标识
	bool							m_bWinDaoMen;						//胜利标识
	bool							m_bFlashResult;						//显示结果
	bool							m_bShowGameResult;					//显示结果
	enDispatchCardTip				m_enDispatchCardTip;				//发牌提示

	//庄家信息
protected:	
	WORD							m_wBankerUser;						//当前庄家
	WORD							m_wBankerTime;						//做庄次数
	LONGLONG						m_lBankerScore;						//庄家积分
	LONGLONG						m_lBankerWinScore;					//庄家成绩	
	LONGLONG						m_lTmpBankerWinScore;				//庄家成绩	
	bool							m_bEnableSysBanker;					//系统做庄

	//当局成绩
public:
	LONGLONG						m_lMeCurGameScore;					//我的成绩
	LONGLONG						m_lMeCurGameReturnScore;			//我的成绩
	LONGLONG						m_lBankerCurGameScore;				//庄家成绩
	LONGLONG						m_lGameRevenue;						//游戏税收

	//作弊变量
public:
	bool							m_bShowCheatInfo;					//显示作弊
	BYTE							m_bCheatCard[4][2];					//作弊扑克
	int								m_nWinInfo[3];						//输赢情况	

	//数据变量
protected:
	CPoint							m_PointJetton[AREA_COUNT];			//筹码位置
	CPoint							m_PointJettonNumber[AREA_COUNT];	//数字位置
	CJettonInfoArray				m_JettonInfoArray[AREA_COUNT];		//筹码数组

	//控件变量
public:
	CRgnButton						m_btJetton100;						//筹码按钮
	CRgnButton						m_btJetton1000;						//筹码按钮
	CRgnButton						m_btJetton10000;					//筹码按钮
	CRgnButton						m_btJetton100000;					//筹码按钮
	CRgnButton						m_btJetton500000;					//筹码按钮
	CRgnButton						m_btJetton1000000;					//筹码按钮
	CRgnButton						m_btJetton5000000;					//筹码按钮
	CSkinButton						m_btApplyBanker;					//申请庄家
	CSkinButton						m_btCancelBanker;					//取消庄家
	CSkinButton						m_btScoreMoveL;						//移动成绩
	CSkinButton						m_btScoreMoveR;						//移动成绩
	CSkinButton						m_btAutoOpenCard;					//自动开牌
	CSkinButton						m_btOpenCard;						//手动开牌
	CSkinButton						m_btBank;							//银行按钮
	CSkinButton						m_btContinueCard;					//继续发牌
	CSkinButton						m_btUp;								//存款按钮
	CSkinButton						m_btDown;							//取款按钮

	CButton							m_btOpenAdmin;
	//控件变量
public:
#ifdef __BANKER___
	CDlgBank						m_DlgBank;							//银行控件
	CSkinButton						m_btBankerStorage;					//存款按钮
	CSkinButton						m_btBankerDraw;						//取款按钮
#endif
	CGameLogic						m_GameLogic;	
	CApplyUser						m_ApplyUser;						//申请列表
	CGameRecord						m_GameRecord;						//记录列表
	CCardControl					m_CardControl[5];					//扑克控件	
	CGameClientDlg					*m_pGameClientDlg;					//父类指针
					//游戏逻辑

	//控制
public:
	HINSTANCE						m_hInst;
	IClientControlDlg*				m_pClientControlDlg;				

	//界面变量
protected:
	CBitImage						m_ImageViewFill;					//背景位图
	CBitImage						m_ImageViewBack;					//背景位图
	CBitImage						m_ImageWinFlags;					//标志位图
	CBitImage						m_ImageJettonView;					//筹码视图
	CBitImage						m_ImageScoreNumber;					//数字视图
	CBitImage						m_ImageMeScoreNumber;				//数字视图
	CBitImage						m_ImageDispatchCardTip;				//发牌提示

	//边框资源
protected:
	//CSkinImage						m_ImageFrameShunMen;				//边框图片
	//CSkinImage						m_ImageFrameJiaoL;					//边框图片
	//CSkinImage						m_ImageFrameQiao;					//边框图片
	//CSkinImage						m_ImageFrameDuiMen;					//边框图片
	//CSkinImage						m_ImageFrameDaoMen;					//边框图片
	//CSkinImage						m_ImageFrameJiaoR;					//边框图片

	CBitImage						m_ImageMeBanker;					//切换庄家
	CBitImage						m_ImageChangeBanker;				//切换庄家
	CBitImage						m_ImageNoBanker;					//切换庄家

	CPngImage						m_PngFrameCenter;					//边框图片				
	CPngImage						m_PngFrameAngleUp;					//边框图片
	CPngImage						m_PngFrameAngleDown;				//边框图片

	CPngImage						m_PngTimeNum;						//时间数字

	//结束资源
protected:
	//CSkinImage						m_ImageGameEnd;						//成绩图片

	//函数定义
public:
	//构造函数
	CGameClientView();
	//析构函数
	virtual ~CGameClientView();

	//继承函数
private:
	//重置界面
	virtual VOID ResetGameView();
	//调整控件
	virtual VOID RectifyControl(INT nWidth, INT nHeight);
	//绘画界面
	virtual VOID DrawGameView(CDC * pDC, INT nWidth, INT nHeight);
	virtual bool RealizeWIN7() { return true; }

	//设置函数
public:
	//设置信息
	void SetMeMaxScore(LONGLONG lMeMaxScore);
	//我的位置
	void SetMeChairID(DWORD dwMeUserID);
	//个人下注
	void SetMePlaceJetton(BYTE cbViewIndex, LONGLONG lJettonCount);
	//庄家信息
	void SetBankerInfo(DWORD dwBankerUserID, LONGLONG lBankerScore);
	//庄家成绩
	void SetBankerScore(WORD wBankerTime, LONGLONG lWinScore) {m_wBankerTime=wBankerTime; m_lTmpBankerWinScore=lWinScore;}
	//当局成绩
	void SetCurGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore, LONGLONG lGameRevenue);
	//设置筹码
	void PlaceUserJetton(BYTE cbViewIndex, LONGLONG lScoreCount);
	//区域限制
	void SetAreaLimitScore(LONGLONG lAreaLimitScore);	
	//设置扑克
	void SetCardInfo(BYTE cbTableCardArray[4][2]);
	//设置筹码
	void SetCurrentJetton(LONGLONG lCurrentJetton);
	//历史记录
	void SetGameHistory(bool bWinShunMen, bool bWinDaoMen, bool bWinDuiMen);
	//轮换庄家
	void ShowChangeBanker( bool bChangeBanker );
	//成绩设置
	void SetGameScore(LONGLONG lMeCurGameScore, LONGLONG lMeCurGameReturnScore, LONGLONG lBankerCurGameScore);
	//允许系统做庄
	void EnableSysBanker(bool bEnableSysBanker) {m_bEnableSysBanker=bEnableSysBanker;}
	//设置提示
	void SetDispatchCardTip(enDispatchCardTip DispatchCardTip);

	void SetFirstShowCard(BYTE bcCard);
	//设置作弊数据
	void SetCheatInfo(BYTE bCardData[], int nWinInfo[]);
	//取消闪动
	void ClearAreaFlash() { m_cbAreaFlash = 0xFF; }

	//动画函数
public:	
	//开始发牌
	void DispatchCard();
	//结束发牌
	void FinishDispatchCard(bool bNotScene);
	//设置爆炸
	bool SetBombEffect(bool bBombEffect, WORD wAreaIndex);
	//终止动画
	void StopAnimal();

	void UpdateGameView( CRect *pRect );

	//计算限制
public:
	//最大下注
	LONGLONG GetUserMaxJetton(BYTE cbArea = 0xFF);

	//绘画函数
protected:
	//闪烁边框
	void FlashJettonAreaFrame(int nWidth, int nHeight, CDC *pDC);

	//界面函数
public:
	//清理筹码
	void CleanUserJetton();
	//设置胜方
	void SetWinnerSide(bool bWinShunMen, bool bWinDuiMen, bool bWinDaoMen, bool bSet);
	//艺术字体
	void DrawTextString(CDC * pDC, LPCTSTR pszString, COLORREF crText, COLORREF crFrame, int nXPos, int nYPos);
	//绘画数字
	void DrawMeJettonNumber(CDC *pDC);
	//绘画庄家
	void DrawBankerInfo(CDC *pDC,int nWidth,int nHeight);
	//绘画玩家
	void DrawMeInfo(CDC *pDC,int nWidth,int nHeight);
	//作弊扑克
	void DrawCheatCard(int nWidth, int nHeight, CDC *pDC);
	//允许控制
	void AllowControl(BYTE cbStatus);

	//内联函数
public:
	//当前筹码
	inline LONGLONG GetCurrentJetton() { return m_lCurrentJetton; }	
		void SetMoveCardTimer();

	//内部函数
private:
	//获取区域
	BYTE GetJettonArea(CPoint MousePoint);
	//绘画数字
	void DrawNumberString(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos, bool bMeScore = false);
	//绘画数字
	void DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, INT nXPos, INT nYPos);
	//绘画数字
	void DrawNumberStringWithSpace(CDC * pDC, LONGLONG lNumber, CRect rcRect, INT nFormat=-1);
	//绘画标识
	void DrawWinFlags(CDC * pDC);
	//显示结果
	void ShowGameResult(CDC *pDC, int nWidth, int nHeight);
	//透明绘画
	bool DrawAlphaRect(CDC* pDC, LPRECT lpRect, COLORREF clr, FLOAT fAlpha);
	//推断赢家
	void DeduceWinner(bool &bWinShunMen, bool &bWinDuiMen, bool &bWinDaoMen);

	//按钮消息
protected:
	//上庄按钮
	afx_msg void OnApplyBanker();
	//下庄按钮
	afx_msg void OnCancelBanker();
	//移动按钮
	afx_msg void OnScoreMoveL();
	//移动按钮
	afx_msg void OnScoreMoveR();
	//手工搓牌
	afx_msg void OnOpenCard();
	//自动搓牌
	afx_msg void OnAutoOpenCard();
	//翻页按钮
	afx_msg void OnUp();
	//翻页按钮
	afx_msg void OnDown();

	//消息映射
protected:
	//定时器消息
	afx_msg void OnTimer(UINT nIDEvent);
	//建立函数
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//鼠标消息
	afx_msg void OnLButtonDown(UINT nFlags, CPoint Point);	
	//鼠标消息
	afx_msg void OnRButtonDown(UINT nFlags, CPoint Point);	
    //鼠标消息
	afx_msg void OnLButtonUp(UINT nFlags, CPoint Point);
	//光标消息
	afx_msg BOOL OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT uMessage);
	//控件命令
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
    //鼠标移动
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	//其他控件鼠标UP消息
	LRESULT OnViLBtUp(WPARAM wParam, LPARAM lParam);
	//银行取款
	afx_msg void OnBankDraw();
	//银行存款
	afx_msg void OnBankStorage();
	//管理员控制
	afx_msg void OpenAdminWnd();

	DECLARE_MESSAGE_MAP()	

};

//////////////////////////////////////////////////////////////////////////

#endif
