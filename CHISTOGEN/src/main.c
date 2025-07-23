#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "cleaner.h"

#define COLOR_RED     "\033[1;31m"
#define COLOR_GREEN   "\033[1;32m"
#define COLOR_YELLOW  "\033[1;33m"
#define COLOR_BLUE    "\033[1;34m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_RESET   "\033[0m"

void print_banner() {
    printf(COLOR_CYAN);
    printf("\n");
    printf(" ██████╗██╗  ██╗██╗███████╗████████╗ ██████╗  ██████╗ ███████╗███╗   ██╗\n");
    printf("██╔════╝██║  ██║██║██╔════╝╚══██╔══╝██╔═══██╗██╔════╝ ██╔════╝████╗  ██║\n");
    printf("██║     ███████║██║███████╗   ██║   ██║   ██║██║  ███╗█████╗  ██╔██╗ ██║\n");
    printf("██║     ██╔══██║██║╚════██║   ██║   ██║   ██║██║   ██║██╔══╝  ██║╚██╗██║\n");
    printf("╚██████╗██║  ██║██║███████║   ██║   ╚██████╔╝╚██████╔╝███████╗██║ ╚████║\n");
    printf(" ╚═════╝╚═╝  ╚═╝╚═╝╚══════╝   ╚═╝    ╚═════╝  ╚═════╝ ╚══════╝╚═╝  ╚═══╝\n");
    printf(COLOR_RESET);
    printf("\n=== " COLOR_GREEN "✅ SYSTEM CLEANER v2.0" COLOR_RESET " ===\n");
}

void print_menu() {
    printf("\n" COLOR_BLUE "MAIN MENU:" COLOR_RESET "\n");
    printf("1. Clear temporary files (/tmp)\n");
    printf("2. Clear user cache (~/.cache)\n");
    printf("3. Delete old logs (/var/log)\n");
    printf("4. " COLOR_RED "Empty Trash (fully)" COLOR_RESET "\n");
    printf("5. Check disk space " COLOR_CYAN "(with visual)" COLOR_RESET "\n");
    printf("6. " COLOR_YELLOW "Find and delete duplicates" COLOR_RESET "\n");
    printf("7. " COLOR_GREEN "Clear all targets" COLOR_RESET "\n");
    printf("8. Clean package cache\n");
    printf("9. Scan for personal data\n");
    printf("10. Secure file deletion\n");
    printf("0. Exit\n");
    printf(COLOR_YELLOW "Select action: " COLOR_RESET);
}

void get_home_path(char* home) {
    const char *home_env = getenv("HOME");
    if (home_env) {
        strcpy(home, home_env);
    } else {
        strcpy(home, "/home/user");
        printf(COLOR_YELLOW "Warning: Using default home path /home/user\n" COLOR_RESET);
    }
}

void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

int main() {
    int choice;
    char home[256];
    get_home_path(home);

    system("clear");
    print_banner();

    do {
        print_menu();
        if (scanf("%d", &choice) != 1) {
            clear_input_buffer();
            printf(COLOR_RED "\nInvalid input! Please enter a number.\n" COLOR_RESET);
            continue;
        }
        clear_input_buffer();

        switch (choice) {
            case 1:
                printf(COLOR_YELLOW "\nCleaning /tmp...\n" COLOR_RESET);
                clean_tmp("/tmp");
                break;
            
            case 2: {
                char cache_path[512];
                snprintf(cache_path, sizeof(cache_path), "%s/.cache", home);
                printf(COLOR_YELLOW "\nCleaning user cache...\n" COLOR_RESET);
                clean_tmp(cache_path);
                break;
            }
            
            case 3:
                printf(COLOR_YELLOW "\nCleaning old logs...\n" COLOR_RESET);
                clean_logs("/var/log");
                break;
            
            case 4: {
                char trash_path[512];
                snprintf(trash_path, sizeof(trash_path), "%s/.local/share/Trash", home);
                printf(COLOR_RED "\nEmptying Trash completely...\n" COLOR_RESET);
                clean_trash_fully(trash_path);
                break;
            }
            
            case 5:
                printf(COLOR_CYAN "\nChecking disk space...\n" COLOR_RESET);
                show_disk_usage("/");
                break;
            
            case 6: {
                char downloads_path[512];
                snprintf(downloads_path, sizeof(downloads_path), "%s/Downloads", home);
                printf(COLOR_YELLOW "\nSearching for duplicates...\n" COLOR_RESET);
                find_duplicates(downloads_path);
                break;
            }
            
            case 7:
                printf(COLOR_GREEN "\nCleaning all targets...\n" COLOR_RESET);
                clean_tmp("/tmp");
                
                char cache_path[512];
                snprintf(cache_path, sizeof(cache_path), "%s/.cache", home);
                clean_tmp(cache_path);
                
                clean_logs("/var/log");
                
                char trash_path[512];
                snprintf(trash_path, sizeof(trash_path), "%s/.local/share/Trash", home);
                clean_trash_fully(trash_path);
                break;
            
            case 8:
                printf(COLOR_CYAN "\nCleaning package cache...\n" COLOR_RESET);
                clean_package_cache();
                break;
            
            case 9: {
                char scan_path[512];
                printf("Enter path to scan [default: ~/]: ");
                fgets(scan_path, sizeof(scan_path), stdin);
                scan_path[strcspn(scan_path, "\n")] = 0;
                if (strlen(scan_path) == 0) {
                    snprintf(scan_path, sizeof(scan_path), "%s", home);
                }
                printf(COLOR_YELLOW "\nScanning for personal data...\n" COLOR_RESET);
                scan_personal_data(scan_path);
                break;
            }
            
            case 10: {
                char filepath[512];
                int passes;
                printf("Enter file path: ");
                fgets(filepath, sizeof(filepath), stdin);
                filepath[strcspn(filepath, "\n")] = 0;
                printf("Enter number of passes (1-7): ");
                scanf("%d", &passes);
                clear_input_buffer();
                printf(COLOR_RED "\nSecurely deleting file...\n" COLOR_RESET);
                secure_delete(filepath, passes);
                break;
            }
            
            case 0:
                printf(COLOR_GREEN "\nExiting. Have a clean day! \n" COLOR_RESET);
                break;
            
            default:
                printf(COLOR_RED "\nInvalid choice! Try again.\n" COLOR_RESET);
        }
    } while (choice != 0);

    return 0;
}