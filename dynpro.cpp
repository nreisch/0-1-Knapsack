#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <string>
#include <algorithm>    // std::reverse
#include <math.h>

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
    Item(){};
};

struct Cell {
    vector<Item> items;
    Cell(){}
};

// Globals: useful to allow recursive call without including arguments continually
vector<Item> items;
int n;
int totalW;

// Function declarations
void dpKnapsack();
vector<vector<int>> computeWeights();

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

    // DP Algorithms
    dpKnapsack();

    return 0;

}
/**
 * DP Refinement Algorithm
 * Stores DP Table values only for specific values of W, not all values of W
 */
void dpKnapsack() {

    // Compute weights we should use for our table, as opposed to filling all weights
    // vector<int> weights = computeWeights();

    vector<vector<int>> weights(n);
    vector<int> row;

    // Returns a vector of rows that share with us each of the weights we need to compute for our rows
    // Refinement
    weights = computeWeights();

    ofstream entries2;
    
    entries2.open("entries2.txt");


    // Creates DP Table
    int ** dp = new int *[n+1];

    // Each cell holds a vector that contains the items included
    Cell ** cells = new Cell *[n+1];

    for (int i = 0; i <= n; i++)
    {
        dp[i] = new int[totalW+1];
        cells[i] = new Cell[totalW+1];
    }

    // Fill dp table of 0s
    for(int i = 0; i <= n; i++)
    {
        for(int j = 0; j <= totalW; j++)
        {
            if(i == 0)
                dp[i][j] = 0;
            if(j == 0)
                dp[i][j] = 0;
        }
    }

    // Each row considers a new item, and each column considers a new weight
    // 0 to i accounts for the number of items to consider
    for(int i = 1; i <= n; i++)
    {

        // Weight of the item and its profit
        int wi = items[i-1].w;
        int pi = items[i-1].p;

        entries2 << "Row" << i << " ";

        for(int count = 0; count < weights[i-1].size(); count++)
        {
            // We get the specific weight for our DP table
            int w = weights[i-1][count];
            

            // Weight, w, of the index we are considering
            if(wi <= w)
            {

                // dp[i][w] = max(dp[i-1][w], pi + dp[i-1][w-wi]);
                if(dp[i-1][w] <= (pi + dp[i-1][w-wi]) )
                {
                    // Update based on cell w-wi
                    dp[i][w] = pi + dp[i-1][w-wi];
                    cells[i][w].items = cells[i-1][w-wi].items;

                    // We append the newest item to our cell like we would add the profit from the newest current item
                    cells[i][w].items.push_back(items[i-1]);
                }
                else {
                    // We set the items in the cell i, which refers to the ith item to hold the items in row i-1
                    dp[i][w] = dp[i-1][w];

                    cells[i][w].items = cells[i-1][w].items;
                }

            }
            else if(wi > w)
            {
                dp[i][w] = dp[i-1][w];
                cells[i][w].items = cells[i-1][w].items;
            }

            // Output the optimal profits for this row
            entries2 << dp[i][w] << " ";
        }

        entries2 << endl;

    }

    // The optimal cell which holds vector of items in solution set
    Cell solutionOutput = cells[n][totalW];

    int wSolution = 0;

    // We get the total weight by summing items in solutionOutput
    for(Item i: solutionOutput.items)
    {
        wSolution += i.w;
    }

    // Total profit
    int pSolution = dp[n][totalW];

    // Write to outfile for output2.txt
    ofstream output2;
    output2.open("output2.txt");

    // First output # of items, the total profit, total weight,

    // We get the total weight by summing
    output2 << solutionOutput.items.size() << " " << pSolution << " " << wSolution << endl;

    int len = solutionOutput.items.size();

    // Now loop through items and output to file
    for(int i = 0; i < len; i++)
    {
        output2 << solutionOutput.items[i].name << " " << solutionOutput.items[i].p << " " << solutionOutput.items[i].w;

        // To not leave an ending new line
        if(i != len-1)
            output2 << endl;
    }

    output2.close();

    // Now write to entries

}
// At each row, we are calculating what the row below us needs based on current weights
// returns vector of rows of ints
// Part of the refinement algorithm
vector<vector<int>> computeWeights()
{
    vector<vector<int>> result(n);
    vector<int> row;

    // Nth row
    row.push_back(totalW);

    result[n-1] = row;

    int count = 1;

    // For each of the weights in the row, look at the last row added in results,
    while(count < n)
    {
        row.clear();

        for (int rowWeight : result[n-count])
        {
            // Add to a new row the w-weight of the item, and then the item
            if(rowWeight-items[n-count].w >= 0)
                row.push_back(rowWeight-items[n-count].w);
            if(rowWeight >= 0)
                row.push_back(rowWeight);
        }
        count++;

        // Sorts the rows of which to compute first
        std::sort(row.begin(), row.end());

        // Removes any duplicates
        row.erase(unique(row.begin(),row.end()),row.end());

        result[n-count] = row;
    }

    return result;
}