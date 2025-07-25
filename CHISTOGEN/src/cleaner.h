#ifndef CLEANER_H
#define CLEANER_H

void clean_tmp(const char *path);
void clean_logs(const char *path);
void show_disk_usage(const char *path);

void clean_trash_fully(const char *trash_path);  
void find_duplicates(const char *dir_path);      

int is_file_duplicate(const char *file1, const char *file2);  
void delete_file_safe(const char *path);         

void clean_package_cache();
void scan_personal_data(const char *path);
void secure_delete(const char *path, int passes);

#endif