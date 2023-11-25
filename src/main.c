#include <windows.h>  // ���� Windows API ��ͷ�ļ�
#include <tchar.h>    // ���� Unicode �� ANSI ���ݵĺ�����
#include <stdio.h>

#include "File_Manage.h"
#include "Login_User.h"
#include "Seat_Info.h"
#include "cJSON.h"
#include "main.h"

#define USER_INFO_DATABASE "UserInfo.json"
#define SEAT_INFO_DATABASE "SeatInfo.json"

Login_User_t Login_User;

// �ַ���ת��
void ConvertTCharToChar(const TCHAR* tcharStr, char* charStr, int charStrSize) {
    if (tcharStr == NULL || charStr == NULL) return;

#ifdef UNICODE
    // ���ʹ�� Unicode �ַ���
    WideCharToMultiByte(CP_UTF8, 0, tcharStr, -1, charStr, charStrSize, NULL, NULL);
#else
    // ���ʹ�ö��ֽ��ַ���
    strncpy(charStr, tcharStr, charStrSize);
#endif
}

// �ַ���ת��
void ConvertCharToTChar(const char* charStr, TCHAR* tcharStr, int tcharStrSize) {
    if (charStr == NULL || tcharStr == NULL) {
        return;
    }

#if defined(UNICODE) || defined(_UNICODE)
    // Unicode �����£��� char ת��Ϊ wchar_t
    size_t length = mbstowcs(NULL, charStr, 0); // ��ȡ����� wchar_t ����
    if (length == (size_t)(-1) || length >= tcharStrSize) {
        return;
    }
    mbstowcs(tcharStr, charStr, tcharStrSize);
#else
    // �� Unicode �����£�char �� TCHAR ����ͬ��
    strncpy(tcharStr, charStr, tcharStrSize);
    tcharStr[tcharStrSize - 1] = '\0'; // ȷ���ַ����� null ��β
#endif
}

// int main() {
//     TCHAR szText_username[100]; // ������������ȡ���� TCHAR �����ַ���
//     // ... ���Ĵ��룬��� szText_username ...

//     const int bufferSize = 100;
//     char username[bufferSize];

//     ConvertTCharToChar(szText_username, username, bufferSize);

//     // ���� username ����������ת������ַ�
// }

// ˢ�´��ڣ���ûɶ�ã������������ɿؼ�
void RefreshWindow(HWND hwnd) {
    InvalidateRect(hwnd, NULL, TRUE);  // ���������ڱ��Ϊ��Ч����
    UpdateWindow(hwnd);                // �����ػ洰��
    RedrawWindow(hwnd, NULL, NULL, RDW_INVALIDATE | RDW_ALLCHILDREN | RDW_UPDATENOW);   // �ػ洰��
}

// User_Login_t User_Login;

// ȫ�ֱ���
HINSTANCE hInst;      // Ӧ�ó���ʵ�����

HWND hwndEdit, hwndEdit2, hwndButton, hwndLabel, hwndLabel2, hwndOpenNewWindowButton;  // �ֱ�Ϊ�༭�򡢰�ť�ͱ�ǩ�Ĵ��ھ��

// �����ڶ������ڵĴ��ڹ��̺���
LRESULT CALLBACK SecondWndProc(HWND, UINT, WPARAM, LPARAM);

// ������¼���ڵĴ��ڹ��̺���
LRESULT CALLBACK Login_WndProc(HWND, UINT, WPARAM, LPARAM);

// ����ע�ᴰ�ڵĴ��ڹ��̺���
LRESULT CALLBACK Signup_WndProc(HWND, UINT, WPARAM, LPARAM);

// �������񴰿ڵĴ��ڹ��̺���
LRESULT CALLBACK Service_WndProc(HWND, UINT, WPARAM, LPARAM);

// ���������ڵĴ��ڹ��̺���
LRESULT CALLBACK Admin_WndProc(HWND, UINT, WPARAM, LPARAM);

// ���������ڹ��̺���
LRESULT CALLBACK Main_WndProc(HWND, UINT, WPARAM, LPARAM);

HWND hwnd_Main_Window, hwnd_Login_Window, hwnd_Signup_Window, hwnd_Service_Window, hwnd_Admin_Window;         // �����ھ��

// WinMain��Ӧ�ó�����ں���
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {    

    hInst = hInstance; // ����ȫ�ֱ��� hInst

    // ע��ڶ���������
    WNDCLASS wcSecond = {0};
    wcSecond.lpfnWndProc = SecondWndProc;
    wcSecond.hInstance = hInstance;
    wcSecond.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wcSecond.lpszClassName = _T("SecondWindowClass");
    if (!RegisterClass(&wcSecond)) {
        MessageBox(NULL, _T("�ڶ���������ע��ʧ�ܣ�"), _T("����"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ע���û���¼������
    WNDCLASS wc_Login = {0};
    wc_Login.lpfnWndProc = Login_WndProc;
    wc_Login.hInstance = hInstance;
    wc_Login.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Login.lpszClassName = _T("LoginWindowClass");
    if (!RegisterClass(&wc_Login)) {
        MessageBox(NULL, _T("�û���¼������ע��ʧ�ܣ�"), _T("����"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ע���û�ע�ᴰ����
    WNDCLASS wc_Signup = {0};
    wc_Signup.lpfnWndProc = Signup_WndProc;
    wc_Signup.hInstance = hInstance;
    wc_Signup.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Signup.lpszClassName = _T("SignupWindowClass");
    if (!RegisterClass(&wc_Signup)) {
        MessageBox(NULL, _T("�û�ע�ᴰ����ע��ʧ�ܣ�"), _T("����"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ע���û����񴰿���
    WNDCLASS wc_Service = {0};
    wc_Service.lpfnWndProc = Service_WndProc;
    wc_Service.hInstance = hInstance;
    wc_Service.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Service.lpszClassName = _T("ServiceWindowClass");
    if (!RegisterClass(&wc_Service)) {
        MessageBox(NULL, _T("�û����񴰿���ע��ʧ�ܣ�"), _T("����"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ע�����Ա������
    WNDCLASS wc_Admin = {0};
    wc_Admin.lpfnWndProc = Admin_WndProc;
    wc_Admin.hInstance = hInstance;
    wc_Admin.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Admin.lpszClassName = _T("AdminWindowClass");
    if (!RegisterClass(&wc_Admin)) {
        MessageBox(NULL, _T("����Ա������ע��ʧ�ܣ�"), _T("����"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // �����������
    WNDCLASS wc;       // ������ṹ
    MSG Msg;           // ��Ϣ�ṹ
    wc.style = 0;                          // ������ʽ
    wc.lpfnWndProc = Main_WndProc;              // ָ�����ڹ��̺���
    wc.cbClsExtra = 0;                     // ����������ڴ�
    wc.cbWndExtra = 0;                     // ���ڶ����ڴ�
    wc.hInstance = hInstance;              // Ӧ�ó���ʵ�����
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);  // ����ͼ��
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);    // ���ع��
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1); // ����ˢ
    wc.lpszMenuName = NULL;                // �˵�����
    wc.lpszClassName = _T("MainWindowClass"); // ����������
    // ע�ᴰ����
    if (!RegisterClass(&wc)) {
        MessageBox(NULL, _T("������ע��ʧ�ܣ�"), _T("����"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ����������
    hwnd_Main_Window = CreateWindow(
        _T("MainWindowClass"),               // ����������
        _T("v1 Makerspace Seat Booking System"),                // ���ڱ���
        WS_OVERLAPPEDWINDOW,               // ������ʽ
        CW_USEDEFAULT, CW_USEDEFAULT,      // ����λ��
        240, 240,                          // ���ڴ�С
        NULL, NULL, hInstance, NULL);      // ��������

    ShowWindow(hwnd_Main_Window, nCmdShow);           // ��ʾ����
    UpdateWindow(hwnd_Main_Window);                   // ���´���

    // ��Ϣѭ��
    while (GetMessage(&Msg, NULL, 0, 0) > 0) {
        TranslateMessage(&Msg);           // ������Ϣ
        DispatchMessage(&Msg);            // �ַ���Ϣ
    }
    return (int)Msg.wParam;
}


boolean Login_Flag = 0;
short Auth_Flag = 0;

HWND hwndButton_Login_Window, hwndButton_Service_Window, hwndButton_Admin_Window;

// �����ڹ��̺���
LRESULT CALLBACK Main_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_CREATE: // ���ڴ�����Ϣ
            printf("Main Window WM_CREATE\n");

            //         printf("%d\n", Login_Flag);
            // if (Login_Flag) // �ѵ�¼
            // {
            //     printf("�ѵ�¼\n");

            //     printf("%d\n", Auth_Flag);
            //     if (!Auth_Flag) // ��ͨ�û�
            //     {
            //         printf("��ͨ�û�\n");
            //     }
            //     else if (Auth_Flag)    // ����Ա
            //     {
            //         printf("����Ա\n");
            //     }
            // }
            // else if (!Login_Flag)   // δ��¼
            // {

            //     printf("δ��¼\n");
                
            //     ShowWindow(hwnd_Main_Window, SW_HIDE);  // ����������

            //     // ��������ʾ�ڶ�������
            //     hwnd_Login_Window = CreateWindow(
            //         _T("LoginWindowClass"),
            //         _T("Login System"),
            //         WS_OVERLAPPEDWINDOW,
            //         CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
            //         NULL, NULL, hInst, NULL);
            //     ShowWindow(hwnd_Login_Window, SW_SHOW);
            //     UpdateWindow(hwnd_Login_Window);

            // }

            // ������̬�ı�
            hwndLabel = CreateWindow(
                _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                _T("USER"),                    // Ĭ���ı�Ϊ��
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                10, 40, 100, 20,           // λ�úʹ�С
                hwnd, (HMENU)3, hInst, NULL); // �����ں���������

            // ������ť
            hwndButton_Login_Window = CreateWindow(
                _T("BUTTON"),              // �ؼ����ͣ���ť
                _T("LOGIN"),                // ��ť�ı�
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                10, 10, 50, 20,           // λ�úʹ�С
                hwnd, (HMENU)1, hInst, NULL); // �����ں���������

            hwndButton_Service_Window = CreateWindow(
                _T("BUTTON"),              // �ؼ����ͣ���ť
                _T("SERVICE"),                // ��ť�ı�
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                80, 10, 50, 20,           // λ�úʹ�С
                hwnd, (HMENU)2, hInst, NULL); // �����ں���������

            hwndButton_Admin_Window = CreateWindow(
                _T("BUTTON"),              // �ؼ����ͣ���ť
                _T("ADMIN"),                // ��ť�ı�
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                80, 40, 50, 20,           // λ�úʹ�С
                hwnd, (HMENU)3, hInst, NULL); // �����ں���������

            break;

        // case WM_PAINT: // �ػ���Ϣ
        // // printf("WM_PAINT\n");

        // if (Login_Flag){
        //     SetWindowText(hwndButton, Login_User.username);     // ���þ�̬�ı���ʾ
        // }
        // else if (!Login_Flag){
        //     SetWindowText(hwndButton, _T("LOGIN"));     // ���þ�̬�ı���ʾ
        // }
            
        // break;

        case WM_ACTIVATE: // ������Ϣ
        // printf("WM_ACTIVATE\n");

        if (Login_Flag){
            SetWindowText(hwndButton_Login_Window, Login_User.username);     // ���þ�̬�ı���ʾ
        }
        else if (!Login_Flag){
            SetWindowText(hwndButton_Login_Window, _T("LOGIN"));     // ���þ�̬�ı���ʾ
        }

        break;

        case WM_SHOWWINDOW: // ������Ϣ

        break;

        case WM_COMMAND: // ������Ϣ

            

            if (LOWORD(wParam) == 1) { // �ж��Ƿ����˴򿪵�¼���ڵİ�ť
                printf("Press Login Button: %d\n", Login_Flag);
                if (Login_Flag == 0) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // ����������

                    // ��������ʾ�ڶ�������
                    hwnd_Login_Window = CreateWindow(
                        _T("LoginWindowClass"),
                        _T("Login System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Login_Window, SW_SHOW);
                    UpdateWindow(hwnd_Login_Window);
                }
                else if (Login_Flag == 1) {
                    // ���û���Ϣ������ֵ
                    MessageBox(NULL, "Already Login", "Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            if (LOWORD(wParam) == 2) { // �ж��Ƿ����˴򿪷��񴰿ڵİ�ť
                printf("Press Service Button: %d\n", Login_Flag);
                if (Login_Flag == 1) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // ����������

                    // ��������ʾ�ڶ�������
                    hwnd_Service_Window = CreateWindow(
                        _T("ServiceWindowClass"),
                        _T("Service System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Service_Window, SW_SHOW);
                    UpdateWindow(hwnd_Service_Window);
                }
                else {
                    MessageBox(NULL, "Please Login First", "Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            if (LOWORD(wParam) == 3) { // �ж��Ƿ����˴򿪹����ڵİ�ť
                printf("Press Admin Button: %d\n", Login_Flag);
                if (Auth_Flag == 1) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // ����������

                    // ��������ʾ�ڶ�������
                    hwnd_Admin_Window = CreateWindow(
                        _T("AdminWindowClass"),
                        _T("Admin System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Admin_Window, SW_SHOW);
                    UpdateWindow(hwnd_Admin_Window);
                }
                else {
                    MessageBox(NULL, "Not a Admin", "Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            break;

        case WM_CLOSE: // ���ڹر���Ϣ
            DestroyWindow(hwnd);          // ���ٴ���
            // ShowWindow(hwnd_Main_Window, SW_HIDE);  // ����������
            break;

        case WM_DESTROY: // ����������Ϣ
            PostQuitMessage(0);           // �����˳���Ϣ
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam); // Ĭ�ϴ���
    }
    return 0;
}

// �ڶ������ڵĴ��ڹ��̺���
LRESULT CALLBACK SecondWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {

            case WM_CREATE: // ���ڴ�����Ϣ
                printf("Second Window WM_CREATE\n");

            // �����༭��
            hwndEdit = CreateWindow(
                _T("EDIT"),                // �ؼ����ͣ��༭��
                _T(""),                    // Ĭ���ı�Ϊ��
                WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
                10, 10, 100, 20,           // λ�úʹ�С
                hwnd, (HMENU)1, hInst, NULL); // �����ں���������

            // �����༭��
            hwndEdit2 = CreateWindow(
                _T("EDIT"),                // �ؼ����ͣ��༭��
                _T(""),                    // Ĭ���ı�Ϊ��
                WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
                10, 80, 100, 20,           // λ�úʹ�С
                hwnd, (HMENU)4, hInst, NULL); // �����ں���������

            // ������ť
            hwndButton = CreateWindow(
                _T("BUTTON"),              // �ؼ����ͣ���ť
                _T("COPY"),                // ��ť�ı�
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                120, 10, 50, 20,           // λ�úʹ�С
                hwnd, (HMENU)2, hInst, NULL); // �����ں���������

            // ������̬�ı�
            hwndLabel = CreateWindow(
                _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                _T("7"),                    // Ĭ���ı�Ϊ��
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                10, 40, 100, 20,           // λ�úʹ�С
                hwnd, (HMENU)3, hInst, NULL); // �����ں���������

            // ������̬�ı�
            hwndLabel2 = CreateWindow(
                _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                _T("666"),                    // Ĭ���ı�Ϊ��
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                10, 100, 200, 20,           // λ�úʹ�С
                hwnd, (HMENU)3, hInst, NULL); // �����ں���������

            hwndOpenNewWindowButton = CreateWindow(
                _T("BUTTON"),              // �ؼ����ͣ���ť
                _T("���´���"),          // ��ť�ı�
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                120, 80, 100, 30,          // λ�úʹ�С
                hwnd, (HMENU)5, hInst, NULL); // �����ں���������

            break;

        case WM_COMMAND: // ������Ϣ
        
            if (LOWORD(wParam) == 4) {    // ������ĸ��ؼ���������Ϣ
                TCHAR szText[100];        // �ı�������
                GetWindowText(hwndEdit, szText, 100); // ��ȡ�༭���ı�
                GetWindowText(hwndEdit2, szText, 100); // ��ȡ�༭���ı�
                SetWindowText(hwndLabel, szText);     // ���þ�̬�ı���ʾ
                SetWindowText(hwndLabel2, szText);     // ���þ�̬�ı���ʾ
            }

            if (LOWORD(wParam) == 5) { // �ж��Ƿ����˴��´��ڵİ�ť
                // ��������ʾ�ڶ�������
                HWND hwndSecond = CreateWindow(
                    _T("SecondWindowClass"), _T("�ڶ�������"),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    NULL, NULL, hInst, NULL);
                ShowWindow(hwndSecond, SW_SHOW);
            }

            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

    HWND hwndLabel_Login_Username, hwndLabel_Login_Password, hwndEdit_Login_Username, hwndEdit_Login_Password, hwndButton_Login, hwndButton_SginUp;
    #define ID_LABEL_LOGIN_USERNAME 1
    #define ID_LABEL_LOGIN_PASSWORD 2
    #define ID_EDIT_LOGIN_USERNAME 3
    #define ID_EDIT_LOGIN_PASSWORD 4
    #define ID_BUTTON_LOGIN 5
    #define ID_BUTTON_SGINUP 6
    // const int ID_LABEL_LOGIN_USERNAME = 1, ID_LABEL_LOGIN_PASSWORD = 2, ID_EDIT_LOGIN_USERNAME = 3, ID_EDIT_LOGIN_PASSWORD = 4, ID_BUTTON_LOGIN = 5, ID_BUTTON_SGINUP = 6;

// ��¼���ڹ��̺���
LRESULT CALLBACK Login_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // ���ڴ�����Ϣ
                    printf("Login Window WM_CREATE\n");
                
                // ������̬�ı�
                hwndLabel_Login_Username = CreateWindow(
                    _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                    _T("USERNAME"),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    10, 10, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_LABEL_LOGIN_USERNAME, hInst, NULL); // �����ں���������

                // ������̬�ı�
                hwndLabel_Login_Password = CreateWindow(
                    _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                    _T("PASSWORD"),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    10, 70, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_LABEL_LOGIN_PASSWORD, hInst, NULL); // �����ں���������

                // �����༭��
                hwndEdit_Login_Username = CreateWindow(
                    _T("EDIT"),                // �ؼ����ͣ��༭��
                    _T(""),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
                    10, 40, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_EDIT_LOGIN_USERNAME, hInst, NULL); // �����ں���������

                // �����༭��
                hwndEdit_Login_Password = CreateWindow(
                    _T("EDIT"),                // �ؼ����ͣ��༭��
                    _T(""),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
                    10, 100, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_EDIT_LOGIN_PASSWORD, hInst, NULL); // �����ں���������

                // ������ť
                hwndButton_Login = CreateWindow(
                    _T("BUTTON"),              // �ؼ����ͣ���ť
                    _T("LOGIN"),                // ��ť�ı�
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    120, 20, 60, 30,           // λ�úʹ�С
                    hwnd, (HMENU)ID_BUTTON_LOGIN, hInst, NULL); // �����ں���������

                hwndButton_SginUp = CreateWindow(
                    _T("BUTTON"),              // �ؼ����ͣ���ť
                    _T("SIGNUP"),          // ��ť�ı�
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    120, 80, 60, 30,          // λ�úʹ�С
                    hwnd, (HMENU)ID_BUTTON_SGINUP, hInst, NULL); // �����ں���������

            break;

        case WM_COMMAND: // ������Ϣ
                if (LOWORD(wParam) == ID_BUTTON_LOGIN) {    // ������ĸ��ؼ���������Ϣ

                // ��¼
                TCHAR szText_username[100];  // �ı�������
                TCHAR szText_passwd[100];    // �ı�������
                GetWindowText(hwndEdit_Login_Username, szText_username, 100); // ��ȡ�û���
                GetWindowText(hwndEdit_Login_Password, szText_passwd, 100);   // ��ȡ����

                // char username[100];
                char password[100];
                ConvertTCharToChar(szText_username, Login_User.username, 100); // ת���û���
                ConvertTCharToChar(szText_passwd, password, 100);   // ת������

                // char* UserInfoFilename = "UserInfo.json";
                char* jsonStr = readFileToString(USER_INFO_DATABASE); // ��ȡ�ļ����ݵ��ַ���
                if (jsonStr != NULL) {
                    printf("File content:\n%s\n", jsonStr);
                }
                else printf("Error reading user info file.\n");

                // char passwd[64];
                // short auth;
                // unsigned int balance;
                int userFound = findUser(jsonStr, Login_User.username, Login_User.passwd, &Login_User.auth, &Login_User.balance); // �����û���Ϣ
                if (userFound) {
                    printf("Password: %s\n", Login_User.passwd);
                    printf("Auth: %d\n", Login_User.auth);
                    printf("Balance: %u\n", Login_User.balance);
                } else {
                    printf("User not found.\n");
                }
                // printf("File content:\n%s\n", jsonStr);

                if (strcmp(password, Login_User.passwd) == 0 && Login_User.username[0] != '\0' && Login_User.passwd[0] != 0)  // ��������Ƿ���ȷ
                {
                    Auth_Flag = Login_User.auth;

                    Login_Flag = 1;

                    printf("Login Success!\n");

                    RefreshWindow(hwnd_Main_Window);
                    // ShowWindow(hwnd, SW_HIDE);  // ���ش���
                    ShowWindow(hwnd_Main_Window, SW_SHOW);  // ���ش���

                    DestroyWindow(hwnd);    // ���ٵ�¼����

                    // // ��������ʾ�ڶ�������
                    // HWND hwndSecond = CreateWindow(
                    //     _T("SecondWindowClass"), _T("�ڶ�������"),
                    //     WS_OVERLAPPEDWINDOW,
                    //     CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    //     NULL, NULL, hInst, NULL);
                    // ShowWindow(hwndSecond, SW_SHOW);
                    // // UpdateWindow(hwndSecond);
                }
                // ��¼����
                else if (szText_username[0] == '\0' && szText_passwd[0] == '\0') {
                    Login_Flag = 0;
                    printf("Login Failed!\n");
                    MessageBox(NULL, "Please Type User Info", "Error", MB_ICONINFORMATION | MB_OK);
                }
                else if (!userFound || szText_username[0] == '\0') {
                    Login_Flag = 0;
                    printf("Username Failed!\n");
                    MessageBox(NULL, "Please Check Username", "Error", MB_ICONINFORMATION | MB_OK);
                }
                else if (strcmp(password, Login_User.passwd) != 0 && Login_User.username[0] != '\0' && Login_User.passwd[0] != '\0') {
                    Login_Flag = 0;
                    printf("Password Failed!\n");
                    MessageBox(NULL, "Please Check Password", "Error", MB_ICONINFORMATION | MB_OK);
                }

                free(jsonStr); // �ͷ��ڴ�

            }

            if (LOWORD(wParam) == ID_BUTTON_SGINUP) { // �ж��Ƿ����˴��´��ڵİ�ť
                // ��������ʾע�ᴰ��
                hwnd_Signup_Window = CreateWindow(
                    _T("SignupWindowClass"), _T("Sign Up"),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    NULL, NULL, hInst, NULL);
                ShowWindow(hwnd_Signup_Window, SW_SHOW);
                UpdateWindow(hwnd_Signup_Window);
                ShowWindow(hwnd, SW_HIDE);  // ���ش���
            }

            break;

        case WM_CLOSE:
            RefreshWindow(hwnd_Main_Window);
            // ShowWindow(hwnd, SW_HIDE);  // ���ش���
            ShowWindow(hwnd_Main_Window, SW_SHOW);  // ���ش���
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

HWND hwndLabel_Signup_Username, hwndLabel_Signup_Password, hwndEdit_Signup_Username, hwndEdit_Signup_Password, hwndButton_Signup;
    #define ID_LABEL_SIGNUP_USERNAME 1
    #define ID_LABEL_SIGNUP_PASSWORD 2
    #define ID_EDIT_SIGNUP_USERNAME 3
    #define ID_EDIT_SIGNUP_PASSWORD 4
    #define ID_BUTTON_SIGNUP 5
// �û�ע�ᴰ�ڹ��̺���
LRESULT CALLBACK Signup_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // ���ڴ�����Ϣ
                    printf("Login Window WM_CREATE\n");
                
                // ������̬�ı�
                hwndLabel_Signup_Username = CreateWindow(
                    _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                    _T("USERNAME"),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    10, 10, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)1, hInst, NULL); // �����ں���������

                // ������̬�ı�
                hwndLabel_Signup_Password = CreateWindow(
                    _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                    _T("PASSWORD"),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    10, 70, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)2, hInst, NULL); // �����ں���������

                // �����༭��
                hwndEdit_Signup_Username = CreateWindow(
                    _T("EDIT"),                // �ؼ����ͣ��༭��
                    _T(""),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
                    10, 40, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)3, hInst, NULL); // �����ں���������

                // �����༭��
                hwndEdit_Signup_Password = CreateWindow(
                    _T("EDIT"),                // �ؼ����ͣ��༭��
                    _T(""),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
                    10, 100, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)4, hInst, NULL); // �����ں���������

                // ������ť
                hwndButton_Signup = CreateWindow(
                    _T("BUTTON"),              // �ؼ����ͣ���ť
                    _T("SIGN UP"),                // ��ť�ı�
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    120, 20, 60, 30,           // λ�úʹ�С
                    hwnd, (HMENU)5, hInst, NULL); // �����ں���������

            break;

        case WM_COMMAND: // ������Ϣ
                if (LOWORD(wParam) == 5) {    // ������ĸ��ؼ���������Ϣ

                // ע��
                TCHAR szText_username[100];  // �ı�������
                TCHAR szText_passwd[100];    // �ı�������
                GetWindowText(hwndEdit_Signup_Username, szText_username, 100); // ��ȡ�û���
                GetWindowText(hwndEdit_Signup_Password, szText_passwd, 100);   // ��ȡ����

                char username[100];
                char password[100];
                ConvertTCharToChar(szText_username, username, 100); // ת���û���
                ConvertTCharToChar(szText_passwd, password, 100);   // ת������

                if (username[0] != 0 && password[0] != 0) {

                    char* UserInfoFilename = "UserInfo.json";
                    char* jsonStr = readFileToString(UserInfoFilename); // ��ȡ�ļ����ݵ��ַ���
                    if (jsonStr != NULL) {
                        printf("File content:\n%s\n", jsonStr);
                    }
                    else printf("Error reading user info file.\n");

                    // ������û�
                    char* updatedJsonStr = addUser(jsonStr, username, password, 0);
                    if (updatedJsonStr != NULL) {
                        printf("������û���\n");
                        viewUser(updatedJsonStr);
                        writeStringToFile(USER_INFO_DATABASE, updatedJsonStr);

                        free(jsonStr);         // �ͷ��ڴ�
                        free(updatedJsonStr);   // �ͷ��ڴ�

                        // ���ص�¼����
                        MessageBox(NULL, "OK to Sign Up \n ", "OK", MB_ICONINFORMATION | MB_OK);
                        RefreshWindow(hwnd_Login_Window);
                        ShowWindow(hwnd_Login_Window, SW_SHOW);  // ���ش���
                        DestroyWindow(hwnd);
                    }
                    else {  // ����û����ظ�
                        printf("������û�ʧ�ܡ�\n");
                        MessageBox(NULL, "Error to Sign Up\nPlease try again\nUsernames May be Duplicated", "Error", MB_ICONINFORMATION | MB_OK);
                    }
                }
                else { // ����û���һ��δ����
                    MessageBox(NULL, "Please enter username and password", "Error", MB_ICONINFORMATION | MB_OK);
                }

            }

            break;

        case WM_CLOSE:
            RefreshWindow(hwnd_Login_Window);
            ShowWindow(hwnd_Login_Window, SW_SHOW);  // ���ش���
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


// ���񴰿ڹ��̺���
LRESULT CALLBACK Service_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // ���ڴ�����Ϣ
                printf("Service Window WM_CREATE\n");
                
                // ������̬�ı�
                hwndLabel_Login_Username = CreateWindow(
                    _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                    _T("USERNAME"),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    10, 10, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_LABEL_LOGIN_USERNAME, hInst, NULL); // �����ں���������

                // ������̬�ı�
                hwndLabel_Login_Password = CreateWindow(
                    _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                    _T("PASSWORD"),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    10, 70, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_LABEL_LOGIN_PASSWORD, hInst, NULL); // �����ں���������

                // �����༭��
                hwndEdit_Login_Username = CreateWindow(
                    _T("EDIT"),                // �ؼ����ͣ��༭��
                    _T(""),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
                    10, 40, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_EDIT_LOGIN_USERNAME, hInst, NULL); // �����ں���������

                // �����༭��
                hwndEdit_Login_Password = CreateWindow(
                    _T("EDIT"),                // �ؼ����ͣ��༭��
                    _T(""),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
                    10, 100, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_EDIT_LOGIN_PASSWORD, hInst, NULL); // �����ں���������

                // ������ť
                hwndButton_Login = CreateWindow(
                    _T("BUTTON"),              // �ؼ����ͣ���ť
                    _T("LOGIN"),                // ��ť�ı�
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    120, 20, 60, 30,           // λ�úʹ�С
                    hwnd, (HMENU)ID_BUTTON_LOGIN, hInst, NULL); // �����ں���������

                hwndButton_SginUp = CreateWindow(
                    _T("BUTTON"),              // �ؼ����ͣ���ť
                    _T("SIGNUP"),          // ��ť�ı�
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    120, 80, 60, 30,          // λ�úʹ�С
                    hwnd, (HMENU)ID_BUTTON_SGINUP, hInst, NULL); // �����ں���������

            break;

        case WM_COMMAND: // ������Ϣ
                if (LOWORD(wParam) == ID_BUTTON_LOGIN) {    // ������ĸ��ؼ���������Ϣ

                // ��¼
                TCHAR szText_username[100];  // �ı�������
                TCHAR szText_passwd[100];    // �ı�������
                GetWindowText(hwndEdit_Login_Username, szText_username, 100); // ��ȡ�û���
                GetWindowText(hwndEdit_Login_Password, szText_passwd, 100);   // ��ȡ����

                // char username[100];
                char password[100];
                ConvertTCharToChar(szText_username, Login_User.username, 100); // ת���û���
                ConvertTCharToChar(szText_passwd, password, 100);   // ת������

                char* UserInfoFilename = "UserInfo.json";
                char* jsonStr = readFileToString(UserInfoFilename); // ��ȡ�ļ����ݵ��ַ���
                if (jsonStr != NULL) {
                    printf("File content:\n%s\n", jsonStr);
                }
                else printf("Error reading user info file.\n");

                // char passwd[64];
                // short auth;
                // unsigned int balance;
                int userFound = findUser(jsonStr, Login_User.username, Login_User.passwd, &Login_User.auth, &Login_User.balance); // �����û���Ϣ
                if (userFound) {
                    printf("Password: %s\n", Login_User.passwd);
                    printf("Auth: %d\n", Login_User.auth);
                    printf("Balance: %u\n", Login_User.balance);
                } else {
                    printf("User not found.\n");
                }
                // printf("File content:\n%s\n", jsonStr);

                if (strcmp(password, Login_User.passwd) == 0 && Login_User.username[0] != '\0' && Login_User.auth != 0 && Login_User.passwd != 0)  // ��������Ƿ���ȷ
                {
                    Auth_Flag = Login_User.auth;

                    Login_Flag = 1;

                    printf("Login Success!\n");

                    RefreshWindow(hwnd_Main_Window);
                    // ShowWindow(hwnd, SW_HIDE);  // ���ش���
                    ShowWindow(hwnd_Main_Window, SW_SHOW);  // ���ش���

                    DestroyWindow(hwnd);    // ���ٵ�¼����
                }
                else
                {
                    Login_Flag = 0;
                    printf("Login Failed!\n");
                }

                free(jsonStr); // �ͷ��ڴ�

            }

            if (LOWORD(wParam) == ID_BUTTON_SGINUP) { // �ж��Ƿ����˴��´��ڵİ�ť
                // ��������ʾ�ڶ�������
                HWND hwndSecond = CreateWindow(
                    _T("SecondWindowClass"), _T("�ڶ�������"),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    NULL, NULL, hInst, NULL);
                ShowWindow(hwndSecond, SW_SHOW);
                UpdateWindow(hwndSecond);
            }

            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// ����Ա���ڹ��̺���
LRESULT CALLBACK Admin_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // ���ڴ�����Ϣ
                printf("Service Window WM_CREATE\n");
                
                // ������̬�ı�
                hwndLabel_Login_Username = CreateWindow(
                    _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                    _T("USERNAME"),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    10, 10, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_LABEL_LOGIN_USERNAME, hInst, NULL); // �����ں���������

                // ������̬�ı�
                hwndLabel_Login_Password = CreateWindow(
                    _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                    _T("PASSWORD"),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    10, 70, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_LABEL_LOGIN_PASSWORD, hInst, NULL); // �����ں���������

                // �����༭��
                hwndEdit_Login_Username = CreateWindow(
                    _T("EDIT"),                // �ؼ����ͣ��༭��
                    _T(""),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
                    10, 40, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_EDIT_LOGIN_USERNAME, hInst, NULL); // �����ں���������

                // �����༭��
                hwndEdit_Login_Password = CreateWindow(
                    _T("EDIT"),                // �ؼ����ͣ��༭��
                    _T(""),                    // Ĭ���ı�Ϊ��
                    WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
                    10, 100, 100, 20,           // λ�úʹ�С
                    hwnd, (HMENU)ID_EDIT_LOGIN_PASSWORD, hInst, NULL); // �����ں���������

                // ������ť
                hwndButton_Login = CreateWindow(
                    _T("BUTTON"),              // �ؼ����ͣ���ť
                    _T("LOGIN"),                // ��ť�ı�
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    120, 20, 60, 30,           // λ�úʹ�С
                    hwnd, (HMENU)ID_BUTTON_LOGIN, hInst, NULL); // �����ں���������

                hwndButton_SginUp = CreateWindow(
                    _T("BUTTON"),              // �ؼ����ͣ���ť
                    _T("SIGNUP"),          // ��ť�ı�
                    WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                    120, 80, 60, 30,          // λ�úʹ�С
                    hwnd, (HMENU)ID_BUTTON_SGINUP, hInst, NULL); // �����ں���������

            break;

        case WM_COMMAND: // ������Ϣ
                if (LOWORD(wParam) == ID_BUTTON_LOGIN) {    // ������ĸ��ؼ���������Ϣ

                // ��¼
                TCHAR szText_username[100];  // �ı�������
                TCHAR szText_passwd[100];    // �ı�������
                GetWindowText(hwndEdit_Login_Username, szText_username, 100); // ��ȡ�û���
                GetWindowText(hwndEdit_Login_Password, szText_passwd, 100);   // ��ȡ����

                // char username[100];
                char password[100];
                ConvertTCharToChar(szText_username, Login_User.username, 100); // ת���û���
                ConvertTCharToChar(szText_passwd, password, 100);   // ת������

                char* UserInfoFilename = "UserInfo.json";
                char* jsonStr = readFileToString(UserInfoFilename); // ��ȡ�ļ����ݵ��ַ���
                if (jsonStr != NULL) {
                    printf("File content:\n%s\n", jsonStr);
                }
                else printf("Error reading user info file.\n");

                // char passwd[64];
                // short auth;
                // unsigned int balance;
                int userFound = findUser(jsonStr, Login_User.username, Login_User.passwd, &Login_User.auth, &Login_User.balance); // �����û���Ϣ
                if (userFound) {
                    printf("Password: %s\n", Login_User.passwd);
                    printf("Auth: %d\n", Login_User.auth);
                    printf("Balance: %u\n", Login_User.balance);
                } else {
                    printf("User not found.\n");
                }
                // printf("File content:\n%s\n", jsonStr);

                if (strcmp(password, Login_User.passwd) == 0 && Login_User.username[0] != '\0' && Login_User.auth != 0 && Login_User.passwd != 0)  // ��������Ƿ���ȷ
                {
                    Auth_Flag = Login_User.auth;

                    Login_Flag = 1;

                    printf("Login Success!\n");

                    RefreshWindow(hwnd_Main_Window);
                    // ShowWindow(hwnd, SW_HIDE);  // ���ش���
                    ShowWindow(hwnd_Main_Window, SW_SHOW);  // ���ش���

                    DestroyWindow(hwnd);    // ���ٵ�¼����

                    // // ��������ʾ�ڶ�������
                    // HWND hwndSecond = CreateWindow(
                    //     _T("SecondWindowClass"), _T("�ڶ�������"),
                    //     WS_OVERLAPPEDWINDOW,
                    //     CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    //     NULL, NULL, hInst, NULL);
                    // ShowWindow(hwndSecond, SW_SHOW);
                    // // UpdateWindow(hwndSecond);
                }
                else
                {
                    Login_Flag = 0;
                    printf("Login Failed!\n");
                }

                free(jsonStr); // �ͷ��ڴ�

            }

            if (LOWORD(wParam) == ID_BUTTON_SGINUP) { // �ж��Ƿ����˴��´��ڵİ�ť
                // ��������ʾ�ڶ�������
                HWND hwndSecond = CreateWindow(
                    _T("SecondWindowClass"), _T("�ڶ�������"),
                    WS_OVERLAPPEDWINDOW,
                    CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
                    NULL, NULL, hInst, NULL);
                ShowWindow(hwndSecond, SW_SHOW);
                UpdateWindow(hwndSecond);
            }

            break;

        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            return 0;
        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}



int main() {
    printf("Hello World!\n");
    // Login_Show();

    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOWNORMAL);
}
