#include <iostream>
#include <string>

using namespace std;

string encodeUnipolarNRZ(const string &data)
{
    string encodedData = "";

    for (char bit : data)
    {
        if (bit == '0')
        {
            encodedData += "0";
        }
        else if (bit == '1')
        {
            encodedData += "1";
        }
    }

    return encodedData;
}

int main()
{
    string data;
    cout << "Enter the binary data: ";
    cin >> data;

    string encodedData = encodeUnipolarNRZ(data);
    cout << "Encoded data: " << encodedData << endl;

    return 0;
}
