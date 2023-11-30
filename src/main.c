#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "main.h"

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


// 写入日志
void write_log(const char* filename, const LogEntry* entry) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    // 写入CSV格式
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

// 读取日志
LogEntry* read_logs(const char* filename, int* count) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        *count = 0;
        return NULL;
    }

    LogEntry* entries = NULL;
    int entryCount = 0;
    char buffer[512]; // 用于存储读取的每行数据

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

// 删除日志
void delete_log(const char* filename, unsigned int id) {
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);
    if (entries == NULL) {
        return; // 读取失败或文件为空
    }

    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        free(entries);
        return;
    }

    for (int i = 0; i < count; i++) {
        if (entries[i].id != id) {
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
}


// 根据日期和座位类型查找已被预订席位
LogEntry* get_booked_time_slots(const char* filename, const char* seat_type, const char* date, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* bookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
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


// 



// 根据预订者统计预订席位-所有
LogEntry* get_booked_seats(const char* filename, const char* subscriber, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
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


// 根据预订者统计已取消席位-所有
LogEntry* get_cancelled_seats(const char* filename, const char* subscriber, int* count) {
    int total = 0;
    LogEntry* entries = read_logs(filename, count);

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
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


// 根据预订者统计有效预订席位-未过期
LogEntry* get_valid_booked_seats(const char* filename, const char* subscriber, int* count) {
    time_t now;
    time(&now); // 获取当前时间

    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* validBookedEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
    }

    for (int i = 0; i < *count; i++) {
        struct tm end_time = {0};
        strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);

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


// 根据预订者统计有效取消席位-未过期
LogEntry* get_valid_cancelled_seats(const char* filename, const char* subscriber, int* count) {
    time_t now;
    time(&now); // 获取当前时间

    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* validCancelledEntries = NULL;

    if (entries == NULL) {
        *count = 0;
        return NULL; // 文件读取失败或文件为空
    }

    for (int i = 0; i < *count; i++) {
        struct tm end_time = {0};
        strptime(entries[i].period_time_end, "%Y/%m/%d %H:%M:%S", &end_time);

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


// 统计预订和收入
void count_bookings_and_revenue(const char* filename, const char* date, int* totalBookings, unsigned int* totalRevenue) {
    *totalBookings = 0;
    *totalRevenue = 0;
    int count = 0;
    LogEntry* entries = read_logs(filename, &count);

    if (entries == NULL) {
        return; // 文件读取失败或文件为空
    }

    for (int i = 0; i < count; i++) {
        if (strncmp(entries[i].period_date, date, 10) == 0 && strcmp(entries[i].action, "appoint") == 0) {
            (*totalBookings)++;
            *totalRevenue += entries[i].amount;
        }
    }

    free(entries);
}


// 根据订单号获取日志信息
LogEntry* get_log_by_order_id(const char* filename, const char* order_id, int* count) {
    *count = 0;
    int total = 0;
    LogEntry* entries = read_logs(filename, count);
    LogEntry* foundEntry = NULL;

    if (entries == NULL) {
        return NULL; // 文件读取失败或文件为空
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



int main() {


    int countBooked, countCancelled;
    LogEntry* bookedSeats;
    LogEntry* cancelledSeats;

    // 获取所有 "张三" 预订的席位
    bookedSeats = get_booked_seats("log.csv", "张三", &countBooked);
    if (bookedSeats != NULL) {
        printf("张三已预订的席位数量: %d\n", countBooked);
        for (int i = 0; i < countBooked; i++) {
            printf("席位ID: %u, 预订日期: %s\n", bookedSeats[i].id, bookedSeats[i].period_date);
        }
        free(bookedSeats); // 释放内存
    } else {
        printf("没有找到已预订的席位或读取日志文件失败。\n");
    }

    // 获取所有 "张三" 取消预订的席位
    cancelledSeats = get_cancelled_seats("log.csv", "张三", &countCancelled);
    if (cancelledSeats != NULL) {
        printf("张三已取消预订的席位数量: %d\n", countCancelled);
        for (int i = 0; i < countCancelled; i++) {
            printf("席位ID: %u, 取消日期: %s\n", cancelledSeats[i].id, cancelledSeats[i].period_date);
        }
        free(cancelledSeats); // 释放内存
    } else {
        printf("没有找到已取消预订的席位或读取日志文件失败。\n");
    }


    int countValidBooked, countValidCancelled;
    LogEntry* validBookedSeats;
    LogEntry* validCancelledSeats;

    // 获取所有 "张三" 未过期的已预订的席位
    validBookedSeats = get_valid_booked_seats("log.csv", "张三", &countValidBooked);
    if (validBookedSeats != NULL) {
        printf("张三未过期的已预订席位数量: %d\n", countValidBooked);
        for (int i = 0; i < countValidBooked; i++) {
            printf("席位ID: %u, 预订日期: %s, 预订结束时间: %s\n", validBookedSeats[i].id, validBookedSeats[i].period_date, validBookedSeats[i].period_time_end);
        }
        free(validBookedSeats); // 释放内存
    } else {
        printf("没有找到未过期的已预订席位或读取日志文件失败。\n");
    }

    // 获取所有 "张三" 未过期的已取消预订的席位
    validCancelledSeats = get_valid_cancelled_seats("log.csv", "张三", &countValidCancelled);
    if (validCancelledSeats != NULL) {
        printf("张三未过期的已取消预订席位数量: %d\n", countValidCancelled);
        for (int i = 0; i < countValidCancelled; i++) {
            printf("席位ID: %u, 取消日期: %s, 预订结束时间: %s\n", validCancelledSeats[i].id, validCancelledSeats[i].period_date, validCancelledSeats[i].period_time_end);
        }
        free(validCancelledSeats); // 释放内存
    } else {
        printf("没有找到未过期的已取消预订席位或读取日志文件失败。\n");
    }





    int totalBookings;
    unsigned int totalRevenue;

    count_bookings_and_revenue("log.csv", "2023/11/30", &totalBookings, &totalRevenue);
    printf("2023年11月30日的预订席位总数: %d, 总收入: %u\n", totalBookings, totalRevenue);


    int count;
    LogEntry* logEntry = get_log_by_order_id("log.csv", "订单编号", &count);

    if (logEntry != NULL && count > 0) {
        printf("订单编号: %s, 预订者: %s, 预订金额: %u\n", logEntry->order_id, logEntry->subscriber, logEntry->amount);
        free(logEntry);
    } else {
        printf("没有找到对应订单编号的日志记录。\n");
    }


    system("pause");
    return 0;
}