#include <iostream>
#include <string>

using namespace std;

string encodePolarNRZ(const string &data)
{
    string encodedData = "";

    for (char bit : data)
    {
        if (bit == '0')
        {
            encodedData += 'L';
        }
        else if (bit == '1')
        {
            encodedData += 'H';
        }
    }

    return encodedData;
}

int main()
{
    string data;
    cout << "Enter the binary data: ";
    cin >> data;

    string encodedData = encodePolarNRZ(data);
    cout << "Encoded data: " << encodedData << endl;

    return 0;
}
