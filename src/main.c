#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "cJSON.h"

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
        cJSON* nameObj = cJSON_GetObjectItem(seatObj, "name");
        cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
        cJSON* amountObj = cJSON_GetObjectItem(seatObj, "amount");
        cJSON* subscriberObj = cJSON_GetObjectItem(seatObj, "subscriber");

        printf("Name: %s\n", cJSON_IsString(nameObj) ? nameObj->valuestring : "null");
        printf("ID: %u\n", cJSON_IsNumber(idObj) ? idObj->valueint : 0);
        printf("Amount: %u\n", cJSON_IsNumber(amountObj) ? amountObj->valueint : 0);
        printf("Subscriber: %s\n", cJSON_IsString(subscriberObj) ? subscriberObj->valuestring : "null");
    }

    cJSON_Delete(jsonObj);
}


void findSeatsBySubscriber(const char* jsonStr, const char* subscriber) {
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

    int seatCount = 0;
    printf("Seats booked by %s:\n", subscriber);

    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* subscriberObj = cJSON_GetObjectItem(seatObj, "subscriber");
        if (cJSON_IsString(subscriberObj) && strcmp(subscriberObj->valuestring, subscriber) == 0) {
            cJSON* nameObj = cJSON_GetObjectItem(seatObj, "name");
            cJSON* idObj = cJSON_GetObjectItem(seatObj, "id");
            cJSON* amountObj = cJSON_GetObjectItem(seatObj, "amount");

            printf("  Name: %s\n", cJSON_IsString(nameObj) ? nameObj->valuestring : "null");
            printf("  ID: %u\n", cJSON_IsNumber(idObj) ? idObj->valueint : 0);
            printf("  Amount: %u\n", cJSON_IsNumber(amountObj) ? amountObj->valueint : 0);
            printf("\n");

            seatCount++;
        }
    }

    printf("Total seats booked by %s: %d\n", subscriber, seatCount);

    cJSON_Delete(jsonObj);
}


// �鿴��λ����
void countSeatsByName(const char* jsonStr, const char* name) {
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

    int totalSeats = 0;
    int bookedSeats = 0;
    int availableSeats = 0;

    cJSON* seatObj;
    cJSON_ArrayForEach(seatObj, seatsArray) {
        cJSON* nameObj = cJSON_GetObjectItem(seatObj, "name");
        cJSON* subscriberObj = cJSON_GetObjectItem(seatObj, "subscriber");

        if (cJSON_IsString(nameObj) && strcmp(nameObj->valuestring, name) == 0) {
            totalSeats++;
            if (cJSON_IsString(subscriberObj) && strlen(subscriberObj->valuestring) > 0) {
                bookedSeats++;
            } else {
                availableSeats++;
            }
        }
    }

    printf("Total seats of type '%s': %d\n", name, totalSeats);
    printf("Booked seats of type '%s': %d\n", name, bookedSeats);
    printf("Available seats of type '%s': %d\n", name, availableSeats);

    cJSON_Delete(jsonObj);
}



// �����λ
char* addSeat(const char* jsonStr, const char* name, unsigned int id, unsigned int amount, const char* subscriber) {
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

    cJSON* newSeatObj = cJSON_CreateObject();
    cJSON_AddItemToObject(newSeatObj, "name", cJSON_CreateString(name));
    cJSON_AddItemToObject(newSeatObj, "id", cJSON_CreateNumber(id));
    cJSON_AddItemToObject(newSeatObj, "amount", cJSON_CreateNumber(amount));
    cJSON_AddItemToObject(newSeatObj, "subscriber", cJSON_CreateString(subscriber));
    cJSON_AddItemToArray(seatsArray, newSeatObj);

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}


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

    char* updatedJsonStr = cJSON_Print(jsonObj);
    cJSON_Delete(jsonObj);
    return updatedJsonStr;
}



int main() {
    const char* jsonStr = "{\"seats\":[{\"name\":\"VIP\",\"id\":101,\"amount\":1000,\"subscriber\":\"Alice\"},{\"name\":\"VIP\",\"id\":102,\"amount\":500,\"subscriber\":\"\"},{\"name\":\"Economy\",\"id\":103,\"amount\":300,\"subscriber\":\"Alice\"}]}";

    printf("ԭʼ��λ��Ϣ��\n");
    viewSeat(jsonStr);
    printf("\n");

    findSeatsBySubscriber(jsonStr, "Alice");
    printf("\n");

    countSeatsByName(jsonStr, "VIP");
    printf("\n");

    // �������λ
    char* updatedJsonStr = addSeat(jsonStr, "Regular", 102, 500, "Bob");
    if (updatedJsonStr != NULL) {
        printf("�������λ��\n");
        viewSeat(updatedJsonStr);
        free(updatedJsonStr);
    }
    printf("\n");

    // ɾ����λ
    char* deletedJsonStr = deleteSeat(jsonStr, 101);
    if (deletedJsonStr != NULL) {
        printf("ɾ����λ��\n");
        viewSeat(deletedJsonStr);
        free(deletedJsonStr);
    }

    system("pause");
    return 0;
}
