/*
 * File: project.cpp
 * Project: Final DS project
 * Programmers: KANI MUSTAFA & MANREET THIND
 * Date: 08/10/2024
 * Description: Implementation of a parcel management system using AVL Trees and Hash Tables.
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <cstdlib>

#define kHashTableSize 127
#define kCharCountry 22

 // the AVL tree node
struct Parcel
{
    char* destination;
    int weight;
    float valuation;
    Parcel* left, * right;
    int height;
};

// hashtable
struct HashTable
{
    Parcel* table[kHashTableSize];
};

/*
 * Function: height()
 * Parameter:
 * Parcel* N: pointer to the AVL tree node.
 * Return Value:
 * int: height of the node.
 * Description: function returns the height of the given node in the AVL tree
 */
int height(Parcel* N)
{
    if (N == NULL)
        return 0;
    return N->height;
}

/*
 * Function: max()
 * Parameter:
 * int a: first integer.
 * int b: second integer.
 * Return Value:
 * int: the maximum of the two integers.
 * Description: function returns the max of two integers
 */
int max(int a, int b)
{
    return (a > b) ? a : b;
}

/*
 * Function: newNode()
 * Parameters:
 * const char* destination: pointer to the destination string.
 * int weight: weight of the parcel.
 * float valuation: valuation of the parcel.
 * Return Value:
 * Parcel*: pointer to the newly created AVL tree node.
 * Description: this function creates and initializes a new AVL tree node with the given values.
 */
Parcel* newNode(const char* destination, int weight, float valuation)
{
    Parcel* node = new Parcel;
    node->destination = new char[strlen(destination) + 1];
    strcpy_s(node->destination, strlen(destination) + 1, destination);
    node->weight = weight;
    node->valuation = valuation;
    node->left = node->right = NULL;
    node->height = 1;
    return(node);
}

/*
 * Function: rightRotate()
 * Parameter:
 * Parcel* y: pointer to the subtree root node to be rotated.
 * Return Value:
 * Parcel*: pointer to the new root of the rotated subtree.
 * Description: function performs a right rotation on the subtree rooted with y
 */
Parcel* rightRotate(Parcel* y)
{
    Parcel* x = y->left;
    Parcel* T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = max(height(y->left), height(y->right)) + 1;
    x->height = max(height(x->left), height(x->right)) + 1;

    return x;
}

/*
 * Function: leftRotate()
 * Parameter:
 * Parcel* x: pointer to the subtree root node to be rotated.
 * Return Value:
 * Parcel*: pointer to the new root of the rotated subtree.
 * Description: function performs a left rotation on the subtree rooted with x
 */
Parcel* leftRotate(Parcel* x)
{
    Parcel* y = x->right;
    Parcel* T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = max(height(x->left), height(x->right)) + 1;
    y->height = max(height(y->left), height(y->right)) + 1;

    return y;
}

/*
 * Function: getBalance()
 * Parameter:
 * Parcel* N: pointer to the AVL tree node.
 * Return Value:
 * int: balance factor of the node.
 * Description: This function returns the balance factor of the given node in the AVL tree.
 */
int getBalance(Parcel* N)
{
    if (N == NULL)
        return 0;
    return height(N->left) - height(N->right);
}

/*
 * Function: insert()
 * Parameters:
 * Parcel* node: pointer to the AVL tree node where the new node will be inserted.
 * const char* destination: pointer to the destination string.
 * int weight: weight of the parcel.
 * float valuation: valuation of the parcel.
 * Return Value:
 * Parcel*: pointer to the root of the AVL tree after insertion.
 * Description: This function inserts a new node with the given values into the AVL tree and balances the tree.
 */
Parcel* insert(Parcel* node, const char* destination, int weight, float valuation) {

    // bst insertion
    if (node == NULL)
        return(newNode(destination, weight, valuation));

    if (weight < node->weight)
        node->left = insert(node->left, destination, weight, valuation);
    else if (weight > node->weight)
        node->right = insert(node->right, destination, weight, valuation);

    // or
    else
        return node;

    // height is updated now
    node->height = 1 + max(height(node->left), height(node->right));

    // node is balanced 
    int balance = getBalance(node);

    // left left Case
    if (balance > 1 && weight < node->left->weight)
        return rightRotate(node);

    // right right Case
    if (balance < -1 && weight > node->right->weight)
        return leftRotate(node);

    // left right Case
    if (balance > 1 && weight > node->left->weight) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // right left Case
    if (balance < -1 && weight < node->right->weight) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

/*
 * Function: hash()
 * Parameter:
 * const char* str: pointer to a C-style string to be hashed.
 * Return Value:
 * int: the index of the hash table bucket.
 * Description: This function computes a hash value for the input string using the djb2 algorithm.
 * Credit: The djb2 algorithm was created by Dan Bernstein
 */
unsigned long hash(const unsigned char* str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
    {
        hash = ((hash << 5) + hash) + c;
    }
    return hash % 127;
}

/*
 * Function: initHashTable()
 * Parameter:
 * HashTable* ht: pointer to the HashTable structure.
 * Return Value:
 * void
 * Description: this function initializes the hash table by setting all buckets to NULL
 */
void initHashTable(HashTable* ht)
{
    for (int i = 0; i < 127; i++)
    {
        ht->table[i] = NULL;
    }
}

/*
 * Function: insertParcel()
 * Parameters:
 * HashTable* ht: pointer to the HashTable structure.
 * const char* destination: pointer to the destination string.
 * int weight: weight of the parcel.
 * float valuation: valuation of the parcel.
 * Return Value:
 * void
 * Description: this function inserts a parcel into the hashtable
 */
void insertParcel(HashTable* ht, const char* destination, int weight, float valuation)
{
    unsigned long index = hash((const unsigned char*)destination);
    ht->table[index] = insert(ht->table[index], destination, weight, valuation);
}

/*
 * Function: loadData()
 * Parameters:
 * HashTable* ht: pointer to the HashTable structure.
 * const char* filename: name of the file to load data from.
 * Return Value:
 * void
 * Description: this function loads parcel data from a file and inserts it into the hashtable
 */
void loadData(HashTable* ht, const char* filename)
{
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }
    std::string line;
    int line_count = 0;  // line count for debugging
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string destination;
        int weight;
        float valuation;
        if (!(iss >> destination >> weight >> valuation))
        {
            continue; // skip lines with errors
        }
        insertParcel(ht, destination.c_str(), weight, valuation);
    }
    file.close();
}

/*
 * Function: inOrder()
 * Parameter:
 * Parcel* root: pointer to the root of the AVL tree.
 * Return Value:
 * void
 * Description: function does an in-order traversal of the AVL tree and presnets parcel details
 */
void inOrder(Parcel* root)
{
    if (root != NULL)
    {
        inOrder(root->left);
        std::cout << "Destination: " << root->destination << ", Weight: " << root->weight << ", Valuation: " << root->valuation << std::endl;
        inOrder(root->right);
    }
}

/*
 * Function: searchAndDisplay()
 * Parameters:
 * HashTable* ht: pointer to the HashTable structure.
 * const char* country: pointer to the country string.
 * Return Value:
 * void
 * Description: this function searches for a parcel by destination in the hash table and displays its details if found
 */
void searchAndDisplay(HashTable* ht, const char* country)
{
    unsigned long index = hash((const unsigned char*)country);
    if (ht->table[index] != NULL)
    {
        inOrder(ht->table[index]);
    }
    else
    {
        std::cout << "No parcels found (check capitalization!) for country " << country << "." << std::endl;
    }
}

/**
 * Function: searchByWeight()
 * Parameters:
 * Parcel* root: pointer to the root of the AVL tree.
 * int weight: the weight to compare against.
 * bool higher: boolean indicating whether to search for parcels with higher or lower weight.
 * Return Value:
 * void
 * Description: function searches the AVL tree for parcels with weights higher or lower than the mentioned weight and displays their detals
 */
bool searchByWeight(Parcel* root, int weight, bool higher)
{
    bool look = false;
    if (root != NULL)
    {
        look = searchByWeight(root->left, weight, higher);
        if ((higher && root->weight > weight) || (!higher && root->weight < weight))
        {
            std::cout << "Destination: " << root->destination << ", Weight: " << root->weight << ", Valuation: " << root->valuation << std::endl;
            look = true;
        }
        look = searchByWeight(root->right, weight, higher) || look;
    }
    return look;
}


/**
 * Function: calculateLoadAndValuation()
 * Parameters:
 * Parcel* root: pointer to the root of the AVL tree.
 * int* totalLoad: pointer to the integer storing the total weight of parcels.
 * float* totalValuation: pointer to the float storing the total valuation of parcels.
 * Return Value:
 * void
 * Description: function calculates the total weight and valuation of all parcels in the AVL tree
 */
void calculateLoadAndValuation(Parcel* root, int* totalLoad, float* totalValuation) {
    if (root != NULL)
    {
        *totalLoad += root->weight;
        *totalValuation += root->valuation;
        calculateLoadAndValuation(root->left, totalLoad, totalValuation);
        calculateLoadAndValuation(root->right, totalLoad, totalValuation);
    }
}

/**
 * Function: minValueNode()
 * Parameter:
 * Parcel* node: pointer to the AVL tree node to find the minimum value node from.
 * Return Value:
 * Parcel*: pointer to the node with the minimum value.
 * Description: this function finds and returns the node with the minimum weight in the AVL tre
 */
Parcel* minValueNode(Parcel* node)
{
    Parcel* current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

/*
 * Function: maxValueNode()
 * Parameter:
 * Parcel* node: pointer to the AVL tree node to find the maximum value node from.
 * Return Value:
 * Parcel*: pointer to the node with the maximum value.
 * Description: function finds and returns the node with the max weight in the AVL tree
 */
Parcel* maxValueNode(Parcel* node)
{
    Parcel* current = node;
    while (current->right != NULL)
        current = current->right;
    return current;
}

/*
 * Function: freeAVLTree()
 * Parameter:
 * Parcel* root: pointer to the root of the AVL tree.
 * Return Value:
 * void
 * Description: this function frees the memory allocated for the AVL tree nodes
 */
void freeAVLTree(Parcel* root)
{
    if (root != NULL)
    {
        freeAVLTree(root->left);
        freeAVLTree(root->right);
        delete[] root->destination;
        delete root;
    }
}

int main()
{
    HashTable ht;  // create an instance of the hash table
    initHashTable(&ht);  // initialize the hash table structure
    loadData(&ht, "couriers.txt");  // load parcel data from file

    int choice;  // variable to store the user's menu choice
    char country[kCharCountry];  // buffer to store the country name input
    int weight;  // variable to store the weight input

    while (true)
    {
        // display menu options for the user
        std::cout << "\nMenu:\n";
        std::cout << "1. Display all parcels for a country\n";
        std::cout << "2. Display parcels based on weight for a country\n";
        std::cout << "3. Display total parcel load and valuation for a country\n";
        std::cout << "4. Display cheapest and most expensive parcels for a country\n";
        std::cout << "5. Display lightest and heaviest parcels for a country\n";
        std::cout << "6. Exit the application\n";
        std::cout << "\nEnter your choice: ";
        std::cin >> choice;  // get user choice from input

        switch (choice)
        {
        case 1:
        {
            std::cout << "Enter country name: ";
            std::cin >> country;
            country[strcspn(country, "\n")] = '\0';

            // check if country name is empty
            if (strlen(country) == 0) {
                std::cout << "Error: Country name cannot be empty." << std::endl;
                break;
            }

            unsigned long index = hash((const unsigned char*)country);
            Parcel* countryParcels = ht.table[index];

            // display parcels if they exist for the given country
            if (countryParcels != NULL)
            {
                std::cout << "\n" << std::endl;
                std::cout << "Parcels for country " << country << ":" << std::endl;
                inOrder(countryParcels);  // display parcels in order
                std::cout << "\n" << std::endl;
            }
            else
            {
                std::cout << "No parcels found for country " << country << "." << std::endl;
            }
            break;
        }

        case 2:
        {
            std::cout << "Enter country name: ";
            std::cin >> country;
            country[strcspn(country, "\n")] = '\0';

            // check if country name is empty
            if (strlen(country) == 0) {
                std::cout << "Error: Country name cannot be empty." << std::endl;
                break;
            }

            std::cout << "Enter weight: ";
            if (!(std::cin >> weight) || weight < 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Error: Please enter a valid non-negative weight." << std::endl;
                break;
            }

            unsigned long index = hash((const unsigned char*)country);
            Parcel* countryParcels = ht.table[index];

            // display parcels based on weight if they exist for the given country
            if (countryParcels != NULL)
            {
                std::cout << "\nParcels with weight higher than " << weight << ":\n";
                bool found = searchByWeight(countryParcels, weight, true);
                if (!found) {
                    std::cout << "No parcels with weight higher than " << weight << " found." << std::endl;
                }

                std::cout << "\nParcels with weight lower than " << weight << ":\n";
                found = searchByWeight(countryParcels, weight, false);
                if (!found) {
                    std::cout << "No parcels with weight lower than " << weight << " found." << std::endl;
                }
            }
            else
            {
                std::cout << "No parcels found for country " << country << "." << std::endl;
            }

            break;
        }

        case 3:
        {
            std::cout << "Enter country name: ";
            std::cin >> country;
            country[strcspn(country, "\n")] = '\0';
            int totalLoad = 0;
            float totalValuation = 0.0f;

            unsigned long index = hash((const unsigned char*)country);
            Parcel* countryParcels = ht.table[index];

            // calculate the total load and valuation for the given country
            if (countryParcels != NULL)
            {
                calculateLoadAndValuation(countryParcels, &totalLoad, &totalValuation);

                std::cout << "\nTotal parcel load: " << totalLoad << " units\n";
                std::cout << "Total parcel valuation: $" << totalValuation << std::endl;
            }
            else
            {
                std::cout << "No parcels found for country " << country << "." << std::endl;
            }
            break;
        }

        case 4:
        {
            std::cout << "Enter country name: ";
            std::cin >> country;
            country[strcspn(country, "\n")] = '\0';

            unsigned long index = hash((const unsigned char*)country);
            Parcel* countryParcels = ht.table[index];
            Parcel* minParcel = minValueNode(countryParcels);
            Parcel* maxParcel = maxValueNode(countryParcels);

            // ensure parcels exist for the given country
            if (minParcel && maxParcel)
            {
                std::cout << "\nCheapest Parcel -> Destination: " << minParcel->destination
                    << ", Weight: " << minParcel->weight
                    << ", Valuation: $" << minParcel->valuation << std::endl;
                std::cout << "Most Expensive Parcel -> Destination: " << maxParcel->destination
                    << ", Weight: " << maxParcel->weight
                    << ", Valuation: $" << maxParcel->valuation << std::endl;
            }
            else
            {
                std::cout << "No parcels found for country " << country << "." << std::endl;
            }
            break;
        }

        case 5:
        {
            std::cout << "Enter country name: ";
            std::cin >> country;
            country[strcspn(country, "\n")] = '\0';

            unsigned long index = hash((const unsigned char*)country);
            Parcel* countryParcels = ht.table[index];
            Parcel* lightestParcel = minValueNode(countryParcels);
            Parcel* heaviestParcel = maxValueNode(countryParcels);

            // ensure parcels exist for the given country
            if (lightestParcel && heaviestParcel)
            {
                std::cout << "\nLightest Parcel -> Destination: " << lightestParcel->destination
                    << ", Weight: " << lightestParcel->weight << " units, "
                    << "Valuation: $" << lightestParcel->valuation << std::endl;
                std::cout << "Heaviest Parcel -> Destination: " << heaviestParcel->destination
                    << ", Weight: " << heaviestParcel->weight << " units, "
                    << "Valuation: $" << heaviestParcel->valuation << std::endl;
            }
            else
            {
                std::cout << "No parcels found for country " << country << "." << std::endl;
            }
            break;
        }

        case 6:
        {
            std::cout << "\nExiting the application." << std::endl;
            // free memory for each hash table entry
            for (int i = 0; i < kHashTableSize; ++i) {
                freeAVLTree(ht.table[i]);
            }
            return 0;
        }

        default:
        {
            std::cout << "Invalid choice. Please enter a valid option." << std::endl;
            break;
        }
        }
    }
    return 0;
}
