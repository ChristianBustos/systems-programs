#include <iostream>
#include <bitset>
#include <iomanip>

//print and separate binary16 componets
void printBinaryComponets(unsigned short int binary16) {
    unsigned short int sign = (binary16 & 0x8000) >> 15;
    unsigned short int exponent = (binary16 & 0x7C00) >> 10;
    unsigned short int significand = (binary16 & 0x03FF);
    std::cout << "Sign: " << sign << " Exponent: " << std::bitset<5>(exponent) << " Significand: " << std::bitset<10>(significand) << std::endl;
}

//validate hexadecimal input from user
bool validHexInput(const char input[]) {
    for (int i = 0; i < 4; i++) {
        char a = input[i];
        if (!(std::isdigit(a) || (a >= 'A' && a <= 'F') || (a >= 'a' && a <= 'f'))) {
            return false;
        }
    }
    return true;
}

//catch special cases based off sign, exponent, significand - output float value if not special case
std::string binary16toFLOAT(unsigned short int binary16, float & exitValue) {
    unsigned short int sign = (binary16 & 0x8000) >> 15;
    unsigned short int exponent = (binary16 & 0x7C00) >> 10;
    unsigned short int significand = (binary16 & 0x03FF);

    if (exponent == 0 && significand == 0) {
        return "Zero";
    }

    if (exponent == 0x1F) {
        if (significand == 0) {
            return sign ? "Negative Infinity" : "Positive Infinity";
        }
        else {
            std::cout << (sign ? "Negative Infinity\n" : "Positive Infinity\n");
            return "...significand should be zero.";
        }
    }
    
    if (exponent == 0) {
        return "Subnormal numbers are not allowed";
    }
    
    float result = static_cast<float>(pow(2.0, exponent - 15)) * (1.0f + significand / 1024.0f);
    exitValue = sign ? -result : result;
    return "Valid";


}

int main()
{
    char hexInput[5]; //4digit hex input + null terminator
    unsigned short int binaryValue;
    float floatValue;
    char userInput = 'N';
    
    do {
        while (true) {
            std::cout << "Enter four hexadecimal digits: \n";
            std::cin >> hexInput;
            std::cout << "\n";

            if (std::cin.fail() || std::strlen(hexInput) != 4 || !validHexInput(hexInput)) {
                std::cin.clear();
                std::cin.ignore(1000, '\n');
                std::cout << "Input characters must be in the ranges of 0-9, a-f, or A-F.\n";

                while (true) {
                    std::cout << "\nContinue? Enter Y or N: ";
                    std::cin >> userInput;
                    std::cout << "\n";

                    if (std::cin.fail() || (userInput != 'Y' && userInput != 'y' && userInput != 'N' && userInput != 'n')) {
                        std::cin.clear();
                        std::cin.ignore(1000, '\n');
                        std::cout << "Input must either Y or N.\n";
                        continue;
                    }
                    break;
                }

                if (userInput == 'N' || userInput == 'n') {
                    std::cout << "Done, program ending." << std::endl;
                    return 0;
                }

                continue;
            }
            break;
        }
    
    binaryValue = static_cast<unsigned short int>(strtoul(hexInput, nullptr, 16));
    
    //std::cout << "Hexadecimal Input: 0x" << std::hex << std::setw(4) << std::setfill('0') << binaryValue << std::endl;
    //std::cout << "Decimal Value: " << std::dec << binaryValue << std::endl;
    //std::cout << "Binary Representation: " << std::bitset<16>(binaryValue) << std::endl;
    printBinaryComponets(binaryValue);

    std::string specialCase = binary16toFLOAT(binaryValue, floatValue);
    if (specialCase == "Valid") {
        std::cout << "Value: " << std::setprecision(6) << floatValue << std::endl;
    }
    else {
        std::cout << specialCase << std::endl;
    }
while (true) {
    std::cout << "\nContinue? Enter Y or N: ";
    std::cin >> userInput;
    std::cout << "\n";

    if (std::cin.fail() || (userInput != 'Y' && userInput != 'y' && userInput != 'N' && userInput != 'n')) {
        std::cin.clear();
        std::cin.ignore(1000, '\n');
        std::cout << "Input must either Y or N.\n";
        continue;
    }
    break;
}

    } while (userInput == 'Y' || userInput == 'y');

    std::cout << "Done, program ending." << std::endl;

    return 0;
}
