#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <time.h>
#include <openssl/evp.h>
#include "cleaner.h"
#include <openssl/md5.h>

#ifndef DT_REG
#define DT_REG 8
#endif

#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_RESET   "\033[0m"

static void calculate_md5(const char *filename, unsigned char *md5_sum) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        memset(md5_sum, 0, EVP_MD_size(EVP_md5()));
        return;
    }

    EVP_MD_CTX *mdctx = EVP_MD_CTX_new();
    if (!mdctx) {
        fclose(file);
        memset(md5_sum, 0, EVP_MD_size(EVP_md5()));
        return;
    }

    if (EVP_DigestInit_ex(mdctx, EVP_md5(), NULL) != 1) {
        EVP_MD_CTX_free(mdctx);
        fclose(file);
        memset(md5_sum, 0, EVP_MD_size(EVP_md5()));
        return;
    }

    unsigned char buffer[4096];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file))) {
        if (EVP_DigestUpdate(mdctx, buffer, bytes_read) != 1) {
            EVP_MD_CTX_free(mdctx);
            fclose(file);
            memset(md5_sum, 0, EVP_MD_size(EVP_md5()));
            return;
        }
    }

    unsigned int md_len;
    if (EVP_DigestFinal_ex(mdctx, md5_sum, &md_len) != 1 || md_len != EVP_MD_size(EVP_md5())) {
        memset(md5_sum, 0, EVP_MD_size(EVP_md5()));
    }

    EVP_MD_CTX_free(mdctx);
    fclose(file);
}

void clean_tmp(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        perror(COLOR_RED "Error opening directory" COLOR_RESET);
        return;
    }

    struct dirent *entry;
    char filepath[512];
    int deleted = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);
        
        struct stat st;
        if (lstat(filepath, &st)) {
            perror(filepath);
            continue;
        }

        if (S_ISDIR(st.st_mode)) {
            clean_tmp(filepath);
            if (rmdir(filepath) == 0) {
                printf(COLOR_GREEN "Deleted directory: %s\n" COLOR_RESET, filepath);
                deleted++;
            }
        } else {
            if (remove(filepath) == 0) {
                printf(COLOR_GREEN "Deleted: %s\n" COLOR_RESET, filepath);
                deleted++;
            } else {
                perror(COLOR_RED "Error deleting" COLOR_RESET);
            }
        }
    }

    closedir(dir);

    if (deleted == 0)
        printf(COLOR_YELLOW "No files to delete in %s\n" COLOR_RESET, path);
    else
        printf(COLOR_GREEN "Deleted %d items from %s\n" COLOR_RESET, deleted, path);
}

void clean_logs(const char *path) {
    DIR *dir = opendir(path);
    if (!dir) {
        perror(COLOR_RED "Error opening logs directory" COLOR_RESET);
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
                    printf(COLOR_GREEN "Deleted old log: %s (%.0f days)\n" COLOR_RESET, filepath, days);
                    deleted++;
                } else {
                    perror(COLOR_RED "Error deleting log" COLOR_RESET);
                }
            }
        }
    }

    closedir(dir);
    printf(COLOR_CYAN "Deleted %d old logs from %s\n" COLOR_RESET, deleted, path);
}

void show_disk_usage(const char *path) {
    struct statvfs fs;
    if (statvfs(path, &fs) != 0) {
        perror(COLOR_RED "Error getting disk info" COLOR_RESET);
        return;
    }

    unsigned long total = fs.f_blocks * fs.f_frsize;
    unsigned long free = fs.f_bfree * fs.f_frsize;
    unsigned long used = total - free;
    double percent_used = (double)used / total * 100;

    printf("\n💽 DISK %s\n", path);
    printf("Total: %.2f GB\n", total / (1024.0 * 1024 * 1024));
    printf("Used: %.2f GB (%.0f%%)\n", used / (1024.0 * 1024 * 1024), percent_used);
    printf("Free: %.2f GB\n\n", free / (1024.0 * 1024 * 1024));

    // Визуализация
    printf("[");
    int bars = (int)(percent_used / 5);
    for (int i = 0; i < 20; i++) {
        if (i < bars) {
            printf(COLOR_RED "|" COLOR_RESET);
        } else {
            printf(" ");
        }
    }
    printf("]\n");
}

void clean_trash_fully(const char *trash_path) {
    char files_path[512];
    char info_path[512];
    
    snprintf(files_path, sizeof(files_path), "%s/files", trash_path);
    snprintf(info_path, sizeof(info_path), "%s/info", trash_path);

    printf(COLOR_YELLOW "\nCleaning Trash files...\n" COLOR_RESET);
    clean_tmp(files_path);
    
    printf(COLOR_YELLOW "\nCleaning Trash info...\n" COLOR_RESET);
    clean_tmp(info_path);
}

void clean_package_cache() {
    printf(COLOR_CYAN "\n🧹 Очистка кеша пакетных менеджеров...\n" COLOR_RESET);
    
    struct {
        const char *cmd;
        const char *name;
    } managers[] = {
        {"apt-get clean", "APT (Debian/Ubuntu)"},
        {"dnf clean all", "DNF (Fedora)"},
        {"yum clean all", "YUM (RHEL/CentOS)"},
        {"pacman -Sc --noconfirm", "Pacman (Arch)"},
        {"npm cache clean --force", "NPM"},
        {"pip cache purge", "PIP"},
        {NULL, NULL}
    };

    int cleaned = 0;
    for (int i = 0; managers[i].cmd; i++) {
        if (system(managers[i].cmd) == 0) {
            printf(COLOR_GREEN "✓ Очищен кеш: %s\n" COLOR_RESET, managers[i].name);
            cleaned++;
        }
    }

    if (cleaned == 0) {
        printf(COLOR_YELLOW "Не найдено поддерживаемых пакетных менеджеров\n" COLOR_RESET);
    } else {
        printf(COLOR_GREEN "Готово! Очищено %d кешей\n" COLOR_RESET, cleaned);
    }
}

#include <regex.h>

void scan_personal_data(const char *path) {
    printf(COLOR_CYAN "\n🔍 Поиск личных данных в %s...\n" COLOR_RESET);
    
    struct {
        const char *pattern;
        const char *description;
    } patterns[] = {
        {"\\b[0-9]{4}[ -]?[0-9]{4}[ -]?[0-9]{4}[ -]?[0-9]{4}\\b", "Номер банковской карты"},
        {"\\b[0-9]{3}-?[0-9]{2}-?[0-9]{4}\\b", "SSN/ИНН"},
        {"\\b[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\\.[A-Za-z]{2,}\\b", "Email"},
        {"\\b\\+?[0-9]{1,3}[ -]?\\(?[0-9]{3}\\)?[ -]?[0-9]{3}[ -]?[0-9]{4}\\b", "Телефон"},
        {NULL, NULL}
    };

    DIR *dir = opendir(path);
    if (!dir) {
        perror(COLOR_RED "Ошибка открытия директории" COLOR_RESET);
        return;
    }

    struct dirent *entry;
    char filepath[1024];
    int total_found = 0;

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            snprintf(filepath, sizeof(filepath), "%s/%s", path, entry->d_name);
            
            FILE *file = fopen(filepath, "r");
            if (!file) continue;

            char line[1024];
            int line_num = 0;
            
            while (fgets(line, sizeof(line), file)) {
                line_num++;
                for (int i = 0; patterns[i].pattern; i++) {
                    regex_t regex;
                    if (regcomp(&regex, patterns[i].pattern, REG_EXTENDED)) continue;
                    
                    if (regexec(&regex, line, 0, NULL, 0) == 0) {
                        printf(COLOR_RED "⚠ Найдено в %s (строка %d): %s - %s\n" COLOR_RESET,
                               filepath, line_num, patterns[i].description, line);
                        total_found++;
                    }
                    regfree(&regex);
                }
            }
            fclose(file);
        }
    }
    closedir(dir);

    printf(COLOR_CYAN "\n🔍 Найдено %d потенциальных утечек данных\n" COLOR_RESET, total_found);
}

#include <fcntl.h>
#include <unistd.h>

void secure_delete(const char *path, int passes) {
    printf(COLOR_CYAN "\n🛡️ Безопасное удаление (метод %d проходов): %s\n" COLOR_RESET, passes, path);
    
    int fd = open(path, O_RDWR);
    if (fd == -1) {
        perror(COLOR_RED "Ошибка открытия файла" COLOR_RESET);
        return;
    }

    struct stat st;
    if (fstat(fd, &st)) {
        perror(COLOR_RED "Ошибка получения размера файла" COLOR_RESET);
        close(fd);
        return;
    }

    const char *patterns[] = { "\x00", "\xFF", "\xAA", "\x55", "\xDB", "\x6D", "\xB6" };
    char *buffer = malloc(st.st_size);
    
    for (int i = 0; i < passes; i++) {
        lseek(fd, 0, SEEK_SET);
        
        // Разные методы перезаписи
        memset(buffer, patterns[i % 7][0], st.st_size);
        write(fd, buffer, st.st_size);
        fsync(fd);
        
        printf("Проход %d/%d завершен (%.0f%%)\r", i+1, passes, (i+1)*100.0/passes);
        fflush(stdout);
    }
    
    free(buffer);
    close(fd);
    
    if (remove(path) == 0) {
        printf(COLOR_GREEN "\n✓ Файл безопасно удален: %s\n" COLOR_RESET, path);
    } else {
        perror(COLOR_RED "\nОшибка удаления файла" COLOR_RESET);
    }
}

void find_duplicates(const char *dir_path) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        perror(COLOR_RED "Error opening directory" COLOR_RESET);
        return;
    }

    printf(COLOR_CYAN "\nSearching for duplicates in %s...\n" COLOR_RESET);
    
    struct dirent *entry;
    char filepath1[512], filepath2[512];
    unsigned char md5_1[MD5_DIGEST_LENGTH], md5_2[MD5_DIGEST_LENGTH];
    int duplicates_found = 0;

    // Проход по всем файлам в директории
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type != DT_REG) continue;

        snprintf(filepath1, sizeof(filepath1), "%s/%s", dir_path, entry->d_name);
        calculate_md5(filepath1, md5_1);

        // Сравнение с остальными файлами
        DIR *inner_dir = opendir(dir_path);
        if (!inner_dir) continue;

        struct dirent *inner_entry;
        while ((inner_entry = readdir(inner_dir)) != NULL) {
            if (inner_entry->d_type != DT_REG) continue;
            if (strcmp(entry->d_name, inner_entry->d_name) == 0) continue;

            snprintf(filepath2, sizeof(filepath2), "%s/%s", dir_path, inner_entry->d_name);
            calculate_md5(filepath2, md5_2);

            if (memcmp(md5_1, md5_2, MD5_DIGEST_LENGTH) == 0) {
                printf(COLOR_YELLOW "Found duplicates:\n  %s\n  %s\n" COLOR_RESET, filepath1, filepath2);
                if (remove(filepath2) == 0) {
                    printf(COLOR_GREEN "Deleted duplicate: %s\n" COLOR_RESET, filepath2);
                    duplicates_found++;
                } else {
                    perror(COLOR_RED "Error deleting duplicate" COLOR_RESET);
                }
            }
        }
        closedir(inner_dir);
    }
    closedir(dir);

    if (duplicates_found == 0) {
        printf(COLOR_GREEN "No duplicates found in %s\n" COLOR_RESET, dir_path);
    } else {
        printf(COLOR_CYAN "Found and deleted %d duplicate files\n" COLOR_RESET, duplicates_found);
    }
}
