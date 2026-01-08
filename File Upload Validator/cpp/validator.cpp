#include <iostream>
#include <fstream>
#include <vector>

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;
    std::ifstream file(argv[1], std::ios::binary);
    if (!file.is_open()) { std::cout << "ERROR" << std::endl; return 1; }

    char buffer[4];
    file.read(buffer, 4);
    if (file.gcount() < 3) { std::cout << "INVALID" << std::endl; return 0; }

    if ((unsigned char)buffer[0] == 0xFF && (unsigned char)buffer[1] == 0xD8 && (unsigned char)buffer[2] == 0xFF) {
        std::cout << "VALID_JPG" << std::endl; return 0;
    }
    if (file.gcount() >= 4 && buffer[0] == 0x25 && buffer[1] == 0x50 && buffer[2] == 0x44 && buffer[3] == 0x46) {
        std::cout << "VALID_PDF" << std::endl; return 0;
    }
    std::cout << "INVALID" << std::endl;
    return 0;
}
