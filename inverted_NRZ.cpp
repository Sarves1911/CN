#include <iostream>
#include <string>

using namespace std;

string encodeNRZInverted(const string &data)
{
    string encodedData = "";

    // The current state of the signal
    char signal = '-';

    for (char bit : data)
    {
        if (bit == '0')
        {
            encodedData += signal;
        }
        else if (bit == '1')
        {
            signal = (signal == '-') ? '+' : '-';
            encodedData += signal;
        }
    }

    return encodedData;
}

int main()
{
    string data;
    cout << "Enter the binary data: ";
    cin >> data;

    string encodedData = encodeNRZInverted(data);
    cout << "Encoded data: " << encodedData << endl;

    return 0;
}
