typedef struct {
    char username[64];
    char passwd[64];
    short auth;    // Ȩ��
    unsigned int balance;   // �˻����
} Login_User_t;

// extern Login_User_t Login_User;

void Login_Test();