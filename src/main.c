#include <stdio.h>
#include <string.h>
#include "archive.h"
#include "extract.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Kullanım:\n");
        printf("./tarsau -b dosya1 dosya2 -o arsiv.sau\n");
        printf("./tarsau -a arsiv.sau [dizin]\n");
        return 1;
    }

    if (strcmp(argv[1], "-b") == 0) {
        return create_archive(argc, argv);
    } 
    else if (strcmp(argv[1], "-a") == 0) {
        return extract_archive(argc, argv);
    } 
    else {
        printf("Hatalı komut!\n");
        return 1;
    }
}
