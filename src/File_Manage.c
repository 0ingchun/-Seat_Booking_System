#include "File_Manage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <windows.h>

// #include "cJSON.h"

// ��ͨ�Ķ�д

// // ��ȡ�ļ����ݵ��ַ���
// char* readFileToString(const char* filename) {
//     FILE* file = fopen(filename, "rb"); // ʹ�� "rb" ģʽ���ļ�
//     if (file == NULL) {
//         perror("Error opening file");
//         return NULL;
//     }

//     fseek(file, 0, SEEK_END);
//     long length = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     char* buffer = (char*)malloc(length + 1);
//     if (buffer) {
//         fread(buffer, 1, length, file);
//         buffer[length] = '\0'; // ȷ���Կ��ַ���β
//     }

//     fclose(file);
//     return buffer;
// }


// // ���ַ���д���ļ�
// void writeStringToFile(const char* filename, const char* jsonStr) {
//     FILE* file = fopen(filename, "w"); // ���� "wb" ���ڶ������ļ�
//     if (file == NULL) {
//         perror("Error opening file");
//         return;
//     }

//     fputs(jsonStr, file);
//     fclose(file);
// }


// ���JSON�Ż��ĺ���

// ��ȡ�ļ����ݵ��ַ���
// char* readFileToString(const char* filename) {
//     FILE* file = fopen(filename, "r");
//     if (file == NULL) {
//         perror("Error opening file");
//         return NULL;
//     }

//     fseek(file, 0, SEEK_END);
//     long length = ftell(file);
//     fseek(file, 0, SEEK_SET);

//     char* buffer = (char*)malloc(length + 1);
//     if (buffer) {
//         size_t readLength = fread(buffer, 1, length, file);
//         if (readLength != length) {
//             perror("Error reading file");
//             free(buffer);
//             fclose(file);
//             return NULL;
//         }
//         buffer[length] = '\0';
//     }

//     fclose(file);
//     return buffer;
// }

char* readFileToString(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, file);
        buffer[length] = '\0';  // ȷ���ַ����� null ��β
    } else {
        perror("Error allocating memory");
    }

    fclose(file);
    return buffer;
}


// ���ַ���д���ļ�
void writeStringToFile(const char* filename, const char* jsonStr) {
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    if (fputs(jsonStr, file) == EOF) {
        perror("Error writing to file");
    }
    fclose(file);
}


// ����ļ����ڲ������ļ�
int check_and_create_file(const char *file_name) {
    printf("Check File: '%s'\n", file_name);
    int file_exists = access(file_name, F_OK);
    if (file_exists == -1) {
        FILE *file = fopen(file_name, "w");
        if (file == NULL) {
            fprintf(stderr, "Fail to Creat File!\n");
            return -1;
        }
        fclose(file);
        printf("File: '%s' Success Create File!\n", file_name);
        return 1;
    }
    else {
        printf("File: '%s' File Already Exists\n", file_name);
        return 0;
    }
}


// �����ļ�
int createFile(const char* filename) {
    // ���ȳ����Զ�ȡģʽ���ļ�������ļ��Ƿ��Ѵ���
    FILE* file = fopen(filename, "r");
    if (file != NULL) {
        fclose(file); // �ļ��Ѵ��ڣ��ر��ļ�
        printf("File '%s' already exists.\n", filename);
        return -2; // �����ض��Ĵ������ʾ�ļ��Ѵ���
    }

    // �ļ������ڣ����Դ����ļ�
    file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error creating file");
        return -1; // ����ʧ��
    }

    fclose(file); // �ر��ļ����������
    printf("File '%s' created successfully.\n", filename);
    return 0; // �����ɹ�
}


// ɾ���ļ�
int deleteFile(const char* filename) {
    if (remove(filename) == 0) {
        printf("File '%s' deleted successfully.\n", filename);
        return 0; // ɾ���ɹ�
    } else {
        perror("Error deleting file");
        return -1; // ɾ��ʧ��
    }
}


// �������ļ�
int renameFile(const char* originalFilename, const char* newFilename) {
    if (rename(originalFilename, newFilename) == 0) {
        printf("File renamed from '%s' to '%s' successfully.\n", originalFilename, newFilename);
        return 0; // �������ɹ�
    } else {
        perror("Error renaming file");
        return -1; // ������ʧ��
    }
}



// �ƶ��ļ�
int moveFile(const char* originalFilename, const char* newDirectory) {
    char newFilePath[256];
    sprintf(newFilePath, "%s\\%s", newDirectory, originalFilename); // Windows ·����ʽ
    // ���� UNIX/Linux��ʹ�� sprintf(newFilePath, "%s/%s", newDirectory, originalFilename);

    if (rename(originalFilename, newFilePath) == 0) {
        printf("File moved to '%s'\n", newFilePath);
        return 0; // �ƶ��ɹ�
    } else {
        perror("Error moving file");
        return -1; // �ƶ�ʧ��
    }
}



// // opfile���ļ���ʹ�øú�����ȡ�ļ����
// HANDLE getFileHandleFromFilePointer(FILE* file) {
//     if (file == NULL) {
//         return INVALID_HANDLE_VALUE;
//     }

//     int fileDescriptor = _fileno(file);
//     HANDLE fileHandle = (HANDLE)_get_osfhandle(fileDescriptor);

//     if (fileHandle == INVALID_HANDLE_VALUE) {
//         printf("Unable to get file handle.\n");
//     }

//     return fileHandle;
// }



// // UNIX�ļ�����
// #include <stdio.h>
// #include <stdlib.h>
// #include <sys/file.h>
// #include <unistd.h>
// // �ļ���������
// int lockFile(FILE* file) {
//     if (flock(fileno(file), LOCK_EX) == -1) {
//         perror("Error locking file");
//         return -1;
//     }
//     return 0;
// }

// // �ļ���������
// int unlockFile(FILE* file) {
//     if (flock(fileno(file), LOCK_UN) == -1) {
//         perror("Error unlocking file");
//         return -1;
//     }
//     return 0;
// }


// Windows�ļ�����

// �ļ���������
int lockFile(HANDLE file) {
    OVERLAPPED overlapped = { 0 };
    if (!LockFile(file, 0, 0, 1, 0)) {
        printf("Error locking file: %lu\n", GetLastError());
        return -1;
    }
    return 0;
}

// �ļ���������
int unlockFile(HANDLE file) {
    OVERLAPPED overlapped = { 0 };
    if (!UnlockFile(file, 0, 0, 1, 0)) {
        printf("Error unlocking file: %lu\n", GetLastError());
        return -1;
    }
    return 0;
}


// // ʹ��WindowsAPI�����ļ���ȡ���ļ����
// HANDLE createFileAndGetHandle(const char* filename) {
//     HANDLE fileHandle = CreateFile(
//         filename,                           // �ļ���
//         GENERIC_READ | GENERIC_WRITE,       // ���ļ����ж�д
//         0,                                  // �������ļ�
//         NULL,                               // Ĭ�ϰ�ȫ����
//         OPEN_ALWAYS,                        // ���ļ�������ļ��������򴴽�
//         FILE_ATTRIBUTE_NORMAL,              // ��ͨ�ļ�
//         NULL);                              // û��ģ���ļ�

//     if (fileHandle == INVALID_HANDLE_VALUE) {
//         printf("Unable to open or create file '%s'. Error: %lu\n", filename, GetLastError());
//         return NULL; // �򿪻򴴽�ʧ��
//     }

//     // ����ļ��Ƿ����´�����
//     if (GetLastError() == ERROR_ALREADY_EXISTS) {
//         printf("File '%s' already exists.\n", filename);
//         CloseHandle(fileHandle); // �رվ��
//         return NULL; // �ļ�����
//     }

//     return fileHandle; // ������Ч���ļ����
// }


// // ʹ��WindowsAPI���ļ�����þ��
// HANDLE openFileAndGetHandle(const char* filename) {
//     HANDLE fileHandle = CreateFile(
//         filename,               // �ļ���
//         GENERIC_READ,           // ���ļ����ж�ȡ
//         FILE_SHARE_READ,        // �����������̶�ȡ�ļ�
//         NULL,                   // Ĭ�ϰ�ȫ����
//         OPEN_EXISTING,          // �������ļ�
//         FILE_ATTRIBUTE_NORMAL,  // ��ͨ�ļ�
//         NULL);                  // û��ģ���ļ�

//     if (fileHandle == INVALID_HANDLE_VALUE) {
//         printf("Unable to open file '%s'. Error: %lu\n", filename, GetLastError());
//         return NULL; // ��ʧ��
//     }

//     return fileHandle; // ������Ч���ļ����
// }


// ��ʾ��
// int main() {

//     const char* inputFilename = "input.txt";

//     // �����ļ�
//     int result = createFile(inputFilename);
//     if (result == 0) {
//         printf("File successfully created.\n");
//     } else if (result == -2) {
//         printf("File already exists, no need to create.\n");
//     } else {
//         printf("Failed to create file.\n");
//     }


//     // const char* inputFilename = "input.txt";
//     const char* outputFilename = "output.txt";

//     // ��ȡ�ļ����ݵ��ַ���
//     char* jsonStr = readFileToString(inputFilename);
//     if (jsonStr != NULL) {
//         printf("File content:\n%s\n", jsonStr);

//         // ���ַ�������д����һ���ļ�
//         writeStringToFile(outputFilename, jsonStr);

//         // �ͷŶ�̬������ڴ�
//         free(jsonStr);
//     }


//     const char* filename = "test.txt";
//     const char* newContent = "New content for the file.";
//     // const char* newDirectory = "C:\\path\\to\\new\\directory"; // Windows Ŀ¼��ʽ
//     const char* newDirectory = "..\\..\\"; // Windows Ŀ¼��ʽ

//     // ɾ���ļ�
//     deleteFile(filename);

//     const char* originalFilename = "old_filename.txt";
//     const char* newFilename = "new_filename.txt";

//     // �������ļ�
//     result = renameFile(originalFilename, newFilename);
//     if (result == 0) {
//         printf("File successfully renamed.\n");
//     } else {
//         printf("Failed to rename file.\n");
//     }

//     // �ƶ��ļ�
//     moveFile(filename, newDirectory);

//     system("pause");

//     return 0;
// }
