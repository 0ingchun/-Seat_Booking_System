#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"
#include "Seat_Info.h"


// �鿴��λ
void viewSeat(const char* jsonStr) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        printf("Seats field is not an array\n");
        cJSON_Delete(jsonObj);
        return;
    }

    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
        cJSON* typeObj = cJSON_GetObjectItem(seatObj, "type");
        cJSON* amountObj = cJSON_GetObjectItem(seatObj, "amount");
        
        printf("ID: %u\n", cJSON_IsNumber(idObj) ? idObj->valueint : 0);
        printf("Type: %s\n", cJSON_IsString(typeObj) ? typeObj->valuestring : "null");
        printf("Amount: %u\n", cJSON_IsNumber(amountObj) ? amountObj->valueint : 0);
    }

    cJSON_Delete(jsonObj);
}


// ������λ
char* updateSeat(const char* jsonStr, const char* type, unsigned int id, unsigned int amount) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
        if (cJSON_IsNumber(idObj) && idObj->valueint == id) {
            // ������λ��Ϣ
            cJSON_ReplaceItemInObject(seatObj, "type", cJSON_CreateString(type));
            cJSON_ReplaceItemInObject(seatObj, "amount", cJSON_CreateNumber(amount));
            break;
        }
    }

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}



// // ���Ҷ����ߵ���λ
// void findSeatsBySubscriber(const char* jsonStr, const char* subscriber) {
//     cJSON* jsonObj = cJSON_Parse(jsonStr);
//     if (jsonObj == NULL) {
//         printf("Failed to parse JSON string\n");
//         return;
//     }

//     cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
//     if (!cJSON_IsArray(seatsArray)) {
//         printf("Seats field is not an array\n");
//         cJSON_Delete(jsonObj);
//         return;
//     }

//     int seatCount = 0;
//     printf("Seats booked by %s:\n", subscriber);

//     cJSON* seatObj;
//     cJSON_ArrayForEach(seatObj, seatsArray) {
//         cJSON* subscriberObj = cJSON_GetObjectItem(seatObj, "subscriber");
//         if (cJSON_IsString(subscriberObj) && strcmp(subscriberObj->valuestring, subscriber) == 0) {
//             cJSON* typeObj = cJSON_GetObjectItem(seatObj, "type");
//             cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
//             cJSON* amountObj = cJSON_GetObjectItem(seatObj, "amount");
//             cJSON* datetimeObj = cJSON_GetObjectItem(seatObj, "datetime");

//             printf("  Type: %s\n", cJSON_IsString(typeObj) ? typeObj->valuestring : "null");
//             printf("  ID: %u\n", cJSON_IsNumber(idObj) ? idObj->valueint : 0);
//             printf("  Amount: %u\n", cJSON_IsNumber(amountObj) ? amountObj->valueint : 0);
//             printf("  Datetime: %s\n", cJSON_IsString(datetimeObj) ? datetimeObj->valuestring : "null");
//             printf("\n");

//             seatCount++;
//         }
//     }

//     printf("Total seats booked by %s: %d\n", subscriber, seatCount);

//     cJSON_Delete(jsonObj);
// }



// �鿴��λ����-�������ͣ���������λID����
int countSeatsByType(const char* jsonStr, const char* type, int** seatIds) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return 0;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        printf("Seats field is not an array\n");
        cJSON_Delete(jsonObj);
        return 0;
    }

    int totalSeats = 0;
    *seatIds = NULL; // ��ʼ��ΪNULL

    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* typeObj = cJSON_GetObjectItem(seatObj, "type");
        if (cJSON_IsString(typeObj) && strcmp(typeObj->valuestring, type) == 0) {
            totalSeats++;
        }
    }

    if (totalSeats > 0) {
        *seatIds = malloc(totalSeats * sizeof(int)); // �����ڴ�
        int index = 0;

        cJSON_ArrayForEach(seatObj, seatsArray) {
            cJSON* typeObj = cJSON_GetObjectItem(seatObj, "type");
            cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
            if (cJSON_IsString(typeObj) && strcmp(typeObj->valuestring, type) == 0) {
                (*seatIds)[index++] = cJSON_IsNumber(idObj) ? idObj->valueint : -1;
            }
        }
    }

    cJSON_Delete(jsonObj);
    return totalSeats; // �����ҵ�����λ����
}


// ���ܣ���JSON����ȡ���в�ͬ����λ����
// ������jsonStr - JSON�ַ�����types - ָ���ַ��������ָ�룬���ڴ洢���
// ���أ��ҵ��Ĳ�ͬ��λ���͵�����
int getSeatTypes(const char* jsonStr, char*** types) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return 0;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        printf("Seats field is not an array\n");
        cJSON_Delete(jsonObj);
        return 0;
    }

    int typesCount = 0;
    *types = NULL; // ��ʼ��ΪNULL
    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* typeObj = cJSON_GetObjectItem(seatObj, "type");
        if (cJSON_IsString(typeObj)) {
            // ���������Ƿ��Ѵ�����������
            int exists = 0;
            for (int i = 0; i < typesCount; i++) {
                if (strcmp((*types)[i], typeObj->valuestring) == 0) {
                    exists = 1;
                    break;
                }
            }

            // ��������Ͳ������������У��������
            if (!exists) {
                *types = realloc(*types, (typesCount + 1) * sizeof(char*));
                (*types)[typesCount] = strdup(typeObj->valuestring);
                typesCount++;
            }
        }
    }

    cJSON_Delete(jsonObj);
    return typesCount; // �����ҵ��Ĳ�ͬ���͵�����
}


// ����id����amount
int findAmountById(const char* jsonStr, unsigned int id) {
    // ����JSON�ַ���
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return -1; // ����-1��ʾ����ʧ��
    }

    // ��ȡseats����
    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        printf("Seats field is not an array\n");
        cJSON_Delete(jsonObj);
        return -1;
    }

    // �������飬���Ҷ�ӦID��amount
    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
        cJSON* amountObj = cJSON_GetObjectItem(seatObj, "amount");

        if (cJSON_IsNumber(idObj) && idObj->valueint == id) {
            if (cJSON_IsNumber(amountObj)) {
                int amount = amountObj->valueint;
                cJSON_Delete(jsonObj);
                return amount; // �����ҵ���amount
            }
        }
    }

    cJSON_Delete(jsonObj);
    return -1; // δ�ҵ�ָ��ID������-1
}


// �����λ
char* addSeat(const char* jsonStr, const char* type, unsigned int id, unsigned int amount) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    // ��������id�Ƿ��Ѵ���
    cJSON* existingSeat = NULL;
    cJSON_ArrayForEach(existingSeat, seatsArray) {
        cJSON* idItem = cJSON_GetObjectItem(existingSeat, "id");
        if (cJSON_IsNumber(idItem) && (unsigned int)idItem->valuedouble == id) {
            cJSON_Delete(jsonObj);
            return ""; // ���ؿ��ַ���
        }
    }

    cJSON* newSeatObj = cJSON_CreateObject();
    cJSON_AddItemToObject(newSeatObj, "id", cJSON_CreateNumber(id));
    cJSON_AddItemToObject(newSeatObj, "type", cJSON_CreateString(type));
    cJSON_AddItemToObject(newSeatObj, "amount", cJSON_CreateNumber(amount));

    cJSON_AddItemToArray(seatsArray, newSeatObj);

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}
// char* addSeat(const char* jsonStr, const char* type, unsigned int id, unsigned int amount) {
//     cJSON* jsonObj = cJSON_Parse(jsonStr);
//     if (jsonObj == NULL) {
//         printf("Failed to parse JSON string\n");
//         return NULL;
//     }

//     cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
//     if (!cJSON_IsArray(seatsArray)) {
//         cJSON_Delete(jsonObj);
//         return NULL;
//     }

//     cJSON* newSeatObj = cJSON_CreateObject();
//     cJSON_AddItemToObject(newSeatObj, "id", cJSON_CreateNumber(id));
//     cJSON_AddItemToObject(newSeatObj, "type", cJSON_CreateString(type));
//     cJSON_AddItemToObject(newSeatObj, "amount", cJSON_CreateNumber(amount));

//     cJSON_AddItemToArray(seatsArray, newSeatObj);

//     char* updatedJsonStr = cJSON_Print(jsonObj);
//     cJSON_Delete(jsonObj);
//     return updatedJsonStr;
// }


// ɾ����λ
char* deleteSeat(const char* jsonStr, unsigned int id) {
    cJSON* jsonObj = cJSON_Parse(jsonStr);
    if (jsonObj == NULL) {
        printf("Failed to parse JSON string\n");
        return NULL;
    }

    cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
    if (!cJSON_IsArray(seatsArray)) {
        cJSON_Delete(jsonObj);
        return NULL;
    }

    int index = 0;
    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
        if (cJSON_IsNumber(idObj) && idObj->valueint == id) {
            cJSON_DeleteItemFromArray(seatsArray, index);
            break;
        }
        index++;
    }

    char* updatedJsonStr;
    if (index == cJSON_GetArraySize(seatsArray)) {
        // ID not found, return empty string
        updatedJsonStr = strdup("");
    } else {
        updatedJsonStr = cJSON_Print(jsonObj);
    }

    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}
// char* deleteSeat(const char* jsonStr, unsigned int id) {
//     cJSON* jsonObj = cJSON_Parse(jsonStr);
//     if (jsonObj == NULL) {
//         printf("Failed to parse JSON string\n");
//         return NULL;
//     }

//     cJSON* seatsArray = cJSON_GetObjectItem(jsonObj, "seats");
//     if (!cJSON_IsArray(seatsArray)) {
//         cJSON_Delete(jsonObj);
//         return NULL;
//     }

//     int index = 0;
//     cJSON* seatObj;
//     cJSON_ArrayForEach(seatObj, seatsArray) {
//         cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
//         if (cJSON_IsNumber(idObj) && idObj->valueint == id) {
//             cJSON_DeleteItemFromArray(seatsArray, index);
//             break;
//         }
//         index++;
//     }

//     char* updatedJsonStr = cJSON_Print(jsonObj);
//     cJSON_Delete(jsonObj);
//     return updatedJsonStr;
// }


// // // ��ʾ��
// int main() {
//     const char* jsonStr = "{\"seats\":[{\"type\":\"VIP\",\"id\":101,\"amount\":1000,\"subscriber\":\"Alice\",\"datetime\":\"2023-04-01 09:00:00\"},{\"type\":\"VIP\",\"id\":102,\"amount\":500,\"subscriber\":\"\",\"datetime\":\"2023-04-01 10:00:00\"},{\"type\":\"Economy\",\"id\":103,\"amount\":300,\"subscriber\":\"Alice\",\"datetime\":\"2023-04-01 11:00:00\"}]}";

//     printf("ԭʼ��λ��Ϣ��\n");
//     viewSeat(jsonStr);
//     printf("\n");

//     char* updatedJsonStr = updateSeat(jsonStr, "VIP", 101, 1500);
//     if (updatedJsonStr != NULL) {
//         printf("���º����λ��Ϣ��\n");
//         viewSeat(updatedJsonStr);
//         free(updatedJsonStr);
//     } else {
//         printf("��λ����ʧ�ܡ�\n");
//     }


//     // findSeatsBySubscriber(jsonStr, "Alice");
//     // printf("\n");

//     // ͳ����������
//     int* seatIds;
//     int count = countSeatsByType(jsonStr, "VIP", &seatIds);
//     printf("Found %d seats of type 'VIP'\n", count);
//     for (int i = 0; i < count; i++) {
//         printf("Seat ID: %d\n", seatIds[i]);
//     }
//     free(seatIds); // �ͷ��ڴ�
//     printf("\n");


//     // ��ȡ������λ����
//     char** seatTypes;
//     int count2 = getSeatTypes(jsonStr, &seatTypes);
//     printf("Found %d different seat types\n", count2);
//     for (int i = 0; i < count2; i++) {
//         printf("Seat Type: %s\n", seatTypes[i]);
//         free(seatTypes[i]); // �ͷ�ÿ���ַ���
//     }
//     free(seatTypes); // �ͷ��ַ�������


// unsigned int idToFind = 102; // Ҫ���ҵ���λID
//     int amount = findAmountById(jsonStr, idToFind);

//     if (amount != -1) {
//         printf("Amount for seat ID %u is %d\n", idToFind, amount);
//     } else {
//         printf("Seat with ID %u not found.\n", idToFind);
//     }


//     // �������λ
//     char* updatedJsonStr2 = addSeat(jsonStr, "Regular", 104, 500);
//     if (updatedJsonStr2 != NULL) {
//         printf("�������λ��\n");
//         viewSeat(updatedJsonStr2);
//         free(updatedJsonStr2);
//     }
//     printf("\n");


//     // ɾ����λ
//     char* deletedJsonStr3 = deleteSeat(jsonStr, 101);
//     if (deletedJsonStr3 != NULL) {
//         printf("ɾ����λ��\n");
//         viewSeat(deletedJsonStr3);
//         free(deletedJsonStr3);
//     }

//     system("pause");
//     return 0;
// }