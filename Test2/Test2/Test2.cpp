#include<Windows.h>
#include <CommCtrl.h>
#include <math.h>
#include "resource.h"

#define WM_CALCULATE WM_USER+10
#define WM_RESETDIALOG WM_USER+11
#define WM_REPAINT WM_USER+12

typedef struct _INPUTDATA//��������
{
	float fPro;
	float fDensity;
	float fVolumeFactor;
	float fViscosity;
	float fPressFactor;
	float fThick;
	float fRadius;
	float fTime[10];
	float fPress[10];
}INPUTDATA,*LPINPUTDATA;

typedef struct _OUTPUTDATA//�������
{
	float fFlowFactor;
	float fPermeability;
	float fDPressFactor;
	float fPoint1X;
	float fPoint1Y;
	float fPoint2X;
	float fPoint2Y;
	float flTime[10];
	float fPress[10];
	char fun[100];
}OUTPUTDATA,*LPOUTPUTDATA;

HWND hWndMain;
LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL CALLBACK DialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);
BOOL GetData(HWND hWnd,LPINPUTDATA lpInput);
BOOL Calculate(LPINPUTDATA lpInput,LPOUTPUTDATA lpOutput);
BOOL DrawData(HDC *hDC,int x,int y,LPSTR lpName,float fValue);
int ChangeX(int nOriginalX);
int ChangeY(int nOrigianlY);

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrecInstance,LPSTR lpCmdLine,int nShowCmd)//������
{
	WNDCLASSEX wcx;
	MSG msg;
	int screen_width;
	int screen_height;
	RECT rect;

	wcx.cbSize=sizeof(wcx);
	wcx.style=CS_HREDRAW|CS_VREDRAW;
	wcx.lpfnWndProc=WndProc;
	wcx.cbClsExtra=0;
	wcx.cbWndExtra=0;
	wcx.hInstance=hInstance;
	wcx.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));
	wcx.hCursor=LoadCursor(NULL,IDC_ARROW);
	wcx.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wcx.lpszMenuName=MAKEINTRESOURCE(IDR_MENU);
	wcx.lpszClassName="WndClass";
	wcx.hIconSm=LoadIcon(hInstance,MAKEINTRESOURCE(IDI_ICON));

	RegisterClassEx(&wcx);

	hWndMain=CreateWindow("WndClass","�����2",WS_OVERLAPPED|WS_CAPTION|
		WS_SYSMENU|WS_MINIMIZEBOX ,
		CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,CW_USEDEFAULT,NULL,NULL,hInstance,NULL);



	screen_width=GetSystemMetrics(SM_CXSCREEN);
	screen_height=GetSystemMetrics(SM_CYSCREEN);
	GetWindowRect(hWndMain,&rect);
	MoveWindow(hWndMain,(screen_width-800)/2,(screen_height-600)/2,
		800,600,FALSE);
	ShowWindow(hWndMain,SW_SHOW);

	while(GetMessage(&msg,NULL,NULL,NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)//������Ϣ������
{
	HDC hDC;
	static HDC hMemDC;
	static HBITMAP hBitmap;
	static BITMAP bm;
	HFONT hFont;
	HFONT hfSmall;
	HBRUSH hBrush;
	HBRUSH hbDot;
	HBRUSH hbGreen;
	HPEN hPen1;
	HPEN hPen2;
	RECT rect;
	PAINTSTRUCT ps;
	static INPUTDATA input;
	static OUTPUTDATA output;
	static BOOL bHasCalc=FALSE;
	int nMin;
	int nMax;
	char buffer[100];

	switch(uMsg)
	{
	case WM_CREATE:
		break;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case ID_FILE_INPUT:
			DialogBox(GetModuleHandle(NULL),MAKEINTRESOURCE(IDD_DIALOG),NULL,DialogProc);
			break;
		case ID_EXTRA_HELP:
			MessageBox(NULL,"ѡ�񡰼��㡱���������ݡ����������ݺ��������㡱���ɡ�","����",MB_OK|MB_ICONINFORMATION);
			break;
		case ID_EXTRA_ABOUT:
			MessageBox(NULL,"�༶��ʯ��09-7��\n����������","�������",MB_OK|MB_ICONINFORMATION);
			break;
		}
		break;
	case WM_PAINT://��ͼ
		//�����豸����
		hDC=BeginPaint(hWnd,&ps);
		hMemDC=CreateCompatibleDC(hDC);

		//ȡ�ÿͻ�����С
		GetClientRect(hWnd,&rect);

		//������������塢���ʡ���ˢ
		hFont=CreateFont(20,0,//��ȸ߶�
			0,0,//�Ƕ�
			FW_BLACK,//��ϸ
			0,//б��
			0,//�»���
			0,//ɾ����
			ANSI_CHARSET,//�ַ���
			OUT_DEFAULT_PRECIS,//�������
			CLIP_DEFAULT_PRECIS,//���о���
			DEFAULT_QUALITY,//��ͨ����
			0,
			"����"
			);
		hfSmall=CreateFont(20,0,//��ȸ߶�
			0,0,//�Ƕ�
			FW_BLACK,//��ϸ
			0,//б��
			0,//�»���
			0,//ɾ����
			ANSI_CHARSET,//�ַ���
			OUT_DEFAULT_PRECIS,//�������
			CLIP_DEFAULT_PRECIS,//���о���
			DEFAULT_QUALITY,//��ͨ����
			0,
			"����"
			);
		hPen1=CreatePen(PS_SOLID,4,RGB(0,255,0));
		hPen2=CreatePen(PS_SOLID,2,RGB(0,0,255));
		hBrush=CreateSolidBrush(RGB(255,255,200));
		hbDot=CreateSolidBrush(RGB(0,0,0));
		hbGreen=CreateSolidBrush(RGB(0,255,0));

		//����λͼ
		hBitmap=LoadBitmap(GetModuleHandle(NULL),MAKEINTRESOURCE(IDB_BITMAP));

		//ѡ��λͼ�����嵽�豸����
		SelectObject(hMemDC,hBitmap);
		SelectObject(hMemDC,hFont);
		SetBkMode(hMemDC,TRANSPARENT);

		//��ʼ����
		if (bHasCalc==TRUE)
		{
			//������Ժ����������
			DrawData(&hMemDC,0,0,"����ϵ��",output.fFlowFactor);
			DrawData(&hMemDC,0,60,"��͸��",output.fPermeability);
			DrawData(&hMemDC,0,120,"��ѹϵ��",output.fDPressFactor);

			//�����Ҳ�Բ�Ǿ���
			SelectObject(hMemDC,hBrush);
			RoundRect(hMemDC,100,0,rect.right,rect.bottom,30,30);

			//���������ᣬ600*400�Ļ�ͼ����
			SelectObject(hMemDC,hbGreen);
			Rectangle(hMemDC,ChangeX(0),ChangeY(0),ChangeX(600),ChangeY(400));
			//���Ʊ��⣬���������ƣ��̶�
			TextOut(hMemDC,ChangeX(230),ChangeY(450),"Pw-lg t��ϵͼ",strlen("Pw-lg t��ϵͼ"));
			TextOut(hMemDC,ChangeX(550),ChangeY(-15),"lg t",strlen("lg t"));
			TextOut(hMemDC,ChangeX(-65),ChangeY(400),"Pw/MPa",strlen("Pw/MPa"));
			TextOut(hMemDC,ChangeX(-10),ChangeY(-10),"O",strlen("O"));
			
			//�����������
			SelectObject(hMemDC,hPen2);
			MoveToEx(hMemDC,ChangeX((int)(output.fPoint1X/output.fPoint2X*550)),
				ChangeY((int)(output.fPoint1Y/output.fPoint2Y*350)),NULL);
			LineTo(hMemDC,ChangeX(550),ChangeY(350));
			SelectObject(hMemDC,hbDot);
			for (int i=0;i<10;i++)
			{
				Ellipse(hMemDC,
					ChangeX((int)(output.flTime[i]/output.fPoint2X*550)-5),
					ChangeY((int)(output.fPress[i]/output.fPoint2Y*350)-5),
					ChangeX((int)(output.flTime[i]/output.fPoint2X*550)+5),
					ChangeY((int)(output.fPress[i]/output.fPoint2Y*350)+5));
			}
			SetTextColor(hMemDC,RGB(0,0,0));
			TextOut(hMemDC,ChangeX(250),ChangeY(250),output.fun,strlen(output.fun));

			ZeroMemory(buffer,100);
			gcvt(output.fPoint1Y,6,buffer);
			TextOut(hMemDC,ChangeX(-60),ChangeY(340),buffer,strlen(buffer));
		}

		//��λͼ���ڴ���Ƶ��ͻ���
		GetObject(hBitmap,sizeof(BITMAP),&bm);
		BitBlt(hDC,0,0,bm.bmWidth,bm.bmHeight,hMemDC,0,0,SRCCOPY);
		EndPaint(hWnd,&ps);
		break;
	case WM_REPAINT://����ú��ػ�����
		memcpy(&input,(LPINPUTDATA)wParam,sizeof(INPUTDATA));
		memcpy(&output,(LPOUTPUTDATA)lParam,sizeof(OUTPUTDATA));
		bHasCalc=TRUE;
		InvalidateRect(hWnd,NULL,TRUE);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;;
	default:
		break;
	}
	return(DefWindowProc(hWnd,uMsg,wParam,lParam));
}

BOOL DrawData(HDC *hDC,int x,int y,LPTSTR lpName,float fValue)//�����ݻ�����ͼ��
{
	char buffer[100];

	ZeroMemory(buffer,100);
	strcpy(buffer,lpName);
	strcat(buffer,"��");
	TextOut(*hDC,x,y,buffer,strlen(buffer));

	ZeroMemory(buffer,100);
	gcvt(fValue,5,buffer);
	TextOut(*hDC,x,y+20,buffer,strlen(buffer));

	return TRUE;
}

int ChangeX(int nOriginalX)
{
	return 170+nOriginalX;
}

int ChangeY(int nOrigianlY)
{
	return 480-nOrigianlY;
}

BOOL CALLBACK DialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)//�Ի�����Ϣ������
{
	int screen_width;
	int screen_height;
	RECT rect;
	LV_COLUMN lvColumn;
	LV_ITEM lvItem;
	char buffer[100];
	float fTime[]={0,10,30,60,100,150,200,250,300,435};
	float fPw[]={7.26f,7.55f,8.02f,8.41f,8.64f,8.75f,8.79f,8.81f,8.83f,8.87f};
	INPUTDATA input;
	OUTPUTDATA output;

	switch(uMsg)
	{
	case WM_INITDIALOG:
		screen_width=GetSystemMetrics(SM_CXSCREEN);
		screen_height=GetSystemMetrics(SM_CYSCREEN);
		GetWindowRect(hWnd,&rect);
		MoveWindow(hWnd,(screen_width-(rect.right-rect.left))/2,(screen_height-(rect.bottom-rect.top))/2,
			(rect.right-rect.left),(rect.bottom-rect.top),FALSE);
		//�б�ؼ����11��
		lvColumn.mask=LVCF_FMT|LVCF_WIDTH|LVCF_TEXT;
		lvColumn.fmt=LVCFMT_LEFT;
		for (int i=0;i<11;i++)
		{
			memset(buffer,0,100);
			itoa(i,buffer,10);;
			if (i==0)
			{
				lvColumn.cx=60;
				lvColumn.pszText="";
			}
			else
			{
				lvColumn.cx=40;
				lvColumn.pszText=buffer;
			}
			ListView_InsertColumn(GetDlgItem(hWnd,IDC_LIST_TIMEPRESS),i,&lvColumn);
		}
		lvItem.mask=NULL;
		for(int i=0;i<2;i++)
		{
			lvItem.iItem=i;
			lvItem.iSubItem=0;
			ListView_InsertItem(GetDlgItem(hWnd,IDC_LIST_TIMEPRESS),&lvItem);
		}
		ListView_SetItemText(GetDlgItem(hWnd,IDC_LIST_TIMEPRESS),0,0,"t/min");
		ListView_SetItemText(GetDlgItem(hWnd,IDC_LIST_TIMEPRESS),1,0,"Pw/MPa");

		SendMessage(hWnd,WM_RESETDIALOG,NULL,NULL);

		return TRUE;
	case WM_COMMAND:
		switch(LOWORD(wParam))
		{
		case IDC_BUTTON_CALCULATE://���㰴ť
			SendMessage(hWnd,WM_CALCULATE,NULL,NULL);
			break;
		case IDC_BUTTON_RESET://���ð�ť
			SendMessage(hWnd,WM_RESETDIALOG,NULL,NULL);
			break;
		case IDC_BUTTON_BACK://���ذ�ť
			EndDialog(hWnd,0);
			break;
		}
		break;
	case WM_CALCULATE://��ȡ���ݲ�����
		ZeroMemory(&input,sizeof(INPUTDATA));
		ZeroMemory(&output,sizeof(OUTPUTDATA));
		GetData(hWnd,&input);
		Calculate(&input,&output);
		
		EndDialog(hWnd,0);
		SendMessage(hWndMain,WM_REPAINT,(WPARAM)&input,(LPARAM)&output);
		break;
	case WM_RESETDIALOG://��������
		SetDlgItemText(hWnd,IDC_EDIT_PRODUCT,"28");
		SetDlgItemText(hWnd,IDC_EDIT_DENSITY,"0.85");
		SetDlgItemText(hWnd,IDC_EDIT_VOLUMEFACTOR,"1.12");
		SetDlgItemText(hWnd,IDC_EDIT_VISCOSITY,"9");
		SetDlgItemText(hWnd,IDC_EDIT_PRESSFACTOR,"0.000375");
		SetDlgItemText(hWnd,IDC_EDIT_THICK,"8.6");
		SetDlgItemText(hWnd,IDC_EDIT_RADIUS,"10");
		for(int i=0;i<10;i++)
		{
			memset(buffer,0,100);
			gcvt(fTime[i],3,buffer);
			ListView_SetItemText(GetDlgItem(hWnd,IDC_LIST_TIMEPRESS),0,i+1,buffer);
			memset(buffer,0,100);
			gcvt(fPw[i],4,buffer);
			ListView_SetItemText(GetDlgItem(hWnd,IDC_LIST_TIMEPRESS),1,i+1,buffer);
		}
		return TRUE;
	case WM_CLOSE:
		EndDialog(hWnd,0);
	}
	return FALSE;
}

BOOL GetData(HWND hWnd,LPINPUTDATA lpInput)//�ӶԻ����ȡ����
{
	char buffer[100];
	HWND hList;

	//��ȡ�༭��
	ZeroMemory(buffer,100);
	GetDlgItemText(hWnd,IDC_EDIT_PRODUCT,buffer,100);
	lpInput->fPro=(float)atof(buffer);
	ZeroMemory(buffer,100);
	GetDlgItemText(hWnd,IDC_EDIT_DENSITY,buffer,100);
	lpInput->fDensity=(float)atof(buffer);
	ZeroMemory(buffer,100);
	GetDlgItemText(hWnd,IDC_EDIT_VOLUMEFACTOR,buffer,100);
	lpInput->fVolumeFactor=(float)atof(buffer);
	ZeroMemory(buffer,100);
	GetDlgItemText(hWnd,IDC_EDIT_VISCOSITY,buffer,100);
	lpInput->fViscosity=(float)atof(buffer);
	ZeroMemory(buffer,100);
	GetDlgItemText(hWnd,IDC_EDIT_PRESSFACTOR,buffer,100);
	lpInput->fPressFactor=(float)atof(buffer);
	ZeroMemory(buffer,100);
	GetDlgItemText(hWnd,IDC_EDIT_THICK,buffer,100);
	lpInput->fThick=(float)atof(buffer);
	ZeroMemory(buffer,100);
	GetDlgItemText(hWnd,IDC_EDIT_RADIUS,buffer,100);
	lpInput->fRadius=(float)atof(buffer);
	//��ȡ�б�
	hList=GetDlgItem(hWnd,IDC_LIST_TIMEPRESS);
	for(int i=0;i<10;i++)
	{
		ZeroMemory(buffer,100);
		ListView_GetItemText(hList,0,i+1,buffer,100);
		lpInput->fTime[i]=(float)atof(buffer);
		ZeroMemory(buffer,100);
		ListView_GetItemText(hList,1,i+1,buffer,100);
		lpInput->fPress[i]=(float)atof(buffer);
	}
	return TRUE;
}

BOOL Calculate(LPINPUTDATA lpInput,LPOUTPUTDATA lpOutput)//�������
{
	if(lpInput->fTime[0]<=0) lpInput->fTime[0]=(float)0.0001;
	int m=10;
	float Sx=0;
	float Sy=0;
	float Sxy=0;
	float Sx2=0;
	char buffer[10];

	for (int i=0;i<10;i++)
	{
		lpOutput->flTime[i]=log10f(lpInput->fTime[i]);
		lpOutput->fPress[i]=lpInput->fPress[i];
		Sx+=log10f(lpInput->fTime[i]);
		Sy+=lpInput->fPress[i]*10;
		Sxy+=log10f(lpInput->fTime[i])*lpInput->fPress[i]*10;
		Sx2+=log10f(lpInput->fTime[i])*log10f(lpInput->fTime[i]);
	}
	float I=(m*Sxy-Sx*Sy)/(m*Sx2-Sx*Sx);
	float A=(Sx2*Sy-Sxy*Sx)/(m*Sx2-Sx*Sx);

	//Pw=A+I*lgt
	lpOutput->fPoint1X=0;
	lpOutput->fPoint1Y=A/10;
	lpOutput->fPoint2X=(float)log10f(lpInput->fTime[9]);
	lpOutput->fPoint2Y=(A+I*log10f(lpInput->fTime[9]))/10;

	float Q=lpInput->fPro*1000.0*1000*lpInput->fVolumeFactor/3600/24/lpInput->fDensity;
	lpOutput->fFlowFactor=0.183*Q/I;
	lpOutput->fPermeability=0.183*Q*lpInput->fViscosity/I/lpInput->fThick/100;
	lpOutput->fDPressFactor=lpOutput->fPermeability/lpInput->fViscosity/lpInput->fPressFactor;

	strcat(lpOutput->fun,"Pw=");
	ZeroMemory(buffer,10);
	gcvt(A/10,6,buffer);
	strcat(lpOutput->fun,buffer);
	strcat(lpOutput->fun,"+");
	ZeroMemory(buffer,10);
	gcvt(I/10,6,buffer);
	strcat(lpOutput->fun,buffer);
	strcat(lpOutput->fun,"*lg t");

	return TRUE;
}