#ifndef GAME_CLIENT_ENGINE_HEAD_FILE
#define GAME_CLIENT_ENGINE_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "GameLogic.h"
#include "GameClientView.h"

//////////////////////////////////////////////////////////////////////////

//游戏引擎
class CGameClientEngine : public CGameFrameEngine
{
	//规则变量
protected:
	WORD							m_wRuleRoundID;						//规则标识
	tagGameRuleInfo					m_GameRuleInfo;						//游戏规则

	//游戏变量
protected:
	WORD							m_wBankerUser;						//庄家玩家
	WORD							m_wCurrentUser;						//当前用户
	BYTE							m_cbChessColor;						//棋子颜色

	//选择信息
protected:
	BYTE							m_cbXSourcePos;						//选择信息
	BYTE							m_cbYSourcePos;						//选择信息

	//辅助变量
protected:
	bool							m_bDrawTimeMode;					//局时模式
	bool							m_bRequestAgree;					//请求同意
	bool							m_bRequestCustomize;				//请求定制
	TCHAR							m_szUserAccounts[2][LEN_ACCOUNTS];	//玩家帐号

	//状态变量
protected:
	WORD							m_wViewStepCount;					//棋谱步数
	tagStatusInfo					m_UserStatusInfo[2];				//用户状态

	//辅助变量
protected:
	tagChessItem					m_ChessItemView[2][COUNT_CHESS];	//棋子信息
	tagChessItem *					m_ChessBoradView[COUNT_X][COUNT_Y];	//棋盘信息

	//控件变量
protected:
	CGameLogic						m_GameLogic;						//游戏逻辑
	CGameClientView					m_GameClientView;					//游戏视图
	CGlobalUnits *                  m_pGlobalUnits;                     //全局单元
	//函数定义
public:
	//构造函数
	CGameClientEngine();
	//析构函数
	virtual ~CGameClientEngine();

	//常规继承
private:
	//初始函数
	virtual bool OnInitGameEngine();
	//重置框架
	virtual bool OnResetGameEngine();
	//游戏设置
	virtual void OnGameOptionSet();

	//时钟事件
public:
	//时钟删除
	virtual bool OnEventGameClockKill(WORD wChairID);
	//时钟信息
	virtual bool OnEventGameClockInfo(WORD wChairID, UINT nElapse, WORD wClockID);

	//游戏事件
public:
	//旁观消息
	virtual bool OnEventLookonMode(VOID * pData, WORD wDataSize);
	//游戏消息
	virtual bool OnEventGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize);
	//场景消息
	virtual bool OnEventSceneMessage(BYTE cbGameStatus, bool bLookonUser, VOID * pData, WORD wDataSize);
	//声音控制
	virtual bool AllowBackGroundSound(bool bAllowSound){return true;}
	//内部函数
protected:
	//更新按钮
	bool UpdateManualControl(WORD wViewStepCount);
	//执行走棋
	bool PerformMoveChess(BYTE cbXSourcePos, BYTE cbYSourcePos, BYTE cbXTargetPos, BYTE cbYTargetPos);

	//消息处理
protected:
	//游戏开始
	bool OnSubGameStart(const VOID * pData, WORD wDataSize);
	//移动棋子
	bool OnSubMoveChess(const VOID * pData, WORD wDataSize);
	//悔棋失败
	bool OnSubRegretFaile(const VOID * pData, WORD wDataSize);
	//悔棋结果
	bool OnSubRegretResult(const VOID * pData, WORD wDataSize);
	//悔棋请求
	bool OnSubRegretRequest(const VOID * pData, WORD wDataSize);
	//和棋响应
	bool OnSubPeaceRespond(const VOID * pData, WORD wDataSize);
	//和棋请求
	bool OnSubPeaceRequest(const VOID * pData, WORD wDataSize);
	//游戏结束
	bool OnSubGameEnd(const VOID * pData, WORD wDataSize);
	//请求设置
	bool OnSubRequestRule(const VOID * pData, WORD wDataSize);
	//请求同意
	bool OnSubRequestAgree(const VOID * pData, WORD wDataSize);
	//游戏棋谱
	bool OnSubChessManual(const VOID * pData, WORD wDataSize);

	//消息定义
protected:
	//开始消息
	LRESULT OnMessageStart(WPARAM wParam, LPARAM lParam);
	//求和消息
	LRESULT OnMessagePeace(WPARAM wParam, LPARAM lParam);
	//悔棋消息
	LRESULT OnMessageRegret(WPARAM wParam, LPARAM lParam);
	//认输消息
	LRESULT OnMessageGiveUp(WPARAM wParam, LPARAM lParam);
	//棋谱消息
	LRESULT OnMessageManual(WPARAM wParam, LPARAM lParam);
	//保存消息
	LRESULT OnMessagePreserve(WPARAM wParam, LPARAM lParam);
	//点击棋盘
	LRESULT OnHitChessBorad(WPARAM wParam, LPARAM lParam);
	//同意规则
	LRESULT OnGameRuleAgree(WPARAM wParam, LPARAM lParam);
	//拒绝规则
	LRESULT OnGameRuleReject(WPARAM wParam, LPARAM lParam);
	//棋谱定位
	LRESULT OnOrientationManual(WPARAM wParam, LPARAM lParam);
	//请求回应
	LRESULT OnUserRequestRespond(WPARAM wParam, LPARAM lParam);

	//系统消息
protected:
	//时间消息
	VOID OnTimer(UINT nIDEvent);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
