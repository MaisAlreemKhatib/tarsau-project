#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "archive.h"

#define MAX_FILES 32
#define MAX_TOTAL_SIZE 209715200

int is_text_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        return 0;
    }

    int ch;
    while ((ch = fgetc(file)) != EOF) {
        if ((ch < 32 || ch > 126) && ch != '\n' && ch != '\r' && ch != '\t') {
            fclose(file);
            return 0;
        }
    }

    fclose(file);
    return 1;
}

int create_archive(int argc, char *argv[]) {
    char *output_file = "a.sau";
    char *input_files[MAX_FILES];
    int file_count = 0;
    long total_size = 0;

    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            if (i + 1 < argc) {
                output_file = argv[i + 1];
                i++;
            } else {
                printf("Arşiv dosya adı belirtilmedi!\n");
                return 1;
            }
        } else {
            if (file_count >= MAX_FILES) {
                printf("En fazla 32 giriş dosyası verilebilir!\n");
                return 1;
            }
            input_files[file_count++] = argv[i];
        }
    }

    if (file_count == 0) {
        printf("Giriş dosyası belirtilmedi!\n");
        return 1;
    }

    char metadata[8192] = "";

    for (int i = 0; i < file_count; i++) {
        struct stat st;

        if (stat(input_files[i], &st) != 0) {
            printf("%s dosyası bulunamadı!\n", input_files[i]);
            return 1;
        }

        if (!S_ISREG(st.st_mode)) {
            printf("%s giriş dosyasının formatı uyumsuzdur!\n", input_files[i]);
            return 1;
        }

        if (!is_text_file(input_files[i])) {
            printf("%s giriş dosyasının formatı uyumsuzdur!\n", input_files[i]);
            return 1;
        }

        total_size += st.st_size;

        if (total_size > MAX_TOTAL_SIZE) {
            printf("Giriş dosyalarının toplam boyutu 200 MB'ı geçemez!\n");
            return 1;
        }

        char record[512];
        sprintf(record, "|%s,%o,%ld|", input_files[i], st.st_mode & 0777, st.st_size);
        strcat(metadata, record);
    }

    int metadata_size = strlen(metadata) + 10;

    FILE *out = fopen(output_file, "wb");
    if (out == NULL) {
        printf("Arşiv dosyası oluşturulamadı!\n");
        return 1;
    }

    fprintf(out, "%010d", metadata_size);
    fwrite(metadata, 1, strlen(metadata), out);

    for (int i = 0; i < file_count; i++) {
        FILE *in = fopen(input_files[i], "rb");
        if (in == NULL) {
            fclose(out);
            printf("%s dosyası açılamadı!\n", input_files[i]);
            return 1;
        }

        int ch;
        while ((ch = fgetc(in)) != EOF) {
            fputc(ch, out);
        }

        fclose(in);
    }

    fclose(out);

    printf("Dosyalar birleştirildi.\n");
    return 0;
}
