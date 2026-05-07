#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "extract.h"

#define MAX_FILES 32

typedef struct {
    char filename[256];
    int permission;
    long size;
} FileInfo;

int extract_archive(int argc, char *argv[]) {
    if (argc < 3 || argc > 4) {
        printf("Hatalı kullanım!\n");
        return 1;
    }

    char *archive_file = argv[2];
    char *output_dir = ".";

    if (argc == 4) {
        output_dir = argv[3];
        mkdir(output_dir, 0755);
    }

    if (strstr(archive_file, ".sau") == NULL) {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        return 1;
    }

    FILE *archive = fopen(archive_file, "rb");
    if (archive == NULL) {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        return 1;
    }

    char size_str[11];

    if (fread(size_str, 1, 10, archive) != 10) {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        fclose(archive);
        return 1;
    }

    size_str[10] = '\0';

    int metadata_size = atoi(size_str);

    if (metadata_size <= 10) {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        fclose(archive);
        return 1;
    }

    size_t metadata_content_size = (size_t)(metadata_size - 10);

    char *metadata = (char *)malloc(metadata_content_size + 1);

    if (metadata == NULL) {
        printf("Bellek hatası!\n");
        fclose(archive);
        return 1;
    }

    if (fread(metadata, 1, metadata_content_size, archive) != metadata_content_size) {
        printf("Arşiv dosyası uygunsuz veya bozuk!\n");
        free(metadata);
        fclose(archive);
        return 1;
    }

    metadata[metadata_content_size] = '\0';

    FileInfo files[MAX_FILES];
    int file_count = 0;

    char *token = strtok(metadata, "|");

    while (token != NULL) {
        if (strlen(token) > 0) {

            if (file_count >= MAX_FILES) {
                printf("Arşiv dosyası uygunsuz veya bozuk!\n");
                free(metadata);
                fclose(archive);
                return 1;
            }

            if (sscanf(token, "%255[^,],%o,%ld",
                       files[file_count].filename,
                       &files[file_count].permission,
                       &files[file_count].size) != 3) {

                printf("Arşiv dosyası uygunsuz veya bozuk!\n");
                free(metadata);
                fclose(archive);
                return 1;
            }

            file_count++;
        }

        token = strtok(NULL, "|");
    }

    for (int i = 0; i < file_count; i++) {

        char output_path[512];

        if (strcmp(output_dir, ".") == 0) {
            snprintf(output_path, sizeof(output_path), "%s", files[i].filename);
        } else {
            snprintf(output_path, sizeof(output_path), "%s/%s", output_dir, files[i].filename);
       }

        FILE *out = fopen(output_path, "wb");

        if (out == NULL) {
            printf("Dosya oluşturulamadı: %s\n", output_path);
            free(metadata);
            fclose(archive);
            return 1;
        }

        for (long j = 0; j < files[i].size; j++) {

            int ch = fgetc(archive);

            if (ch == EOF) {
                printf("Arşiv dosyası uygunsuz veya bozuk!\n");
                fclose(out);
                free(metadata);
                fclose(archive);
                return 1;
            }

            fputc(ch, out);
        }

        fclose(out);

        chmod(output_path, files[i].permission);
    }

    free(metadata);
    fclose(archive);

    printf("Dosyalar açıldı.\n");

    return 0;
}
