// ClientControlItemSink.cpp : 实现文件
//

#include "stdafx.h"
#include "Resource.h"
#include "ClientControlItemSink.h"
#include ".\clientcontrolitemsink.h"


// CClientControlItemSinkDlg 对话框

IMPLEMENT_DYNAMIC(CClientControlItemSinkDlg, IClientControlDlg)

CClientControlItemSinkDlg::CClientControlItemSinkDlg(CWnd* pParent /*=NULL*/)
	: IClientControlDlg(CClientControlItemSinkDlg::IDD, pParent)
{
	ZeroMemory(m_cbWinArea,sizeof(m_cbWinArea));	
	m_cbExcuteTimes=0;			//执行次数
}

CClientControlItemSinkDlg::~CClientControlItemSinkDlg()
{
}

void CClientControlItemSinkDlg::DoDataExchange(CDataExchange* pDX)
{
	IClientControlDlg::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CClientControlItemSinkDlg, IClientControlDlg)
	ON_BN_CLICKED(IDC_BUTTON_OK, OnExcute)
	ON_BN_CLICKED(IDC_BUTTON_RESET,OnReSet)
	ON_BN_CLICKED(IDC_BUTTON_SYN,OnRefresh)
	ON_BN_CLICKED(IDC_BUTTON_CANCEL,OnCancel)
	ON_CONTROL_RANGE(BN_CLICKED,IDC_CHECK_FEIQIN,IDC_CHECK_7,OnSexClicked)
//	ON_WM_TIMER()
//ON_BN_CLICKED(IDC_CHECK_5, OnBnClickedCheck5)
END_MESSAGE_MAP()

// CClientControlItemSinkDlg 消息处理程序

//初始化
BOOL CClientControlItemSinkDlg::OnInitDialog()
{
	IClientControlDlg::OnInitDialog();

	SetDlgItemText(IDC_STATIC_TIMES,TEXT("控制局数:"));
	SetDlgItemText(IDC_STATIC_AREA, TEXT("区域控制:"));
	SetDlgItemText(IDC_STATIC_NOTIC,TEXT("控制说明:"));
	SetDlgItemText(IDC_STATIC_TEXT,TEXT("区域输赢控制比游戏库存控制策略优先!"));
	SetDlgItemText(IDC_CHECK_FEIQIN,TEXT("飞禽"));
	SetDlgItemText(IDC_CHECK_JINSHA,TEXT("金鲨"));
	SetDlgItemText(IDC_CHECK_YINSHA,TEXT("银鲨"));
	SetDlgItemText(IDC_CHECK_ZOUSHOU,TEXT("走兽"));	
	SetDlgItemText(IDC_CHECK_0,TEXT("狮子"));
	SetDlgItemText(IDC_CHECK_1,TEXT("熊猫"));
	SetDlgItemText(IDC_CHECK_2,TEXT("猴子"));
	SetDlgItemText(IDC_CHECK_3,TEXT("兔子"));
	SetDlgItemText(IDC_CHECK_4,TEXT("老鹰"));
	SetDlgItemText(IDC_CHECK_5,TEXT("孔雀"));
	SetDlgItemText(IDC_CHECK_6,TEXT("鸽子"));
	SetDlgItemText(IDC_CHECK_7,TEXT("燕子"));

	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("1"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("2"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("3"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("4"));
	((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->AddString(TEXT("5"));


	SetDlgItemText(IDC_BUTTON_RESET,TEXT("取消控制"));
	SetDlgItemText(IDC_BUTTON_SYN,TEXT("当前设置"));
	SetDlgItemText(IDC_BUTTON_OK,TEXT("执行"));
	SetDlgItemText(IDC_BUTTON_CANCEL,TEXT("取消"));

	return TRUE; 
}

void CClientControlItemSinkDlg::OnReSet()
{
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_RESET_CONTROL;
	CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
	GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_ZOUSHOU)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_FEIQIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_JINSHA)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_YINSHA)->EnableWindow(TRUE);
}

void CClientControlItemSinkDlg::OnRefresh()
{
	CMD_C_AdminReq adminReq;
	adminReq.cbReqType=RQ_PRINT_SYN;
	CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
	GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_ZOUSHOU)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_FEIQIN)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_JINSHA)->EnableWindow(TRUE);
	GetDlgItem(IDC_CHECK_YINSHA)->EnableWindow(TRUE);
}

void CClientControlItemSinkDlg::OnSexClicked(UINT nCmdID)
{
	switch(nCmdID)
	{
		case IDC_CHECK_FEIQIN:
		{
			GetDlgItem(IDC_CHECK_0)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_1)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_2)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_3)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_ZOUSHOU)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_FEIQIN)->EnableWindow(FALSE);
			if (IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
			{
				((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(0);	
			}
			if (IsDlgButtonChecked(IDC_CHECK_1))
			{
				((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
			}
			if (IsDlgButtonChecked(IDC_CHECK_2))
			{
				((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
			}
			if (IsDlgButtonChecked(IDC_CHECK_3))
			{
				((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
			}
			if (IsDlgButtonChecked(IDC_CHECK_0))
			{
				((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
			}
		} 
		break;
		case IDC_CHECK_ZOUSHOU:
		{
			GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
			GetDlgItem(IDC_CHECK_ZOUSHOU)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_4)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_5)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_6)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_7)->EnableWindow(FALSE);
			GetDlgItem(IDC_CHECK_FEIQIN)->EnableWindow(TRUE);
			if (IsDlgButtonChecked(IDC_CHECK_FEIQIN))
			{
				((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(0);	
			}
			if (IsDlgButtonChecked(IDC_CHECK_4))
			{
				((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
			}
			if (IsDlgButtonChecked(IDC_CHECK_5))
			{
				((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
			}
			if (IsDlgButtonChecked(IDC_CHECK_6))
			{
				((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
			}
			if (IsDlgButtonChecked(IDC_CHECK_7))
			{
				((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
			}
		}
		break;
		case IDC_CHECK_JINSHA:
			{
				GetDlgItem(IDC_CHECK_JINSHA)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_YINSHA)->EnableWindow(TRUE);
				if (IsDlgButtonChecked(IDC_CHECK_YINSHA))
				{
					((CButton*)GetDlgItem(IDC_CHECK_YINSHA))->SetCheck(0);	
				}
			}
			break;
		case IDC_CHECK_YINSHA:
			{
				GetDlgItem(IDC_CHECK_JINSHA)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_YINSHA)->EnableWindow(FALSE);
				if (IsDlgButtonChecked(IDC_CHECK_JINSHA))
				{
					((CButton*)GetDlgItem(IDC_CHECK_JINSHA))->SetCheck(0);	
				}
			}
			break;
		case IDC_CHECK_0:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_ZOUSHOU)->EnableWindow(FALSE);
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_1:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_ZOUSHOU)->EnableWindow(FALSE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_2:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_ZOUSHOU)->EnableWindow(FALSE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_3:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_ZOUSHOU)->EnableWindow(FALSE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_ZOUSHOU))
				{
					((CButton*)GetDlgItem(IDC_CHECK_ZOUSHOU))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_4:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_FEIQIN)->EnableWindow(FALSE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_5:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_FEIQIN)->EnableWindow(FALSE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_6:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_FEIQIN)->EnableWindow(FALSE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_7))
				{
					((CButton*)GetDlgItem(IDC_CHECK_7))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(1);	
				}
			}
			break;
		case IDC_CHECK_7:
			{
				GetDlgItem(IDC_CHECK_0)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_1)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_2)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_3)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_4)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_5)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_6)->EnableWindow(TRUE);
				GetDlgItem(IDC_CHECK_7)->EnableWindow(FALSE);
				GetDlgItem(IDC_CHECK_FEIQIN)->EnableWindow(FALSE);
				if (IsDlgButtonChecked(IDC_CHECK_0))
				{
					((CButton*)GetDlgItem(IDC_CHECK_0))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_2))
				{
					((CButton*)GetDlgItem(IDC_CHECK_2))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_3))
				{
					((CButton*)GetDlgItem(IDC_CHECK_3))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_4))
				{
					((CButton*)GetDlgItem(IDC_CHECK_4))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_5))
				{
					((CButton*)GetDlgItem(IDC_CHECK_5))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_6))
				{
					((CButton*)GetDlgItem(IDC_CHECK_6))->SetCheck(0);	
				}
				if (IsDlgButtonChecked(IDC_CHECK_1))
				{
					((CButton*)GetDlgItem(IDC_CHECK_1))->SetCheck(0);	
				}
				if (!IsDlgButtonChecked(IDC_CHECK_FEIQIN))
				{
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN))->SetCheck(1);	
				}
			}
			break;
	}
}

void CClientControlItemSinkDlg::OnExcute()
{
	CMD_C_AdminReq adminReq;
	BYTE cbIndex=0;
	int nCheckID[3]={0,0,0};
	BYTE CheckCount=0;
	for(cbIndex=0;cbIndex<12;cbIndex++)
	{
		if(((CButton*)GetDlgItem(IDC_CHECK_FEIQIN+cbIndex))->GetCheck())
		{
			nCheckID[CheckCount++]=cbIndex+1;
			if(CheckCount>2) break;
		}
	}

	int nSelect=((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->GetCurSel();
	
	if(nCheckID[0]>0&&nSelect>=0)
	{
		adminReq.cbReqType=RQ_SET_WIN_AREA;
		if (nCheckID[2]>0)
		{
			nCheckID[0]=nCheckID[1];
			nCheckID[1]=nCheckID[2];
		}
		adminReq.cbExtendData[0]=(BYTE)nCheckID[0];
		adminReq.cbExtendData[1]=(BYTE)nCheckID[1];
		adminReq.cbExtendData[2]=(BYTE)nSelect+1;
		m_cbWinArea[0]=adminReq.cbExtendData[0];
		m_cbWinArea[1]=adminReq.cbExtendData[1];
		m_cbExcuteTimes=adminReq.cbExtendData[2];

		CGameFrameEngine::GetInstance()->SendMessage(IDM_ADMIN_COMMDN,(WPARAM)&adminReq,0);
		
	}
	else
	{
		AfxMessageBox(TEXT("请选择受控次数以及受控区域!"));
		OnRefresh();
	}
	// TODO: 在此添加控件通知处理程序代码
}
void CClientControlItemSinkDlg::SetText(SCORE m_Score,int x,int y,bool bXor)
{
	/*CClientDC dc(this);*/
	CString str;
	str.Format(TEXT("%d"),m_Score);
	//dc.SetBkColor(RGB(236,233,216));
	//dc.TextOut(x,y,str);
	CDC *m_tpDC; 
	m_tpDC=GetDC();
	if (bXor)
	{
		m_tpDC->SetTextColor(RGB(0,0,0));
		m_tpDC->SetBkColor(RGB(236,233,216));
		m_tpDC->TextOut(x,y,str);
	}
	else
	{
		m_tpDC->SetTextColor(RGB(236,233,216));
		m_tpDC->SetBkColor(RGB(236,233,216));
		m_tpDC->TextOut(x,y,str,20);
	}
	m_tpDC->DeleteDC();
}
bool CClientControlItemSinkDlg::ReqResult(const void * pBuffer)
{

	const CMD_S_CommandResult*pResult=(CMD_S_CommandResult*)pBuffer;
	CString str;
	switch(pResult->cbAckType)
	{

	case ACK_SET_WIN_AREA:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str="输赢控制命令已经接受！";
				/*switch(m_cbWinArea)
				{
				case 0:str.Format(TEXT("无受控区域"));break;
				case 1:str.Format(TEXT("请求已接受,胜利区域:闲对子,执行次数:%d"),m_cbExcuteTimes);break;
				case 2:str.Format(TEXT("请求已接受,胜利区域:  闲  ,执行次数:%d"),m_cbExcuteTimes);break;
				case 3:str.Format(TEXT("请求已接受,胜利区域:闲天王,执行次数:%d"),m_cbExcuteTimes);break;
				case 4:str.Format(TEXT("请求已接受,胜利区域:  平  ,执行次数:%d"),m_cbExcuteTimes);break;
				case 5:str.Format(TEXT("请求已接受,胜利区域:同点平,执行次数:%d"),m_cbExcuteTimes);break;
				case 6:str.Format(TEXT("请求已接受,胜利区域:庄对子,执行次数:%d"),m_cbExcuteTimes);break;
				case 7:str.Format(TEXT("请求已接受,胜利区域:  庄  ,执行次数:%d"),m_cbExcuteTimes);break;
				case 8:str.Format(TEXT("请求已接受,胜利区域:庄天王,执行次数:%d"),m_cbExcuteTimes);break;
				case 9:str.Format(TEXT("请求已接受,胜利区域:庄天王,执行次数:%d"),m_cbExcuteTimes);break;
				case 10:str.Format(TEXT("请求已接受,胜利区域:庄天王,执行次数:%d"),m_cbExcuteTimes);break;
				case 11:str.Format(TEXT("请求已接受,胜利区域:庄天王,执行次数:%d"),m_cbExcuteTimes);break;
				default:break;
				}*/
			}
			else
			{
				str.Format(TEXT("请求失败!"));
				m_cbExcuteTimes=0;
				ZeroMemory(m_cbWinArea,sizeof(m_cbWinArea));
			}
			break;
		}
	case ACK_RESET_CONTROL:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str="重置请求已接受!";
				for(int nIndex=0;nIndex<12;nIndex++)
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN+nIndex))->SetCheck(0);
				if(m_cbExcuteTimes>0)
					((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(-1);
				ZeroMemory(m_cbWinArea,sizeof(m_cbWinArea));
				m_cbExcuteTimes=0;
			}
			break;
		}
	case ACK_PRINT_SYN:
		{
			if(pResult->cbResult==CR_ACCEPT)
			{  
				str="服务器同步请求已接受!";
				for(int nIndex=0;nIndex<12;nIndex++)
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN+nIndex))->SetCheck(0);

				m_cbWinArea[0]=pResult->cbExtendData[0];
				m_cbWinArea[1]=pResult->cbExtendData[1];
				m_cbExcuteTimes=pResult->cbExtendData[2];
				if(m_cbWinArea[0]>0)
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN+m_cbWinArea[0]-1))->SetCheck(1);
				if(m_cbWinArea[1]>0)
					((CButton*)GetDlgItem(IDC_CHECK_FEIQIN+m_cbWinArea[1]-1))->SetCheck(1);
				if(m_cbExcuteTimes>0&&m_cbExcuteTimes<=5)
					((CComboBox*)GetDlgItem(IDC_COMBO_TIMES))->SetCurSel(m_cbExcuteTimes-1);

				//SetTimer(10,2000,0);
			}
			else
			{
				str.Format(TEXT("请求失败!"));
			}
			break;
		}

	default: break;
	}

	SetWindowText(str);
	return true;
}


void CClientControlItemSinkDlg::OnCancel()
{
	// TODO: 在此添加专用代码和/或调用基类
	ShowWindow(SW_HIDE);
	//	CDialog::OnCancel();
}

//void CClientControlItemSinkDlg::OnTimer(UINT nIDEvent)
//{
//	// TODO: 在此添加消息处理程序代码和/或调用默认值
//	KillTimer(10);
//	CMD_S_CommandResult Result;
//	Result.cbAckType=ACK_SET_WIN_AREA;
//	Result.cbResult=CR_ACCEPT;
//	ReqResult(&Result);
//	CDialog::OnTimer(nIDEvent);
//}

void __cdecl CClientControlItemSinkDlg::OnAllowControl(bool bEnable)
{
	GetDlgItem(IDC_BUTTON_OK)->EnableWindow(bEnable);
}


