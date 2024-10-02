#include <cctype>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ostream>
#include <stdexcept>
#include <unordered_map>
#include <string>
#include <regex>

std::unordered_map<std::string, std::string> Comp = {
    {"0",    "101010"},
    {"1",    "111111"},
    {"-1",   "111010"},
    {"D",    "001100"},
    {"A",    "110000"},
    {"!D",   "001101"},
    {"!A",   "110001"},
    {"-D",   "001111"},
    {"-A",   "110011"},
    {"D+1",  "011111"},
    {"A+1", "110111"},
    {"D-1", "001110"},
    {"A-1", "110010"},
    {"D+A", "000010"},
    {"D-A", "010011"},
    {"A-D", "000111"},
    {"D&A", "000000"},
    {"D|A", "010101"},

    {"M",   "110000"},
    {"!M",  "110001"},
    {"-M",  "110011"},
    {"M+1", "110111"},
    {"M-1", "110010"},
    {"D+M", "000010"},
    {"D-M", "010011"},
    {"M-D", "000111"},
    {"D&M", "000000"},
    {"D|M", "010101"}
};

std::unordered_map<std::string, std::string> Dest = {
    {"null", "000"},
    {"M",    "001"},
    {"D",    "010"},
    {"DM",   "011"},
    {"A",    "100"},
    {"AM",   "101"},
    {"AD",   "110"},
    {"ADM",  "111"}
};

std::unordered_map<std::string, std::string> Jump = {
    {"null", "000"},
    {"JGT",  "001"},
    {"JEQ",  "010"},
    {"JGE",  "011"},
    {"JLT",  "100"},
    {"JNE",  "101"},
    {"JLE",  "110"},
    {"JMP",  "111"}
};

std::unordered_map<std::string, std::string> SymbolTable = {
    {"R0","0"},
    {"R1","1"},
    {"R2","2"},
    {"R3","3"},
    {"R4","4"},
    {"R5","5"},
    {"R6","6"},
    {"R7","7"},
    {"R8","8"},
    {"R9","9"},
    {"R10","10"},
    {"R11","11"},
    {"R12","12"},
    {"R13","13"},
    {"R14","14"},
    {"R15","15"},
    {"SCREEN","16384"},
    {"KBD","24576"},
    {"SP","0"},
    {"LCL","1"},
    {"ARG","2"},
    {"THIS","3"},
    {"THAT","4"},
};

void usage()
{
    std::cerr << "Usage: " << std::endl;
    exit(EXIT_FAILURE);
}

bool is_digits(const std::string& str)
{
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

std::string trim(const std::string& line)
{
    const char* WhiteSpace = " \t\v\r\n";
    std::size_t start = line.find_first_not_of(WhiteSpace);
    std::size_t end = line.find_last_not_of(WhiteSpace);
    return start == end ? std::string() : line.substr(start, end - start+1); 
}

int main(int argc, char** argv)
{
    std::regex fnameRegex("([A-Za-z0-9]+).asm");
    std::string fname(argv[1]);

    if(argc < 2)
        usage();



    if(!(std::regex_match(fname, fnameRegex)))
    {
        std::cerr << "Error invalid file type" << std::endl;
        exit(EXIT_FAILURE);
    }

    std::smatch temp;
    std::string outFile;
    if(std::regex_match(fname, temp, fnameRegex))
    {
        std::ssub_match temp2 = temp[1];    
        outFile = temp2.str();
    }

    std::ifstream file(argv[1]);
    std::ofstream osFile(outFile);
    std::string line;

    unsigned int lineNumber = 1;
    const std::regex symbolRegex("\\([A-Za-z0-9]*\\)");
    //Find symbols
    while(std::getline(file, line))
    {
        line = trim(line);
        if(line.starts_with("//"))
        {
            continue;
        }

        if(std::regex_match(line, symbolRegex))
        {
            std::string string = line.substr(1, line.size()-2);
            std::string out = std::bitset<16>(lineNumber+1).to_string();
            SymbolTable.insert({string, out});
            lineNumber++;
        }else {
            lineNumber++;
        }
    }

    file.close();
    file.open(argv[1]);

    unsigned int variables = 16;
    while(std::getline(file, line))
    {
        line = trim(line);
        std::string out;
        if(line.starts_with("//") || line.starts_with("\n"))
        {
            continue;
        }

        if(line[0] == '@')
        {
            std::size_t pos();
            std::string temp = line.substr(1, line.length()-1);

            if(SymbolTable.contains(temp))
            {
                out = SymbolTable.at(temp);
            }
            else
            {
                std::string t = std::bitset<16>(variables).to_string();
                SymbolTable.insert({temp, t});
                out = t;
            }
        } else if(line[0] == '('){
            std::string string = line.substr(1, line.size()-2);
            std::string out = std::bitset<16>(lineNumber+1).to_string();
            out = SymbolTable.at(string);
        } 
        else {
            const std::regex cInst("([A-Za-z0-9]*)=*([A-Za-z0-9+-]+);*([A-Za-z0-9+]*)");
            const std::regex dest("([A-Za-z0-9+-]+)=");
            const std::regex src("=([A-Za-z0-9+-]+)");
            const std::regex jump(";([A-Za-z0-9+]+)");
            std::smatch base_match;

            std::cout << line << std::endl;

            if(std::regex_match(line, base_match, cInst))
            {
                std::ssub_match base_match_dest = base_match[1];
                std::ssub_match base_match_src = base_match[2];
                std::ssub_match base_match_jump = base_match[3];

                std::string dest = base_match_dest.str();
                std::string src = base_match_src.str();
                std::string jump = base_match_jump.str();

                std::cout << dest << std::endl;
                std::cout << src << std::endl;
                std::cout << jump << std::endl;
            }
        }

        osFile << out << std::endl;
    }    
}
