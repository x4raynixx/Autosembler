#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

std::string selectedFormat;

void help() {
    std::cout << "> Usage: autosembler <file.asm> <format> [run]\n";
}

bool fileExists(const std::string& pathStr) {
    std::ifstream file(pathStr);
    return file.good();
}

bool commandExists(const std::string& cmd) {
#ifdef _WIN32
    std::string check = "where " + cmd + " >nul 2>nul";
#else
    std::string check = "command -v " + cmd + " >/dev/null 2>&1";
#endif
    return system(check.c_str()) == 0;
}

bool installNasm() {
#ifdef _WIN32
    if (system("winget install -e --id NASM.NASM") == 0) return true;
    if (system("choco install nasm -y") == 0) return true;
    return false;
#else
    if (system("sudo apt-get update && sudo apt-get install -y nasm") == 0) return true;
    if (system("sudo dnf install -y nasm") == 0) return true;
    if (system("sudo pacman -Sy --noconfirm nasm") == 0) return true;
    return false;
#endif
}

bool installGcc() {
#ifdef _WIN32
    if (system("winget install -e --id GNU.GCC") == 0) return true;
    if (system("choco install mingw -y") == 0) return true;
    return false;
#else
    if (system("sudo apt-get update && sudo apt-get install -y build-essential") == 0) return true;
    if (system("sudo dnf install -y gcc gcc-c++") == 0) return true;
    if (system("sudo pacman -Sy --noconfirm gcc") == 0) return true;
    return false;
#endif
}


bool assembly(const std::string& pathStr, bool runAfter = false) {
    if (!commandExists("nasm")) {
        if (!installNasm()) {
            std::cerr << "> Failed to install 'nasm', please install manually!\n";
            return false;
        }
    }

    if (!commandExists("gcc")) {
        if (!installGcc()) {
            std::cerr << "> Failed to install 'gcc', please install manually!\n";
            return false;
        }
    }

    std::string objFile = "out.o";
    std::string exeFile =
#ifdef _WIN32
        "out.exe";
#else
        "out";
#endif

    std::string nasm_cmd = "nasm -f " + selectedFormat + " " + pathStr + " -o " + objFile;
#ifdef _WIN32
    std::string ld_cmd = "gcc " + objFile + " -o " + exeFile + " -m64 -static";
#else
    std::string ld_cmd = "ld " + objFile + " -o " + exeFile;
#endif

    if (system(nasm_cmd.c_str()) != 0) {
        std::cerr << "> Failed to assemble source file.\n";
        return false;
    }

    if (system(ld_cmd.c_str()) != 0) {
        std::cerr << "> Failed to link object file.\n";
        return false;
    }

    std::cout << "Assembling successful -> " << exeFile << "\n";

    if (runAfter) {
        std::cout << "Running " << exeFile << "...\n";
        std::cout << "=-=-=-=-=-=-=-=-=-=OUTPUT=-=-=-=-=-=-=-=-=-=-=-=-=\n";
#ifdef _WIN32
        system(exeFile.c_str());
#else
        system(("./" + exeFile).c_str());
#endif
        std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n";
    }

    return true;
}

int main(int argc, char* argv[]) {
    if (argc == 2 && std::string(argv[1]) == "help") {
        help();
        return 0;
    }

    if (argc < 3) {
        std::cerr << "Usage: autosembler <file.asm> <format> [run]\nToo few arguments [Min: 2]\n";
        return EXIT_FAILURE;
    }

    std::string filePath = argv[1];
    std::string formats[] = {"elf64", "win64"};
    bool formatFound = false;

    for (auto& fmt : formats) {
        if (fmt == argv[2]) {
            formatFound = true;
            selectedFormat = argv[2];
            break;
        }
    }

    if (!formatFound) {
        std::cerr << "Invalid format, supported:\n";
        for (auto& fmt : formats) std::cerr << "> " << fmt << "\n";
        return EXIT_FAILURE;
    }

    bool runAfter = (argc >= 4 && std::string(argv[3]) == "run");

    std::cout << "Format: " << selectedFormat;
    if (fileExists(filePath)) {
        std::cout << "\nFile: " << filePath;
        std::cout << "\nAssembling...\n=-=-=-=-=-=-=-=-=-=BUILD=-=-=-=-=-=-=-=-=-=-=-=-=\n";
        assembly(filePath, runAfter);
    } else {
        std::cerr << "\n> File doesn't exist!\n";
    }

    return 0;
}
