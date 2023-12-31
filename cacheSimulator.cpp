#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>

using namespace std;

int log_to(int a, int b) // To find log of a with base b
{
    if (a == 0)
    {
        return 0;
    }
    if (a == 1)
    {
        return 0;
    }
    else
    {
        return 1 + log_to(a / b, b);
    }
}

int convert_deci(string s)
{
    int n = 0;
    for (int i = 0; i < s.length(); i++)
    {
        if (s[i] >= '0' && s[i] <= '9')
        {
            n = n * 16 + (s[i] - '0');
        }
        else if (s[i] >= 'a' && s[i] <= 'f')
        {
            n = n * 16 + (s[i] - 'a' + 10);
        }
        else if (s[i] >= 'A' && s[i] <= 'F')
        {
            n = n * 16 + (s[i] - 'A' + 10);
        }
    }
    return n;
}

int cacheSize, blockSize, associativity;
string replacementPolicy, writeBackPolicy;
void readConfig()
{
    // Create an input file stream
    ifstream config;

    // Open the file
    config.open("cache.config");

    // Check if the file is open
    if (!config)
    {
        cout << "Unable to open file\n";
        return;
    }

    // Read the cache size
    string line;
    getline(config, line);
    cacheSize = stoi(line);

    // Read the block size
    getline(config, line);
    blockSize = stoi(line);

    // Read the associativity
    getline(config, line);
    associativity = stoi(line);

    // Read the replacement policy
    getline(config, line);
    replacementPolicy = line;

    // Read the write back policy
    getline(config, line);
    writeBackPolicy = line;

    // Close the file
    config.close();
}

int cache_stimulated(char mode, int index, int tag, vector<vector<int>> &cache, vector<vector<int>> &validity, vector<vector<int>> &dirty, vector<vector<int>> &replacement, int number_of_elements[]) // here index is the index of the address
{
    int val = 0;

    if (mode == 'R')
    {
        int hit = 0;
        for (int i = 0; i < associativity; i++)
        {
            if (cache[index][i] == tag && validity[index][i] == 1)
            {
                hit = 1;
                val = 1;

                if (replacementPolicy == "LRU")
                {
                    replacement[index][i] = number_of_elements[index];
                    for (int j = 0; j < associativity; j++)
                    {
                        if (replacement[index][j] != 0 && j != i)
                        {
                            replacement[index][j]--;
                        }
                    }
                }
                break;
            }
        }
        if (hit == 0)
        {
            val = 0;

            int flag = 0;
            for (int i = 0; i < associativity; i++)
            {
                if (validity[index][i] == 0)
                {
                    cache[index][i] = tag;
                    validity[index][i] = 1;
                    flag = 1;
                    number_of_elements[index]++;
                    if (replacementPolicy == "LRU")
                    {
                        replacement[index][i] = number_of_elements[index];
                    }
                    if (replacementPolicy == "FIFO")
                    {
                        replacement[index][i] = number_of_elements[index];
                    }
                    break;
                }
            }
            if (flag == 0)
            {
                if (replacementPolicy == "RANDOM")
                {
                    int random = rand() % associativity;
                    cache[index][random] = tag;
                    validity[index][random] = 1;
                }
                else
                {

                    int min = number_of_elements[index];
                    int min_index = 0;
                    for (int i = 0; i < associativity; i++)
                    {
                        if (replacement[index][i] <= min)
                        {
                            min = replacement[index][i];
                            min_index = i;
                        }
                        replacement[index][i]--;
                    }
                    cache[index][min_index] = tag;
                    validity[index][min_index] = 1;
                    if (replacementPolicy == "LRU")
                    {
                        replacement[index][min_index] = number_of_elements[index];
                    }
                    if (replacementPolicy == "FIFO")
                    {
                        replacement[index][min_index] = number_of_elements[index];
                    }
                }
            }
        }

        //  Myfile << ", Tag: 0x" << hex << tag << endl;
    }

    else if (mode == 'W')
    {
        int hit = 0;
        for (int i = 0; i < associativity; i++)
        {
            if (cache[index][i] == tag && validity[index][i] == 1)
            {
                hit = 1;
                val = 1;

                if (replacementPolicy == "LRU")
                {
                    replacement[index][i] = number_of_elements[index];
                    for (int j = 0; j < associativity; j++)
                    {
                        if (replacement[index][j] != 0 && j != i)
                        {
                            replacement[index][j]--;
                        }
                    }
                }
                dirty[index][i] = 1;

                break;
            }
        }
        if (hit == 0)
        {
            val = 0;

            if (writeBackPolicy != "WT")
            {
                int flag = 0;
                for (int i = 0; i < associativity; i++)
                {
                    if (validity[index][i] == 0)
                    {
                        cache[index][i] = tag;
                        validity[index][i] = 1;
                        flag = 1;
                        number_of_elements[index]++;
                        if (replacementPolicy == "LRU")
                        {
                            replacement[index][i] = number_of_elements[index];
                        }
                        if (replacementPolicy == "FIFO")
                        {
                            replacement[index][i] = number_of_elements[index];
                        }
                        dirty[index][i] = 1;

                        break;
                    }
                }
                if (flag == 0)
                {
                    if (replacementPolicy == "RANDOM")
                    {
                        int random = rand() % associativity;
                        cache[index][random] = tag;
                        validity[index][random] = 1;

                        dirty[index][random] = 1;
                    }
                    else
                    {

                        int min = number_of_elements[index];
                        int min_index = 0;
                        for (int i = 0; i < associativity; i++)
                        {
                            if (replacement[index][i] <= min)
                            {
                                min = replacement[index][i];
                                min_index = i;
                            }
                            replacement[index][i]--;
                        }
                        cache[index][min_index] = tag;
                        if (replacementPolicy == "LRU")
                        {
                            replacement[index][min_index] = number_of_elements[index];
                        }
                        if (replacementPolicy == "FIFO")
                        {
                            replacement[index][min_index] = number_of_elements[index];
                        }
                    }
                }
            }
        }
    }
    return val;
}

void accessAddress()
{
    int index;
    if (associativity != 0)
        index = (cacheSize / blockSize) / associativity;

    else
    {
        index = 1;
        associativity = (cacheSize / blockSize);
    }

    int index_bits = log_to(index, 2);
    int offset_bits = log_to(blockSize, 2);

    vector<vector<int>> cache(index, vector<int>(associativity, 0));
    vector<vector<int>> validity(index, vector<int>(associativity, 0));
    vector<vector<int>> dirtybit(index, vector<int>(associativity, 0));
    // vector<vector<int>> tag(index, vector<int>(associativity, 0));
    vector<vector<int>> replacement(index, vector<int>(associativity, 0));
    int number_of_elements[index];

    ifstream address;
    string line;
    address.open("cache.access");

    ofstream MyFile("output.txt");
    while (getline(address, line))
    {

        string x = line;

        char mode = x[0];
        x.erase(0, 5);
        MyFile << "Address: 0x" << x << ", Set: 0x";

        int add = convert_deci(x); // add is address here
        int tag = add >> (index_bits + offset_bits);

        int index_of_address = add >> offset_bits & ((1 << index_bits) - 1);
        MyFile << hex << index_of_address << ", ";
        int val = cache_stimulated(mode, index_of_address, tag, cache, validity, dirtybit, replacement, number_of_elements);
        string s;
        if (val == 1)
        {
            s = "Hit";
        }
        else
        {
            s = "Miss";
        }
        MyFile << s;
        MyFile << ", Tag: 0x" << hex << tag << endl;
    }
    MyFile.close();
}

int main()
{
    readConfig();
    accessAddress();
    return 0;
}