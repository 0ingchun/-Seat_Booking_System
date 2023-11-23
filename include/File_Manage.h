// #include <windows.h>

char* readFileToString(const char* filename);

void writeStringToFile(const char* filename, const char* jsonStr);

int createFile(const char* filename);

int deleteFile(const char* filename);

int renameFile(const char* originalFilename, const char* newFilename);

int moveFile(const char* originalFilename, const char* newDirectory);

// HANDLE getFileHandleFromFilePointer(FILE* file);

// int lockFile(HANDLE file);

// int unlockFile(HANDLE file);

// HANDLE createFileAndGetHandle(const char* filename);

// HANDLE openFileAndGetHandle(const char* filename);

