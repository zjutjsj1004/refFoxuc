#include "StdAfx.h"
#include "TableFrameSink.h"
#include <conio.h>

//////////////////////////////////////////////////////////////////////////

//静态变量
const WORD			CTableFrameSink::m_wPlayerCount=GAME_PLAYER;			//游戏人数
bool				CTableFrameSink::m_bFirstInit=true;
//////////////////////////////////////////////////////////////////////////

#define	IDI_SO_OPERATE							2							//代打定时器
#define	TIME_SO_OPERATE							80000						//代打定时器

//////////////////////////////////////////////////////////////////////////

//构造函数
CTableFrameSink::CTableFrameSink()
{
	//游戏变量	
	m_lExitScore=0;	
	m_lDynamicScore=0;
	m_wBankerUser=INVALID_CHAIR;
	m_wFisrtCallUser=INVALID_CHAIR;
	m_wCurrentUser=INVALID_CHAIR;

	//用户状态
	ZeroMemory(m_cbDynamicJoin,sizeof(m_cbDynamicJoin));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbCallStatus,sizeof(m_cbCallStatus));
	ZeroMemory(m_bSpecialClient,sizeof(m_bSpecialClient));
	for(BYTE i=0;i<m_wPlayerCount;i++)m_cbOxCard[i]=0xff;

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));

	ZeroMemory(m_bBuckleServiceCharge,sizeof(m_bBuckleServiceCharge));

	//组件变量
	m_pITableFrame=NULL;
	m_pGameServiceOption=NULL;

	//服务控制
	m_hInst = NULL;
	m_pServerContro = NULL;
	m_hInst = LoadLibrary(TEXT("OxNewServerControl.dll"));
	if ( m_hInst )
	{
		typedef void * (*CREATE)(); 
		CREATE ServerControl = (CREATE)GetProcAddress(m_hInst,"CreateServerControl"); 
		if ( ServerControl )
		{
			m_pServerContro = static_cast<IServerControl*>(ServerControl());
		}
	}


	if(m_bFirstInit)
	{
		CString strName = GetFileDialogPath()+	"\\OxNewBattle.log";
		CFileFind findLogFile;
		if(findLogFile.FindFile(strName))
		{
			::DeleteFile(strName);
		}

		m_bFirstInit=false;
	}
	return;
}

//析构函数
CTableFrameSink::~CTableFrameSink(void)
{
	if( m_pServerContro )
	{
		delete m_pServerContro;
		m_pServerContro = NULL;
	}

	if( m_hInst )
	{
		FreeLibrary(m_hInst);
		m_hInst = NULL;
	}
}

//接口查询--检测相关信息版本
void * CTableFrameSink::QueryInterface(const IID & Guid, DWORD dwQueryVer)
{
	QUERYINTERFACE(ITableFrameSink,Guid,dwQueryVer);
	QUERYINTERFACE(ITableUserAction,Guid,dwQueryVer);
	QUERYINTERFACE_IUNKNOWNEX(ITableFrameSink,Guid,dwQueryVer);
	return NULL;
}

//初始化
bool CTableFrameSink::Initialization(IUnknownEx * pIUnknownEx)
{
	//查询接口
	ASSERT(pIUnknownEx!=NULL);
	m_pITableFrame=QUERY_OBJECT_PTR_INTERFACE(pIUnknownEx,ITableFrame);
	if (m_pITableFrame==NULL) return false;

	m_pITableFrame->SetStartMode(START_MODE_ALL_READY);

	//游戏配置
	m_pGameServiceAttrib=m_pITableFrame->GetGameServiceAttrib();
	m_pGameServiceOption=m_pITableFrame->GetGameServiceOption();

	//获取目录
	TCHAR szPath[MAX_PATH]=TEXT("");
	GetCurrentDirectory(CountArray(szPath),szPath);

	//读取配置
	TCHAR szFileName[MAX_PATH]=TEXT("");
	_sntprintf(szFileName,CountArray(szFileName),TEXT("%s\\OxConfig.ini"),szPath);

	//开始存储数目
	TCHAR szServerName[MAX_PATH]=TEXT("");
	_sntprintf(szServerName,CountArray(szServerName),TEXT("%s"),m_pGameServiceOption->szServerName);
	m_lStockScore=GetPrivateProfileInt(szServerName,TEXT("StorageStart"),0,szFileName);
	m_lStorageDeduct=GetPrivateProfileInt(szServerName,TEXT("StorageDeduct"),0,szFileName);
	m_lStockLimit=0;

	if(m_lStockScore>=m_lStockLimit)m_lStockLimit=0;

	if( m_lStorageDeduct < 0 || m_lStorageDeduct > 1000 )
		m_lStorageDeduct = 0;
	if ( m_lStorageDeduct > 1000 )
		m_lStorageDeduct = 1000;

	return true;
}

//复位桌子
void CTableFrameSink::RepositionSink()
{
	//游戏变量
	m_lExitScore=0;	
	m_wCurrentUser=INVALID_CHAIR;
	m_lDynamicScore=0;
	//用户状态
	ZeroMemory(m_cbDynamicJoin,sizeof(m_cbDynamicJoin));
	ZeroMemory(m_lTableScore,sizeof(m_lTableScore));
	ZeroMemory(m_cbPlayStatus,sizeof(m_cbPlayStatus));
	ZeroMemory(m_cbCallStatus,sizeof(m_cbCallStatus));
	ZeroMemory(m_bSpecialClient,sizeof(m_bSpecialClient));
	for(BYTE i=0;i<m_wPlayerCount;i++)m_cbOxCard[i]=0xff;

	//扑克变量
	ZeroMemory(m_cbHandCardData,sizeof(m_cbHandCardData));

	//下注信息
	ZeroMemory(m_lTurnMaxScore,sizeof(m_lTurnMaxScore));

	return;
}

//游戏状态
bool CTableFrameSink::IsUserPlaying(WORD wChairID)
{
	ASSERT(wChairID<m_wPlayerCount && m_cbPlayStatus[wChairID]==TRUE);
	if(wChairID<m_wPlayerCount && m_cbPlayStatus[wChairID]==TRUE)return true;
	return false;
}


//用户坐下
bool CTableFrameSink::OnActionUserSitDown(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false) m_HistoryScore.OnEventUserEnter(pIServerUserItem->GetChairID());

	if(m_pITableFrame->GetGameStatus()!=GS_TK_FREE)
		m_cbDynamicJoin[pIServerUserItem->GetChairID()]=true;

	return true;
}

//用户起立
bool CTableFrameSink::OnActionUserStandUp(WORD wChairID, IServerUserItem * pIServerUserItem, bool bLookonUser)
{
	//历史积分
	if (bLookonUser==false) 
	{
		m_HistoryScore.OnEventUserLeave(pIServerUserItem->GetChairID());
		m_cbDynamicJoin[pIServerUserItem->GetChairID()]=false;
		m_bSpecialClient[pIServerUserItem->GetChairID()]=false;
	}

	return true;
}

//游戏开始
bool CTableFrameSink::OnEventGameStart()
{

	//库存
	if(m_lStockScore>0)m_lStockScore=m_lStockScore-m_lStockScore*m_lStorageDeduct/1000;

	CString strInfo;
	strInfo.Format(TEXT("[第%d桌] 当前库存：%I64d"),m_pITableFrame->GetTableID()+1, m_lStockScore);
	NcaTextOut(strInfo);

	//设置状态
	m_pITableFrame->SetGameStatus(GS_TK_CALL);

	//用户状态
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);

		if(pIServerUser==NULL)
		{
			m_cbPlayStatus[i]=FALSE;
		}
		else
		{
			m_cbPlayStatus[i]=TRUE;
		}
		if(pIServerUser!=NULL&&pIServerUser->IsAndroidUser())
		{
			m_pITableFrame->SendTableData(i,SUB_S_BANKER_OPERATE);
		}
	}

	ZeroMemory(m_bBuckleServiceCharge,sizeof(m_bBuckleServiceCharge));

	//首局随机始叫
	if(m_wFisrtCallUser==INVALID_CHAIR)
	{
		m_wFisrtCallUser=rand()%m_wPlayerCount;
	}
	else
	{
		m_wFisrtCallUser=(m_wFisrtCallUser+1)%m_wPlayerCount;
	}

	//始叫用户
	while(m_cbPlayStatus[m_wFisrtCallUser]!=TRUE)
	{
		m_wFisrtCallUser=(m_wFisrtCallUser+1)%m_wPlayerCount;
	}

	//当前用户
	m_wCurrentUser=m_wFisrtCallUser;

	//设置变量
	CMD_S_CallBanker CallBanker;
	CallBanker.wCallBanker=m_wCurrentUser;
	CallBanker.bFirstTimes=true;

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]!=TRUE) continue;
		m_pITableFrame->SendTableData(i,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	}
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);

	//随机扑克
	CMD_S_AllCard AllCard={0};
	BYTE bTempArray[GAME_PLAYER*MAX_COUNT];
	m_GameLogic.RandCardList(bTempArray,sizeof(bTempArray));
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);	
		if(pIServerUser==NULL)continue;
		if(pIServerUser->IsAndroidUser())AllCard.bAICount[i] =true;

		//派发扑克
		CopyMemory(m_cbHandCardData[i],&bTempArray[i*MAX_COUNT],MAX_COUNT);
		CopyMemory(AllCard.cbCardData[i],&bTempArray[i*MAX_COUNT],MAX_COUNT);
	}

	//发送数据
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		IServerUserItem *pIServerUser=m_pITableFrame->GetTableUserItem(i);	
		if(pIServerUser==NULL)continue;
#ifndef _DEBUG
		if(CUserRight::IsGameCheatUser(pIServerUser->GetUserRight())==false || m_bSpecialClient[i]==false)continue;
#endif 
		m_pITableFrame->SendTableData(i,SUB_S_ALL_CARD,&AllCard,sizeof(AllCard));
	}
	return true;
}

//游戏结束
bool CTableFrameSink::OnEventGameConclude(WORD wChairID, IServerUserItem * pIServerUserItem, BYTE cbReason)
{
	switch (cbReason)
	{
	case GER_DISMISS:		//游戏解散
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<GAME_PLAYER);

			//构造数据
			CMD_S_GameEnd GameEnd = {0};

			//发送信息
			m_pITableFrame->SendTableData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

			//结束游戏
			m_pITableFrame->ConcludeGame(GAME_STATUS_FREE);

			return true;
		}
	case GER_NORMAL:		//常规结束
		{
			//定义变量
			CMD_S_GameEnd GameEnd;
			ZeroMemory(&GameEnd,sizeof(GameEnd));
			WORD wWinTimes[GAME_PLAYER],wWinCount[GAME_PLAYER];
			ZeroMemory(wWinCount,sizeof(wWinCount));
			ZeroMemory(wWinTimes,sizeof(wWinTimes));

			//保存扑克
			BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
			CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

			//庄家倍数
			ASSERT(m_cbOxCard[m_wBankerUser]<2);
			if(m_cbOxCard[m_wBankerUser]==TRUE)
				wWinTimes[m_wBankerUser]=m_GameLogic.GetTimes(cbUserCardData[m_wBankerUser],MAX_COUNT);
			else wWinTimes[m_wBankerUser]=1;

			//对比玩家
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)continue;

				ASSERT(m_cbOxCard[i]<2);

				//对比扑克
				if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,m_cbOxCard[i],m_cbOxCard[m_wBankerUser])) 
				{
					wWinCount[i]++;
					//获取倍数
					if(m_cbOxCard[i]==TRUE)
						wWinTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
					else wWinTimes[i]=1;
				}
				else
				{
					wWinCount[m_wBankerUser]++;
				}
			}

			//统计得分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)continue;

				if(wWinCount[i]>0)	//闲家胜利
				{
					GameEnd.lGameScore[i]=m_lTableScore[i]*wWinTimes[i];
					GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameScore[i];
					m_lTableScore[i]=0;
				}
				else					//庄家胜利
				{
					GameEnd.lGameScore[i]=(-1)*m_lTableScore[i]*wWinTimes[m_wBankerUser];
					GameEnd.lGameScore[m_wBankerUser]+=(-1)*GameEnd.lGameScore[i];
					m_lTableScore[i]=0;
				}
			}

			//闲家强退分数	
			GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;

			//离开用户
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_lTableScore[i]>0)GameEnd.lGameScore[i]=-m_lTableScore[i];
			}

			//修改积分
			tagScoreInfo ScoreInfoArray[GAME_PLAYER];
			ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));

			//积分税收
			for(WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;

				if(GameEnd.lGameScore[i]>0L)
				{
					GameEnd.lGameTax[i] = m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);//GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenueRatio/1000l;
					if(GameEnd.lGameTax[i]>0)
						GameEnd.lGameScore[i] -= GameEnd.lGameTax[i];
				}

				//历史积分
				m_HistoryScore.OnEventUserScore(i,GameEnd.lGameScore[i]);

				//保存积分
				ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];
				ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
				ScoreInfoArray[i].cbType = (GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
			}

			//发送信息
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE) continue;
				m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			}

			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
			
			TryWriteTableScore(ScoreInfoArray);
			
			//库存统计
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				//获取用户
				IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);
				if (pIServerUserIte==NULL) continue;

				//库存累计
				if(!pIServerUserIte->IsAndroidUser())
					m_lStockScore-=GameEnd.lGameScore[i];

			}

			//结束游戏
			m_pITableFrame->ConcludeGame(GS_TK_FREE);

			return true;
		}
	case GER_USER_LEAVE:		//用户强退
	case GER_NETWORK_ERROR:
		{
			//效验参数
			ASSERT(pIServerUserItem!=NULL);
			ASSERT(wChairID<m_wPlayerCount && (m_cbPlayStatus[wChairID]==TRUE||m_cbDynamicJoin[wChairID]==FALSE));

			//设置状态
			m_cbPlayStatus[wChairID]=FALSE;
			m_cbDynamicJoin[wChairID]=FALSE;

			//定义变量
			CMD_S_PlayerExit PlayerExit;
			PlayerExit.wPlayerID=wChairID;

			//发送信息
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(i==wChairID || (m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE))continue;
				m_pITableFrame->SendTableData(i,SUB_S_PLAYER_EXIT,&PlayerExit,sizeof(PlayerExit));
			}
			m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_PLAYER_EXIT,&PlayerExit,sizeof(PlayerExit));


			WORD wWinTimes[GAME_PLAYER];
			
			ZeroMemory(wWinTimes,sizeof(wWinTimes));

			if (m_pITableFrame->GetGameStatus()>GS_TK_CALL)
			{
				if (wChairID==m_wBankerUser)	//庄家强退
				{
					//定义变量
					CMD_S_GameEnd GameEnd;
					ZeroMemory(&GameEnd,sizeof(GameEnd));
					ZeroMemory(wWinTimes,sizeof(wWinTimes));

					BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
					CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(cbUserCardData));

					//得分倍数
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)continue;
						wWinTimes[i]=(m_pITableFrame->GetGameStatus()!=GS_TK_PLAYING)?(1):(m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT));
					}

					//统计得分 已下或没下
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || m_cbPlayStatus[i]==FALSE)continue;
						GameEnd.lGameScore[i]=m_lTableScore[i]*wWinTimes[i];
						GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameScore[i];
						m_lTableScore[i]=0;
					}

					//闲家强退分数 
					GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;

					//离开用户
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_lTableScore[i]>0)GameEnd.lGameScore[i]=-m_lTableScore[i];
					}

					//修改积分
					tagScoreInfo ScoreInfoArray[GAME_PLAYER];
					ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));

					//积分税收
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_cbPlayStatus[i]==FALSE && i!=m_wBankerUser)continue;
						
						if(GameEnd.lGameScore[i]>0L)
						{
							GameEnd.lGameTax[i]=m_pITableFrame->CalculateRevenue(i,GameEnd.lGameScore[i]);//GameEnd.lGameScore[i]*m_pGameServiceOption->wRevenueRatio/1000l;
							if(GameEnd.lGameTax[i]>0)
								GameEnd.lGameScore[i]-=GameEnd.lGameTax[i];
						}

						//保存积分
						ScoreInfoArray[i].lRevenue = GameEnd.lGameTax[i];
						ScoreInfoArray[i].lScore = GameEnd.lGameScore[i];

						if(i==m_wBankerUser)
							ScoreInfoArray[i].cbType =SCORE_TYPE_FLEE;
						else if(m_cbPlayStatus[i]==TRUE)
							ScoreInfoArray[i].cbType =(GameEnd.lGameScore[i]>0L)?SCORE_TYPE_WIN:SCORE_TYPE_LOSE;
					}
					
					TryWriteTableScore(ScoreInfoArray);

					//发送信息
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(i==m_wBankerUser || (m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE))continue;
						m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					}
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
						
					//写入库存
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_cbPlayStatus[i]==FALSE && i!=m_wBankerUser)continue;

						//获取用户
						IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(i);

						//库存累计
						if ((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser())) 
							m_lStockScore-=GameEnd.lGameScore[i];

					}
					//结束游戏
					m_pITableFrame->ConcludeGame(GS_TK_FREE);
				}
				else						//闲家强退
				{
					//已经下注
					if (m_lTableScore[wChairID]>0L)
					{
						ZeroMemory(wWinTimes,sizeof(wWinTimes));

						//用户扑克
						BYTE cbUserCardData[MAX_COUNT];
						CopyMemory(cbUserCardData,m_cbHandCardData[m_wBankerUser],MAX_COUNT);

						//用户倍数
						wWinTimes[m_wBankerUser]=(m_pITableFrame->GetGameStatus()==GS_TK_SCORE)?(1):(m_GameLogic.GetTimes(cbUserCardData,MAX_COUNT));

						//修改积分
						LONGLONG lScore=-m_lTableScore[wChairID]*wWinTimes[m_wBankerUser];
						m_lExitScore+=(-1*lScore);
						m_lTableScore[wChairID]=(-1*lScore);
						
						tagScoreInfo ScoreInfoArray[GAME_PLAYER];
						ZeroMemory(ScoreInfoArray,sizeof(ScoreInfoArray));
						ScoreInfoArray[wChairID].lScore = lScore;
						ScoreInfoArray[wChairID].cbType = SCORE_TYPE_FLEE;		
						
						TryWriteTableScore(ScoreInfoArray);

						//获取用户
						IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);
						
						//库存累计
						if ((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser())) 
							m_lStockScore-=lScore;


					}

					//玩家人数
					WORD wUserCount=0;
					for (WORD i=0;i<m_wPlayerCount;i++)if(m_cbPlayStatus[i]==TRUE)wUserCount++;

					//结束游戏
					if(wUserCount==1)
					{
						//定义变量
						CMD_S_GameEnd GameEnd;
						ZeroMemory(&GameEnd,sizeof(GameEnd));
						ASSERT(m_lExitScore>=0L); 

						
						//统计得分
						GameEnd.lGameScore[m_wBankerUser]+=m_lExitScore;
						GameEnd.lGameTax[m_wBankerUser]=m_pITableFrame->CalculateRevenue(m_wBankerUser,GameEnd.lGameScore[m_wBankerUser]);//GameEnd.lGameScore[m_wBankerUser]*m_pGameServiceOption->wRevenueRatio/1000l;
						GameEnd.lGameScore[m_wBankerUser]-=GameEnd.lGameTax[m_wBankerUser];

						//离开用户
						for (WORD i=0;i<m_wPlayerCount;i++)
						{
							if(m_lTableScore[i]>0)GameEnd.lGameScore[i]=-m_lTableScore[i];
						}

						//发送信息
						m_pITableFrame->SendTableData(m_wBankerUser,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
						m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

						for (WORD Zero=0;Zero<m_wPlayerCount;Zero++)if(m_lTableScore[Zero]!=0)break;
						if(Zero!=m_wPlayerCount)
						{
							//修改积分
							tagScoreInfo ScoreInfoArray[GAME_PLAYER];
							ZeroMemory(&ScoreInfoArray,sizeof(ScoreInfoArray));
							ScoreInfoArray[m_wBankerUser].lScore=GameEnd.lGameScore[m_wBankerUser];
							ScoreInfoArray[m_wBankerUser].lRevenue = GameEnd.lGameTax[m_wBankerUser];
							ScoreInfoArray[m_wBankerUser].cbType = SCORE_TYPE_WIN;

							TryWriteTableScore(ScoreInfoArray);

							//获取用户
							IServerUserItem * pIServerUserIte=m_pITableFrame->GetTableUserItem(wChairID);
							
							//库存累计
							if ((pIServerUserIte!=NULL)&&(!pIServerUserIte->IsAndroidUser())) 
								m_lStockScore-=GameEnd.lGameScore[m_wBankerUser];

						}

						//结束游戏
						m_pITableFrame->ConcludeGame(GS_TK_FREE);		
					}
					else if	(m_pITableFrame->GetGameStatus()==GS_TK_SCORE && m_lTableScore[wChairID]==0L)
					{
						OnUserAddScore(wChairID,0);
					}
					else if (m_pITableFrame->GetGameStatus()==GS_TK_PLAYING && m_cbOxCard[wChairID]==0xff)
					{
						OnUserOpenCard(wChairID,0);
					}
				}
			}
			else 
			{
				//玩家人数
				WORD wUserCount=0;
				for (WORD i=0;i<m_wPlayerCount;i++)if(m_cbPlayStatus[i]==TRUE)wUserCount++;

				//结束游戏
				if(wUserCount==1)
				{
					//定义变量
					CMD_S_GameEnd GameEnd;
					ZeroMemory(&GameEnd,sizeof(GameEnd));

					//发送信息
					for (WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
						m_pITableFrame->SendTableData(i,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));
					}
					m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_END,&GameEnd,sizeof(GameEnd));

					//结束游戏
					m_pITableFrame->ConcludeGame(GS_TK_FREE);			
				}
				else if(m_wCurrentUser==wChairID)OnUserCallBanker(wChairID,0);
			}
			return true;
		}
	}

	return false;
}

//发送场景
bool CTableFrameSink::OnEventSendGameScene(WORD wChiarID, IServerUserItem * pIServerUserItem, BYTE cbGameStatus, bool bSendSecret)
{
	switch (cbGameStatus)
	{
	case GAME_STATUS_FREE:		//空闲状态
		{
			//构造数据
			CMD_S_StatusFree StatusFree;
			ZeroMemory(&StatusFree,sizeof(StatusFree));

			//设置变量
			StatusFree.lCellScore=0L;

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusFree.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusFree.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//房间名称
			CopyMemory(StatusFree.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusFree.szGameRoomName));


			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusFree,sizeof(StatusFree));
		}
	case GS_TK_CALL:	//叫庄状态
		{
			//构造数据
			CMD_S_StatusCall StatusCall;
			ZeroMemory(&StatusCall,sizeof(StatusCall));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusCall.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusCall.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//设置变量
			StatusCall.wCallBanker=m_wCurrentUser;
			StatusCall.cbDynamicJoin=m_cbDynamicJoin[wChiarID];
			CopyMemory(StatusCall.cbPlayStatus,m_cbPlayStatus,sizeof(StatusCall.cbPlayStatus));

			//房间名称
			CopyMemory(StatusCall.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusCall.szGameRoomName));


			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusCall,sizeof(StatusCall));
		}
	case GS_TK_SCORE:	//下注状态
		{
			//构造数据
			CMD_S_StatusScore StatusScore;
			memset(&StatusScore,0,sizeof(StatusScore));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusScore.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusScore.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//加注信息
			StatusScore.lTurnMaxScore=GetUserMaxTurnScore(wChiarID);
			StatusScore.wBankerUser=m_wBankerUser;
			StatusScore.cbDynamicJoin=m_cbDynamicJoin[wChiarID];
			CopyMemory(StatusScore.cbPlayStatus,m_cbPlayStatus,sizeof(StatusScore.cbPlayStatus));

			//设置积分
			for (WORD i=0;i<m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;
				StatusScore.lTableScore[i]=m_lTableScore[i];
			}

			//房间名称
			CopyMemory(StatusScore.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusScore.szGameRoomName));

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusScore,sizeof(StatusScore));
		}
	case GS_TK_PLAYING:	//游戏状态
		{
			//构造数据
			CMD_S_StatusPlay StatusPlay;
			memset(&StatusPlay,0,sizeof(StatusPlay));

			//历史积分
			for (WORD i=0;i<GAME_PLAYER;i++)
			{
				tagHistoryScore * pHistoryScore=m_HistoryScore.GetHistoryScore(i);
				StatusPlay.lTurnScore[i]=pHistoryScore->lTurnScore;
				StatusPlay.lCollectScore[i]=pHistoryScore->lCollectScore;
			}

			//设置信息
			StatusPlay.lTurnMaxScore=GetUserMaxTurnScore(wChiarID);
			StatusPlay.wBankerUser=m_wBankerUser;
			StatusPlay.cbDynamicJoin=m_cbDynamicJoin[wChiarID];
			CopyMemory(StatusPlay.bOxCard,m_cbOxCard,sizeof(StatusPlay.bOxCard));
			CopyMemory(StatusPlay.cbPlayStatus,m_cbPlayStatus,sizeof(StatusPlay.cbPlayStatus));

			//房间名称
			CopyMemory(StatusPlay.szGameRoomName, m_pGameServiceOption->szServerName, sizeof(StatusPlay.szGameRoomName));

			//设置扑克
			for (WORD i=0;i< m_wPlayerCount;i++)
			{
				if(m_cbPlayStatus[i]==FALSE)continue;
				WORD j= i;
				StatusPlay.lTableScore[j]=m_lTableScore[j];
				CopyMemory(StatusPlay.cbHandCardData[j],m_cbHandCardData[j],MAX_COUNT);
			}

			//发送场景
			return m_pITableFrame->SendGameScene(pIServerUserItem,&StatusPlay,sizeof(StatusPlay));
		}
	}
	//效验错误
	ASSERT(FALSE);

	return false;
}

//定时器事件
bool CTableFrameSink::OnTimerMessage(DWORD dwTimerID, WPARAM wBindParam)
{
	switch(dwTimerID)
	{
	case IDI_SO_OPERATE:
		{
			//删除时间
			m_pITableFrame->KillGameTimer(IDI_SO_OPERATE);

			//游戏状态
			switch( m_pITableFrame->GetGameStatus() )
			{
			case GS_TK_CALL:			//用户叫庄
				{
					OnUserCallBanker(m_wCurrentUser, 0);
					break;
				}
			case GS_TK_SCORE:			//下注操作
				{
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_lTableScore[i]>0L || m_cbPlayStatus[i]==FALSE || i==m_wBankerUser)
							continue;

						if ( m_lTurnMaxScore[i] > 0 )
						{
							OnUserAddScore(i,m_lTurnMaxScore[i]/8);
						}
						else
						{
							OnUserAddScore(i,1);
						}
					}

					break;
				}
			case GS_TK_PLAYING:			//用户开牌
				{
					for(WORD i=0;i<m_wPlayerCount;i++)
					{
						if(m_cbOxCard[i]<2 || m_cbPlayStatus[i]==FALSE)continue;
						OnUserOpenCard(i, 0);
					}

					break;
				}
			default:
				{
					break;
				}
			}

			if(m_pITableFrame->GetGameStatus()!=GS_TK_FREE)
				m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
			return true;
		}
	}
	return false;
}

//游戏消息处理 
bool CTableFrameSink::OnGameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	bool bResult=false;
	switch (wSubCmdID)
	{
	case SUB_C_CALL_BANKER:			//用户叫庄
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_CallBanker));
			if (wDataSize!=sizeof(CMD_C_CallBanker)) return false;

			//变量定义
			CMD_C_CallBanker * pCallBanker=(CMD_C_CallBanker *)pDataBuffer;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(IsUserPlaying(pUserData->wChairID));
			if (!IsUserPlaying(pUserData->wChairID)) return false;

			//消息处理
			bResult=OnUserCallBanker(pUserData->wChairID,pCallBanker->bBanker);
			break;
		}
	case SUB_C_ADD_SCORE:			//用户加注
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_AddScore));
			if (wDataSize!=sizeof(CMD_C_AddScore)) return false;

			//变量定义
			CMD_C_AddScore * pAddScore=(CMD_C_AddScore *)pDataBuffer;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(IsUserPlaying(pUserData->wChairID));
			if (!IsUserPlaying(pUserData->wChairID)) return false;

			//消息处理
			bResult=OnUserAddScore(pUserData->wChairID,pAddScore->lScore);
			break;
		}
	case SUB_C_OPEN_CARD:			//用户摊牌
		{
			//效验数据
			ASSERT(wDataSize==sizeof(CMD_C_OxCard));
			if (wDataSize!=sizeof(CMD_C_OxCard)) return false;

			//变量定义
			CMD_C_OxCard * pOxCard=(CMD_C_OxCard *)pDataBuffer;

			//用户效验
			tagUserInfo * pUserData=pIServerUserItem->GetUserInfo();
			if (pUserData->cbUserStatus!=US_PLAYING) return true;

			//状态判断
			ASSERT(m_cbPlayStatus[pUserData->wChairID]!=FALSE);
			if(m_cbPlayStatus[pUserData->wChairID]==FALSE)return false;

			//消息处理
			bResult=OnUserOpenCard(pUserData->wChairID,pOxCard->bOX);
			break;
		}
	case SUB_C_SPECIAL_CLIENT_REPORT:  //特殊客户端
		{
			////效验数据
			//ASSERT(wDataSize==sizeof(CMD_C_SPECIAL_CLIENT_REPORT));
			//if (wDataSize!=sizeof(CMD_C_SPECIAL_CLIENT_REPORT)) return false;

			////变量定义
			//CMD_C_SPECIAL_CLIENT_REPORT * pSpecailClient=(CMD_C_SPECIAL_CLIENT_REPORT *)pDataBuffer;
			//if(pSpecailClient==NULL) return false;

			////设置变量
			//m_bSpecialClient[pSpecailClient->wUserChairID]=true;

			return true;
		}
	case SUB_C_AMDIN_COMMAND:
		{
			ASSERT(wDataSize==sizeof(CMD_C_AdminReq));
			if(wDataSize!=sizeof(CMD_C_AdminReq)) return false;

			ASSERT(pIServerUserItem->GetUserStatus()!=GS_TK_PLAYING);
			if (pIServerUserItem->GetUserStatus()==GS_TK_PLAYING) return true;

			if ( m_pServerContro == NULL)
				return true;

			return m_pServerContro->ServerControl(wSubCmdID, pDataBuffer, wDataSize, pIServerUserItem, m_pITableFrame);
		}
	}

	//操作定时器
	if(bResult)
	{
		m_pITableFrame->SetGameTimer(IDI_SO_OPERATE,TIME_SO_OPERATE,1,0);
		return true;
	}

	return false;
}

//框架消息处理
bool CTableFrameSink::OnFrameMessage(WORD wSubCmdID, void * pDataBuffer, WORD wDataSize, IServerUserItem * pIServerUserItem)
{
	return false;
}


//叫庄事件
bool CTableFrameSink::OnUserCallBanker(WORD wChairID, BYTE bBanker)
{
	//状态效验
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_CALL);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_CALL) return true;
	ASSERT(m_wCurrentUser==wChairID);
	if (m_wCurrentUser!=wChairID) return true;

	//设置变量
	m_cbCallStatus[wChairID]=TRUE;

	//叫庄人数
	WORD wCallUserCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]==TRUE && m_cbCallStatus[i]==TRUE) wCallUserCount++;
		else if(m_cbPlayStatus[i]!=TRUE) wCallUserCount++;
	}

	if(bBanker==FALSE && wCallUserCount==m_wPlayerCount)
	{
		m_wBankerUser=m_wFisrtCallUser;
	}

	

	//下注开始
	if(bBanker==TRUE || wCallUserCount==m_wPlayerCount)
	{
		//始叫用户
		if(bBanker==TRUE)
		{
			m_wBankerUser=wChairID;
		}
		m_wCurrentUser=INVALID_CHAIR;
		
		//过滤最后一个叫庄用户强退情况
		while(m_cbPlayStatus[m_wBankerUser]==FALSE)
		{
			m_wBankerUser=(m_wBankerUser+1)%GAME_PLAYER;
		}
		m_bBuckleServiceCharge[m_wBankerUser]=true;
		//设置状态
		m_pITableFrame->SetGameStatus(GS_TK_SCORE);

		//庄家积分
		IServerUserItem *pIServerUserItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
		LONGLONG lBankerScore=pIServerUserItem->GetUserScore();

		//最大下注
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]!=TRUE || i==m_wBankerUser)continue;

			//下注变量 客户要求
			m_lTurnMaxScore[i]=GetUserMaxTurnScore(i);
		}

		//设置变量
		CMD_S_GameStart GameStart;
		GameStart.wBankerUser=m_wBankerUser;
		GameStart.lTurnMaxScore=0;

		//发送数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
			GameStart.lTurnMaxScore=m_lTurnMaxScore[i];
			m_pITableFrame->SendTableData(i,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_GAME_START,&GameStart,sizeof(GameStart));
	}
	else		 //用户叫庄
	{
		//查找下个玩家
		do{
			m_wCurrentUser=(m_wCurrentUser+1)%m_wPlayerCount;
		}while(m_cbPlayStatus[m_wCurrentUser]==FALSE);

		//设置变量
		CMD_S_CallBanker CallBanker;
		CallBanker.wCallBanker=m_wCurrentUser;
		CallBanker.bFirstTimes=false;

		//发送数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
			m_pITableFrame->SendTableData(i,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_CALL_BANKER,&CallBanker,sizeof(CallBanker));
	}
	return true;
}

//加注事件
bool CTableFrameSink::OnUserAddScore(WORD wChairID, LONGLONG lScore)
{
	//状态效验
	ASSERT(m_pITableFrame->GetGameStatus()==GS_TK_SCORE);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_SCORE) return true;

	//金币效验
	if(m_cbPlayStatus[wChairID]==TRUE)
	{
		ASSERT(lScore>0 && lScore<=m_lTurnMaxScore[wChairID]);
		if (lScore<=0 || lScore>m_lTurnMaxScore[wChairID]) return false;
	}
	else //没下注玩家强退
	{
		ASSERT(lScore==0);
		if (lScore!=0) return false;
	}

	if(!UserCanAddScore(wChairID,lScore)) return false;

	if(lScore>0L)
	{
		//下注金币
		m_lTableScore[wChairID]=lScore;
		m_bBuckleServiceCharge[wChairID]=true;
		//构造数据
		CMD_S_AddScore AddScore;
		AddScore.wAddScoreUser=wChairID;
		AddScore.lAddScoreCount=m_lTableScore[wChairID];

		//发送数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
			m_pITableFrame->SendTableData(i,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_ADD_SCORE,&AddScore,sizeof(AddScore));
	}

	//下注人数
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_lTableScore[i]>0L && m_cbPlayStatus[i]==TRUE)bUserCount++;
		else if(m_cbPlayStatus[i]==FALSE || i==m_wBankerUser)bUserCount++;
	}

	//闲家全到
	if(bUserCount==m_wPlayerCount)
	{
		//设置状态
		m_pITableFrame->SetGameStatus(GS_TK_PLAYING);

		//构造数据
		CMD_S_SendCard SendCard;
		ZeroMemory(SendCard.cbCardData,sizeof(SendCard.cbCardData));

		//分析扑克
		AnalyseCard();

		//控制
		bool bControl = false;
		BYTE bMaxCount=0;
		if( m_pServerContro != NULL && m_pServerContro->NeedControl() )//在此处加入控制
		{
			if(m_pServerContro->ControlResult(m_cbHandCardData))
			{
				bControl=true;
				m_pServerContro->CompleteControl();

			}			
		}

		//发送扑克
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE)continue;

			//派发扑克
			CopyMemory(SendCard.cbCardData[i],m_cbHandCardData[i],MAX_COUNT);
		}

		//发送数据
		for (WORD i=0;i< m_wPlayerCount;i++)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
			m_pITableFrame->SendTableData(i,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
		}
		m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_SEND_CARD,&SendCard,sizeof(SendCard));
	}

	return true;
}

//摊牌事件
bool CTableFrameSink::OnUserOpenCard(WORD wChairID, BYTE bOx)
{
	//状态效验
	ASSERT (m_pITableFrame->GetGameStatus()==GS_TK_PLAYING);
	if (m_pITableFrame->GetGameStatus()!=GS_TK_PLAYING) return true;
	if (m_cbOxCard[wChairID]!=0xff) return true;

	//效验数据
	ASSERT(bOx<=TRUE);
	if(bOx>TRUE)return false;

	//效验数据
	if(bOx)
	{
		ASSERT(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0);
		if(!(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>0))return false;
	}
	else if(m_cbPlayStatus[wChairID]==TRUE)
	{
		if(m_GameLogic.GetCardType(m_cbHandCardData[wChairID],MAX_COUNT)>=OX_THREE_SAME)bOx=TRUE;
	}

	//牛牛数据
	m_cbOxCard[wChairID] = bOx;

	//摊牌人数
	BYTE bUserCount=0;
	for(WORD i=0;i<m_wPlayerCount;i++)
	{
		if(m_cbOxCard[i]<2 && m_cbPlayStatus[i]==TRUE)bUserCount++;
		else if(m_cbPlayStatus[i]==FALSE)bUserCount++;
	}

	 //构造变量
	CMD_S_Open_Card OpenCard;
	ZeroMemory(&OpenCard,sizeof(OpenCard));

	//设置变量
	OpenCard.bOpen=bOx;
	OpenCard.wPlayerID=wChairID;

	//发送数据
	for (WORD i=0;i< m_wPlayerCount;i++)
	{
		if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
		m_pITableFrame->SendTableData(i,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));
	}
	m_pITableFrame->SendLookonData(INVALID_CHAIR,SUB_S_OPEN_CARD,&OpenCard,sizeof(OpenCard));	

	//结束游戏
	if(bUserCount == m_wPlayerCount)
	{
		return OnEventGameConclude(INVALID_CHAIR,NULL,GER_NORMAL);
	}

	return true;
}

//扑克分析
void CTableFrameSink::AnalyseCard()
{
	//机器人数
	bool bIsAiBanker = false;
	WORD wAiCount = 0;
	WORD wPlayerCount = 0;
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		//获取用户
		IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);
		if (pIServerUserItem!=NULL)
		{
			if(m_cbPlayStatus[i]==FALSE&&m_cbDynamicJoin[i]==FALSE)continue;
			if(pIServerUserItem->IsAndroidUser()) 
			{
				wAiCount++ ;
				if(!bIsAiBanker && i==m_wBankerUser)bIsAiBanker = true;
			}
			wPlayerCount++; 
		}
	}

	//全部机器
	if(wPlayerCount == wAiCount || wAiCount==0)return;

	//扑克变量
	BYTE cbUserCardData[GAME_PLAYER][MAX_COUNT];
	CopyMemory(cbUserCardData,m_cbHandCardData,sizeof(m_cbHandCardData));

	//牛牛数据
	BOOL bUserOxData[GAME_PLAYER];
	ZeroMemory(bUserOxData,sizeof(bUserOxData));
	for(WORD i=0;i<GAME_PLAYER;i++)
	{
		if(m_cbPlayStatus[i]==FALSE)continue;
		bUserOxData[i] = (m_GameLogic.GetCardType(cbUserCardData[i],MAX_COUNT)>0)?TRUE:FALSE;
	}

	//排列扑克
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		m_GameLogic.SortCardList(cbUserCardData[i],MAX_COUNT);
	}

	//变量定义
	LONGLONG lAndroidScore=0;

	//倍数变量
	BYTE cbCardTimes[GAME_PLAYER];
	ZeroMemory(cbCardTimes,sizeof(cbCardTimes));

	//查找倍数
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		if (m_cbPlayStatus[i]==TRUE)
		{
			cbCardTimes[i]=m_GameLogic.GetTimes(cbUserCardData[i],MAX_COUNT);
		}
	}

	//机器庄家
	if(bIsAiBanker)
	{
		//对比扑克
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//用户过滤
			if ((i==m_wBankerUser)||(m_cbPlayStatus[i]==FALSE)) continue;

			//获取用户
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//机器过滤
			if ((pIServerUserItem!=NULL)&&(pIServerUserItem->IsAndroidUser())) continue;

			//对比扑克
			if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
			{
				lAndroidScore-=cbCardTimes[i]*m_lTableScore[i];
			}
			else
			{
				lAndroidScore+=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
			}
		}
	}
	else//用户庄家
	{
		//对比扑克
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//获取用户
			IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(i);

			//用户过滤
			if ((i==m_wBankerUser)||(pIServerUserItem==NULL)||!(pIServerUserItem->IsAndroidUser())) continue;

			//对比扑克
			if (m_GameLogic.CompareCard(cbUserCardData[i],cbUserCardData[m_wBankerUser],MAX_COUNT,bUserOxData[i],bUserOxData[m_wBankerUser])==true)
			{
				lAndroidScore+=cbCardTimes[i]*m_lTableScore[i];
			}
			else
			{
				lAndroidScore-=cbCardTimes[m_wBankerUser]*m_lTableScore[i];
			}
		}
	}

	//库存判断
	bool bCon[]={((m_lStockScore+lAndroidScore)<0),false};
	bool bMaxOrMin[]={bIsAiBanker,!bIsAiBanker};
	for(WORD j=0;j<CountArray(bCon);j++)
	{
		if(!bCon[j])continue;
		//变量定义
		WORD wWinUser=INVALID_CHAIR;

		//判断最大
		bool bCardMax = bMaxOrMin[j];

		//查找数据
		for (WORD i=0;i<m_wPlayerCount;i++)
		{
			//用户过滤
			if (m_cbPlayStatus[i]==FALSE) continue;

			//设置用户
			if (wWinUser==INVALID_CHAIR)
			{
				wWinUser=i;
				continue;
			}

			//对比扑克
			if (m_GameLogic.CompareCard(cbUserCardData[i],m_cbHandCardData[wWinUser],MAX_COUNT,bUserOxData[i],bUserOxData[wWinUser])==bCardMax)
			{
				wWinUser=i;
			}
		}

		//交换数据
		BYTE cbTempData[MAX_COUNT];
		CopyMemory(cbTempData,m_cbHandCardData[m_wBankerUser],MAX_COUNT);
		CopyMemory(m_cbHandCardData[m_wBankerUser],m_cbHandCardData[wWinUser],MAX_COUNT);
		CopyMemory(m_cbHandCardData[wWinUser],cbTempData,MAX_COUNT);
		if(bCon[j])break;
	}
	return;
}


//查询是否扣服务费
bool CTableFrameSink::QueryBuckleServiceCharge(WORD wChairID)
{
	for (BYTE i=0;i<GAME_PLAYER;i++)
	{
		IServerUserItem *pUserItem=m_pITableFrame->GetTableUserItem(i);
		if(pUserItem==NULL) continue;
		
		if (m_bBuckleServiceCharge[i]&&i==wChairID)
		{
			return true;
		}
		
	}
	return false;
}


bool CTableFrameSink::TryWriteTableScore(tagScoreInfo ScoreInfoArray[])
{
	//修改积分
	tagScoreInfo ScoreInfo[GAME_PLAYER];
	ZeroMemory(&ScoreInfo,sizeof(ScoreInfo));
	memcpy(&ScoreInfo,ScoreInfoArray,sizeof(ScoreInfo));
	//记录异常
	LONGLONG beforeScore[GAME_PLAYER];
	ZeroMemory(beforeScore,sizeof(beforeScore));
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		IServerUserItem *pItem=m_pITableFrame->GetTableUserItem(i);
		if(pItem!=NULL)
		{
			beforeScore[i]=pItem->GetUserScore();

		}
	}
	m_pITableFrame->WriteTableScore(ScoreInfo,CountArray(ScoreInfo));
	LONGLONG afterScore[GAME_PLAYER];
	ZeroMemory(afterScore,sizeof(afterScore));
	for (WORD i=0;i<m_wPlayerCount;i++)
	{
		IServerUserItem *pItem=m_pITableFrame->GetTableUserItem(i);
		if(pItem!=NULL)
		{
			afterScore[i]=pItem->GetUserScore();

			if(afterScore[i]<0)
			{
				//异常写入日志

				CString strInfo;
				strInfo.Format(TEXT("[%s] 出现负分"),pItem->GetNickName());
				NcaTextOut(strInfo);

				strInfo.Format(TEXT("写分前分数：%I64d"),beforeScore[i]);
				NcaTextOut(strInfo);

				strInfo.Format(TEXT("写分信息：写入积分 %I64d，税收 %I64d"),ScoreInfoArray[i].lScore,ScoreInfoArray[i].lRevenue);
				NcaTextOut(strInfo);

				strInfo.Format(TEXT("写分后分数：%I64d"),afterScore[i]);
				NcaTextOut(strInfo);

			}

		}
	}
	return true;
}

//最大下分
SCORE CTableFrameSink::GetUserMaxTurnScore(WORD wChairID)
{

	SCORE lMaxTurnScore=0L;
	if(wChairID==m_wBankerUser)  return 0;
	//庄家积分
	IServerUserItem *pIBankerItem=m_pITableFrame->GetTableUserItem(m_wBankerUser);
	LONGLONG lBankerScore=0L;
	if(pIBankerItem!=NULL)
		lBankerScore=pIBankerItem->GetUserScore();

	//玩家人数
	WORD wUserCount=0;
	for (WORD i=0;i<m_wPlayerCount;i++)
		if(m_cbPlayStatus[i]==TRUE )wUserCount++;

	//获取用户
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

	if(pIServerUserItem!=NULL)
	{
		//获取积分
		LONGLONG lScore=pIServerUserItem->GetUserScore();

		lMaxTurnScore=__min(lBankerScore/(wUserCount-1)/MAX_TIMES,lScore/MAX_TIMES);
	}

	return lMaxTurnScore;
	

}

//是否可加
bool CTableFrameSink::UserCanAddScore(WORD wChairID, LONGLONG lAddScore)
{

	//获取用户
	IServerUserItem * pIServerUserItem=m_pITableFrame->GetTableUserItem(wChairID);

	if(pIServerUserItem!=NULL)
	{
		//获取积分
		LONGLONG lScore=pIServerUserItem->GetUserScore();

		if(lAddScore>lScore/MAX_TIMES)
		{
			return false;
		}
	}
	return true;

}

//查询限额
SCORE CTableFrameSink::QueryConsumeQuota(IServerUserItem * pIServerUserItem)
{
	SCORE consumeQuota=0L;
	/*SCORE lMinTableScore=m_pGameServiceOption->lMinTableScore;
	if(m_pITableFrame->GetGameStatus()==GAME_STATUS_FREE)
	{
		consumeQuota=pIServerUserItem->GetUserScore()-lMinTableScore;

	}*/
	return consumeQuota;
}


//////////////////////////////////////////////////////////////////////////
