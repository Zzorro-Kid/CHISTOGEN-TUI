#ifndef CLEANER_H
#define CLEANER_H

// Основные функции очистки
void clean_tmp(const char *path);
void clean_logs(const char *path);
void show_disk_usage(const char *path);

// Новые функции для версии 2.0
void clean_trash_fully(const char *trash_path);  // Полная очистка корзины (файлы + метаданные)
void find_duplicates(const char *dir_path);      // Поиск и удаление дубликатов файлов

// Вспомогательные функции (можно добавить в private-часть)
int is_file_duplicate(const char *file1, const char *file2);  // Сравнение файлов
void delete_file_safe(const char *path);         // Безопасное удаление с проверками

void clean_package_cache();
void scan_personal_data(const char *path);
void secure_delete(const char *path, int passes);

#endif