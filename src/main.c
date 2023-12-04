#include <windows.h>  // ���� Windows API ��ͷ�ļ�
#include <tchar.h>    // ���� Unicode �� ANSI ���ݵĺ�����
#include <stdio.h>
#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")

#include "User_Lib.h"
#include "File_Manage.h"
#include "Login_User.h"
#include "Seat_Info.h"
#include "Reserve_Record.h"

#include "cJSON.h"
#include "main.h"


#define USER_INFO_DATABASE "UserInfo.json"
#define SEAT_INFO_DATABASE "SeatInfo.json"
#define RESERVE_RECORD_LOG "ReserveLog.csv"

// ��Ҫ����ȫ�ֱ���
Login_User_t Login_User;

boolean Login_Flag = 0;
short Auth_Flag = 0;

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

// ���������ڹ��̺���
LRESULT CALLBACK Main_WndProc(HWND, UINT, WPARAM, LPARAM);

// �����ڶ������ڵĴ��ڹ��̺���
LRESULT CALLBACK SecondWndProc(HWND, UINT, WPARAM, LPARAM);

// ������¼���ڵĴ��ڹ��̺���
LRESULT CALLBACK Login_WndProc(HWND, UINT, WPARAM, LPARAM);

// ����ע�ᴰ�ڵĴ��ڹ��̺���
LRESULT CALLBACK Signup_WndProc(HWND, UINT, WPARAM, LPARAM);

// �����û���Ϣ���ڵĴ��ڹ��̺���
LRESULT CALLBACK User_WndProc(HWND, UINT, WPARAM, LPARAM);

// �������񴰿ڵĴ��ڹ��̺���
LRESULT CALLBACK Service_WndProc(HWND, UINT, WPARAM, LPARAM);

// �����������ڵĴ��ڹ��̺���
LRESULT CALLBACK Order_WndProc(HWND, UINT, WPARAM, LPARAM);

// ���������ڵĴ��ڹ��̺���
LRESULT CALLBACK Admin_WndProc(HWND, UINT, WPARAM, LPARAM);

HWND hwnd_Main_Window, hwnd_Login_Window, hwnd_Signup_Window, hwnd_User_Window, hwnd_Service_Window, hwnd_Order_Window, hwnd_Admin_Window;         // �����ھ��

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
        MessageBox(NULL, _T("�ڶ���������ע��ʧ�ܣ�"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ע���û���¼������
    WNDCLASS wc_Login = {0};
    wc_Login.lpfnWndProc = Login_WndProc;
    wc_Login.hInstance = hInstance;
    wc_Login.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Login.lpszClassName = _T("LoginWindowClass");
    if (!RegisterClass(&wc_Login)) {
        MessageBox(NULL, _T("�û���¼������ע��ʧ�ܣ�"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ע���û�ע�ᴰ����
    WNDCLASS wc_Signup = {0};
    wc_Signup.lpfnWndProc = Signup_WndProc;
    wc_Signup.hInstance = hInstance;
    wc_Signup.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Signup.lpszClassName = _T("SignupWindowClass");
    if (!RegisterClass(&wc_Signup)) {
        MessageBox(NULL, _T("�û�ע�ᴰ����ע��ʧ�ܣ�"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ע���û���Ϣ������
    WNDCLASS wc_User = {0};
    wc_User.lpfnWndProc = User_WndProc;
    wc_User.hInstance = hInstance;
    wc_User.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_User.lpszClassName = _T("UserWindowClass");
    if (!RegisterClass(&wc_User)) {
        MessageBox(NULL, _T("�û���Ϣ������ע��ʧ�ܣ�"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ע���û����񴰿���
    WNDCLASS wc_Service = {0};
    wc_Service.lpfnWndProc = Service_WndProc;
    wc_Service.hInstance = hInstance;
    wc_Service.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Service.lpszClassName = _T("ServiceWindowClass");
    if (!RegisterClass(&wc_Service)) {
        MessageBox(NULL, _T("�û����񴰿���ע��ʧ�ܣ�"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ע���û�����������
    WNDCLASS wc_Order = {0};
    wc_Order.lpfnWndProc = Order_WndProc;
    wc_Order.hInstance = hInstance;
    wc_Order.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Order.lpszClassName = _T("OrderWindowClass");
    if (!RegisterClass(&wc_Order)) {
        MessageBox(NULL, _T("�û�����������ע��ʧ�ܣ�"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ע�����Ա������
    WNDCLASS wc_Admin = {0};
    wc_Admin.lpfnWndProc = Admin_WndProc;
    wc_Admin.hInstance = hInstance;
    wc_Admin.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc_Admin.lpszClassName = _T("AdminWindowClass");
    if (!RegisterClass(&wc_Admin)) {
        MessageBox(NULL, _T("����Ա������ע��ʧ�ܣ�"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
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
        MessageBox(NULL, _T("��������ע��ʧ�ܣ�"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
        return 0;
    }

    // ����������
    hwnd_Main_Window = CreateWindow(
        _T("MainWindowClass"),               // ����������
        _T("Makerspace Seat Booking System"),                // ���ڱ���
        WS_OVERLAPPEDWINDOW,               // ������ʽ
        CW_USEDEFAULT, CW_USEDEFAULT,      // ����λ��
        210, 120,                          // ���ڴ�С
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


HWND hwndButton_Login_Window, hwndButton_Service_Window, hwndButton_Order_Window, hwndButton_Admin_Window;
#define ID_BUTTON_MAIN_LOGIN 1
#define ID_BUTTON_MAIN_SERVICE 2
#define ID_BUTTON_MAIN_ORDER 3
#define ID_BUTTON_MAIN_ADMIN 4
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

            // ������ť
            hwndButton_Login_Window = CreateWindow(
                _T("BUTTON"),              // �ؼ����ͣ���ť
                _T("LOGIN"),                // ��ť�ı�
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                10, 10, 75, 20,           // λ�úʹ�С
                hwnd, (HMENU)ID_BUTTON_MAIN_LOGIN, hInst, NULL); // �����ں���������

            hwndButton_Service_Window = CreateWindow(
                _T("BUTTON"),              // �ؼ����ͣ���ť
                _T("SERVICE"),                // ��ť�ı�
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                100, 10, 75, 20,           // λ�úʹ�С
                hwnd, (HMENU)ID_BUTTON_MAIN_SERVICE, hInst, NULL); // �����ں���������

            hwndButton_Order_Window = CreateWindow(
                _T("BUTTON"),              // �ؼ����ͣ���ť
                _T("ORDER"),                    // Ĭ���ı�Ϊ��
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                10, 40, 75, 20,           // λ�úʹ�С
                hwnd, (HMENU)ID_BUTTON_MAIN_ORDER, hInst, NULL); // �����ں���������

            hwndButton_Admin_Window = CreateWindow(
                _T("BUTTON"),              // �ؼ����ͣ���ť
                _T("ADMIN"),                // ��ť�ı�
                WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                100, 40, 75, 20,           // λ�úʹ�С
                hwnd, (HMENU)ID_BUTTON_MAIN_ADMIN, hInst, NULL); // �����ں���������

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

            

            if (LOWORD(wParam) == ID_BUTTON_MAIN_LOGIN) { // �ж��Ƿ����˴򿪵�¼���ڵİ�ť
                printf("Press Login Button: %d\n", Login_Flag);
                if (Login_Flag == 0) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // ����������

                    // ��������ʾ�ڶ�������
                    hwnd_Login_Window = CreateWindow(
                        _T("LoginWindowClass"),
                        _T("Login System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 210, 170,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Login_Window, SW_SHOW);
                    UpdateWindow(hwnd_Login_Window);
                }
                else if (Login_Flag == 1) { // ����û��ѵ�¼
                    // ���û���Ϣ������ֵ
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // ����������

                    // ��������ʾ�ڶ�������
                    hwnd_User_Window = CreateWindow(
                        _T("UserWindowClass"),
                        _T("User Info"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 210, 170,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_User_Window, SW_SHOW);
                    UpdateWindow(hwnd_User_Window);


                    // MessageBox(NULL, "Already Login", "Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            if (LOWORD(wParam) == ID_BUTTON_MAIN_SERVICE) { // �ж��Ƿ����˴򿪷��񴰿ڵİ�ť
                printf("Press Service Button: %d\n", Login_Flag);
                if (Login_Flag == 1) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // ����������

                    // ��������ʾ���񴰿�
                    hwnd_Service_Window = CreateWindow(
                        _T("ServiceWindowClass"),
                        _T("Service System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 650, 600,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Service_Window, SW_SHOW);
                    UpdateWindow(hwnd_Service_Window);
                }
                else {
                    MessageBox(NULL, "Please Login First", "SERVICE Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            if (LOWORD(wParam) == ID_BUTTON_MAIN_ORDER) { // �ж��Ƿ����˴򿪶������ڵİ�ť
                printf("Press Service Button: %d\n", Login_Flag);
                if (Login_Flag == 1) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // ����������

                    // ��������ʾ��������
                    hwnd_Order_Window = CreateWindow(
                        _T("OrderWindowClass"),
                        _T("Order System"),
                        WS_OVERLAPPEDWINDOW,
                        CW_USEDEFAULT, CW_USEDEFAULT, 1150, 500,
                        NULL, NULL, hInst, NULL);
                    ShowWindow(hwnd_Order_Window, SW_SHOW);
                    UpdateWindow(hwnd_Order_Window);
                }
                else {
                    MessageBox(NULL, "Please Login First", "ORDER Error", MB_ICONINFORMATION | MB_OK);
                }
            }

            if (LOWORD(wParam) == ID_BUTTON_MAIN_ADMIN) { // �ж��Ƿ����˴򿪹����ڵİ�ť
                printf("Press Admin Button: %d\n", Login_Flag);
                if (Auth_Flag == 1) {
                    ShowWindow(hwnd_Main_Window, SW_HIDE);  // ����������

                    // ��������ʾ������
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
                    MessageBox(NULL, "Not an Admin", "ADMIN Error", MB_ICONINFORMATION | MB_OK);
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
                    CW_USEDEFAULT, CW_USEDEFAULT, 210, 170,
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

// �û���Ϣ���ڹ��̺���
LRESULT CALLBACK User_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // ���ڴ�����Ϣ
                    printf("Login Window WM_CREATE\n");

            break;

        case WM_COMMAND: // ������Ϣ

            break;

        case WM_CLOSE:
            RefreshWindow(hwnd_Main_Window);
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


// ����У����б���ͼ,���񴰿ڸ�������
void AddItemsToListView_Service(HWND hwndListView, LogEntry* logEntry_p, int logEntryCount) {
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    ListView_DeleteAllItems(hwndListView); // ��վ��б���ʾ
    lvItem.iSubItem = 0;

    if (logEntry_p) {
        for (int i = 0; i < logEntryCount; i++) {
            char pszTextStr[50]; // ���ӻ�������С�Ա������
            sprintf(pszTextStr, "<%d> %s", i+1, logEntry_p[i].period_date);
            lvItem.iItem = i;
            lvItem.pszText = pszTextStr;
            ListView_InsertItem(hwndListView, &lvItem);
            ListView_SetItemText(hwndListView, i, 1, logEntry_p[i].period_time_start);
            ListView_SetItemText(hwndListView, i, 2, logEntry_p[i].period_time_end);

            printf("i=%d\n", i);
        }
    }
    else {
        lvItem.iItem = 0;
        lvItem.pszText = "<NULL>";
    }
}
// ����У����б���ͼ�����񴰿ڸ�������
void AddColumnsToListView_Service(HWND hWndListView) {
    LVCOLUMN lvColumn;
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    // ��һ��
    lvColumn.pszText = "DATE";
    lvColumn.cx = 150;
    ListView_InsertColumn(hWndListView, 0, &lvColumn);

    // �ڶ���
    lvColumn.pszText = "START";
    lvColumn.cx = 150;
    ListView_InsertColumn(hWndListView, 1, &lvColumn);

    // ������
    lvColumn.pszText = "END";
    lvColumn.cx = 150;
    ListView_InsertColumn(hWndListView, 2, &lvColumn);
}

HWND hwndComboBox_Service_Type, hwndComboBox_Service_Id, hwndLabel_Service_Amount, hWndListView_Service_Output, hwndEditText_Service_Output;
#define ID_COMBOBOX_SERVICE_TYPE 1
#define ID_COMBOBOX_SERVICE_ID 2
#define ID_LABEL_SERVICE_AMOUNT 3
#define ID_BUTTON_SERVICE_CHECK 4
#define ID_DATEPICKER_SERVICE_DATE 5
#define ID_TIMEPICKER_SERVICE_START 6
#define ID_TIMEPICKER_SERVICE_END 7
#define ID_BUTTON_SERVICE_RESERVE 8
#define ID_LISTVIEW_SERVICE_OUTPUT 9
// #define ID_EDITTEXT_SERVICE_OUTPUT 10
// ���񴰿ڹ��̺���
LRESULT CALLBACK Service_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    SYSTEMTIME st;
    // char buffer[128];

    switch (msg) {
        case WM_CREATE: // ���ڴ�����Ϣ
            printf("Service Window WM_CREATE\n");

            // ������һ��������
            hwndComboBox_Service_Type = CreateWindow(WC_COMBOBOX, "", 
                            CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                            10, 10, 120, 150,  // λ�úʹ�С
                            hwnd, (HMENU)ID_COMBOBOX_SERVICE_TYPE,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            SendMessage(hwndComboBox_Service_Type, CB_SELECTSTRING, -1, (LPARAM)"TYPE");

            // �����ڶ���������
            hwndComboBox_Service_Id = CreateWindow(WC_COMBOBOX, "", 
                            CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                            10, 60, 120, 150,  // λ�úʹ�С
                            hwnd, (HMENU)ID_COMBOBOX_SERVICE_ID,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            SendMessage(hwndComboBox_Service_Id, CB_SELECTSTRING, -1, (LPARAM)"ID");

            // ������̬�ı���ʾ�۸�
            hwndLabel_Service_Amount = CreateWindow(
                            _T("STATIC"),              // �ؼ����ͣ���̬�ı�
                            _T("AMOUNT"),                    // Ĭ���ı�Ϊ��
                            WS_CHILD | WS_VISIBLE,     // ��ʽ���Ӵ��ڡ��ɼ�
                            10, 110, 100, 20,           // λ�úʹ�С
                            hwnd, (HMENU)2, hInst, NULL); // �����ں���������

            // ������鰴ť
            CreateWindowEx(0, "BUTTON", "Check Reservation",
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            10, 160, 140, 30,
                            hwnd, (HMENU)ID_BUTTON_SERVICE_CHECK,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // ��������ѡ��
            CreateWindowEx(0, DATETIMEPICK_CLASS, NULL,
                            DTS_UPDOWN | WS_BORDER | WS_CHILD | WS_VISIBLE,
                            10, 210, 140, 25,
                            hwnd, (HMENU)ID_DATEPICKER_SERVICE_DATE,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // ������ʼʱ��ѡ��
            CreateWindowEx(0, DATETIMEPICK_CLASS, NULL,
                            DTS_UPDOWN | DTS_TIMEFORMAT | WS_BORDER | WS_CHILD | WS_VISIBLE,
                            10, 260, 140, 25,
                            hwnd, (HMENU)ID_TIMEPICKER_SERVICE_START,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // ��������ʱ��ѡ��
            CreateWindowEx(0, DATETIMEPICK_CLASS, NULL,
                            DTS_UPDOWN | DTS_TIMEFORMAT | WS_BORDER | WS_CHILD | WS_VISIBLE,
                            10, 310, 140, 25,
                            hwnd, (HMENU)ID_TIMEPICKER_SERVICE_END,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // ����ԤԼ��ť
            CreateWindowEx(0, "BUTTON", "Make Reservation",
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            10, 360, 140, 30,
                            hwnd, (HMENU)ID_BUTTON_SERVICE_RESERVE,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // �����б���ͼ
            hWndListView_Service_Output = CreateWindow(WC_LISTVIEW, "",
                            WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL,
                            180, 10, 450, 700,  // λ�úʹ�С
                            hwnd, (HMENU)ID_LISTVIEW_SERVICE_OUTPUT,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            AddColumnsToListView_Service(hWndListView_Service_Output);  // �б���ͼ�����


            // hwndEditText_Service_Output = CreateWindowEx(0, "EDIT", "",
            //                 WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_READONLY,
            //                 300, 50, 200, 100, // λ�úʹ�С
            //                 hwnd, (HMENU)ID_EDITTEXT_SERVICE_OUTPUT,
            //                 ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // ���ñ༭�ؼ�����
            // SetWindowText(hwndEditText_Service_Output, "�����ǳ��ı�����...\n���԰�������\n��չʾ����Ч����\n�����Ǹ�����ı�����\n�����ǳ��ı�����...\n���԰�������\n��չʾ����Ч����\n�����ǳ��ı�����...\n���԰�������\n��չʾ����Ч����\n");

            break;

        case WM_COMMAND:

            if (LOWORD(wParam) == ID_COMBOBOX_SERVICE_TYPE) {    // type�������ȡtype
                // ����1����ȡ��ǰѡ�е�����
                TCHAR selectedType[50];
                int itemIndexType = SendMessage(hwndComboBox_Service_Type, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Type, CB_GETLBTEXT, itemIndexType, (LPARAM)selectedType);

                SendMessage(hwndComboBox_Service_Type, CB_RESETCONTENT, 0, 0);  // ���������
                SendMessage(hwndComboBox_Service_Id, CB_RESETCONTENT, 0, 0);

                char* jsonStr = readFileToString(SEAT_INFO_DATABASE); // ��ȡ�ļ����ݵ��ַ���
                if (jsonStr != NULL) {
                    printf("File content:\n%s\n", jsonStr);
                }
                else printf("Error reading user info file.\n");

                // ��ȡ������λ����
                char** seatTypes;
                int countType = getSeatTypes(jsonStr, &seatTypes);
                printf("Found %d different seat types\n", countType);
                for (int i = 0; i < countType; i++) {
                    printf("Seat Type: %s\n", seatTypes[i]);
                    SendMessage(hwndComboBox_Service_Type, CB_ADDSTRING, 0, (LPARAM)seatTypes[i]);
                    free(seatTypes[i]); // �ͷ�ÿ���ַ���
                }
                free(seatTypes); // �ͷ��ַ�������
                free(jsonStr);

                // ����4���ָ���ǰѡ�е�����
                SendMessage(hwndComboBox_Service_Type, CB_SELECTSTRING, -1, (LPARAM)selectedType);

            }

            if (LOWORD(wParam) == ID_COMBOBOX_SERVICE_ID) {    // id�������ȡid
                // ��ȡtype
                TCHAR selectedType[50];
                int itemIndexType = SendMessage(hwndComboBox_Service_Type, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Type, CB_GETLBTEXT, itemIndexType, (LPARAM)selectedType);

                // ��ȡ��ѡ���ID
                TCHAR selectedId[50];
                int itemIndexId = SendMessage(hwndComboBox_Service_Id, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Id, CB_GETLBTEXT, itemIndexId, (LPARAM)selectedId);


                SendMessage(hwndComboBox_Service_Id, CB_RESETCONTENT, 0, 0);    // ���������

                char* jsonStr = readFileToString(SEAT_INFO_DATABASE); // ��ȡ�ļ����ݵ��ַ���
                if (jsonStr != NULL) {
                    printf("File content:\n%s\n", jsonStr);
                }
                else printf("Error reading user info file.\n");

                // ͳ����������
                int* seatIds;
                int count = countSeatsByType(jsonStr, selectedType, &seatIds);
                printf("Found %d seats of type %s\n", count, selectedType);
                for (int i = 0; i < count; i++) {
                    char seatIdsStr[10]; // ����һ���㹻����ַ��������洢ת������ַ���
                    sprintf(seatIdsStr, "%d", seatIds[i]);
                    SendMessage(hwndComboBox_Service_Id, CB_ADDSTRING, 0, (LPARAM)seatIdsStr);
                    printf("Seat ID: %d\n", seatIds[i]);
                }
                free(seatIds); // �ͷ��ڴ�

                // ����4���ָ���ǰѡ�е�����
                SendMessage(hwndComboBox_Service_Id, CB_SELECTSTRING, -1, (LPARAM)selectedId);
                
                char str_amount[10] = {'\0'};
                sprintf(str_amount, "%d", findAmountById(jsonStr, atoi(selectedId)));
                SetWindowText(hwndLabel_Service_Amount, str_amount);     // ���ü۸��ı���ʾ

                free(jsonStr); // �ͷ��ڴ�
                printf("\n");

            }

            if (LOWORD(wParam) == ID_BUTTON_SERVICE_CHECK) {    // ������Ԥ����¼
                // ��ȡtype
                TCHAR selectedType[50] = {'\0'};
                int itemIndexType = SendMessage(hwndComboBox_Service_Type, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Type, CB_GETLBTEXT, itemIndexType, (LPARAM)selectedType);

                // ��ȡ��ѡ���ID
                TCHAR selectedId[50] = {'\0'};
                int itemIndexId = SendMessage(hwndComboBox_Service_Id, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Id, CB_GETLBTEXT, itemIndexId, (LPARAM)selectedId);

                if (selectedId[0] != 0 && selectedType[0] != 0) { // ���type��id�Ƿ�����       
        
                    int count1;
                    LogEntry* bookedSlots = get_booked_id_slots(RESERVE_RECORD_LOG, selectedType, atoi(selectedId), &count1);
                    // ... ������� bookedSlots �е�����
                    if (bookedSlots) {
                        for (int i = 0; i < count1; i++) {
                            printf("��Ԥ��ʱ��Σ�%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
                        }
                    }
                    else printf("û���ҵ���Ԥ��ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");

                    AddItemsToListView_Service(hWndListView_Service_Output, bookedSlots, count1); // ����е��б�

                    free(bookedSlots); // �ͷ��ڴ�

                }
                else {
                    printf("����дtype��id\n");
                    MessageBox(NULL, _T("Please Fill Type and ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                }

            }

            if (LOWORD(wParam) == ID_BUTTON_SERVICE_RESERVE) {

                LogEntry LogEntry_s_temp;
                char buffer[128];
                char buffer_temp[25];

                HWND hDatePicker = GetDlgItem(hwnd, ID_DATEPICKER_SERVICE_DATE);
                HWND hTimePicker_Start = GetDlgItem(hwnd, ID_TIMEPICKER_SERVICE_START);
                HWND hTimePicker_End = GetDlgItem(hwnd, ID_TIMEPICKER_SERVICE_END);

                // ��ȡ��ѡ���type
                TCHAR selectedType[50] = {'\0'};
                int itemIndexType = SendMessage(hwndComboBox_Service_Type, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Type, CB_GETLBTEXT, itemIndexType, (LPARAM)selectedType);

                // ��ȡ��ѡ���ID
                TCHAR selectedId[50] = {'\0'};
                int itemIndexId = SendMessage(hwndComboBox_Service_Id, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Service_Id, CB_GETLBTEXT, itemIndexId, (LPARAM)selectedId);

                printf("Selected type: %s\n", selectedType);
                printf("Selected id: %s\n", selectedId);

                if (selectedId[0] != 0 && selectedType[0] != 0) { // ���type��id�Ƿ�����

                    char* jsonStr_seat = readFileToString(SEAT_INFO_DATABASE); // ��ȡ�ļ����ݵ��ַ���
                    if (jsonStr_seat != NULL) {
                        printf("File content:\n%s\n", jsonStr_seat);
                    }
                    else printf("Error reading user info file.\n");

                    int amount = findAmountById(jsonStr_seat, atoi(selectedId)); // ��ȡ�۸�

                    printf("Price: %d\n", amount);
                    printf("Balance: %d\n", Login_User.balance);

                    free(jsonStr_seat);

                    if (Login_User.balance >= amount) {

                        LogEntry_s_temp.amount = amount;

                        // ���ɶ�����
                        char order_id[24];
                        generate_order_id(order_id);
                        strcpy(LogEntry_s_temp.order_id, order_id);

                        // ��ȡ����
                        DateTime_GetSystemtime(hDatePicker, &st);
                        sprintf(buffer_temp, "%04d/%02d/%02d", st.wYear, st.wMonth, st.wDay);
                        strcpy(LogEntry_s_temp.period_date, buffer_temp);

                        sprintf(buffer, "Date: %04d-%02d-%02d\n", st.wYear, st.wMonth, st.wDay);
                        
                        // ��ȡʱ��
                        DateTime_GetSystemtime(hTimePicker_Start, &st);
                        sprintf(buffer_temp, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
                        strcpy(LogEntry_s_temp.period_time_start, buffer_temp);

                        sprintf(buffer + strlen(buffer), "Time: %02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond);

                        // ��ȡʱ��
                        DateTime_GetSystemtime(hTimePicker_End, &st);
                        sprintf(buffer_temp, "%02d:%02d:%02d", st.wHour, st.wMinute, st.wSecond);
                        strcpy(LogEntry_s_temp.period_time_end, buffer_temp);

                        sprintf(buffer + strlen(buffer), "Time: %02d:%02d:%02d\n", st.wHour, st.wMinute, st.wSecond);
                        
                        printf("%s", buffer);   //  ��ӡ���ڣ�ʱ��

                        strcpy(LogEntry_s_temp.operate, Login_User.username);
                        strcpy(LogEntry_s_temp.action, "appoint");
                        LogEntry_s_temp.id = atoi(selectedId);
                        strcpy(LogEntry_s_temp.seat_type, selectedType);

                        strcpy(LogEntry_s_temp.subscriber, Login_User.username);

                        // д��Ԥ����־
                        if (write_log_realtime_conflict(RESERVE_RECORD_LOG, &LogEntry_s_temp) == 0)
                        {
                            MessageBox(NULL, _T("Success to Reserve"), _T("OK"), MB_ICONEXCLAMATION | MB_OK);
                            
                            Login_User.balance = Login_User.balance - amount;   // �۷�
                            char* jsonStr_user = readFileToString(USER_INFO_DATABASE); // ��ȡ�ļ����ݵ��ַ���
                            if (jsonStr_user != NULL) {
                                printf("File content:\n%s\n", jsonStr_user);
                            }
                            else printf("Error reading user info file.\n");
                            char* modifiedJsonStr = modifyUser(jsonStr_user, Login_User.username, Login_User.passwd, Login_User.auth, Login_User.balance);
                            if (modifiedJsonStr != NULL) {
                                printf("�޸��û���Ϣ��\n");
                                viewUser(modifiedJsonStr);
                                writeStringToFile(USER_INFO_DATABASE, modifiedJsonStr); // д��
                            }
                            printf("\n");
                            free(modifiedJsonStr);
                            free(jsonStr_user);
                        }
                        else MessageBox(NULL, _T("Fail to Reserve"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                    }
                    else {
                        printf("����\n");
                        MessageBox(NULL, _T("Not Sufficient Balance"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                    }

                    // ˢ���б���ͼ
                    int count1;
                    LogEntry* bookedSlots = get_booked_id_slots(RESERVE_RECORD_LOG, selectedType, atoi(selectedId), &count1);
                    // ... ������� bookedSlots �е�����
                    if (bookedSlots) {
                        for (int i = 0; i < count1; i++) {
                            printf("��Ԥ��ʱ��Σ�%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
                        }
                    }
                    else printf("û���ҵ���Ԥ��ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");

                    AddItemsToListView_Service(hWndListView_Service_Output, bookedSlots, count1); // ����е��б�

                    free(bookedSlots); // �ͷ��ڴ�

                }
                else {
                    printf("����дtype��id\n");
                    MessageBox(NULL, _T("Please Fill Type and ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                }

            }
            break;

        case WM_CLOSE:
            // ��������������رմ���ǰ�Ĵ���

            RefreshWindow(hwnd_Main_Window);
            ShowWindow(hwnd_Main_Window, SW_SHOW);  // ���ش���
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            // PostQuitMessage(0);
            return 0;
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}


// ����У����б���ͼ,���񴰿ڸ�������
void AddItemsToListView_Order(HWND hwndListView, LogEntry* logEntry_p, int logEntryCount) {
    LVITEM lvItem;
    lvItem.mask = LVIF_TEXT;
    ListView_DeleteAllItems(hwndListView); // ��վ��б���ʾ
    lvItem.iSubItem = 0;

    if (logEntry_p) {
        for (int i = 0; i < logEntryCount; i++) {
            // char pszTextStr[50]; // ���ӻ�������С�Ա������
            // sprintf(pszTextStr, "<%d> %s", i+1, logEntry_p[i].logtime);
            lvItem.iItem = i;
            // lvItem.pszText = pszTextStr;
            lvItem.pszText = logEntry_p[i].order_id;
            ListView_InsertItem(hwndListView, &lvItem);
            ListView_SetItemText(hwndListView, i, 1, logEntry_p[i].seat_type);
            char str_id[12];
            sprintf(str_id, "%d", logEntry_p[i].id);
            ListView_SetItemText(hwndListView, i, 2, str_id);
            ListView_SetItemText(hwndListView, i, 3, logEntry_p[i].period_date);
            ListView_SetItemText(hwndListView, i, 4, logEntry_p[i].period_time_start);
            ListView_SetItemText(hwndListView, i, 5, logEntry_p[i].period_time_end);
            char str_amount[12];
            sprintf(str_amount, "%d", logEntry_p[i].amount);
            ListView_SetItemText(hwndListView, i, 6, str_amount);
            char pszTextStr[50]; // ���ӻ�������С�Ա������
            sprintf(pszTextStr, "%s <%d>", logEntry_p[i].logtime, i+1);
            ListView_SetItemText(hwndListView, i, 7, pszTextStr);

            printf("i=%d\n", i);
        }
    }
    else {
        lvItem.iItem = 0;
        lvItem.pszText = "<NULL>";
    }
}
// ����У����б���ͼ�����񴰿ڸ�������
void AddColumnsToListView_Order(HWND hWndListView) {
    LVCOLUMN lvColumn;
    lvColumn.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;

    // ��0��
    lvColumn.pszText = "ORDER-ID";
    lvColumn.cx = 180;
    ListView_InsertColumn(hWndListView, 0, &lvColumn);

    // ��1��
    lvColumn.pszText = "TYPE";
    lvColumn.cx = 100;
    ListView_InsertColumn(hWndListView, 1, &lvColumn);

    // ��2��
    lvColumn.pszText = "ID";
    lvColumn.cx = 50;
    ListView_InsertColumn(hWndListView, 2, &lvColumn);

    // ��3��
    lvColumn.pszText = "DATE";
    lvColumn.cx = 100;
    ListView_InsertColumn(hWndListView, 3, &lvColumn);

    // ��4��
    lvColumn.pszText = "START";
    lvColumn.cx = 100;
    ListView_InsertColumn(hWndListView, 4, &lvColumn);

    // ��5��
    lvColumn.pszText = "END";
    lvColumn.cx = 100;
    ListView_InsertColumn(hWndListView, 5, &lvColumn);

    // ��6��
    lvColumn.pszText = "AMOUNT";
    lvColumn.cx = 70;
    ListView_InsertColumn(hWndListView, 6, &lvColumn);

    // ��7��
    lvColumn.pszText = "LogTime";
    lvColumn.cx = 200;
    ListView_InsertColumn(hWndListView, 7, &lvColumn);
}

HWND hWndListView_Order_Output, hwndComboBox_Order_OrderId;
// hwndEditText_Order_OrderId, 
#define ID_BUTTON_ORDER_CHECKALL 1
#define ID_DATEPICKER_ORDER_DATE 2
#define ID_BUTTON_ORDER_CHECKDATE 3
#define ID_LISTVIEW_ORDER_OUTPUT 4
// #define ID_EDITTEXT_ORDER_ORDERID 5
#define ID_COMBOBOX_ORDER_ORDERID 5
#define ID_BUTTON_ORDER_CANCEL 6
// �������ڹ��̺���
LRESULT CALLBACK Order_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    SYSTEMTIME st;

    switch (msg) {
        case WM_CREATE: // ���ڴ�����Ϣ
            printf("Order Window WM_CREATE\n");

            // ����������������
            hwndComboBox_Order_OrderId = CreateWindow(WC_COMBOBOX, "", 
                            CBS_DROPDOWN | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
                            10, 310, 150, 20,  // λ�úʹ�С
                            hwnd, (HMENU)ID_COMBOBOX_ORDER_ORDERID,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);
            SendMessage(hwndComboBox_Order_OrderId, CB_ADDSTRING, 0, (LPARAM)"Fuck");

            // ����������а�ť
            CreateWindowEx(0, "BUTTON", "All Reservation",
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            10, 10, 140, 30,
                            hwnd, (HMENU)ID_BUTTON_ORDER_CHECKALL,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // ��������ѡ��
            CreateWindowEx(0, DATETIMEPICK_CLASS, NULL,
                            DTS_UPDOWN | WS_BORDER | WS_CHILD | WS_VISIBLE,
                            10, 60, 140, 25,
                            hwnd, (HMENU)ID_DATEPICKER_ORDER_DATE,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // ���������ڼ�鰴ť
            CreateWindowEx(0, "BUTTON", "Check by Date",
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            10, 110, 140, 30,
                            hwnd, (HMENU)ID_BUTTON_ORDER_CHECKDATE,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            // �����б���ͼ
            hWndListView_Order_Output = CreateWindow(WC_LISTVIEW, "",
                            WS_CHILD | WS_VISIBLE | LVS_REPORT | WS_VSCROLL,
                            180, 10, 900, 450,  // λ�úʹ�С
                            hwnd, (HMENU)ID_LISTVIEW_ORDER_OUTPUT,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            AddColumnsToListView_Order(hWndListView_Order_Output);  // �б���ͼ�����

            // // �����༭��
            // hwndEditText_Order_OrderId = CreateWindow(
            //                 _T("EDIT"),                // �ؼ����ͣ��༭��
            //                 _T(""),                    // Ĭ���ı�Ϊ��
            //                 WS_CHILD | WS_VISIBLE | WS_BORDER, // ��ʽ���Ӵ��ڡ��ɼ������߿�
            //                 10, 160, 150, 20,           // λ�úʹ�С
            //                 hwnd, (HMENU)ID_EDITTEXT_ORDER_ORDERID, hInst, NULL); // �����ں���������

            // ����ȡ��ԤԼ��ť
            CreateWindowEx(0, "BUTTON", "CANCEL ORDER",
                            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                            10, 210, 140, 30,
                            hwnd, (HMENU)ID_BUTTON_ORDER_CANCEL,
                            ((LPCREATESTRUCT)lParam)->hInstance, NULL);

            break;

        case WM_COMMAND:

            if (LOWORD(wParam) == ID_BUTTON_ORDER_CHECKALL)
            {
                // �������ԤԼ
                int count1;
                LogEntry* bookedSlots = get_booked_seats(RESERVE_RECORD_LOG, Login_User.username, &count1);
                // ... ������� bookedSlots �е�����
                if (bookedSlots) {
                    for (int i = 0; i < count1; i++) {
                        printf("��Ԥ��ʱ��Σ�%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
                    }
                }
                else printf("û���ҵ���Ԥ��ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");

                AddItemsToListView_Order(hWndListView_Order_Output, bookedSlots, count1); // ����е��б�

                free(bookedSlots); // �ͷ��ڴ�
            }

            if (LOWORD(wParam) == ID_BUTTON_ORDER_CHECKDATE)
            {
                // �����ڼ��ԤԼ
                char buffer_date[25];
                HWND hDatePicker = GetDlgItem(hwnd, ID_DATEPICKER_ORDER_DATE);
                // ��ȡ����
                DateTime_GetSystemtime(hDatePicker, &st);
                sprintf(buffer_date, "%04d/%02d/%02d", st.wYear, st.wMonth, st.wDay);

                int count1;
                LogEntry* bookedSlots = get_subscriber_booked_time_slots(RESERVE_RECORD_LOG, Login_User.username, buffer_date, &count1);
                // ... ������� bookedSlots �е�����
                if (bookedSlots) {
                    for (int i = 0; i < count1; i++) {
                        printf("��Ԥ��ʱ��Σ�%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
                    }
                }
                else printf("û���ҵ���Ԥ��ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");

                AddItemsToListView_Order(hWndListView_Order_Output, bookedSlots, count1); // ����е��б�

                free(bookedSlots); // �ͷ��ڴ�
            }

            if (LOWORD(wParam) == ID_COMBOBOX_ORDER_ORDERID) {  // �������ȡ������
                printf("�������ȡ������\n");
                // ����1����ȡ��ǰѡ�е�����
                TCHAR selectedOrderId[100] = {'\0'};
                int itemIndexOrderId = SendMessage(hwndComboBox_Order_OrderId, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Order_OrderId, CB_GETLBTEXT, itemIndexOrderId, (LPARAM)selectedOrderId);

                SendMessage(hwndComboBox_Order_OrderId, CB_RESETCONTENT, 0, 0);  // ���������

                // �������ԤԼ
                int count1;
                LogEntry* bookedSlots = get_booked_seats(RESERVE_RECORD_LOG, Login_User.username, &count1);
                // ... ������� bookedSlots �е�����
                if (bookedSlots) {
                    for (int i = 0; i < count1; i++) {
                        printf("�����ţ�%s\n", bookedSlots[i].order_id);
                        SendMessage(hwndComboBox_Order_OrderId, CB_ADDSTRING, 0, (LPARAM)bookedSlots[i].order_id);
                    }
                }
                else printf("û���ҵ���Ԥ��ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");

                free(bookedSlots);

                // ����4���ָ���ǰѡ�е�����
                SendMessage(hwndComboBox_Order_OrderId, CB_SELECTSTRING, -1, (LPARAM)selectedOrderId);
            }

            if (LOWORD(wParam) == ID_BUTTON_ORDER_CANCEL)
            {
                // ȡ��ԤԼ

                // ��ȡ������
                // TCHAR szText_orderid[30];  // �ı�������
                // GetWindowText(hwndEditText_Order_OrderId, szText_orderid, 30);
                // ����1����ȡ��ǰѡ�е�����
                TCHAR selectedOrderId[50];
                int itemIndexOrderId = SendMessage(hwndComboBox_Order_OrderId, CB_GETCURSEL, 0, 0);
                SendMessage(hwndComboBox_Order_OrderId, CB_GETLBTEXT, itemIndexOrderId, (LPARAM)selectedOrderId);

                char str_orderid[30];
                ConvertTCharToChar(selectedOrderId, str_orderid, 30); // ת��������

                printf ("CANCEL�����ţ�%s\n", str_orderid);

                if (str_orderid[0] != '\0') {

                    int count2;
                    LogEntry* bookedSlots = get_log_by_order_id(RESERVE_RECORD_LOG, str_orderid, &count2);

                    if (bookedSlots) {
                        int delete_flag = delete_entries_by_orderid(RESERVE_RECORD_LOG, str_orderid);
                        if (delete_flag == 1) {

                            // ����
                            Login_User.balance += bookedSlots[0].amount;
                            char* jsonStr_user = readFileToString(USER_INFO_DATABASE); // ��ȡ�ļ����ݵ��ַ���
                            if (jsonStr_user != NULL) {
                                printf("File content:\n%s\n", jsonStr_user);
                            }
                            else printf("Error reading user info file.\n");
                            char* modifiedJsonStr = modifyUser(jsonStr_user, Login_User.username, Login_User.passwd, Login_User.auth, Login_User.balance);
                            if (modifiedJsonStr != NULL) {
                                printf("�޸��û���Ϣ��\n");
                                viewUser(modifiedJsonStr);
                                writeStringToFile(USER_INFO_DATABASE, modifiedJsonStr); // д��
                            }
                            printf("\n");
                            free(modifiedJsonStr);
                            free(jsonStr_user);

                            ListView_DeleteAllItems(hWndListView_Order_Output); // ��վ��б���ʾ
                            MessageBox(NULL, _T("Success to Cancel"), _T("OK"), MB_ICONEXCLAMATION | MB_OK);
                        }
                        else if (delete_flag == -3) {
                            printf("CanNot Match Order-ID, Please Check Order-ID\n");
                            MessageBox(NULL, _T("CanNot Match Order-ID\nPlease Check Order-ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                        }
                        else MessageBox(NULL, _T("Fail to Cancel\nPlease Check Order-ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                    
                    }
                    else {
                        printf("û���ҵ�������\n");
                        MessageBox(NULL, _T("CanNot Match Order-ID\nPlease Check Order-ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);
                    }

                    free(bookedSlots); // �ͷ��ڴ�
                }
                else MessageBox(NULL, _T("Please Input Order-ID"), _T("Error"), MB_ICONEXCLAMATION | MB_OK);

            }

            break;

        case WM_CLOSE:
            // ��������������رմ���ǰ�Ĵ���

            RefreshWindow(hwnd_Main_Window);
            ShowWindow(hwnd_Main_Window, SW_SHOW);  // ���ش���
            DestroyWindow(hwnd);
            break;

        case WM_DESTROY:
            // PostQuitMessage(0);
            return 0;
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

// ����Ա���ڹ��̺���
LRESULT CALLBACK Admin_WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch (msg) {

                case WM_CREATE: // ���ڴ�����Ϣ
                printf("Admin Window WM_CREATE\n");
                

            break;

        case WM_COMMAND: // ������Ϣ
                
            break;

        case WM_CLOSE:
            RefreshWindow(hwnd_Main_Window);
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



int main() {
    // Login_Flag = 1;
    // HWND hWnd = GetConsoleWindow();
    // ShowWindow(hWnd, SW_HIDE);
    printf("Hello World!\n");
    // Login_Show();

    return WinMain(GetModuleHandle(NULL), NULL, GetCommandLine(), SW_SHOWNORMAL);
}
