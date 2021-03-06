
// 20142285Dlg.cpp: 구현 파일
//

#include "stdafx.h"
#include "20142285.h"
#include "20142285Dlg.h"
#include "afxdialogex.h"

#include <openssl/aes.h>
#include <openssl/md5.h>
#include "zip.h"
#include "unzip.h"

char md5Hash[32] = { 0, };
#define BLOCK_SIZE 16
#define FREAD_COUNT 4096
#define KEY_BIT 256
#define IV_SIZE 16
#define RW_SIZE 1

AES_KEY aes_ks3;
unsigned char iv[IV_SIZE];

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


int passwd_aes(char *plain);
int encrypt_aes(char *in_file, char *out_file);
int decrypt_aes(char *in_file, char *out_file);

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMy20142285Dlg 대화 상자



CMy20142285Dlg::CMy20142285Dlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_MY20142285_DIALOG, pParent)
	, passwd(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMy20142285Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_EDIT1, passwd);
	DDX_Control(pDX, IDC_EDIT1, ctrpw);
}

BEGIN_MESSAGE_MAP(CMy20142285Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMy20142285Dlg::OnFileOpen)
	ON_BN_CLICKED(IDC_BUTTON2, &CMy20142285Dlg::OnZip)
	ON_BN_CLICKED(IDC_BUTTON3, &CMy20142285Dlg::OnUnzip)
END_MESSAGE_MAP()


// CMy20142285Dlg 메시지 처리기

BOOL CMy20142285Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CMy20142285Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CMy20142285Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CMy20142285Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMy20142285Dlg::OnFileOpen() {
	UpdateData(TRUE);
	m_List.ResetContent();
	fileList.RemoveAll();
	pathList.RemoveAll();
	DWORD dwFlag = OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT; // 속성
	CString strFilter, strTemp;
	strFilter.Format(_T("All Files(*.*)|*.*||"));

	CFileDialog dlg(TRUE, NULL, NULL, dwFlag, strFilter, this);
	CString strDir;

	//strDir.Format(_T("%s%s"), BPATH, ENA_PATH_RESULT);
	dlg.m_ofn.lpstrInitialDir = strDir;  // 초기 경로

	CString strFile;
	dlg.m_ofn.lpstrFile = strFile.GetBuffer(10000);  // 다중 선택을 위한 버퍼 지정
	dlg.m_ofn.nMaxFile = 10000;


	if (dlg.DoModal() == IDOK) {
		for (POSITION pos = dlg.GetStartPosition(); pos != NULL; ) {
			strPath = dlg.GetNextPathName(pos);
			strName = strPath;
			pathList.AddTail(strPath);

			strName.Delete(0, strName.ReverseFind('\\') + 1);
			fileList.AddTail(strName);
			m_List.AddString(strName);
		}
	}
	strFile.ReleaseBuffer();

	POSITION pos = pathList.GetHeadPosition();
	ZipPathName = pathList.GetNext(pos);
	UpdateData(FALSE);
}


void CMy20142285Dlg::OnZip() {
	UpdateData(TRUE);
	CString pw = passwd;
	UpdateData(FALSE);

	if (!(pw.Compare(""))) {
		AfxMessageBox("비밀번호가 입력되지 않았습니다.");
	}
	else {
		if (!(ZipPathName.Compare(""))) {
			AfxMessageBox("파일이 선택되지 않았습니다.");
		}
		else {
			ZipPathName = ZipPathName + ".zip";
			HZIP hz = CreateZip(ZipPathName, 0);

			POSITION pathpos = pathList.GetHeadPosition();
			POSITION filepos = fileList.GetHeadPosition();
			while (pathpos) {
				CString filename = fileList.GetNext(filepos);
				CString patnname = pathList.GetNext(pathpos);
				ZipAdd(hz, filename, patnname);
			}
			CloseZip(hz);
			//압축파일을 암호화
			char *repasswd = (char *)malloc(pw.GetLength());
			strcpy(repasswd, (LPTSTR)(LPCTSTR)pw);
			passwd_aes(repasswd);

			char *infile = (char *)malloc(ZipPathName.GetLength());
			strcpy(infile, (LPTSTR)(LPCTSTR)ZipPathName);

			char *out_file = (char *)malloc(ZipPathName.GetLength() + 4);
			strcpy(out_file, (LPTSTR)(LPCTSTR)ZipPathName);
			strcat(out_file, ".Ezip");

			if (!(encrypt_aes(infile, out_file))) {
				DeleteFile(infile);
				m_List.ResetContent();
				ctrpw.SetWindowText("");
				AfxMessageBox("압축이 성공하였습니다.");
			}
		}
	}
}
//압축풀때 확장자명 검사!!
void CMy20142285Dlg::OnUnzip() {
	UpdateData(TRUE);
	CString pw = passwd;
	UpdateData(FALSE);

	if (!(pw.Compare(""))) {
		AfxMessageBox("비밀번호가 입력되지 않았습니다.");
	}
	else {
		char *passwd = (char *)malloc(pw.GetLength());
		strcpy(passwd, (LPTSTR)(LPCTSTR)pw);
		passwd_aes(passwd);

		char *infile = (char *)malloc(ZipPathName.GetLength());
		strcpy(infile, (LPTSTR)(LPCTSTR)ZipPathName);

		char *out_file = (char *)malloc(ZipPathName.GetLength() - 4);
		strcpy(out_file, (LPTSTR)(LPCTSTR)ZipPathName);
		for (int i = 0; i < ZipPathName.GetLength(); i++) {
			if (out_file[i] == '.')
				if (out_file[i + 1] == 'E')
					if (out_file[i + 2] == 'z')
						if (out_file[i + 3] == 'i')
							if (out_file[i + 4] == 'p') {
								out_file[i] = '\0';
							}

		}

		if (!(decrypt_aes(infile, out_file))) {
			HZIP hz = OpenZip(out_file, 0);
			ZIPENTRY ze;
			GetZipItem(hz, -1, &ze); // -1은 zip 파일에 대한 전체적인 정보를 의미
			int numitems = ze.index;
			if (numitems == 0) {
				AfxMessageBox("비밀번호가 틀렸습니다.");

				DeleteFile(out_file);
			}
			else {
				for (int zi = 0; zi < numitems; zi++)
				{
					ZIPENTRY ze;
					GetZipItem(hz, zi, &ze); // 압축 파일 내의 각각의 파일(아이템)에 대한 정보 가져오기
					CString path = strPath;
					path.Delete((strPath.GetLength() - strName.GetLength()), strName.GetLength());
					UnzipItem(hz, zi, path+ze.name);         // 각 아이템에 대한 압축 풀기
				}
				CloseZip(hz);
				DeleteFile(out_file);
				m_List.ResetContent();
				ctrpw.SetWindowText("");
				AfxMessageBox("압축해제에 성공하였습니다.");
			}
		}

	}
}


int encrypt_aes(char *in_file, char *out_file) {
	int i = 0;
	int len = 0;
	int padding_len = 0;
	int size;

	//암호화 할 파일 구조체 선언
	FILE *fp = fopen(in_file, "rb");
	if (fp == NULL) {
		CString err = "[ERROR] can not fopen in_file";
		AfxMessageBox(err);
		return 1;
	}

	//파일의 길이를 알아낸 뒤에 파일의 길이반큼의 buffer를 생성
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *buf = (char *)malloc(size + BLOCK_SIZE);

	//암호화 된 파일 구조체 선언
	FILE *wfp = fopen(out_file, "wb");
	if (wfp == NULL) {
		CString err = "[ERROR] can not fopen out_file";
		AfxMessageBox(err);
		return 1;
	}

	//iv 초기화
	memset(iv, 0, sizeof(iv));

	//암호화에 사용할 키를 md5방식으로 aes_ks3라는 aes_key 자료형에 256bit으로 저장
	AES_set_encrypt_key((const unsigned char *)md5Hash, KEY_BIT, &aes_ks3);

	//읽은 파일의 내용을 buf에 쓴뒤 암호화해서 암호화한 내용이 저장된 파일에 쓰기
	while (len = fread(buf, RW_SIZE, size, fp)) {
		if (size != len) {
			break;
		}
		AES_cbc_encrypt((const unsigned char*)buf, (unsigned char*)buf, len, &aes_ks3, iv, AES_ENCRYPT);
		fwrite(buf, RW_SIZE, len, wfp);

		fclose(wfp);
		fclose(fp);

		return 0;
	}
}



int decrypt_aes(char *in_file, char *out_file) {
	int len = 0;
	int total_size = 0;
	int save_len = 0;
	int w_len = 0;
	int size;

	//복호화 할 파일 구조체 선언
	FILE *fp = fopen(in_file, "rb");
	if (fp == NULL) {
		CString err = "[ERROR] can not fopen in_file";
		AfxMessageBox(err);
		return 1;
	}

	//파일의 길이를 알아낸 뒤에 파일의 길이반큼의 buffer를 생성
	fseek(fp, 0, SEEK_END);
	size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *buf = (char *)malloc(size + BLOCK_SIZE);

	//복호화 된 파일 구조체 선언
	FILE *wfp = fopen(out_file, "wb");
	if (wfp == NULL) {
		CString err = "[ERROR] can not fopen out_file";
		AfxMessageBox(err);
		return 1;
	}

	//iv 초기화
	memset(iv, 0, sizeof(iv));

	//복호화에 사용할 키를 md5방식으로 aes_ks3라는 aes_key 자료형에 256bit으로 저장
	AES_set_decrypt_key((const unsigned char *)md5Hash, KEY_BIT, &aes_ks3);

	//파일의 길이를 알아낸 뒤에 파일의 길이반큼의 buffer를 생성
	fseek(fp, 0, SEEK_END);
	total_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	//읽은 파일의 내용을 buf에 쓴뒤 암호화해서 복호화한 내용이 저장된 파일에 쓰기
	while (len = fread(buf, RW_SIZE, size, fp)) {
		if (size == 0) {
			break;
		}
		save_len += len;
		w_len = len;

		AES_cbc_encrypt((const unsigned char*)buf, (unsigned char*)buf, len, &aes_ks3, iv, AES_DECRYPT);
		if (save_len == total_size) { // check last block
			buf[len - 3] = NULL;
			buf[len - 2] = NULL;
			buf[len-1] = NULL;
			buf[len] = NULL;
		}

		fwrite(buf, RW_SIZE, w_len, wfp);
	}

	fclose(wfp);
	fclose(fp);

	return 0;
}


int passwd_aes(char *plain) {
	unsigned char digest[MD5_DIGEST_LENGTH];

	MD5_CTX context;
	MD5_Init(&context);
	MD5_Update(&context, plain, strlen(plain));
	MD5_Final(digest, &context);

	for (int i = 0; i < MD5_DIGEST_LENGTH; ++i) {
		sprintf(md5Hash + (i * 2), "%02x", digest[i]);
	}

	return 0;
}