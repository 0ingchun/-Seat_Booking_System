typedef struct {
    unsigned int id;    // ��λid
    char name[64];  // ��λ����
    unsigned int amount;    // ���
    // char subscriber[64];    // Ԥ����
    // char datetime [64];    // Ԥ��ʱ��
} Seat_Info_t;


void viewSeat(const char* jsonStr);

char* updateSeat(const char* jsonStr, const char* name, unsigned int id, unsigned int amount);

// void findSeatsBySubscriber(const char* jsonStr, const char* subscriber);

void countSeatsByName(const char* jsonStr, const char* name);

char* addSeat(const char* jsonStr, const char* name, unsigned int id, unsigned int amount);

char* deleteSeat(const char* jsonStr, unsigned int id);

