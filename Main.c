#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

// Helper function to check if file has an image extension
int is_image_file(const char *filename) {
    const char *ext[] = {".png", ".jpg", ".jpeg", ".bmp", ".gif", ".webp"};
    int ext_count = sizeof(ext) / sizeof(ext[0]);
    const char *dot = strrchr(filename, '.');
    if (!dot) return 0;
    for (int i = 0; i < ext_count; i++) {
        if (strcasecmp(dot, ext[i]) == 0)
            return 1;
    }
    return 0;
}

int main() {
    DIR *dir;
    struct dirent *entry;
    char new_name[256];
    char old_path[512], new_path[512];
    char base_name[100];
    int count = 1;

    printf("Enter Image Name: ");
    if (fgets(base_name, sizeof(base_name), stdin) == NULL) {
        fprintf(stderr, "Error reading input.\n");
        return 1;
    }

    // Remove newline character from input
    base_name[strcspn(base_name, "\n")] = 0;

    dir = opendir(".");
    if (dir == NULL) {
        perror("opendir");
        return 1;
    }

    while ((entry = readdir(dir)) != NULL) {
        struct stat st;
        if (stat(entry->d_name, &st) == 0 && S_ISREG(st.st_mode)) {
            if (is_image_file(entry->d_name)) {
                const char *ext = strrchr(entry->d_name, '.');
                snprintf(new_name, sizeof(new_name), "%s_%d%s", base_name, count, ext);
                snprintf(old_path, sizeof(old_path), "./%s", entry->d_name);
                snprintf(new_path, sizeof(new_path), "./%s", new_name);

                if (rename(old_path, new_path) == 0) {
                    printf("[%s] -> [%s]\n", entry->d_name, new_name);
                    count++;
                } else {
                    perror("rename");
                }
            }
        }
    }

    closedir(dir);
    printf("Renamed %d images successfully.\n", count - 1);
    return 0;
}
