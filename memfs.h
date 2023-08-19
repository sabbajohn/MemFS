#ifndef MEMFS_H
#define MEMFS_H

typedef struct File {
    char *name;
    char *contents;
    int size;
} File;

typedef struct Directory {
    char *name;
    int permission;
    struct Directory* parent; 
    struct Directory** directories; 
    int directory_count; 
    struct File** files; 
    int file_count; 
} Directory;

typedef struct FileSystem {
    int total_size;
    Directory* root_directory; 
} FileSystem;


FileSystem *newFileSystem();
Directory *newDirectory(const char *name, int permission);
File *newFile(const char *name);
void addDirectory(Directory *parent, Directory *child);
void addFile(Directory *dir, File *file);
void mkdir(FileSystem *fs, const char *path, int permission);
File* findFile(Directory *dir, const char *name);
void writeFile(FileSystem *fs, const char *path, const char *data);
const char *readFile(FileSystem *fs, const char *path);
void listDirectories(Directory *dir);
void listFiles(Directory *dir);
int calculateDirectoryMemoryHelper(Directory *dir, int total_memory);
int calculateDirectoryMemory(Directory *dir);
int calculateFileSystemMemory(FileSystem *fs);
void freeMemory(Directory *dir);

#endif // MEMFS_H