#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <time.h>
#include "cleaner.h"

void clean_tmp(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror("Error opening directory");
        return;
    }

    struct dirent *entry;
    char filepath[512];
    int deleted = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);
        if (remove(filepath) == 0) {
            printf("Deleted: %s\n", filepath);
            deleted++;
        } else {
            perror(filepath);
        }
    }

    closedir(dir);

    if (deleted == 0)
        printf("There are no files to delete in %s\n", path);
    else
        printf("Deleted %d files from %s\n", deleted, path);
}

void clean_logs(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror("Opening error /var/log");
        return;
    }

    struct dirent *entry;
    char filepath[512];
    time_t now = time(NULL);
    int deleted = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG)
            continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);

        struct stat st;
        if (stat(filepath, &st) == 0) {
            double days = difftime(now, st.st_mtime) / (60 * 60 * 24);
            if (days >= 7) {
                if (remove(filepath) == 0) {
                    printf("Log deleted: %s (%.0f days)\n", filepath, days);
                    deleted++;
                } else {
                    perror(filepath);
                }
            }
        }
    }

    closedir(dir);
    printf("Deleted %d old logs from %s\n", deleted, path);
}

void show_disk_usage(const char *path) {
    struct statvfs fs;
    if (statvfs(path, &fs) != 0) {
        perror("Error getting disk information");
        return;
    }

    unsigned long total = fs.f_blocks * fs.f_frsize;
    unsigned long free  = fs.f_bfree * fs.f_frsize;
    unsigned long used  = total - free;

    printf("\n💽 DISK %s\n", path);
    printf("Total: %.2f GB\n", total / (1024.0 * 1024 * 1024));
    printf("Used: %.2f GB\n", used / (1024.0 * 1024 * 1024));
    printf("Free: %.2f GB\n", free / (1024.0 * 1024 * 1024));
}
