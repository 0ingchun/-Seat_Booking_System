#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"


// ��ȡ���鿴�û���Ϣ
void viewUser(const char* jsonStr) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return;
    }

    cJSON* usersArray = cJSON_GetObjectItem(jsonObj, "users");
    if (!cJSON_IsArray(usersArray)) {
        printf("Users field is not an array\n");
        cJSON_Delete(jsonObj);
        return;
    }

    cJSON* userObj;
    cJSON_ArrayForEach(userObj, usersArray) {
        cJSON* usernameObj = cJSON_GetObjectItem(userObj, "username");
        cJSON* passwdObj = cJSON_GetObjectItem(userObj, "passwd");
        cJSON* authObj = cJSON_GetObjectItem(userObj, "auth");
        cJSON* balanceObj = cJSON_GetObjectItem(userObj, "balance");

        printf("Username: %s\n", cJSON_IsString(usernameObj) ? usernameObj->valuestring : "null");
        printf("Password: %s\n", cJSON_IsString(passwdObj) ? passwdObj->valuestring : "null");
        printf("Auth: %d\n", cJSON_IsNumber(authObj) ? authObj->valueint : -1);
        printf("Balance: %u\n", cJSON_IsNumber(balanceObj) ? balanceObj->valueint : 0);
    }

    cJSON_Delete(jsonObj);
}


// �����û�������������Ϣ
int findUser(const char* jsonStr, const char* username, char* outPasswd, short* outAuth, unsigned int* outBalance) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return 0; // ��ʾ�û�δ�ҵ�
    }

    cJSON* userArray = cJSON_GetObjectItem(jsonObj, "users");
    if (!cJSON_IsArray(userArray)) {
        cJSON_Delete(jsonObj);
        return 0; // ��ʾ�û�δ�ҵ�
    }

    cJSON* userObj;
    cJSON_ArrayForEach(userObj, userArray) {
        cJSON* usernameObj = cJSON_GetObjectItem(userObj, "username");
        cJSON* passwdObj = cJSON_GetObjectItem(userObj, "passwd");
        cJSON* authObj = cJSON_GetObjectItem(userObj, "auth");
        cJSON* balanceObj = cJSON_GetObjectItem(userObj, "balance");

        if (cJSON_IsString(usernameObj) && strcmp(usernameObj->valuestring, username) == 0) {
            if (outPasswd != NULL && cJSON_IsString(passwdObj)) {
                strcpy(outPasswd, passwdObj->valuestring);
            }
            if (outAuth != NULL && cJSON_IsNumber(authObj)) {
                *outAuth = authObj->valueint;
            }
            if (outBalance != NULL && cJSON_IsNumber(balanceObj)) {
                *outBalance = balanceObj->valueint;
            }
            cJSON_Delete(jsonObj);
            return 1; // ��ʾ�û��ҵ�
        }
    }

    cJSON_Delete(jsonObj);
    return 0; // ��ʾ�û�δ�ҵ�
}


// д�벢�޸��û���Ϣ
char* modifyUser(const char* jsonStr, const char* newUsername, const char* newPasswd, short newAuth) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* userArray = cJSON_GetObjectItem(jsonObj, "users");
    if (!cJSON_IsArray(userArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    cJSON* userObj = NULL;
    cJSON_ArrayForEach(userObj, userArray) {
        cJSON* usernameObj = cJSON_GetObjectItem(userObj, "username");
        if (cJSON_IsString(usernameObj) && strcmp(usernameObj->valuestring, newUsername) == 0) {
            cJSON_ReplaceItemInObject(userObj, "passwd", cJSON_CreateString(newPasswd));
            cJSON_ReplaceItemInObject(userObj, "auth", cJSON_CreateNumber(newAuth));
            // ���޸� balance
            char* modifiedJsonStr = cJSON_Print(jsonObj);
            cJSON_Delete(jsonObj);
            return modifiedJsonStr;
        }
    }

    printf("Username not found.\n");
    cJSON_Delete(jsonObj);
    return NULL;
}


// ������û�
char* addUser(const char* jsonStr, const char* username, const char* passwd, short auth) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* userArray = cJSON_GetObjectItem(jsonObj, "users");
    if (!cJSON_IsArray(userArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    // ����û����Ƿ��Ѵ���
    cJSON* existingUser = NULL;
    cJSON_ArrayForEach(existingUser, userArray) {
        cJSON* usernameObj = cJSON_GetObjectItem(existingUser, "username");
        if (cJSON_IsString(usernameObj) && strcmp(usernameObj->valuestring, username) == 0) {
            printf("Username already exists.\n");
            cJSON_Delete(jsonObj);
            return NULL;
        }
    }

    // ������û�
    cJSON* newUserObj = cJSON_CreateObject();
    cJSON_AddItemToObject(newUserObj, "username", cJSON_CreateString(username));
    cJSON_AddItemToObject(newUserObj, "passwd", cJSON_CreateString(passwd));
    cJSON_AddItemToObject(newUserObj, "auth", cJSON_CreateNumber(auth));
    cJSON_AddItemToObject(newUserObj, "balance", cJSON_CreateNumber(0)); // ��ʼ�������Ϊ0
    cJSON_AddItemToArray(userArray, newUserObj);

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}


// ɾ���Ѵ��ڵ��û�
char* deleteUser(const char* jsonStr, const char* username) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* userArray = cJSON_GetObjectItem(jsonObj, "users");
    if (!cJSON_IsArray(userArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    int index = 0;
    cJSON* userObj;
    cJSON_ArrayForEach(userObj, userArray) {
        cJSON* usernameObj = cJSON_GetObjectItem(userObj, "username");
        if (cJSON_IsString(usernameObj) && strcmp(usernameObj->valuestring, username) == 0) {
            cJSON_DeleteItemFromArray(userArray, index);
            break;
        }
        index++;
    }

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}


// int main() {
//     // ���� JSON �ַ��������� balance �ֶ�
//     const char* jsonStr = "{\"users\":[{\"username\":\"user1\",\"passwd\":\"pass1\",\"auth\":1,\"balance\":1000},{\"username\":\"user2\",\"passwd\":\"pass2\",\"auth\":0,\"balance\":500}]}";

//     printf("ԭʼ�û���Ϣ��\n");
//     viewUser(jsonStr);
//     printf("\n");

//     // �����û���Ϣ
//     char passwd[64];
//     short auth;
//     unsigned int balance;
//     int userFound = findUser(jsonStr, "user1", passwd, &auth, &balance);
//     printf("�����û���Ϣ��\n");
//     if (userFound) {
//         printf("Password: %s\n", passwd);
//         printf("Auth: %d\n", auth);
//         printf("Balance: %u\n", balance);
//     } else {
//         printf("User not found.\n");
//     }
//     printf("\n");

//     // �޸��û���Ϣ
//     char* modifiedJsonStr = modifyUser(jsonStr, "user1", "newpass", 2);
//     if (modifiedJsonStr != NULL) {
//         printf("�޸��û���Ϣ��\n");
//         viewUser(modifiedJsonStr);
//         free(modifiedJsonStr);
//     }
//     printf("\n");

//     // ������û�
//     char* updatedJsonStr = addUser(jsonStr, "user3", "pass3", 1);
//     if (updatedJsonStr != NULL) {
//         printf("������û���\n");
//         viewUser(updatedJsonStr);
//         free(updatedJsonStr);
//     }
//     printf("\n");

//     // ɾ���Ѵ��ڵ��û�
//     char* deletedJsonStr = deleteUser(jsonStr, "user2");
//     if (deletedJsonStr != NULL) {
//         printf("ɾ���û���\n");
//         viewUser(deletedJsonStr);
//         free(deletedJsonStr);
//     }

//     system("pause");
//     return 0;
// }

