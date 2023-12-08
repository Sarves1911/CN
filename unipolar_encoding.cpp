#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <graphics.h>

// Function to generate unipolar encoding
std::vector<double> generateUnipolarEncoding(int numBits)
{
    std::vector<double> encoding;
    int maxVal = pow(2, numBits) - 1;

    for (int i = 0; i <= maxVal; i++)
    {
        double value = static_cast<double>(i) / maxVal;
        encoding.push_back(value);
    }

    return encoding;
}

// Function to save the unipolar encoding data to a file
void saveDataToFile(const std::vector<double> &encoding)
{
    std::ofstream dataFile("unipolar_encoding_data.txt");

    for (const auto &value : encoding)
    {
        dataFile << value << std::endl;
    }

    dataFile.close();

    std::cout << "Unipolar encoding data saved to unipolar_encoding_data.txt." << std::endl;
}

int main()
{
    int numBits = 8; // Number of bits for the encoding
    std::vector<double> encoding = generateUnipolarEncoding(numBits);

    saveDataToFile(encoding);

    std::cout << "Unipolar encoding graph data generated successfully." << std::endl;

    return 0;
}
