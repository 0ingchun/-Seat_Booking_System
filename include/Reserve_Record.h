#ifndef RESERVE_RECORD_H
#define RESERVE_RECORD_H

#include <time.h>

typedef struct {
    char logtime[64];    // ��־��¼����ʱ�� ��/��/��/ ʱ:��:��:����
    char operate[64];    // ������
    char action[64];    // �������ͣ�appoint / cancel

    unsigned int id;    // ��λid
    char seat_type[64];  // ��λ����
    char subscriber[64];    // Ԥ����
    char period_date [64];    // Ԥ����ʱ�������
    char period_time_start [64];    // Ԥ����ʼʱ��
    char period_time_end [64];    // Ԥ������ʱ��

    unsigned int amount;    // ���
    char order_id[64];    // �������

} Reserve_Record_t;


// LogEntry �ṹ�嶨��
typedef struct {
    char logtime[64];
    char operate[64];
    char action[64];

    unsigned int id;
    char seat_type[64];
    char subscriber[64];
    char period_date[64];
    char period_time_start[64];
    char period_time_end[64];

    unsigned int amount;
    char order_id[64];
} LogEntry;

// ����ԭ������
int my_strptime(const char *s, const char *format, struct tm *tm);

void generate_order_id(char* order_id);

void write_log(const char* filename, const LogEntry* entry);

void write_log_realtime(const char* filename, const LogEntry* entry);

int is_time_conflict(const LogEntry* entries, int count, const LogEntry* new_entry);

int write_log_realtime_conflict(const char* filename, const LogEntry* entry);

LogEntry* read_logs(const char* filename, int* count);

void delete_entries_by_date(const char* filename, const char* target_date);

int delete_entries_by_orderid(const char* filename, const char* target_orderid);

LogEntry* get_booked_id_slots(const char* filename, const char* seat_type, unsigned int id, int* count);

LogEntry* get_booked_time_slots(const char* filename, const char* seat_type, const char* date, int* count);

LogEntry* get_subscriber_booked_time_slots(const char* filename, const char* subscriber, const char* date, int* count);

int is_time_expired(const char* time_str);

LogEntry* get_unbooked_time_slots(const char* filename, const char* seat_type, const char* date, int* count);

LogEntry* get_booked_seats(const char* filename, const char* subscriber, int* count);

LogEntry* get_cancelled_seats(const char* filename, const char* subscriber, int* count);

LogEntry* get_valid_booked_seats(const char* filename, const char* subscriber, int* count);

LogEntry* get_valid_cancelled_seats(const char* filename, const char* subscriber, int* count);

void count_bookings_and_revenue(const char* filename, const char* date, int* totalBookings, unsigned int* totalRevenue);

LogEntry* get_log_by_order_id(const char* filename, const char* order_id, int* count);

#endif

// �ҵ���λԤ��ϵͳ��־��¼������Ҫ��¼������ЩԪ��
//     char logtime[64];    // ��־��¼����ʱ�� ��/��/��/ʱ/��/��/����
//     char operate[64];    // ������
//     char action[64];    // �������ͣ�appoint / cancel
//     unsigned int id;    // ��λid
//     char seat_type[64];  // ��λ����
//     char subscriber[64];    // Ԥ����
//     char period_date [64];    // Ԥ����ʱ�������
//     char period_time_start [64];    // Ԥ����ʼʱ��
//     char period_time_end [64];    // Ԥ������ʱ��
//     unsigned int amount;    // ���
//     char order_id[64];    // �������

// ����Ҫһ����дcsv��ʽ��־�Ŀ⣬�������Է���ʵʱʱ�䣬������д����־�ĺ�������ȡ��־��Ϣ�ĺ�����ɾ��ĳ����־�ĺ�����

// ������λ���Ͳ���ͳ��ĳ�����Ԥ��ʱ��κ�δԤ����ʱ��Σ�δ��Ԥ��ϯλ�Ľ���ʱ�����Ҫ�뵱ǰʵʱʱ��ԱȺ�ȷ����Ч�ĲŸ����������ֹ������ڵ�ʱ��Σ��ĺ�����

// ����Ԥ���߲���ͳ�Ƹ�Ԥ���ߵ���Ԥ��ϯλ�ĺ������Ա�ʵʱʱ�����λԤ������ʱ�䣬���ֹ��ڵĲ������������

// ����Ԥ���߲���ͳ�Ƹ�Ԥ���ߵ�ȡ��Ԥ��ϯλ�ĺ������Ա�ʵʱʱ�����λԤ������ʱ�䣬���ֹ��ڵĲ������������

// ����Ԥ���߲���ͳ�Ƹ�Ԥ�������е���Ԥ��ϯλ�ĺ�����

// ����Ԥ���߲���ͳ�Ƹ�Ԥ�������е�ȡ��Ԥ��ϯλ�ĺ�����

// ͳ��ĳ�ձ�Ԥ��ϯλ������ĳ��������ĺ�����

// ���붩����Ų鿴��־��Ϣ�ĺ���