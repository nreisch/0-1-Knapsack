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

    Item(){}
};

// Custom comparator to sort item based on p/w
// Sorted in descending order of ratio
struct SortingItem
{
    inline bool operator() (const Item& i1, const Item& i2)
    {
        float i1PW = (float)i1.p / i1.w;
        float i2PW = (float)i2.p / i2.w;
        return (i1PW > i2PW);
    }
};

// Globals: useful to allow recursive call without including arguments continually
vector<Item> items;
int n;
int capacity;
int globalRowCount = 1;
ofstream entries3;

// Function declarations
void knapsackBacktracking(vector<Item> set, vector<vector<Item>>& solution, int pos, int currentProfit, int currentWeight, int & maxProfit);
bool promising(int i, int currentWeight, int currentProfit, int maxProfit );
float KWF2(int i, int weight, int profit);
void entryOutput(int currentWeight, int currentProfit, float bound);

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
            capacity = stoi(line.substr(0, line.size()));
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


    // Backtracking Algorithm
    vector<Item> subset;
    vector<vector<Item>> solution; // Just continually append the new best and the latest will be the answer
    int currentW = 0;
    int currentP = 0;
    int maxP = 0;

    // Open entries file at global level, to write to it in entryOutput function
    entries3.open("entries3.txt");

    // Sort items
    std::sort(items.begin(), items.end(), SortingItem());

    knapsackBacktracking(subset, solution, 0, currentW, currentP, maxP);

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
    writeFile.open("output3.txt");

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
    entries3.close();

    return 0;
}

// Modified DFS with pruning to decide whether or not to evaluate node
void knapsackBacktracking(vector<Item> set, vector<vector<Item>>& solution, int pos, int currentProfit, int currentWeight, int& maxProfit)
{
    // Check if we have a solution, if we do add to solution set
    if(currentWeight <= capacity && currentProfit > maxProfit)
    {
        // Copy items from subset to solution set
        solution.push_back(set);
        maxProfit = currentProfit;
    }

    if(promising(pos, currentWeight, currentProfit, maxProfit))
    {
        // Add to the subset the current item, update current W, and update currentP
        set.push_back(items[pos]);

        // Depth First Traversal in Knapsack
        knapsackBacktracking(set, solution, pos+1, currentProfit + items[pos].p, currentWeight + items[pos].w, maxProfit);

        // Remove node we just evaluated and once root is reached, will enter a new path
        set.pop_back();

        // Other half of preorder traversal, not including the item
        knapsackBacktracking(set, solution, pos+1, currentProfit, currentWeight, maxProfit);

    }
}

// Knapsack w/Fraction returns bound
float KWF2(int i, int weight, int profit)
{
    // Bound is currently profit
    float bound = profit;

    while(weight < capacity && i < n)
    {
        if(weight + items[i].w <= capacity)
        {
            weight = weight + items[i].w;
            bound = bound + items[i].p;
        }
        else {
            // Calculate bound based on remaining weight
            float partOfWeight = (capacity - weight);
            float partOfProfit = partOfWeight/items[i].w;
            bound = bound + (items[i].p * partOfProfit);

            // Set weight to the capacity
            weight = capacity;
        }
        i++;
    }
    return bound;
}

// Returns true if node is promising, false otherwise
bool promising(int i, int currentWeight, int currentProfit, int maxProfit )
{
    // Cannot get a solution by expanding node
    if(currentWeight >= capacity)
        return false;

    // Compute upper bound
    float bound = KWF2(i, currentWeight, currentProfit);

    // Output to entries 
    entries3 << globalRowCount++ << " " << currentProfit << " " << currentWeight << " " << bound << endl;

    return bound > maxProfit;
}