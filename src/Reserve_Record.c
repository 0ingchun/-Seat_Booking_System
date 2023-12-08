#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include <time.h>
#include <windows.h>

#include "Reserve_Record.h"

// typedef struct {
//     char logtime[64];
//     char operate[64];
//     char action[64];
//     unsigned int id;
//     char seat_type[64];
//     char subscriber[64];
//     char period_date[64];
//     char period_time_start[64];
//     char period_time_end[64];
//     unsigned int amount;
//     char order_id[64];
// } LogEntry;



// ƽ��strptime����
int my_strptime(const char *s, const char *format, struct tm *tm) {
    // Ŀǰ��֧�� "%Y/%m/%d %H:%M:%S" ���ָ�ʽ
    if (strcmp(format, "%Y/%m/%d %H:%M:%S") == 0) {
        return sscanf(s, "%d/%d/%d %d:%d:%d", 
                        &tm->tm_year, &tm->tm_mon, &tm->tm_mday, 
                        &tm->tm_hour, &tm->tm_min, &tm->tm_sec) == 6;
    } else {
        // �����Ҫ��������Ӷ�������ʽ��֧��
        return 0; // ����0��ʾ��ʽ��֧�ֻ����ʧ��
    }
}
/*
���Զ��庯��my_strptime��֧��һ������ʱ���ʽ"%Y/%m/%d %H:%M:%S"��
ʹ��sscanf���ַ����н����ꡢ�¡��ա�Сʱ�����Ӻ��룬Ȼ����䵽struct tm�ṹ�С�
�����Ը�����Ҫ��չ�˺�����֧��������ʽ��
��ʹ�ô˺���֮��ȷ��������ݺ��·ݵ�ֵ����Ϊstruct tm��tm_year�ֶδ�1900�꿪ʼ���㣬tm_mon�ֶδ�0��ʼ���㡣
ͨ���滻��Ŀ������strptime�ĵ���Ϊmy_strptime����������Windowsƽ̨��ʵ�����ƵĹ��ܡ�
*/


// ����24λ������
void generate_order_id(char* order_id) {
    // ��ȡ��ǰʱ��
    time_t now;
    time(&now);
    struct tm *timeinfo = localtime(&now);

    // ���������
    srand((unsigned)time(NULL));

    // ��ʽ��ʱ�䲢���������
    snprintf(order_id, 24, "%04d%02d%02d%02d%02d%02d%04d",
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, rand() % 10000);
}


// д����־
void write_log(const char* filename, const LogEntry* entry) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    // д��CSV��ʽ
    fprintf(file, "%s,%s,%s,%u,%s,%s,%s,%s,%s,%u,%s\n",
        entry->logtime,
        entry->operate,
        entry->action,
        entry->id,
        entry->seat_type,
        entry->subscriber,
        entry->period_date,
        entry->period_time_start,
        entry->period_time_end,
        entry->amount,
        entry->order_id);
    fclose(file);
}

// д����־-ʵʱʱ��
void write_log_realtime(const char* filename, const LogEntry* entry) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    // ��ȡ��ǰʱ�䲢��ʽ��
    time_t now;
    time(&now);
    struct tm *tm_now = localtime(&now);
    char formatted_time[64];
    strftime(formatted_time, sizeof(formatted_time), "%Y/%m/%d %H:%M:%S", tm_now);

    // д��CSV��ʽ��ʹ���Զ���д�ĵ�ǰʱ��
    fprintf(file, "%s,%s,%s,%u,%s,%s,%s,%s,%s,%u,%s\n",
        formatted_time,
        entry->operate,
        entry->action,
        entry->id,
        entry->seat_type,
        entry->subscriber,
        entry->period_date,
        entry->period_time_start,
        entry->period_time_end,
        entry->amount,
        entry->order_id);

    fclose(file);
}


// ���ʱ���ͻ��������
int is_time_conflict(const LogEntry* entries, int count, const LogEntry* new_entry) {
    for (int i = 0; i < count; i++) {
        if (entries[i].id == new_entry->id && strcmp(entries[i].action, "cancel") != 0) {
            // ���ʱ���ͻ
            if (strcmp(entries[i].period_date, new_entry->period_date) == 0 &&
                strcmp(entries[i].period_time_end, new_entry->period_time_start) > 0 &&
                strcmp(entries[i].period_time_start, new_entry->period_time_end) < 0) {
                return 1; // ����ʱ���ͻ
            }
        }
    }
    return 0; // û�г�ͻ
}


// д����־-ʵʱʱ��-����ͻ
int write_log_realtime_conflict(const char* filename, const LogEntry* entry) {
    // ���ȼ���Ƿ���ʱ���ͻ
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);
    if (entries == NULL) {
        printf("��ȡ��־�ļ�ʧ�ܻ��ļ�Ϊ�ա�\n");
        return -1;
    }

    if (is_time_conflict(entries, count, entry)) {
        printf("ʱ���ͻ���޷�Ԥ����\n");
        free(entries);
        return -2;
    }

    // û�г�ͻʱ��д����־

    // �Ƿ��ܿ����ļ�
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        printf("Error opening file\n");
        free(entries);
        return -3;
    }

    // ��λ���ļ�ĩβ��������һ���ַ��Ƿ��ǻ��з�
    fseek(file, -1, SEEK_END);
    char lastChar = fgetc(file);
    if (lastChar != '\n') {
        fprintf(file, "\n"); // ������ǻ��з�����д��һ�����з�
    }

    // ��ȡ��ǰʱ�䲢��ʽ��
    time_t now;
    time(&now);
    struct tm *tm_now = localtime(&now);
    char formatted_time[64];
    strftime(formatted_time, sizeof(formatted_time), "%Y/%m/%d %H:%M:%S", tm_now);

    fprintf(file, "%s,%s,%s,%u,%s,%s,%s,%s,%s,%u,%s\n",
            formatted_time,
            entry->operate,
            entry->action,
            entry->id,
            entry->seat_type,
            entry->subscriber,
            entry->period_date,
            entry->period_time_start,
            entry->period_time_end,
            entry->amount,
            entry->order_id);

    fclose(file);
    free(entries);

    printf("д���¼�ɹ�\n");

    return 0;
}


// ��ȡ��־
LogEntry* read_logs(const char* filename, int* count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        *count = 0;
        return NULL;
    }

    LogEntry* entries = NULL;
    int entryCount = 0;
    char buffer[512]; // ���ڴ洢��ȡ��ÿ������

    while (fgets(buffer, sizeof(buffer), file)) {
        entries = realloc(entries, sizeof(LogEntry) * (entryCount + 1));
        sscanf(buffer, "%[^,],%[^,],%[^,],%u,%[^,],%[^,],%[^,],%[^,],%[^,],%u,%s",
            entries[entryCount].logtime,
            entries[entryCount].operate,
            entries[entryCount].action,
            &entries[entryCount].id,
            entries[entryCount].seat_type,
            entries[entryCount].subscriber,
            entries[entryCount].period_date,
            entries[entryCount].period_time_start,
            entries[entryCount].period_time_end,
            &entries[entryCount].amount,
            entries[entryCount].order_id);
        entryCount++;
    }

    fclose(file);
    *count = entryCount;
    return entries;
}

// ɾ����־-��Ԥ������
void delete_entries_by_date(const char* filename, const char* target_date) {
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);
    if (entries == NULL) {
        printf("��ȡʧ�ܻ��ļ�Ϊ��\n");
        return; // ��ȡʧ�ܻ��ļ�Ϊ��
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        free(entries);
        printf("Error opening file\n");
        return;
    }

    for (int i = 0; i < count; i++) {
        if (strncmp(entries[i].period_date, target_date, strlen(target_date)) != 0) {
            // ����Ŀ�����ڵļ�¼д���ļ�
            fprintf(file, "%s,%s,%s,%u,%s,%s,%s,%s,%s,%u,%s\n",
                entries[i].logtime,
                entries[i].operate,
                entries[i].action,
                entries[i].id,
                entries[i].seat_type,
                entries[i].subscriber,
                entries[i].period_date,
                entries[i].period_time_start,
                entries[i].period_time_end,
                entries[i].amount,
                entries[i].order_id);
        }
    }

    fclose(file);
    free(entries);
    printf("OK to Delete\n");
    return;
}


// ɾ����־-��������
int delete_entries_by_orderid(const char* filename, const char* target_orderid) {
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);
    if (entries == NULL) {
        printf("��ȡʧ�ܻ��ļ�Ϊ��\n");
        return -1; // ��ȡʧ�ܻ��ļ�Ϊ��
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        free(entries);
        printf("Error opening file\n");
        return -2;
    }

    int match_found = 0; // Flag to check if there is a match for target_orderid

    for (int i = 0; i < count; i++) {
        if (strncmp(entries[i].order_id, target_orderid, strlen(target_orderid)) != 0) {
            // ����Ŀ�����ڵļ�¼д���ļ�
            fprintf(file, "%s,%s,%s,%u,%s,%s,%s,%s,%s,%u,%s\n",
                entries[i].logtime,
                entries[i].operate,
                entries[i].action,
                entries[i].id,
                entries[i].seat_type,
                entries[i].subscriber,
                entries[i].period_date,
                entries[i].period_time_start,
                entries[i].period_time_end,
                entries[i].amount,
                entries[i].order_id);
        }
        else {
            match_found = 1;
        }
    }

    fclose(file);
    free(entries);
    // free(filename);

    if (!match_found) {
        printf("No match found for target_orderid\n");
        return -3;
    }
    else {
        printf("OK to Delete\n");
        return 1;
    }
}


// ����ID����λ���Ͳ����ѱ�Ԥ��ϯλ
LogEntry* get_booked_id_slots(const char* filename, const char* seat_type, unsigned int id, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* bookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].seat_type, seat_type) == 0 && entries[i].id == id && strcmp(entries[i].action, "appoint") == 0) {
            bookedEntries = realloc(bookedEntries, sizeof(LogEntry) * (total + 1));
            bookedEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return bookedEntries;
}


// �������ں���λ���Ͳ����ѱ�Ԥ��ϯλ
LogEntry* get_booked_time_slots(const char* filename, const char* seat_type, const char* date, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* bookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].seat_type, seat_type) == 0 && strncmp(entries[i].period_date, date, 10) == 0 && strcmp(entries[i].action, "appoint") == 0) {
            bookedEntries = realloc(bookedEntries, sizeof(LogEntry) * (total + 1));
            bookedEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return bookedEntries;
}


// �������ں�Ԥ���߲����ѱ�Ԥ��ϯλ
LogEntry* get_subscriber_booked_time_slots(const char* filename, const char* subscriber, const char* date, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* bookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].subscriber, subscriber) == 0 && strncmp(entries[i].period_date, date, 10) == 0 && strcmp(entries[i].action, "appoint") == 0) {
            bookedEntries = realloc(bookedEntries, sizeof(LogEntry) * (total + 1));
            bookedEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return bookedEntries;
}


// �����������ж�ʱ���Ƿ��ѹ���
int is_time_expired(const char* time_str) {
    struct tm time_info = {0};
    // strptime(time_str, "%Y/%m/%d %H:%M:%S", &time_info);
    my_strptime(time_str, "%Y/%m/%d %H:%M:%S", &time_info);
    time_t time_val = mktime(&time_info);
    time_t now;
    time(&now);

    return now > time_val; // �����ǰʱ������ʱ������������
}

// ��ȡ�ض����ں���λ���͵�δԤ��ʱ���
LogEntry* get_unbooked_time_slots(const char* filename, const char* seat_type, const char* date, int* count) {
    int total = 0;
    int log_count = 0;
    LogEntry* entries = read_logs(filename, &log_count);
    LogEntry* unbookedEntries = NULL;

    // �˴�ʡ���˷���ʱ��εĸ����߼�
    // ����������һ�ַ�����ȷ����Щʱ�����δ��Ԥ����
    /*????????????????????????????????????????????????????*/

    for (int i = 0; i < log_count; i++) {
        if (strcmp(entries[i].seat_type, seat_type) == 0 && strncmp(entries[i].period_date, date, 10) == 0) {
            // ����Ƿ����
            if (!is_time_expired(entries[i].period_time_end)) {
                unbookedEntries = realloc(unbookedEntries, sizeof(LogEntry) * (total + 1));
                unbookedEntries[total] = entries[i];
                total++;
            }
        }
    }

    free(entries);
    *count = total;
    return unbookedEntries;
}


// ����Ԥ����ͳ��Ԥ��ϯλ-����
LogEntry* get_booked_seats(const char* filename, const char* subscriber, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    LogEntry* bookedEntries = NULL;

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].subscriber, subscriber) == 0 && strcmp(entries[i].action, "appoint") == 0) {
            bookedEntries = realloc(bookedEntries, sizeof(LogEntry) * (total + 1));
            bookedEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return bookedEntries;
}


// ����Ԥ����ͳ����ȡ��ϯλ-����
LogEntry* get_cancelled_seats(const char* filename, const char* subscriber, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    LogEntry* cancelledEntries = NULL;

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].subscriber, subscriber) == 0 && strcmp(entries[i].action, "cancel") == 0) {
            cancelledEntries = realloc(cancelledEntries, sizeof(LogEntry) * (total + 1));
            cancelledEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return cancelledEntries;
}


// ����Ԥ����ͳ����ЧԤ��ϯλ-δ����
LogEntry* get_valid_booked_seats(const char* filename, const char* subscriber, int* count) {
    time_t now;
    time(&now); // ��ȡ��ǰʱ��

    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* validBookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    for (int i = 0; i < *count; i++) {
        struct tm end_time = {0};
        // strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);
        my_strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);

        time_t end_time_t = mktime(&end_time);
        if (strcmp(entries[i].subscriber, subscriber) == 0 && strcmp(entries[i].action, "appoint") == 0 && difftime(end_time_t, now) > 0) {
            validBookedEntries = realloc(validBookedEntries, sizeof(LogEntry) * (total + 1));
            validBookedEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return validBookedEntries;
}


// ����Ԥ����ͳ����Чȡ��ϯλ-δ����
LogEntry* get_valid_cancelled_seats(const char* filename, const char* subscriber, int* count) {
    time_t now;
    time(&now); // ��ȡ��ǰʱ��

    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* validCancelledEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    for (int i = 0; i < *count; i++) {
        struct tm end_time = {0};
        // strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);
        my_strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);

        time_t end_time_t = mktime(&end_time);
        if (strcmp(entries[i].subscriber, subscriber) == 0 && strcmp(entries[i].action, "cancel") == 0 && difftime(end_time_t, now) > 0) {
            validCancelledEntries = realloc(validCancelledEntries, sizeof(LogEntry) * (total + 1));
            validCancelledEntries[total] = entries[i];
            total++;
        }
    }

    free(entries);
    *count = total;
    return validCancelledEntries;
}


// ͳ��Ԥ��������
void count_bookings_and_revenue(const char* filename, const char* date, int* totalBookings, unsigned int* totalRevenue) {
    *totalBookings = 0;
    *totalRevenue = 0;
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);

    if (entries == NULL) {
        return; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    for (int i = 0; i < count; i++) {
        if (strncmp(entries[i].period_date, date, 10) == 0 && strcmp(entries[i].action, "appoint") == 0) {
            (*totalBookings)++;
            *totalRevenue += entries[i].amount;
        }
    }

    free(entries);
}


// ���ݶ����Ż�ȡ��־��Ϣ
LogEntry* get_log_by_order_id(const char* filename, const char* order_id, int* count) {
    *count = 0;
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* foundEntry = NULL;

    if (entries == NULL) {
        return NULL; // �ļ���ȡʧ�ܻ��ļ�Ϊ��
    }

    for (int i = 0; i < *count; i++) {
        if (strcmp(entries[i].order_id, order_id) == 0) {
            foundEntry = malloc(sizeof(LogEntry));
            *foundEntry = entries[i];
            total = 1;
            break;
        }
    }

    free(entries);
    *count = total;
    return foundEntry;
}


// // ��ʾ��
// int main() {

//     LogEntry entry;
//     // �����־��Ŀ����
//     strcpy(entry.logtime, "2023/12/01 10:00:00");
//     strcpy(entry.operate, "ϵͳ");
//     strcpy(entry.action, "appoint");
//     entry.id = 1;
//     strcpy(entry.seat_type, "��ͨ��");
//     strcpy(entry.subscriber, "����");
//     strcpy(entry.period_date, "2023/12/04");
//     strcpy(entry.period_time_start, "2023/12/01 10:00:00");
//     strcpy(entry.period_time_end, "2023/12/01 12:00:00");
//     entry.amount = 100;
//     strcpy(entry.order_id, "order127");
//     // д����־
//     write_log_realtime("log.csv", &entry);


//     // ��ȡ������־
//     int count0;
//     LogEntry* entries = read_logs("log.csv", &count0);
//     if (entries != NULL) {
//         for (int i = 0; i < count0; i++) {
//             printf("Log Entry %d: %s, %s, %s, %u, %s, %s, %s, %s, %s, %u, %s\n",
//                     i + 1,
//                     entries[i].logtime,
//                     entries[i].operate,
//                     entries[i].action,
//                     entries[i].id,
//                     entries[i].seat_type,
//                     entries[i].subscriber,
//                     entries[i].period_date,
//                     entries[i].period_time_start,
//                     entries[i].period_time_end,
//                     entries[i].amount,
//                     entries[i].order_id);
//         }
//         free(entries); // �ͷ��ڴ�
//     } else {
//         printf("�޷���ȡ��־����־�ļ�Ϊ�ա�\n");
//     }


//     // ɾ��ָ�����ڵ�������־��Ŀ
//     delete_entries_by_date("log.csv", "2023/12/04");


//     const char *datetime_str = "2023/12/1 12:30:45";
//     struct tm time_info = {0};

//     if (my_strptime(datetime_str, "%Y/%m/%d %H:%M:%S", &time_info)) {
//         time_info.tm_year -= 1900; // ��ݴ�1900�꿪ʼ
//         time_info.tm_mon -= 1;     // �·ݴ�0��ʼ

//         time_t time_val = mktime(&time_info); // ת��Ϊtime_t
//         // ʹ�� time_val ...
//     } else {
//         printf("����ʱ�����ʧ�ܡ�\n");
//     }


//     int countBooked, countCancelled;
//     LogEntry* bookedSeats;
//     LogEntry* cancelledSeats;

//     // ��ȡ���� "����" Ԥ����ϯλ
//     bookedSeats = get_booked_seats("log.csv", "ZhangSan", &countBooked);
//     if (bookedSeats != NULL) {
//         printf("������Ԥ����ϯλ����: %d\n", countBooked);
//         for (int i = 0; i < countBooked; i++) {
//             printf("ϯλID: %u, Ԥ������: %s\n", bookedSeats[i].id, bookedSeats[i].period_date);
//         }
//         free(bookedSeats); // �ͷ��ڴ�
//     } else {
//         printf("û���ҵ���Ԥ����ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");
//     }

//     // ��ȡ���� "����" ȡ��Ԥ����ϯλ
//     cancelledSeats = get_cancelled_seats("log.csv", "ZhangSan", &countCancelled);
//     if (cancelledSeats != NULL) {
//         printf("������ȡ��Ԥ����ϯλ����: %d\n", countCancelled);
//         for (int i = 0; i < countCancelled; i++) {
//             printf("ϯλID: %u, ȡ������: %s\n", cancelledSeats[i].id, cancelledSeats[i].period_date);
//         }
//         free(cancelledSeats); // �ͷ��ڴ�
//     } else {
//         printf("û���ҵ���ȡ��Ԥ����ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");
//     }


//     int countValidBooked, countValidCancelled;
//     LogEntry* validBookedSeats;
//     LogEntry* validCancelledSeats;

//     // ��ȡ���� "����" δ���ڵ���Ԥ����ϯλ
//     validBookedSeats = get_valid_booked_seats("log.csv", "ZhangSan", &countValidBooked);
//     if (validBookedSeats != NULL) {
//         printf("����δ���ڵ���Ԥ��ϯλ����: %d\n", countValidBooked);
//         for (int i = 0; i < countValidBooked; i++) {
//             printf("ϯλID: %u, Ԥ������: %s, Ԥ������ʱ��: %s\n", validBookedSeats[i].id, validBookedSeats[i].period_date, validBookedSeats[i].period_time_end);
//         }
//         free(validBookedSeats); // �ͷ��ڴ�
//     } else {
//         printf("û���ҵ�δ���ڵ���Ԥ��ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");
//     }

//     // ��ȡ���� "����" δ���ڵ���ȡ��Ԥ����ϯλ
//     validCancelledSeats = get_valid_cancelled_seats("log.csv", "ZhangSan", &countValidCancelled);
//     if (validCancelledSeats != NULL) {
//         printf("����δ���ڵ���ȡ��Ԥ��ϯλ����: %d\n", countValidCancelled);
//         for (int i = 0; i < countValidCancelled; i++) {
//             printf("ϯλID: %u, ȡ������: %s, Ԥ������ʱ��: %s\n", validCancelledSeats[i].id, validCancelledSeats[i].period_date, validCancelledSeats[i].period_time_end);
//         }
//         free(validCancelledSeats); // �ͷ��ڴ�
//     } else {
//         printf("û���ҵ�δ���ڵ���ȡ��Ԥ��ϯλ���ȡ��־�ļ�ʧ�ܡ�\n");
//     }


//     int count1;
//     LogEntry* bookedSlots = get_booked_time_slots("log.csv", "Normal", "2023/12/1", &count1);
//     // ... ������� bookedSlots �е�����
//     if (bookedSlots) {
//         for (int i = 0; i < count1; i++) {
//             printf("��Ԥ��ʱ��Σ�%s - %s\n", bookedSlots[i].period_time_start, bookedSlots[i].period_time_end);
//         }
//     }
//     free(bookedSlots); // �ͷ��ڴ�


//     int count2;
//     LogEntry* unbookedSlots = get_unbooked_time_slots("log.csv", "Normal", "2023/12/1", &count2);

//     if (unbookedSlots) {
//         for (int i = 0; i < count2; i++) {
//             printf("δԤ��ʱ��Σ�%s - %s\n", unbookedSlots[i].period_time_start, unbookedSlots[i].period_time_end);
//         }
//         free(unbookedSlots);
//     } else {
//         printf("û���ҵ�δԤ����ʱ��Ρ�\n");
//     }


//     int totalBookings;
//     unsigned int totalRevenue;

//     count_bookings_and_revenue("log.csv", "2023/12/1", &totalBookings, &totalRevenue);
//     printf("2023��12��1�յ�Ԥ��ϯλ����: %d, ������: %u\n", totalBookings, totalRevenue);


//     int count;
//     LogEntry* logEntry = get_log_by_order_id("log.csv", "order123", &count);

//     if (logEntry != NULL && count > 0) {
//         printf("�������: %s, Ԥ����: %s, Ԥ�����: %u\n", logEntry->order_id, logEntry->subscriber, logEntry->amount);
//         free(logEntry);
//     } else {
//         printf("û���ҵ���Ӧ������ŵ���־��¼��\n");
//     }


//     system("pause");
//     return 0;
// }