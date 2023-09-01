#include <iostream>
#include <fstream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    if (argc != 3) return 1;
    ifstream inputFile(argv[1]);
    ofstream outputFile(argv[2]);
    string currentLine;
    outputFile << "#ifndef XAEROTOJOURNEYMAP_STATETOCOLOR_H\n" <<
                  "#define XAEROTOJOURNEYMAP_STATETOCOLOR_H\n\n" <<
                  "#include <map>\n\n" <<
                  "static std::map<int, int> STATE_TO_COLOR = {\n";
    while (getline(inputFile, currentLine)) {
        outputFile << "    {" << currentLine << "},\n";
    }
    outputFile << "};\n\n";
    outputFile << "#endif //XAEROTOJOURNEYMAP_STATETOCOLOR_H\n";
    outputFile.close();
    inputFile.close();
}