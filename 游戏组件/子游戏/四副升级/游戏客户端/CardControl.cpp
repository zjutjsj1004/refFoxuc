#include "StdAfx.h"
#include "Resource.h"
#include "CardControl.h"


#define LINE_SPACE					50									//间隔行距


//////////////////////////////////////////////////////////////////////////
//静态变量

//bool					CCardControl::m_bLoad=false;					//加载标志
//CSize					CCardControl::m_CardSize;						//扑克大小
//CBitImage				CCardControl::m_ImageCard;						//图片资源


//////////////////////////////////////////////////////////////////////////

BEGIN_MESSAGE_MAP(CCardControl, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_SETCURSOR()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()


//////////////////////////////////////////////////////////////////////////

//构造函数
CCardControl::CCardControl()
{
	//配置变量
	m_bDisplay=false;
	m_bHorizontal=true;
	m_bPositively=false;

	//间隔变量
	m_dwCardHSpace=DEF_CARD_H_SPACE;
	m_dwCardVSpace=DEF_CARD_V_SPACE;
	m_dwShootAltitude=DEF_CARD_SHOOT;
	m_nMaxWidth = 0;

	//位置变量
	m_XCollocateMode=enXCenter;
	m_YCollocateMode=enYCenter;
	m_BenchmarkPos.SetPoint(0,0);

	//运行变量
	m_dwCurrentIndex=0xFFFFFFFF;

	//内部数据
	m_pSinkWindow=NULL;

	m_bLoad=false;

	//资源变量
	if (m_bLoad==false)
	{
		//加载资源
		m_bLoad=true;
		m_ImageCard.LoadFromResource(AfxGetInstanceHandle(),IDB_CARD);

		//获取大小
		//CImageHandle CardImageHandle(&m_ImageCard);
		m_CardSize.cx=m_ImageCard.GetWidth()/13;
		m_CardSize.cy=m_ImageCard.GetHeight()/5;
	}

	return;
}

//析构函数
CCardControl::~CCardControl()
{
}

//设置窗口
void CCardControl::SetSinkWindow(CWnd * pSinkWindow)
{
	//设置变量
	m_pSinkWindow=pSinkWindow;

	return;
}

//设置方向
void CCardControl::SetDirection(bool bHorizontal)
{
	//状态判断
	if (m_bHorizontal==bHorizontal) return;

	//设置变量
	m_bHorizontal=bHorizontal;

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return;
}

//弹起扑克
void CCardControl::ShootAllCard(bool bShoot)
{
	//变量定义
	bool bRectify=false;
	tagCardItem * pCardItem=NULL;
	DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();

	//设置扑克
	for (DWORD i=0;i<dwCardCount;i++)
	{
		//获取扑克
		pCardItem=&m_CardDataItem[i];

		//扑克分析
		if (pCardItem->bShoot!=bShoot)
		{
			bRectify=true;
			pCardItem->bShoot=bShoot;
		}
	}

	//调整控件
	if (bRectify==true) RectifyControl();

	return;
}

//设置显示
void CCardControl::SetDisplayFlag(bool bDisplay)
{
	//状态判断
	if (m_bDisplay==bDisplay) return;

	//设置变量
	m_bDisplay=bDisplay;

	//重画控件
	if (m_hWnd!=NULL) RectifyControl();

	return;
}

//设置响应
void CCardControl::SetPositively(bool bPositively)
{
	//设置变量
	m_bPositively=bPositively;

	return;
}

//设置间距
void CCardControl::SetCardSpace(DWORD dwCardHSpace, DWORD dwCardVSpace, DWORD dwShootAltitude)
{
	//设置变量
	m_dwCardHSpace=dwCardHSpace;
	m_dwCardVSpace=dwCardVSpace;
	m_dwShootAltitude=dwShootAltitude;

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return;
}

//基准位置
void CCardControl::SetBenchmarkPos(int nXPos, int nYPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos.x=nXPos;
	m_BenchmarkPos.y=nYPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return;
}

//基准位置
void CCardControl::SetBenchmarkPos(const CPoint & BenchmarkPos, enXCollocateMode XCollocateMode, enYCollocateMode YCollocateMode)
{
	//设置变量
	m_BenchmarkPos=BenchmarkPos;
	m_XCollocateMode=XCollocateMode;
	m_YCollocateMode=YCollocateMode;

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return;
}

//获取数目
DWORD CCardControl::GetCardCount()
{
	return (DWORD)m_CardDataItem.GetCount();
}

//获取扑克
DWORD CCardControl::GetShootCard(BYTE cbCardData[], DWORD dwMaxCount)
{
	//变量定义
	DWORD bShootCount=0L;
	DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();

	//搜索扑克
	tagCardItem * pCardItem=NULL;
	for (DWORD i=0;i<dwCardCount;i++)
	{
		//获取扑克
		pCardItem=&m_CardDataItem[i];

		//扑克分析
		if (pCardItem->bShoot==true) cbCardData[bShootCount++]=pCardItem->cbCardData;
	}

	return bShootCount;
}

//弹起扑克
DWORD CCardControl::SetShootCard(BYTE cbCardData[], DWORD dwCardCount)
{
	//变量定义
	bool bChangeStatus=false;

	//收起扑克
	for (INT_PTR i=0;i<m_CardDataItem.GetCount();i++) 
	{
		if (m_CardDataItem[i].bShoot==true)
		{
			bChangeStatus=true;
			m_CardDataItem[i].bShoot=false;
		}
	}

	//弹起扑克
	for (DWORD i=0;i<dwCardCount;i++)
	{
		for (INT_PTR j=0;j<m_CardDataItem.GetCount();j++)
		{
			tagCardItem * pCardItem=&m_CardDataItem[j];
			if ((pCardItem->bShoot==false)&&(pCardItem->cbCardData==cbCardData[i])) 
			{
				bChangeStatus=true;
				pCardItem->bShoot=true;
				break;
			}
		}
	}

	//设置界面
	if (m_hWnd!=NULL) RectifyControl();

	return dwCardCount;
}

//设置扑克
DWORD CCardControl::SetCardData(const BYTE cbCardData[], DWORD dwCardCount)
{
	//设置扑克
	m_CardDataItem.SetSize(dwCardCount);
	for (DWORD i=0;i<dwCardCount;i++)
	{
		m_CardDataItem[i].bShoot=false;
		m_CardDataItem[i].cbCardData=cbCardData[i];
	}

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();

	return dwCardCount;
}

//获取扑克
DWORD CCardControl::GetCardData(tagCardItem CardItemArray[], DWORD dwMaxCount)
{
	//效验参数
	ASSERT(dwMaxCount>=(DWORD)m_CardDataItem.GetCount());

	//拷贝扑克
	DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();
	tagCardItem * pCardItemData=m_CardDataItem.GetData();
	CopyMemory(CardItemArray,pCardItemData,dwCardCount*sizeof(tagCardItem));

	return dwCardCount;
}

//设置扑克
DWORD CCardControl::SetCardItem(const tagCardItem CardItemArray[], DWORD dwCardCount)
{
	//设置扑克
	m_CardDataItem.SetSize(dwCardCount);
	CopyMemory(m_CardDataItem.GetData(),CardItemArray,dwCardCount*sizeof(tagCardItem));

	//调整控件
	if (m_hWnd!=NULL) RectifyControl();
 
	return 0;
}

//调整位置
void CCardControl::RectifyControl()
{
	//变量定义
	DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();

	//变量定义
	UINT nMaxCount = 0xFFFFFFFF;
	if( m_bHorizontal && m_nMaxWidth > 0 )
		nMaxCount = (m_nMaxWidth-m_CardSize.cx+m_dwCardHSpace)/m_dwCardHSpace;

	//计算大小
	CSize ControlSize;
	if (m_bHorizontal==true)
	{
		ControlSize.cy=m_CardSize.cy+m_dwShootAltitude;
		if( dwCardCount > nMaxCount ) ControlSize.cy += dwCardCount/nMaxCount*LINE_SPACE;

		ControlSize.cx=(dwCardCount>0)?(m_CardSize.cx+(dwCardCount-1)*m_dwCardHSpace):0;
		if( m_nMaxWidth>0 && (UINT)ControlSize.cx > m_nMaxWidth ) 
			ControlSize.cx = (nMaxCount-1)*m_dwCardHSpace+m_CardSize.cx;
	}
	else
	{
		ControlSize.cx=m_CardSize.cx;
		ControlSize.cy=(dwCardCount>0)?(m_CardSize.cy+(dwCardCount-1)*m_dwCardVSpace):0;
	}

	//横向位置
	int nXPos=0;
	switch (m_XCollocateMode)
	{
	case enXLeft:	{ nXPos=m_BenchmarkPos.x; break; }
	case enXCenter: { nXPos=m_BenchmarkPos.x-ControlSize.cx/2; break; }
	case enXRight:	{ nXPos=m_BenchmarkPos.x-ControlSize.cx; break; }
	}

	//竖向位置
	int nYPos=0;
	switch (m_YCollocateMode)
	{
	case enYTop:	{ nYPos=m_BenchmarkPos.y; break; }
	case enYCenter: { nYPos=m_BenchmarkPos.y-ControlSize.cy/2; break; }
	case enYBottom: { nYPos=m_BenchmarkPos.y-ControlSize.cy; break; }
	}

	//移动位置
	SetWindowPos(NULL,nXPos,nYPos,ControlSize.cx,ControlSize.cy,SWP_NOZORDER);

	//变量定义
	CRgn CardRegion,SignedRegion;
	tagCardItem * pCardItem=NULL;

	//设置区域
	CardRegion.CreateRectRgn(0,0,0,0);
	int nMinusSpace = 0;
	for (DWORD i=0;i<dwCardCount;i++)
	{
		//获取扑克
		pCardItem=&m_CardDataItem[i];

		//间隙过滤
		if (pCardItem->cbCardData==255) continue;

		//计算位置
		if (m_bHorizontal==true)
		{
			nXPos=m_dwCardHSpace*(i%nMaxCount);
			nYPos=(pCardItem->bShoot==false)?(i/nMaxCount)*LINE_SPACE+m_dwShootAltitude:(i/nMaxCount)*LINE_SPACE;
		}
		else 
		{
			nXPos=0;
			nYPos=m_dwCardVSpace*i;
		}

		//合并区域
		SignedRegion.CreateRoundRectRgn(nXPos,nYPos,nXPos+m_CardSize.cx+1,nYPos+m_CardSize.cy+1,3,3);
		CardRegion.CombineRgn(&CardRegion,&SignedRegion,RGN_OR);
		SignedRegion.DeleteObject();
	}

	//设置区域
	SetWindowRgn(CardRegion,TRUE);
	m_CardRegion.DeleteObject();
	m_CardRegion.Attach(CardRegion.Detach());

	//重画界面
	Invalidate(FALSE);

	return;
}

//索引切换
DWORD CCardControl::SwitchCardPoint(const CPoint & MousePoint)
{
	if (m_bHorizontal==true)
	{
		//变量定义
		DWORD dwCardCount=(DWORD)m_CardDataItem.GetCount();
		UINT nMaxCount = 0xFFFFFFFF;
		if( m_bHorizontal && m_nMaxWidth > 0 )
			nMaxCount = (m_nMaxWidth-m_CardSize.cx+m_dwCardHSpace)/m_dwCardHSpace;

		//基准位置
		INT nXPos=MousePoint.x;
		INT nYPos=MousePoint.y;

		//位置判断
		for (DWORD i=0;i<dwCardCount;i++)
		{
			//索引定义
			DWORD wIndex=dwCardCount-i-1;

			//间隙过滤
			if (m_CardDataItem[wIndex].cbCardData==255) continue;

			//位置计算
			INT nXCardPos=m_dwCardHSpace*(wIndex%nMaxCount);
			INT nYCardPos=(m_CardDataItem[wIndex].bShoot==false)?(wIndex/nMaxCount)*LINE_SPACE+m_dwShootAltitude:(wIndex/nMaxCount)*LINE_SPACE;

			//区域判断
			if ((nXPos<nXCardPos)||(nXPos>nXCardPos+m_CardSize.cx)) continue;
			if ((nYPos<nYCardPos)||(nYPos>nYCardPos+m_CardSize.cy)) continue;

			return wIndex;
		}
	}

	return DWORD(-1);
}

//重画函数
void CCardControl::OnPaint()
{
	CPaintDC dc(this);

	//获取位置
	CRect rcClient;
	GetClientRect(&rcClient);

	//加载位图
	//CImageHandle HandleCard(&m_ImageCard);

	//创建位图
	CBitmap BufferImage;
	BufferImage.CreateCompatibleBitmap(&dc,rcClient.Width(),rcClient.Height());

	//创建 DC
	CDC BufferDC;
	BufferDC.CreateCompatibleDC(&dc);
	BufferDC.SelectObject(&BufferImage);

	//变量定义
	UINT nMaxCount = 0xFFFFFFFF;
	if( m_bHorizontal && m_nMaxWidth > 0 )
		nMaxCount = (m_nMaxWidth-m_CardSize.cx+m_dwCardHSpace)/m_dwCardHSpace;

	//绘画扑克
	DWORD dwXImagePos,dwYImagePos;
	DWORD dwXScreenPos,dwYScreenPos;
	for (INT i=0;i<m_CardDataItem.GetCount();i++)
	{
		//获取扑克
		tagCardItem * pCardItem=&m_CardDataItem[i];

		//间隙过滤
		if (pCardItem->cbCardData==255) continue;

		//图片位置
		if ((m_bDisplay==true)&&(pCardItem->cbCardData!=0))
		{
			if ((pCardItem->cbCardData==0x4E)||(pCardItem->cbCardData==0x4F))
			{
				dwYImagePos=((pCardItem->cbCardData&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
				dwXImagePos=((pCardItem->cbCardData&CARD_MASK_VALUE)%14)*m_CardSize.cx;
			}
			else
			{
				dwXImagePos=((pCardItem->cbCardData&CARD_MASK_VALUE)-1)*m_CardSize.cx;
				dwYImagePos=((pCardItem->cbCardData&CARD_MASK_COLOR)>>4)*m_CardSize.cy;
			}
		}
		else
		{
			dwXImagePos=m_CardSize.cx*2;
			dwYImagePos=m_CardSize.cy*4;
		}

		//屏幕位置
		if (m_bHorizontal==true) 
		{
			dwXScreenPos=m_dwCardHSpace*(i%nMaxCount);
			dwYScreenPos=(pCardItem->bShoot==false)?(i/nMaxCount)*LINE_SPACE+m_dwShootAltitude:(i/nMaxCount)*LINE_SPACE;
		}
		else
		{
			dwXScreenPos=0;
			dwYScreenPos=m_dwCardVSpace*i;
		}

		//绘画扑克
		m_ImageCard.TransDrawImage(&BufferDC,dwXScreenPos,dwYScreenPos,m_CardSize.cx,m_CardSize.cy,dwXImagePos,dwYImagePos,RGB(255,0,255));
	}

	//绘画界面
	dc.BitBlt(0,0,rcClient.Width(),rcClient.Height(),&BufferDC,0,0,SRCCOPY);

	//清理资源
	BufferDC.DeleteDC();
	BufferImage.DeleteObject();

	return;
}

//建立消息
int CCardControl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (__super::OnCreate(lpCreateStruct)==-1) return -1;

	//设置背景
	SetClassLong(m_hWnd,GCL_HBRBACKGROUND,NULL);

	return 0;
}

//鼠标消息
void CCardControl::OnRButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnRButtonUp(nFlags, Point);

	//状态判断
	if ((m_bHorizontal==false)||(m_bPositively==false)) return;

	//发送消息
	if (m_pSinkWindow!=NULL) 
	{
		CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
		if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_RIGHT_HIT_CARD,0,0);

		//m_pSinkWindow->PostMessage(IDM_RIGHT_HIT_CARD,0,0);
	}

	return;
}

//鼠标消息
void CCardControl::OnLButtonUp(UINT nFlags, CPoint Point)
{
	__super::OnLButtonUp(nFlags, Point);

	//状态判断
	if (m_dwCurrentIndex==0xFFFFFFFF) return;
	if ((m_bHorizontal==false)||(m_bPositively==false)) return;

	//获取索引
	DWORD dwCurrentIndex=SwitchCardPoint(Point);
	if (dwCurrentIndex!=m_dwCurrentIndex) return;

	//设置控件
	tagCardItem * pCardItem=&m_CardDataItem[dwCurrentIndex];
	if (pCardItem->cbCardData!=255) pCardItem->bShoot=!pCardItem->bShoot;

	//发送消息
	if (m_pSinkWindow!=NULL) 
	{
		//m_pSinkWindow->PostMessage(IDM_LEFT_HIT_CARD,0,0);

		CGameFrameEngine * pGameFrameEngine=CGameFrameEngine::GetInstance();
		if (pGameFrameEngine!=NULL) pGameFrameEngine->PostMessage(IDM_LEFT_HIT_CARD,0,0);
	}

	//调整控件
	RectifyControl();

	return;
}

//鼠标消息
void CCardControl::OnLButtonDown(UINT nFlags, CPoint Point)
{
	__super::OnLButtonDown(nFlags, Point);

	//状态判断
	if ((m_bHorizontal==false)||(m_bPositively==false)) return;

	//获取索引
	m_dwCurrentIndex=SwitchCardPoint(Point);

	return;
}

//光标消息
BOOL CCardControl::OnSetCursor(CWnd * pWnd, UINT nHitTest, UINT message)
{
	//设置光标
	if (m_bPositively==true)
	{
		SetCursor(LoadCursor(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDC_CARD_CUR)));
		return TRUE;
	}

	return __super::OnSetCursor(pWnd,nHitTest,message);
}

//设置牌最大宽
void CCardControl::SetMaxWidth( UINT nMaxWidth )
{
	if( nMaxWidth != m_nMaxWidth )
	{
		m_nMaxWidth = nMaxWidth;
		RectifyControl();
	}
}

//////////////////////////////////////////////////////////////////////////

