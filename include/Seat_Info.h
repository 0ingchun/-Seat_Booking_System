#ifndef SEAT_INFO__H
#define SEAT_INFO__H

typedef struct {
    unsigned int id;    // ��λid
    char type[64];  // ��λ����
    unsigned int amount;    // ���
    // char subscriber[64];    // Ԥ����
    // char datetime [64];    // Ԥ��ʱ��
} Seat_Info_t;


void viewSeat(const char* jsonStr);

char* updateSeat(const char* jsonStr, const char* type, unsigned int id, unsigned int amount);

// void findSeatsBySubscriber(const char* jsonStr, const char* subscriber);

int countSeatsByType(const char* jsonStr, const char* type, int** seatIds);

int getSeatTypes(const char* jsonStr, char*** types);

int findAmountById(const char* jsonStr, unsigned int id);

char* addSeat(const char* jsonStr, const char* type, unsigned int id, unsigned int amount);

char* deleteSeat(const char* jsonStr, unsigned int id);

#endif