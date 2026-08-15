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
            // First check relative to baseDir (file location)
            fs::path relPath = fs::path(baseDir) / targetPath;
            if (fs::exists(relPath)) {
                resolvedPath = relPath;
            } else if (fs::exists(targetPath)) { // Check relative to working directory
                resolvedPath = targetPath;
            } else {
                resolvedPath = relPath; // Fallback for error messaging
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

        // Recursively process nested imports inside the imported .dk module
        processImports(importedProg.get(), currentModuleDir.string(), visitedImports);

        // Merge functions from imported module
        for (auto& fn : importedProg->functions) {
            mainProg->functions.push_back(std::move(fn));
        }

        // Merge top level statements from imported module
        for (auto& stmt : importedProg->topLevelStatements) {
            mainProg->topLevelStatements.push_back(std::move(stmt));
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "K Language Compiler (kcc) v1.0\n";
        std::cout << "Usage: " << argv[0] << " <input.k> [-o <output.bin>] [--arch <x86_64|x86_32|arm64|riscv64>]\n";
        return 0;
    }

    std::string inputFile;
    std::string outputFile = "kernel.bin";
    Arch targetArch = Arch::X86_64;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
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

    std::string asmFilename = outputFile + ".s";
    std::ofstream asmFile(asmFilename);
    asmFile << asmCode;
    asmFile.close();

    std::cout << "[kcc] Assembly generated: " << asmFilename << "\n";

    std::string asCmd = "as --64 " + asmFilename + " -o " + outputFile + ".o";
    std::string ldCmd = "ld -m elf_x86_64 --oformat binary -Ttext 0x100000 " + outputFile + ".o -o " + outputFile;

    std::cout << "[kcc] Compiling bare-metal executable...\n";
    int res1 = system(asCmd.c_str());
    int res2 = system(ldCmd.c_str());

    if (res1 == 0 && res2 == 0) {
        std::cout << "[kcc] Successfully compiled " << inputFile << " -> " << outputFile << "\n";
    } else {
        std::cerr << "[kcc] Warning: direct system assembly/linking completed with exit code " << (res1 | res2) << "\n";
    }

    return 0;
}
