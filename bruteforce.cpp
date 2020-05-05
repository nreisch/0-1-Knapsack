#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>

using namespace std;

// Class definitions
struct Item {
    int p; // profit
    int w; // weight
    string name; // name
    Item(int p, int w, string name)
    {
        this->p = p;
        this->w = w;
        this->name = name;
    }
};

// Globals: useful to allow recursive call without including arguments continually
vector<Item> items;
int n;
int totalW;

// Function declarations
void bruteforce(vector<Item>, vector<vector<Item>>&, int, int, int, int&);

int main(int argc, char **argv) {
    
    // Seed random
    srand ( time(NULL) );

    // First get file to read from
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

    // Reading from a file
    fstream readFile;
    readFile.open(fileName);
    string line;
    int lineNum = 0;

    while(getline(readFile, line))
    {
        // First Line, get the items and total count
        if(lineNum == 0)
        {
            // Get the index of first space
            size_t pos = line.find(' ');

            // Holds value from beginning to space
            n = stoi(line.substr(0, pos));

            // Erase the contents we just read
            line.erase(0, pos + 1);

            // Get the total weight from the rest
            totalW = stoi(line.substr(0, line.size()));
        }
        else
        {
             // Get the index of first space
            size_t pos = line.find(' ');

            // Get item name
            string name = line.substr(0, pos);

            // Erase the contents we just read
            line.erase(0, pos + 1);
            
            // Get the index of next tab to get profit
            pos = line.find(' '); 

            // Holds value from beginning to space
            int p = stoi(line.substr(0, pos));

            // Erase the contents we just read
            line.erase(0, pos + 1);

            // Get the total weight from the rest
           
            int w = stoi(line.substr(0, line.size()));
            
            // Save the item in a vector  
            items.push_back(Item(p, w, name));
        }
        
        lineNum++;
    }


    // Bruteforce Algorithm
    vector<Item> subset;
    vector<vector<Item>> solution; // Just continually append the new best and the latest will be the answer
    int currentW = 0;
    int currentP = 0;
    int maxP = 0;

    bruteforce(subset, solution, 0, currentW, currentP, maxP);

    // Compute the weight of the solution set
    // Compute the profit of the solution set
    int pSolution = 0;
    int wSolution = 0;

    for(Item i: solution[solution.size()-1])
    {
        pSolution += i.p;
        wSolution += i.w;
    }
    vector<Item> solutionOutput = solution[solution.size()-1];

    // Write to out file
    ofstream writeFile;

    writeFile.open("output1.txt");

    // First output # of items, the total profit, total weight, 
    writeFile << solutionOutput.size() << " " << pSolution << " " << wSolution << endl;
    
    int len = solutionOutput.size();
    
    // Now loop through items and output to file
    for(int i = 0; i < len; i++)
    {
        writeFile << solutionOutput[i].name << " " << solutionOutput[i].p << " " << solutionOutput[i].w;
        
        // To not leave an ending new line
        if(i != len-1)
            writeFile << endl;
    }
    
    // Close file
    readFile.close();
    writeFile.close();

    return 0;
}

/**
 * Bruteforce algorithm for knapsack
 * Performs in O(2^n) time by considering each subset of items
 * Does not perform any pruning / and or branch and bounding
 * Performs algorithm recursively, does not bracktrack if does not think branch is promising
 */
void bruteforce(vector<Item> subset, vector<vector<Item>>& solution, int pos, int currentW, int currentP, int& maxP)
{
    // Check if we have a solution, if we do add to solution set
    if(currentW <= totalW && currentP >= maxP)
    {
        // Copy items from subset to solution set
        solution.push_back(subset);
        maxP = currentP;
    }

    for(int i = pos; i < n; i++)
    {
        // Add to the subset the current item, update current W, and update currentP
        subset.push_back(items[i]);
        currentP += items[i].p;
        currentW += items[i].w;

        // Maintain reference to maxP, so that updated value stays present among all recursive calls
        bruteforce(subset, solution, i+1, currentW, currentP, maxP);

        // When we unwind the call stack and recurse back, the most recent item is removed, but because of our loop, we will continually try out the same values in the same order,
        // We don't want to try out the same item again in the same subset vector position, and to do so we pop_back() from the subset
        Item toRemove = subset.back();
        subset.pop_back();

        // The parameter of currentW, and currentP does not update as expected because although the values update after returning to previous call, when we popoff an item we didn't account for updating the W and P values,
        // Therefore, we need to make sure we reduce the current weight, and profit also
        currentW -= toRemove.w;
        currentP -= toRemove.p;

    }

}