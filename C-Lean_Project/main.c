#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cleaner.h"

void print_menu() {
    printf("\n=== 🧹 SYSTEM CLEANING ===\n");
    printf("1. Clear temporary files (/tmp)\n");
    printf("2. Clear user cache (~/.cache)\n");
    printf("3. Delete old logs (/var/log)\n");
    printf("4. Empty Trash (~/.local/share/Trash)\n");
    printf("5. Check free space\n");
    printf("6. Clear all\n");
    printf("0. Exit\n");
    printf("Select action: ");
}

int main() {
    int choice;

    do {
        print_menu();
        scanf("%d", &choice);

        char home[256];
        const char *home_env = getenv("HOME");
        strcpy(home, home_env ? home_env : "/home/user");

        switch (choice) {
            case 1:
                clean_tmp("/tmp");
                break;
            case 2: {
                char cache_path[512];
                snprintf(cache_path, sizeof(cache_path), "%s/.cache", home);
                clean_tmp(cache_path);
                break;
            }
            case 3:
                clean_logs("/var/log");
                break;
            case 4: {
                char trash_path[512];
                snprintf(trash_path, sizeof(trash_path), "%s/.local/share/Trash/files", home);
                clean_tmp(trash_path);
                break;
            }
            case 5:
                show_disk_usage("/");
                break;
            case 6:
                clean_tmp("/tmp");

                char cache_path[512];
                snprintf(cache_path, sizeof(cache_path), "%s/.cache", home);
                clean_tmp(cache_path);

                clean_logs("/var/log");

                char trash_path[512];
                snprintf(trash_path, sizeof(trash_path), "%s/.local/share/Trash/files", home);
                clean_tmp(trash_path);

                break;
            case 0:
                printf("Exit.\n");
                break;
            default:
                printf("Wrong choice.\n");
        }

    } while (choice != 0);

    return 0;
}
