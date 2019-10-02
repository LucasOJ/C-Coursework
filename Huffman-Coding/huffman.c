//This program generates a huffman coding for a given file
//Import standard libraries
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>

//Typedef and struct definitions
typedef unsigned char Byte;

struct Node{
    Byte bitPattern;
    int frequency;
    struct Node *left;
    struct Node *right;
    struct Node *next;
    unsigned int encoding;
    unsigned int encodingLength;
};
typedef struct Node Node;

struct NodeList{
    Node *prenode;
    int length;
};
typedef struct NodeList NodeList;

//Useful global constants
const int byteCountLength = 256;
const int byteLength = 8;

//Safely opens a file
//In case of user error displays the filename, error message and safely closes the program
FILE *fopenCheck(const char fileName[], char mode[]){
    FILE *p = fopen(fileName, mode);
    if (p != NULL) return p;
    fprintf(stderr, "Can't open %s\n", fileName);
    fflush(stderr);
    perror("");
    exit(1);
}

//Returns the length of a file
long fileLength(const char fileName[]){
    FILE *f = fopenCheck(fileName, "r");
    fseek(f, 0, SEEK_END);
    long length = ftell(f);
    fclose(f);
    return length;
}

//Imports a file into an array of bytes
void importFile(const char fileName[], const long length, Byte rawBytes[]){
    FILE *f = fopenCheck(fileName, "r");
    fread(rawBytes, length, 1, f);
    fclose(f);
}

//Counts the number of occurrences of individual bytes
void generateFreq(const long length, Byte rawBytes[length], int byteCounts[]){
    Byte currentByte;
    for (int i = 0; i < length; i++){
        currentByte = rawBytes[i];
        byteCounts[currentByte]++;
    }
}

//Allocates memory for a new node and returns a pointer
Node *newNode(Byte bitPattern, int frequency){
    Node *new = malloc(sizeof(Node));
    *new = (Node) {bitPattern, frequency, NULL, NULL, NULL, 0, 0};
    return new;
}

//Converts the array of frequencies into an array of pointers to nodes
//All bytes which never occurred (frequency == 0) are ignored and the NULL pointer is used instead
void freqToObjects(int byteCounts[], Node *nodeArray[]){
    for (int i = 0; i < byteCountLength; i++) {
        if (byteCounts[i] == 0) nodeArray[i] = NULL;
        else nodeArray[i] = newNode(i, byteCounts[i]);
    }
}

//Returns the number of Nodes
int countValidNodes(Node *nodeArray[]){
    int count = 0;
    for (int i = 0; i < byteCountLength; i++){
        if (nodeArray[i] != NULL) count++;
    }
    return count;
}

//Swaps two nodes in the node array
void swap(int i, int j, Node *nodeArray[]){
    Node *temp = nodeArray[i];
    nodeArray[i] = nodeArray[j];
    nodeArray[j] = temp;
}

//Partitions an array of nodes by frequency around a pivot 
int partition(int lo, int hi, Node *nodeArray[]){
    swap(lo + (hi -lo)/2, lo, nodeArray);
    int p = nodeArray[lo]->frequency;
    while(true){
        while(nodeArray[lo]->frequency < p) lo++;
        while(nodeArray[hi - 1]->frequency > p) hi--;
        if (hi - lo <= 1) return hi;
        swap(lo, hi-1, nodeArray);
        lo++;
        hi--;
    }
}

//Performs a quicksort on the node array
//Array could be up to 256 elements long so simpler sorts not suitable
void sort(int lo, int hi, Node *nodeArray[]){
    if (hi - lo <= 1) return;
    int split = partition(lo, hi, nodeArray);
    sort(lo, split, nodeArray);
    sort(split, hi, nodeArray);
}

//Allocates memory for a new NodeList object and returns a pointer
NodeList *newNodeList(){
    NodeList *nodeQueue = malloc(sizeof(NodeList));
    Node *prenode = malloc(sizeof(Node));
    nodeQueue->prenode = prenode;
    return nodeQueue;
}

//Frees the memory for the node list 
void freeNodeList(NodeList *nodeList){
    free(nodeList->prenode);
    free(nodeList);
}

//Links together the NodeList object and sorted nodes into a singly linked list
void populateList(int nodeQueueLength, Node *nodeArray[], NodeList *nodeQueue){
    nodeQueue->length = nodeQueueLength;
    nodeQueue->prenode->next = nodeArray[0];
    for (int i = 0; i < nodeQueueLength - 1; i++) nodeArray[i]->next = nodeArray[i+1];
}

//Creates a new node queue (singly linked list of nodes in acending order of frequency)
NodeList *newNodeQueue(Node *nodes[], int nodeQueueLength){
    Node *nodeArray[nodeQueueLength];
    int index = 0;
    for (int i = 0; i < byteCountLength; i++){
        if (nodes[i] != NULL){
            nodeArray[index] = nodes[i];
            index++;
        }
    }
    sort(0, nodeQueueLength, nodeArray);
    NodeList *nodeQueue = newNodeList();
    populateList(nodeQueueLength, nodeArray, nodeQueue);
    return nodeQueue;
}

//Removes a node from the nodeQueue (ordered singly linked list of nodes)
void removeListNode(Node *node, NodeList *nodeQueue){
    Node *prevNode = nodeQueue->prenode;
    while (prevNode->next != node) prevNode = prevNode->next;
    prevNode->next = prevNode->next->next;
}

//Adds a new node to the nodeQueue (ordered singly linked list of nodes)
//Inserts node in correct place in list to maintain acending order by frequency
void addListNode(Node *node, NodeList *nodeQueue){
    Node *prevNode = nodeQueue->prenode;
    Node *currentNode;
    while (prevNode->next != NULL){
        currentNode = prevNode->next;
        if (node->frequency < currentNode->frequency) break;
        else prevNode = prevNode->next;
    }
    
    Node *nextNode = prevNode->next;
    prevNode->next = node;
    node->next = nextNode;
}

//Creates a new internal node (a node which is not a leaf) for the Huffman Tree
Node *newListNode(Node *minFreqNode1, Node *minFreqNode2){
    Node *new = malloc(sizeof(Node));
    new->frequency = minFreqNode1->frequency + minFreqNode2->frequency;
    new->left = minFreqNode2;
    new->right = minFreqNode1;
    return new;
}

//Traverses the Huffman Tree recursively assigning all the nodes a coding
void generateNodeEncoding(Node *tree, unsigned int current){
    tree->encoding = current;
    if (tree->left != NULL) generateNodeEncoding(tree->left, (current << 1) | 0);
    if (tree->right != NULL) generateNodeEncoding(tree->right, (current << 1) | 1);
}

//Traverses the Huffman Tree recursively assigning all the nodes a coding length in bits
void generateEncodingLength(Node *tree, unsigned int current){
    tree->encodingLength = current;
    if (tree->left != NULL) generateEncodingLength(tree->left, current + 1);
    if (tree->right != NULL) generateEncodingLength(tree->right, current + 1);
}

//Generates a new Huffman Tree object
//Removes the 2 nodes with the smallest frequency from the front of the nodeQueue
//Creates a new internal node with a frequency which is the sum of the 2 nodes which have just been removed
//Adds this new internal node back onto the nodeQueue
//Repeat until only one node is remaining (the root node of the Huffman Tree)
Node *newHuffmanTree(NodeList *nodeQueue){
    Node *minFreqNode1;
    Node *minFreqNode2;
    Node *newNode;
    while (nodeQueue->length > 1){
        minFreqNode1 = nodeQueue->prenode->next;
        minFreqNode2 = minFreqNode1->next;
        newNode = newListNode(minFreqNode1, minFreqNode2);
        removeListNode(minFreqNode1, nodeQueue);
        removeListNode(minFreqNode2, nodeQueue);
        addListNode(newNode, nodeQueue);
        nodeQueue->length--;
    }
    Node *tree = nodeQueue->prenode->next;
    generateNodeEncoding(tree, 0);
    generateEncodingLength(tree, 0);
    return tree;
}

//Frees the memory of the Huffman Tree and all its nodes
void freeTree(Node *tree){
    if (tree->left != NULL) freeTree(tree->left);
    if (tree->right != NULL) freeTree(tree->right);
    free(tree);
}

//Displays a value to the terminal in binary
void printBinary(unsigned int value, int length){
    for (int i = 0; i < length; i++){
        printf("%d", (value >> (length - i - 1)) & 1);
    }
}

//Display a bit pattern and its corresponding Huffman coding
void displayNode(Node *node){
    printBinary(node->bitPattern, byteLength);
    printf(" -> ");
    printBinary(node->encoding, node->encodingLength);
    printf("\n");
}

//Traverse the Huffman Tree displaying the Huffman coding for each bit pattern (represented by a node)
void displayHuffmanTree(Node *tree){
    if (tree->left != NULL) displayHuffmanTree(tree->left);
    if (tree->right != NULL) displayHuffmanTree(tree->right);
    if (tree->left == NULL && tree->right == NULL) displayNode(tree);
}

//Calculates the compression ratio
int calculateCompressionRatio(Node *nodeArray[], int fLength, Byte rawBytes[]){
    int total = 0;
    for (int i = 0; i < fLength; i++) total += nodeArray[rawBytes[i]]->encodingLength;
    total = total / byteLength;
    if (total % byteLength == 0) total++;
    int ratio = (total * 100) / fLength;
    return ratio;
}

//Displays the compression ratio
void displayCompressionRatio(int compressionRatio){
    printf("The compressed file would be %d%% of its original size (with optimal bit packing and excluding the tree encoding table)\n", compressionRatio);
}

//Produce the Huffman coding for a given file
void huffmanEncoding(const char fileName[]){
    long fLength = fileLength(fileName);

    Byte rawBytes[fLength];
    importFile(fileName, fLength, rawBytes);

    int byteCounts[byteCountLength];
    for (int i = 0; i < byteCountLength; i++) byteCounts[i] = 0;
    generateFreq(fLength, rawBytes, byteCounts);

    Node *nodeArray[byteCountLength];
    freqToObjects(byteCounts, nodeArray);

    int nodeQueueLength = countValidNodes(nodeArray);
    NodeList *nodeQueue = newNodeQueue(nodeArray, nodeQueueLength);

    Node *tree = newHuffmanTree(nodeQueue);

    displayHuffmanTree(tree);
    int compressionRatio = calculateCompressionRatio(nodeArray, fLength, rawBytes);
    displayCompressionRatio(compressionRatio);

    freeTree(tree);
    freeNodeList(nodeQueue);
}

//Automates the assignment of the test values
void initTestByteCounts(int byteCounts[], int testCases, Byte bytes[], int frequencies[]){
    for (int i = 0; i < byteCountLength; i++) byteCounts[i] = 0;
    for (int i = 0; i < testCases; i++) byteCounts[bytes[i]] = frequencies[i];
}

//Simulates an entire Huffman coding given a set of bytes and their associated frequencies
//Tests individual functions at each stage
void runTests(){
    int byteCounts[byteCountLength];
    const int testCases = 4;
    Byte bytes[] = {'a', 'b', 'c', '\n'};
    int frequencies[] = {1, 4, 3, 1};
    initTestByteCounts(byteCounts, testCases, bytes, frequencies);
    
    Node *nodeArray[byteCountLength];
    freqToObjects(byteCounts, nodeArray);

    for(int i = 0; i < testCases; i++) {
        assert(nodeArray[bytes[i]]->frequency == frequencies[i]);
        assert(nodeArray[bytes[i]]->bitPattern == bytes[i]);
    }
    assert(nodeArray['e'] == NULL);

    int nodeQueueLength = countValidNodes(nodeArray);

    assert(nodeQueueLength == testCases);

    NodeList *nodeQueue = newNodeQueue(nodeArray, nodeQueueLength);

    Node *currentNode = nodeQueue->prenode->next;
    Byte sortedBytes[] = {'\n', 'a', 'c', 'b'};
    for (int i = 0; i < nodeQueue->length; i++){
        assert(currentNode->bitPattern == sortedBytes[i]);
        currentNode = currentNode->next;
    }

    Node *tree = newHuffmanTree(nodeQueue);

    int frequencySum = 0;
    for (int i = 0; i < testCases; i++) frequencySum += frequencies[i];
    
    assert(tree->frequency == frequencySum);

    assert(tree->left->left->bitPattern == 'c');
    assert(tree->left->left->encodingLength == 2);
    assert(tree->left->left->encoding == 0);

    assert(tree->right->bitPattern == 'b');
    assert(tree->right->encodingLength == 1);
    assert(tree->right->encoding == 1);
    
    assert(tree->left->right->left->bitPattern == 'a');
    assert(tree->left->right->left->encodingLength == 3);
    assert(tree->left->right->left->encoding == 2);
    
    assert(tree->left->right->right->bitPattern == '\n');
    assert(tree->left->right->right->encodingLength == 3);
    assert(tree->left->right->right->encoding == 3);

    freeTree(tree);
    freeNodeList(nodeQueue);

    printf("All tests passed\n");

}

//Entry point to the program
int main(int argNum, char *args[argNum]){
    setbuf(stdout,NULL);
    
    if (argNum == 1) runTests();
    else if (argNum == 2) huffmanEncoding(args[1]);
    
    return 0;
}
