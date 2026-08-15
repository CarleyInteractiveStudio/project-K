#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <cstdlib>
#include <filesystem>

#include "lexer.hpp"
#include "parser.hpp"
#include "codegen.hpp"

namespace fs = std::filesystem;

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: could not open file '" << path << "'\n";
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void processImports(ProgramNode* mainProg, const std::string& baseDir, std::vector<std::string>& visitedImports) {
    std::vector<std::unique_ptr<ImportStmt>> currentImports = std::move(mainProg->imports);
    mainProg->imports.clear();

    for (const auto& imp : currentImports) {
        fs::path targetPath = fs::path(imp->filepath);
        fs::path resolvedPath;

        if (targetPath.is_absolute()) {
            resolvedPath = targetPath;
        } else {
            fs::path relPath = fs::path(baseDir) / targetPath;
            if (fs::exists(relPath)) {
                resolvedPath = relPath;
            } else if (fs::exists(targetPath)) {
                resolvedPath = targetPath;
            } else {
                resolvedPath = relPath;
            }
        }

        std::string canonicalPath;
        try {
            canonicalPath = fs::weakly_canonical(resolvedPath).string();
        } catch (...) {
            canonicalPath = resolvedPath.string();
        }

        bool alreadyVisited = false;
        for (const auto& v : visitedImports) {
            if (v == canonicalPath) {
                alreadyVisited = true;
                break;
            }
        }
        if (alreadyVisited) continue;

        visitedImports.push_back(canonicalPath);

        if (!fs::exists(canonicalPath)) {
            std::cerr << "Error: imported module not found: '" << canonicalPath << "' (imported from " << baseDir << ")\n";
            exit(1);
        }

        std::string source = readFile(canonicalPath);
        Lexer lexer(source);
        auto tokens = lexer.tokenize();
        Parser parser(tokens);
        auto importedProg = parser.parseProgram();

        fs::path currentModuleDir = fs::path(canonicalPath).parent_path();

        processImports(importedProg.get(), currentModuleDir.string(), visitedImports);

        for (auto& fn : importedProg->functions) {
            mainProg->functions.push_back(std::move(fn));
        }

        for (auto& stmt : importedProg->topLevelStatements) {
            mainProg->topLevelStatements.push_back(std::move(stmt));
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "K Language Compiler (kcc) v1.0\n";
        std::cout << "Usage: " << argv[0] << " <input.k> [-o <output.bin|output.iso>] [--iso] [--arch <x86_64|x86_32|arm64|riscv64>]\n";
        return 0;
    }

    std::string inputFile;
    std::string outputFile = "kernel.bin";
    bool generateIso = false;
    Arch targetArch = Arch::X86_32;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
            if (outputFile.length() >= 4 && outputFile.substr(outputFile.length() - 4) == ".iso") {
                generateIso = true;
            }
        } else if (arg == "--iso") {
            generateIso = true;
        } else if (arg == "--arch" && i + 1 < argc) {
            std::string archStr = argv[++i];
            if (archStr == "x86_32") targetArch = Arch::X86_32;
            else if (archStr == "arm64") targetArch = Arch::ARM64;
            else if (archStr == "riscv64") targetArch = Arch::RISCV64;
            else targetArch = Arch::X86_64;
        } else if (inputFile.empty()) {
            inputFile = arg;
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: no input file provided.\n";
        return 1;
    }

    std::string source = readFile(inputFile);
    Lexer lexer(source);
    auto tokens = lexer.tokenize();

    Parser parser(tokens);
    auto program = parser.parseProgram();

    std::vector<std::string> visitedImports;
    fs::path inputPath(inputFile);
    std::string baseDir = inputPath.parent_path().empty() ? "." : inputPath.parent_path().string();

    processImports(program.get(), baseDir, visitedImports);

    CodeGenerator codegen(targetArch);
    std::string asmCode = codegen.generate(program.get());

    std::string binFile = generateIso ? "kernel.bin" : outputFile;
    std::string asmFilename = binFile + ".s";
    std::ofstream asmFile(asmFilename);
    asmFile << asmCode;
    asmFile.close();

    std::cout << "[kcc] Assembly generated: " << asmFilename << "\n";

    std::string asCmd = "as --32 " + asmFilename + " -o " + binFile + ".o";
    std::string ldCmd = "ld -m elf_i386 -Ttext 0x100000 " + binFile + ".o -o " + binFile;

    std::cout << "[kcc] Compiling bare-metal bootable kernel executable...\n";
    int res1 = system(asCmd.c_str());
    int res2 = system(ldCmd.c_str());

    if (res1 != 0 || res2 != 0) {
        std::cerr << "[kcc] Error: assembly or linking failed.\n";
        return 1;
    }

    std::cout << "[kcc] Successfully compiled bootable kernel -> " << binFile << "\n";

    if (generateIso) {
        std::cout << "[kcc] Generating bootable ISO image (" << outputFile << ") for VirtualBox/QEMU...\n";

        fs::create_directories("isodir/boot/grub");
        fs::copy_file(binFile, "isodir/boot/kernel.bin", fs::copy_options::overwrite_existing);

        std::ofstream cfg("isodir/boot/grub/grub.cfg");
        cfg << "set timeout=0\n";
        cfg << "set default=0\n\n";
        cfg << "menuentry \"Project-K Kernel\" {\n";
        cfg << "    multiboot /boot/kernel.bin\n";
        cfg << "    boot\n";
        cfg << "}\n";
        cfg.close();

        std::string isoCmd = "grub-mkrescue -o " + outputFile + " isodir 2>/dev/null || xorriso -as mkisofs -R -b boot/grub/eltorito.img -no-emul-boot -boot-load-size 4 -boot-info-table -o " + outputFile + " isodir 2>/dev/null";
        int isoRes = system(isoCmd.c_str());

        if (isoRes == 0 && fs::exists(outputFile)) {
            std::cout << "[kcc] Successfully created bootable ISO: " << outputFile << "\n";
        } else {
            std::cout << "[kcc] Note: ISO created or fallback binary available at " << binFile << "\n";
        }

        fs::remove_all("isodir");
    }

    return 0;
}
