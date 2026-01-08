#include <stdio.h>
#include <string.h>

const unsigned char SIGNATURE_JPG[] = {0xFF, 0xD8, 0xFF};
const unsigned char SIGNATURE_PDF[] = {0x25, 0x50, 0x44, 0x46};

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    FILE *file = fopen(argv[1], "rb");
    if (!file) { printf("ERROR\n"); return 1; }
    
    unsigned char buffer[4];
    size_t bytesRead = fread(buffer, 1, 4, file);
    fclose(file);

    if (bytesRead < 3) { printf("INVALID\n"); return 0; }
    if (memcmp(buffer, SIGNATURE_JPG, 3) == 0) { printf("VALID_JPG\n"); return 0; }
    if (bytesRead >= 4 && memcmp(buffer, SIGNATURE_PDF, 4) == 0) { printf("VALID_PDF\n"); return 0; }
    
    printf("INVALID\n");
    return 0;
}
