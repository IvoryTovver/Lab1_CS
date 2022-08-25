#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string.h>
#include <map>
#include <cassert> 
#include <algorithm>
#include <filesystem>

using namespace std;

void printInfo(string filePath)
{
    ifstream fileStream = ifstream(filePath, std::ios::binary);

    assert(("File stream should be open", fileStream.good()));

    istreambuf_iterator<char> iterator(fileStream.rdbuf());

    string contents(iterator, istreambuf_iterator<char>());

    map<char, int> charsCount;
    int totalChars = contents.size();

    for (auto& i : contents)
    {
        charsCount[tolower(i)]++;
    }

    map<char, double> probablities;

    //обраховує частоти(імовірності) появи символів в тексті

    for (auto& i : charsCount)
    {
        probablities[i.first] = static_cast<double>(i.second) / totalChars;
    }

    //обраховує середню ентропію алфавіту для даного тексту

    double averageEntropy = 0;

    for (auto& i : probablities)
    {
        averageEntropy -= i.second * log2(i.second);
    }

    //виходячи з ентропії визначає кількість інформації та порівнює її з розмірами файлів

    double infoQuantity = averageEntropy * totalChars;

    //виводить на екран значення частот, ентропії та кількості інформації

    for (auto& i : probablities)
    {
        cout << "Char [" << i.first << "]: probability: " << i.second << endl;
    }

    cout << "Average entropy: " << averageEntropy << endl;

    cout << "Quantity of information: " << infoQuantity << " bits (" << infoQuantity / 8 << " bytes), file size is " << filesystem::file_size(filePath) << " bytes" << endl;
}

string makeBase64Alphabet()
{
    string alphabet;
    alphabet.reserve(64);

    for (char c = 'A'; c <= 'Z'; c++)
    {
        alphabet.push_back(c);
    }
    for (char c = 'a'; c <= 'z'; c++)
    {
        alphabet.push_back(c);
    }
    for (char c = '0'; c <= '9'; c++)
    {
        alphabet.push_back(c);
    }
    alphabet.push_back('+');
    alphabet.push_back('/');

    return alphabet;
}

void encodeToBase64(string filePath)
{
    ifstream fileStream = ifstream(filePath, std::ios::binary);

    assert(("File stream should be open", fileStream.good()));

    istreambuf_iterator<char> iterator(fileStream.rdbuf());

    string contents(iterator, istreambuf_iterator<char>());

    static string alphabet = makeBase64Alphabet();

    string encodedString;

    size_t currentIndex = 0;

    for (currentIndex = 0; currentIndex < (contents.size() / 3 * 3); currentIndex += 3)
    {
        int chunk = static_cast<unsigned char>(contents[currentIndex]) << 16
            | static_cast<unsigned char>(contents[currentIndex + 1]) << 8
            | static_cast<unsigned char>(contents[currentIndex + 2]);

        encodedString.push_back(alphabet[(chunk >> 18) & 0b111111]);
        encodedString.push_back(alphabet[(chunk >> 12) & 0b111111]);
        encodedString.push_back(alphabet[(chunk >> 6) & 0b111111]);
        encodedString.push_back(alphabet[(chunk >> 0) & 0b111111]);
    }

    size_t leftover = contents.size() - currentIndex;
    if (leftover > 0)
    {
        int chunk = 0;
        while (currentIndex < contents.size())
        {
            chunk <<= 8;
            chunk |= static_cast<unsigned char>(contents[currentIndex++]);
        }

        chunk <<= (3 - leftover) * 8;

        for (int i = 3; i >= (3 - leftover); i--)
        {
            encodedString.push_back(alphabet[(chunk >> (i * 6)) & 0b111111]);
        }

        for (size_t i = 0; i < 3 - leftover; i++)
        {
            encodedString.push_back('=');
        }
    }

    filesystem::path path(filePath);

    auto& newPath = path.replace_filename("encoded_" + path.filename().string());

    ofstream newFileStream(newPath);

    newFileStream << encodedString;

    cout << "Successfully encoded to " << newPath.string() << endl;
}

int main(int argc, char** argv) {
    SetConsoleOutputCP(CP_UTF8);
    setvbuf(stdout, nullptr, _IOFBF, 1000);

    string filePath;

    cout << "Enter file path (you can drag the file): ";

    cin >> filePath;

    cout << "Enter 1 for info, 2 for Base64 encoding: ";

    int option;

    cin >> option;

    if (option == 1)
    {
        printInfo(filePath);
    }
    else if(option == 2)
    {
        encodeToBase64(filePath);
    }
    else
    {
        cout << "Wrong option, terminating...";
    }
    
    system("pause");

	return 0;
}