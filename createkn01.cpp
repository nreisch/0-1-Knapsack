#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>

using namespace std;

struct Item {
    int p; // profit
    int w; // weight
    string name; // name

    Item(int p, int w)
    {
        this->p = p;
        this->w = w;
    }
};

int main(int argc, char **argv) {
    
    // Seed random
    srand ( time(NULL) );

    // Gets value between 0 and 4, + 4, brings range to 4 to 8
    int n = (rand() % 5) + 4;

    // Vector of items
    vector<Item> items;

    // Creating list of items
    for (int i = 0; i < n; i++) 
    {
        // Gets value between 0 and 20, + 10 brings between 10 to 30
        int p = (rand() % 21) + 10;

        // Gets value between 0 and 15, + 5, brings range to 5 to 20
        int w = (rand() % 16) + 5; 

        items.push_back(Item(p, w));
    }

    // Getting the total weight
    int totalW = 0;
    for(Item i: items )
    {
        totalW += i.w;
    }

    // Setting the W
    totalW = floor(.6 * totalW);

    // First get file to write to
    string fileName;
    bool filesUpcoming = false;

    for (int i = 0; i < argc; i++)
    {
        // Hit the -k flag
        if(strcmp(argv[i],"-k") == 0)
        {
            filesUpcoming = true;
        }
        else if(filesUpcoming)
        {
            fileName = argv[i];
        }

    }

    // Writing to a file
    ofstream file;
    file.open(fileName);

    // First line contains the number of tiems and totalW
    file << n << " " << totalW << endl;
    
    // Now loop through items and output to file
    
    for(int i = 0; i < n; i++)
    {
        file << "Item" << i+1 << " " << items[i].p << " " << items[i].w;
        // To not leave an ending new line
        if(i != n-1)
            file << endl;
    }

    // Close file
    file.close();

    return 0;
}