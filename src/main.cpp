#include <iostream>
#include <string>
#include <memory>

#include "converter.h"

using namespace std; // :3

int main(int argc, char* argv[]) {
    cout << argc << endl;
    if (argc < 3 || argc > 3) {
        cout << "incorrect amount of arguments, please use the format <input folder path> <output folder path>" << endl;
        return 1;
    }

    string inputFolder = argv[1];
    string outputFolder = argv[2];

    unique_ptr<Converter> program(new Converter(inputFolder, outputFolder));
    program->runConversion();

    return 0;
}
