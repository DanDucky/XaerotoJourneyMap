#include <iostream>
#include <string>
#include <memory>
#include <CLIParser.hpp>

#include "converter.h"

using namespace std; // :3

int main(int argc, char* argv[]) {

    cli::Opt<string> inputFolder("Input Folder", "Xaero data folder", "i", "f");
    cli::Opt<string> outputFolder("Output Folder", "JM data folder", "o");
//    cli::Opt<bool> help("Help", "prints help info", "h", "help");
    cli::parse(argc, argv, inputFolder, outputFolder);

    Converter program(inputFolder, outputFolder);
    program.runConversion();

    return 0;
}
