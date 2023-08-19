#include "memfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FileSystem *newFileSystem()
{
    FileSystem *fs = (FileSystem *)malloc(sizeof(FileSystem));
    Directory* root_dir = (Directory*)malloc(sizeof(Directory));
    root_dir->name = strdup("/");
    root_dir->permission = 0;
    root_dir->parent = NULL;
    root_dir->directories = NULL;
    root_dir->directory_count = 0;
    root_dir->files = NULL;
    root_dir->file_count = 0;

    fs->root_directory = root_dir;
    fs->total_size = 0;
    return fs;
}

Directory *newDirectory(const char *name, int permission)
{
    Directory *dir = (Directory *)malloc(sizeof(Directory));
    dir->name = strdup(name);
    dir->permission = permission;
    dir->directories = NULL;
    dir->directory_count = 0;
    dir->files = NULL;
    dir->file_count = 0;
    return dir;
}

File *newFile(const char *name)
{
    File *file = (File *)malloc(sizeof(File));
    file->name = strdup(name);
    file->contents = NULL;
    file->size = 0;
    return file;
}

void addDirectory(Directory *parent, Directory *child){
    parent->directories = (Directory **)realloc(parent->directories, (parent->directory_count + 1) * sizeof(Directory *));
    parent->directories[parent->directory_count] = child;
    parent->directory_count++;
    child->parent = parent;
}

void addFile(Directory *dir, File *file){
    dir->files = (File **)realloc(dir->files, (dir->file_count + 1) * sizeof(File *));
    dir->files[dir->file_count] = file;
    dir->file_count++;
}

void mkdir(FileSystem *fs, const char *path, int permission){
    char *token;
    char *path_copy = strdup(path);

    Directory *current_dir = fs->root_directory;
    token = strtok(path_copy, "/");

    while (token != NULL){
        Directory *sub_dir = NULL;
        for (int i = 0; i < current_dir->directory_count; i++){
            if(strcmp(current_dir->directories[i]->name, token) == 0){
                sub_dir = current_dir->directories[i];
            }
        }

        if(sub_dir == NULL){
            sub_dir = newDirectory(token, permission);
            addDirectory(current_dir, sub_dir);
        }

        current_dir = sub_dir;
        token = strtok(NULL, "/");
    }

    free(path_copy);
    
}

Directory * findDirectory(Directory *dir, const char *name){
    for (int i = 0; i < dir->directory_count; i++){
        if(strcmp(dir->directories[i]->name, name) == 0){
            return dir->directories[i];
        }
    }
    return NULL;
}

File* findFile(Directory *dir, const char *name){
    for (int i = 0; i < dir->file_count; i++){
        if(strcmp(dir->files[i]->name, name) == 0){
            return dir->files[i];
        }
    }
    return NULL;
}

void writeFile(FileSystem *fs, const char *path, const char *data){
    char *token;
    char *path_copy = strdup(path);

    Directory *current_dir = fs->root_directory;
    token = strtok(path_copy, "/");

    while (token != NULL){
        if(current_dir->directory_count  > 0){
            Directory *sub_dir = NULL;
            
            for(int i = 0; i < current_dir->directory_count; i++){
                if(strcmp(current_dir->directories[i]->name, token) == 0){
                    sub_dir = current_dir->directories[i];
                    break;
                }
            }

            if (sub_dir != NULL){
                current_dir = sub_dir;
                token = strtok(NULL, "/");
                continue;
            }
        }

        File *file = findFile(current_dir, token);

        if(file != NULL){
            free(file->contents);
            file->contents = strdup(data);
            file->size = strlen(data);
            fs->total_size += file->size;
            free(path_copy);
            return;
        }

        file = newFile(token);
        file->contents = strdup(data);
        file->size =strlen(data);
        fs->total_size += file->size;

        addFile(current_dir, file);
        free(path_copy);
        return;
    }
    printf("Error: File not found\n");
    free(path_copy);
}
    
const char *readFile(FileSystem *fs, const char *path){
    char *token;
    char *path_copy = strdup(path);

    Directory *current_dir = fs->root_directory;
    token = strtok(path_copy, "/");

    while (token != NULL){
        if(current_dir->directory_count  > 0){
            Directory *sub_dir = NULL;
            for(int i = 0; i < current_dir->directory_count; i++){
                if(strcmp(current_dir->directories[i]->name, token) == 0){
                    sub_dir = current_dir->directories[i];
                    break;
                }
            }

            if (sub_dir != NULL){
                current_dir = sub_dir;
                token = strtok(NULL, "/");
                continue;
            }
        }

        File *file = findFile(current_dir, token);

        if(file != NULL){
            if(current_dir->permission){
                free(path_copy);
                return file->contents;
            }else{
                printf("Error: Permission denied\n");
                free(path_copy);
                return NULL;
            }
        }
        printf("Error: File not found\n");
        free(path_copy);
        return NULL;
    }
    free(path_copy);
    return NULL;    
}

Directory *getDirectoryByName(Directory *dir, const char *name){
    char *token;
    char *path_copy = strdup(name);

    Directory *current_dir = dir;
    token = strtok(path_copy, "/");

    while (token!=NULL){
        if(current_dir->directory_count > 0){
            Directory *sub_dir = findDirectory(current_dir, token);
            if(sub_dir != NULL){
                current_dir = sub_dir;
                token = strtok(NULL, "/");
                continue;
            }
        }
        free(path_copy);
        return NULL;
    }
    free(path_copy);
    return current_dir;
}
    
    

void listDirectories(Directory *dir){
    for(int i = 0; i < dir->directory_count; i++){
        printf("%s\n", dir->directories[i]->name);
    }
}

void listFiles(Directory *dir){
    for(int i = 0; i < dir->file_count; i++){
        printf("%s\n", dir->files[i]->name);
    }
}



int calculateDirectoryMemory(Directory *dir) {
    return calculateDirectoryMemoryHelper(dir, sizeof(*dir));
}

int calculateDirectoryMemoryHelper(Directory *dir, int total_memory) {
    int file_size = sizeof(File); // Calculate the size of the File structure once

    for (int i = 0; i < dir->file_count; i++) {
        File *file = dir->files[i];
        total_memory += file_size + sizeof(char) * (strlen(file->name) + 1);
    }

    for (int i = 0; i < dir->directory_count; i++) {
        Directory *sub_dir = dir->directories[i];
        total_memory = calculateDirectoryMemoryHelper(sub_dir, total_memory);
    }
    
    return total_memory;
}

int calculateFileSystemMemory(FileSystem *fs) {
    return calculateDirectoryMemory(fs->root_directory);
}


void freeMemory(Directory *dir){
    for(int i = 0; i < dir->file_count; i++){
        File *file = dir->files[i];
        free(file->name);
        free(file->contents);
        free(file);
    }
    
    for(int i = 0; i < dir->directory_count; i++){
        freeMemory(dir->directories[i]);
    }
    
    free(dir->directories);
    free(dir->name);
    free(dir);

}

int main() {
    FileSystem* fs = newFileSystem();
    
    mkdir(fs, "/my_directory", 1);
    writeFile(fs, "/my_directory/my_file.txt", "Hello, World!"); //24 char
    
    const char* content = readFile(fs, "/my_directory/my_file.txt");
    
    if (content) {
        printf("File contents: %s\n", content);
    }
    
    listDirectories(fs->root_directory);
    listFiles(getDirectoryByName(fs->root_directory, "my_directory"));
    
    printf("Total memory used: %d\n", calculateFileSystemMemory(fs));

    freeMemory(fs->root_directory);
    free(fs);
    
    return 0;
}