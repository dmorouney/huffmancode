/*******************************************************************************/
/* CP312 Spring-Summer 2023                                                    */
/* Assignment 4(BONUS): Huffman Coding                                         */
/* Date: 2023/08/20                                                            */
/* Author: Robert Morouney                                                     */
/* Student Number: 069001422                                                   */
/* Email: moro1422@mylaurier.ca                                                */
/* Group : 17                                                                  */
/* Note this was a solo project and there should be no other members in my     */
/* group.                                                                      */
/*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define CHUNK_SIZE 4096
#define CONTROL_CHAR 255
#define ASCII_CHAR_COUNT 256

/* -------------------------------------------------------------------------- */
//HUFFMAN TREE FUNCTIONS:

typedef struct HuffmanTreeNode {
  uint8_t value;   // Character node value (8 bits)
  size_t weight;  // Frequency of the character (64 bits)
  struct HuffmanTreeNode* left; // left child
  struct HuffmanTreeNode* right; // right child
} HuffmanTreeNode;

HuffmanTreeNode *newNode(unsigned char value, size_t weight);
void writeTree(FILE* fp, HuffmanTreeNode* node);
HuffmanTreeNode* readTree(FILE* fp);
HuffmanTreeNode* buildRandomHuffmanTree();
HuffmanTreeNode* buildHuffmanTree(size_t* charCounts, size_t totalChars);
typedef HuffmanTreeNode** HuffmanStack;
void sortHuffmanStack(HuffmanStack nodes, int size);
void printTreeGraphiz(HuffmanTreeNode* root);
HuffmanTreeNode* treeFromFile(const char* filename);
void freeTree(HuffmanTreeNode* root);

/* -------------------------------------------------------------------------- */
// HUFFMAN QUEUE FUNCTIONS:

typedef struct HuffmanQueue {
  HuffmanTreeNode *node; // Huffman tree node
  struct HuffmanQueue *next; // Pointer to next node
} HuffmanQueue;

HuffmanQueue *newQueue(HuffmanTreeNode *node);
void enqueue(HuffmanQueue **queue, HuffmanTreeNode *node);
HuffmanTreeNode *dequeue(HuffmanQueue **queue);
void freeQueue(HuffmanQueue **queue);

/* -------------------------------------------------------------------------- */
//HUFFMAN TABLE FUNCTIONS: 

typedef struct HuffmanTableEntry {
  bool *code; // Huffman code
  size_t length; // Length of the code
} HuffmanTableEntry;

typedef struct HuffmanTable {
  HuffmanTableEntry* entries; // Array of HuffmanTableEntry
} HuffmanTable;

void createHuffmanCodes(HuffmanTreeNode* root, bool arr[], size_t len, 
                        HuffmanTable* table);
HuffmanTable* createHuffmanCodesTable(HuffmanTreeNode* root);
void printHuffmanTable(HuffmanTable* table);
void writeHuffmanTable(HuffmanTable* table, const char* filename);
void freeHuffmanTable(HuffmanTable* table);


/* -------------------------------------------------------------------------- */
//HUFFMAN UTILITY FUNCTIONS: 

#define N_CHARS 39
const int _CHARS[N_CHARS] = {0x20,0x2E,0x2C,0x30,0x31,0x32,0x33,0x34,0x35,
                              0x36,0x37,0x38,0x39,0x61,0x62,0x63,0x64,0x65,
                              0x66,0x67,0x68,0x69,0x6A,0x6B,0x6C,0x6D,0x6E,
                              0x6F,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,
                              0x78,0x79,0x7A};

size_t readAndCountChars(const char* filename, size_t* charCounts);
void writeFrequencyTable(const char* filename, size_t* charCounts);


/* -------------------------------------------------------------------------- */
//GENERAL UTILITY FUNCTIONS: 

void cleanFile(const char* inputFilename, const char* outputFilename);
void writeBit(FILE* fout, bool bit);

void encodeFile(const char* inputFilename, const char* outputFilename, 
                HuffmanTable* table);
void decode(const char* inputFilename, const char* outputFilename, 
            const char* treeFilename);

/* -------------------------------------------------------------------------- */
//MAIN PROGRAM:

/*****************************************************************************
 * Function: main
 * Description: Main program encodes a file whose name is read from stdin
 *             using Huffman coding.
 * Inputs: N/A
 * Outputs: Writes files to disk:
 *          compressed.bin - the encoded file
 *          tree.txt - the Huffman tree
 *          frequency.txt - the frequency table
 *          codes.txt - the Huffman codes table
 *          .clean.tmp - the cleaned file (removed after encoding)
 * Time Complexity: unknown
 * Space Complexity: unknown
 * Usage: ./encode
 *****************************************************************************/
int main(int argc, char** argv) {
  
  // Get the filename from stdin 
  char filename[ASCII_CHAR_COUNT];
  printf("Enter the filename: ");
  scanf("%s", filename);

  // Clean the file
  cleanFile(filename, ".clean.tmp");

  // Build the Huffman tree
  HuffmanTreeNode* huffmanTree = treeFromFile(".clean.tmp");
  
  // Write the tree to a file
  FILE* fp;
  fp = fopen("tree.txt", "wb");
  writeTree(fp, huffmanTree);
  fclose(fp);
  
  HuffmanTable* table = createHuffmanCodesTable(huffmanTree);
  writeHuffmanTable(table, "codes.txt");
  
  // Encode the file
  encodeFile(".clean.tmp", "compressed.bin", table);
  // Remove the cleaned file to save space
  remove(".clean.tmp");
  freeHuffmanTable(table);
  freeTree(huffmanTree);
  

  return 0;
}

/* -------------------------------------------------------------------------- */
//HUFFMAN TREE FUNCTION DEFINITIONS: 

/*****************************************************************************
 * Function: newNode
 * Description: Creates a new Huffman tree node
 * Inputs: value (char) - the character value of the node
 *         weight (size_t) - the frequency of the character
 * Outputs: A pointer to the new node (HuffmanTreeNode*)
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 * Usage: HuffmanTreeNode* node = newNode('a', 99);
 *****************************************************************************/
HuffmanTreeNode *newNode(unsigned char value, size_t weight) {
  HuffmanTreeNode *node = (HuffmanTreeNode *)malloc(sizeof(HuffmanTreeNode));
  node->value = value;
  node->weight = weight;
  node->left = NULL;
  node->right = NULL;
  return node;
}

/*****************************************************************************
 * Function: writeTree
 * Description: Writes a Huffman tree to a file
 * Inputs: fp (FILE*) - the file pointer to write to
 *         node (HuffmanTreeNode*) - the root of the tree to write to the file 
 * Outputs: None
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Usage: writeTree(fp, root);
 *****************************************************************************/
void writeTree(FILE* fp, HuffmanTreeNode* node) {
    if (node == NULL) {
      return;
    }
    fwrite(&node->value, sizeof(uint8_t), 1, fp);
    fwrite(&node->weight, sizeof(size_t), 1, fp);
    writeTree(fp, node->left);
    writeTree(fp, node->right);
}


/*****************************************************************************
 * Function: readTree
 * Description: Reads a Huffman tree from a file
 * Inputs: fp (FILE*) - the file pointer to read from
 * Outputs: A pointer to the root of the tree (HuffmanTreeNode*)
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Usage: HuffmanTreeNode* root = readTree(fp);
 *****************************************************************************/
HuffmanTreeNode* readTree(FILE* fp) {
    uint8_t value;
    size_t weight;
    fread(&value, sizeof(uint8_t), 1, fp);
    fread(&weight, sizeof(size_t), 1, fp);
    HuffmanTreeNode* node = newNode(value, weight);
    if (value != CONTROL_CHAR) {
      return node;
    }
    node->left = readTree(fp);
    node->right = readTree(fp);
    return node;
}

/*****************************************************************************
 * Function: buildRandomHuffmanTree
 * Description: Builds a random Huffman tree for testing
 * Inputs: None
 * Outputs: A pointer to the root of the tree (HuffmanTreeNode*)
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 * Usage: HuffmanTreeNode* root = buildRandomHuffmanTree();
 *****************************************************************************/
HuffmanTreeNode* buildRandomHuffmanTree() {
    HuffmanTreeNode* root = newNode('a', 99);
    root->left = newNode('b', 43);
    root->right = newNode('c', 34);
    root->left->left = newNode('d', 29);
    root->left->right = newNode('e', 23);
    root->right->left = newNode('f', 19);
    root->right->right = newNode('g', 7);
    return root;
}

/*****************************************************************************
 * Function: buildHuffmanTree 
 * Description: Builds a Huffman tree from the character counts
 * Inputs: charCounts (size_*) - an array of character counts
 *         totalChars (size_t) - the total number of characters
 * Outputs: A pointer to the root of the tree (HuffmanTreeNode*)
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 * Usage: HuffmanTreeNode* root = buildHuffmanTree(charCounts, totalChars);
 *****************************************************************************/
HuffmanTreeNode* buildHuffmanTree(size_t* charCounts, size_t totalChars) {
  if (totalChars == 0) {
    return NULL;
  }

  HuffmanTreeNode* tree = (HuffmanTreeNode*)malloc(sizeof(HuffmanTreeNode));

  // Initialize the stack to be empty
  HuffmanTreeNode* stack[ASCII_CHAR_COUNT];
  size_t stackSize = 0;
  
  // Push all nodes with non-zero weight onto the stack
  for (size_t i = 0; i < ASCII_CHAR_COUNT; i++) {
    if (charCounts[i] > 0) {
      HuffmanTreeNode* node = newNode(i, charCounts[i]);
      stack[stackSize++] = node;
    }
  }
  
  // Sort the stack
  sortHuffmanStack(stack, stackSize);

  size_t i = stackSize - 1;
  while (stackSize > 1) {
    // Pop the top two nodes off the stack
    HuffmanTreeNode* node1 = stack[i];
    i -= 1;
    HuffmanTreeNode* node2 = stack[i];

    // Create a new node with the sum of the weights of the two nodes
    HuffmanTreeNode* CombineNode = newNode(CONTROL_CHAR, 
                                           node1->weight + node2->weight);
    CombineNode->left = node1;
    CombineNode->right = node2;

    // Push the new node onto the stack
    stack[i] = CombineNode;
    stack[i+1] = NULL;
    stackSize -= 1;

    // Re-sort the stack
    sortHuffmanStack(stack, stackSize);
  }

  tree = stack[0];
  return tree;
}

/*****************************************************************************
 * Function: sortHuffmanStack
 * Description: Sorts an array of Huffman tree nodes in descending order based
 *              on the weight of the node
 * Inputs: nodes (HuffmanStack) - the array of nodes to sort
 *        size (int) - the number of nodes in the array
 * Outputs: None
 * Time Complexity: O(n^2)
 * Space Complexity: O(1)
 * Usage:
 *****************************************************************************/
void sortHuffmanStack(HuffmanStack nodes, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size - 1 - i; j++) {
            if ((nodes[j]->weight < nodes[j + 1]->weight) || 
                (nodes[j]->weight == nodes[j + 1]->weight && 
                nodes[j]->value < nodes[j + 1]->value)) {
                HuffmanTreeNode* temp = nodes[j];
                nodes[j] = nodes[j + 1];
                nodes[j + 1] = temp;
            }
        }
    }
}

/*****************************************************************************
 * Function: printTreeGraphiz
 * Description: Prints a Huffman tree in a graphiz format for debugging
 * Inputs: root (HuffmanTreeNode) - the root of the tree to print
 * Outputs: None
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 * Usage: printTreeGraphiz(root);
 * Notes: To use the output with graphiz, pipe the output of this function to
 *        graphiz by running the following command:
 *        `./mybinary | dot -Tpng -o tree.png tree.dot`
 *****************************************************************************/
void printTreeGraphiz(HuffmanTreeNode* root) {
  HuffmanQueue *queue = NULL;
  enqueue(&queue, root);
  uint8_t nodeID = 0;
  uint8_t childID = 1;
  printf("digraph g {\n");
  printf("node [shape=record, height=.1];\n");
  while (queue != NULL) {
    HuffmanTreeNode *node = dequeue(&queue);
    if (node == NULL) {
      continue;
    }
    if (node->value == CONTROL_CHAR) {
      printf("node%u [label=\"<f0> 0|<f1> %zu|<f2> 1\", color=black];\n", 
             nodeID, node->weight);
    } else {
      if(node->value <= 0x20){
        printf("node%u [label=\"<f1> 0x%x (%zu)\", color=green, style=filled];\n", 
               nodeID, node->value, node->weight);
      } else {
        printf("node%u [label=\"<f1> \'%c\' (%zu)\", color=green, style=filled];\n", 
               nodeID, node->value, node->weight);
      }
    }

    if (node->left != NULL) {
      printf("\"node%u\":f0 -> \"node%u\":f1;\n", nodeID, childID);
      enqueue(&queue, node->left);
      childID++;
    }
    if (node->right != NULL) {
      printf("\"node%u\":f2 -> \"node%u\":f1;\n", nodeID, childID);
      enqueue(&queue, node->right);
      childID++;
    }
    nodeID++;
  }
  printf("}\n");
  freeQueue(&queue);
}


/*****************************************************************************
 * Function: treeFromFile 
 * Description: Creates a Huffman tree from a file
 * Inputs: filename (const char*)- the name of the file to read
 * Outputs: A pointer to the root of the tree created (HuffmanTreeNode*)
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 * Usage: HuffmanTreeNode* root = treeFromFile("myfile.txt");
 *****************************************************************************/
HuffmanTreeNode* treeFromFile(const char* filename) {
    size_t charCounts[ASCII_CHAR_COUNT] = {0};
    size_t totalChars = readAndCountChars(filename, charCounts);
    writeFrequencyTable("frequency.txt", charCounts);
    HuffmanTreeNode* huffmanTree = buildHuffmanTree(charCounts, totalChars);
    return huffmanTree;
}

/*****************************************************************************
 * Function: freeTree
 * Description: Frees all memory associated with a Huffman tree
 * Inputs: root (HuffmanTreeNode)- the root of the tree to free
 * Outputs: None
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 * Usage: freeTree(root);
 *****************************************************************************/
void freeTree(HuffmanTreeNode* root) {
    if (root == NULL) {
        return;
    }
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

/* -------------------------------------------------------------------------- */
//HUFFMAN QUEUE FUNCTION DEFINITIONS: 

/*****************************************************************************
 * Function: newQueue
 * Description: Creates a new node in the Huffman queue
 * Inputs: node (HuffmanTreeNode)- the node to add to the queue
 * Outputs: A pointer to the new node (HuffmanQueue*)
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 * Usage: HuffmanQueue *queue = newQueue(node);
 *****************************************************************************/
HuffmanQueue *newQueue(HuffmanTreeNode *node) {
  HuffmanQueue *queue = (HuffmanQueue *)malloc(sizeof(HuffmanQueue));
  queue->node = node;
  queue->next = NULL;
  return queue;
}

/*****************************************************************************
 * Function: enqueue 
 * Description: Adds a node to the Huffman queue
 * Inputs: queue (HuffmanQueue**)- the queue to add the node to
 *         node (HuffmanTreeNode*)- the node to add to the queue
 * Outputs: None
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Usage: enqueue(&queue, node);
 *****************************************************************************/
void enqueue(HuffmanQueue **queue, HuffmanTreeNode *node) {
  HuffmanQueue *new = newQueue(node);
  if (*queue == NULL) {
    *queue = new;
    return;
  }
  HuffmanQueue *temp = *queue;
  while (temp->next != NULL) {
    temp = temp->next;
  }
  temp->next = new;
}


// Function to remove a node from the Huffman queue
/*****************************************************************************
 * Function: dequeue
 * Description: Removes a node from the Huffman queue
 * Inputs: queue (HuffmanQueue**)- the queue to remove the node from
 * Outputs: The node removed from the queue (HuffmanTreeNode*)
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 * Usage: HuffmanTreeNode *node = dequeue(&queue);
 *****************************************************************************/
HuffmanTreeNode *dequeue(HuffmanQueue **queue) {
  if (*queue == NULL) {
    return NULL;
  }
  HuffmanQueue *temp = *queue;
  *queue = (*queue)->next;
  HuffmanTreeNode *node = temp->node;
  free(temp);
  return node;
}

// Function to free the memory allocated for the Huffman queue
/*****************************************************************************
 * Function: freeQueue
 * Description: Frees all memory associated with the Huffman queue
 * Inputs: queue (HuffmanQueue**)- the queue to free
 * Outputs: None
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Usage: freeQueue(&queue);
 *****************************************************************************/
void freeQueue(HuffmanQueue **queue) {
  while (*queue != NULL) {
    dequeue(queue);
  }
}

/* -------------------------------------------------------------------------- */
//HUFFMAN TABLE FUNCTION DEFINITIONS: 

/*****************************************************************************
 * Function: createHuffmanTable
 * Description: Creates a Huffman table from a Huffman tree
 * Inputs: root (HuffmanTreeNode*)- the root of the Huffman tree
 *         arr (bool[])- the array of booleans to create the Huffman codes
 *         len (size_t)- the length of the Huffman code
 *         table (HuffmanTable*)- the Huffman table to create
 * Outputs: A pointer to the Huffman table created (HuffmanTable*)
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 * Usage: HuffmanTable *table = createHuffmanTable(root);
 *****************************************************************************/
void createHuffmanCodes(HuffmanTreeNode* root, bool arr[], size_t len, 
                        HuffmanTable* table) {
  if (root->left) {
    arr[len] = 0;
    createHuffmanCodes(root->left, arr, len + 1, table);
  }

  if (root->right) {
    arr[len] = 1;
    createHuffmanCodes(root->right, arr, len + 1, table);
  }

  if (root->left == NULL && root->right == NULL) {
    unsigned char idx = root->value;
    table->entries[idx].length = len;
    table->entries[idx].code = (bool*)calloc(len, sizeof(bool));
    for (size_t i = 0; i < len; i++) {
      table->entries[idx].code[i] = arr[i];
    }
  }
}

/*****************************************************************************
 * Function: createHuffmanCodesTable
 * Description: Creates a Huffman table from a Huffman tree
 * Inputs: root (HuffmanTreeNode*)- the root of the Huffman tree
 * Outputs: A pointer to the Huffman table created (HuffmanTable*)
 * Time Complexity: O(n)
 * Space Complexity: O(n)
 * Usage: HuffmanTable *table = createHuffmanCodesTable(root);
 *****************************************************************************/
HuffmanTable* createHuffmanCodesTable(HuffmanTreeNode* root) {

  // Create a table with the number of chars
  HuffmanTable* table = (HuffmanTable*)malloc(sizeof(HuffmanTable));
  table->entries = (HuffmanTableEntry*)calloc(ASCII_CHAR_COUNT, 
                                              sizeof(HuffmanTableEntry));
   
  // Create the table by traversing the tree
  bool arr[ASCII_CHAR_COUNT];
  size_t top = 0;
  createHuffmanCodes(root, arr, top, table);
  return table;

}

/*****************************************************************************
 * Function: printHuffmanTable 
 * Description: Prints the Huffman table
 * Inputs: table (HuffmanTable*)- the Huffman table to print
 * Outputs: None
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Usage: printHuffmanTable(table);
 *****************************************************************************/
void printHuffmanTable(HuffmanTable* table) {
  for (int i = 1; i < ASCII_CHAR_COUNT; i++) {
    if (table->entries[i].length != 0) {
      if(i <= 0x20) {
        printf("[0x%X]: ", i);
      } else {
        printf("'%c': ", i);
      }
      for (int j = 0; j < table->entries[i].length; j++) {
         printf("%d", table->entries[i].code[j]);
      }
      printf("\n");
    }
  }
}

/*****************************************************************************
 * Function: writeHuffmanTable
 * Description: Writes the Huffman table to the file "codes.txt"
 *              with the format [char]:[code]
 * Inputs: table (HuffmanTable*)- the Huffman table to write
 *         filename (const char*)- the name of the file to write to
 * Outputs: None (writes to file)
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Usage: writeHuffmanTable(table, file);
 *****************************************************************************/
void writeHuffmanTable(HuffmanTable* table, const char* filename) {
  FILE* file = fopen(filename, "w");
  for (int i = 0; i < N_CHARS; i++) {
    fprintf(file, "%c:", _CHARS[i]);
    for (int j = 0; j < table->entries[_CHARS[i]].length; j++) {
       fprintf(file, "%d", table->entries[_CHARS[i]].code[j]);
    }
    fprintf(file, "\n");
  }
  fclose(file);
}

/*****************************************************************************
 * Function: freeHuffmanTable
 * Description: Frees all memory associated with a Huffman table
 * Inputs: table (HuffmanTable*)- the Huffman table to free
 * Outputs: None
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Usage: freeHuffmanTable(table);
 *****************************************************************************/
void freeHuffmanTable(HuffmanTable* table) {
  for (int i = 0; i < ASCII_CHAR_COUNT; i++) {
    if (table->entries[i].length != 0) {
      free(table->entries[i].code);
    }
  }
  free(table->entries);
  free(table);
}

/* -------------------------------------------------------------------------- */
//UTILITY FUNCTION DEFINITIONS: 


/*****************************************************************************
 * Function: readAndCountChars 
 * Description: Reads a file and counts the number of occurrences of each char
 *              in the file
 * Inputs: filename (const char*)- the name of the file to read
 *         charCounts (size_t*)- the array to store the char counts in
 * Outputs: The total number of chars read (size_t)
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Usage: size_t totalChars = readAndCountChars(filename, charCounts);
 *****************************************************************************/
size_t readAndCountChars(const char* filename, size_t* charCounts) {
  FILE* fp = fopen(filename, "r");
  if (fp == NULL) {
    return 0;
  }

  unsigned char buffer[CHUNK_SIZE];
  size_t totalChars = 0;
  while (true) {
    size_t bytesRead = fread(buffer, sizeof(unsigned char), CHUNK_SIZE, fp);
    if (bytesRead == 0) {
      break;
    }

    for (size_t i = 0; i < bytesRead; i++) {
      charCounts[buffer[i]]++;
      totalChars++;
    }
  }

  fclose(fp);
  return totalChars;
}

/*****************************************************************************
 * Function: writeFrequencyTable 
 * Description: Writes the frequency table to a file
 * Inputs: filename (const char*)- the name of the file to write to
 *         charCounts (size_t*)- the array containing the char counts
 * Outputs: None
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Usage: writeFrequencyTable(filename, charCounts);
 *****************************************************************************/
void writeFrequencyTable(const char* filename, size_t* charCounts) {
  FILE* fp = fopen(filename, "w");
  if (fp == NULL) {
    printf("Error opening file %s\n", filename);
    exit(1);
  }

  for (int i = 0; i < N_CHARS; i++) {
      fprintf(fp, "%c:%zu\n", _CHARS[i], charCounts[_CHARS[i]]);
  }

  fclose(fp);
}

/*****************************************************************************
 * Function: cleanFile
 * Description: Cleans a file by converting all chars to lowercase and
 *              converting all whitespace chars to a regular spaces
 * Inputs: inputFilename (const char*)- the name of the file to read
 *         outputFilename (const char*)- the name of the file to write to
 * Outputs: None (writes to a file)
 * Time Complexity: O(n)
 * Space Complexity: O(1)
 * Usage: cleanFile(inputFilename, outputFilename);
 *****************************************************************************/
void cleanFile(const char* inputFilename, const char* outputFilename) {
  FILE* fin = fopen(inputFilename, "r");
  FILE* fout = fopen(outputFilename, "w");

  unsigned char buffer[CHUNK_SIZE];
  size_t totalChars = 0;
  while (true) {
    size_t bytesRead = fread(buffer, sizeof(unsigned char), 
                             CHUNK_SIZE, fin);
    if (bytesRead == 0) {
      break;
    }
    
    // Convert all chars to lowercase
    for (size_t i = 0; i < bytesRead; i++) {
      char idx = buffer[i];
      if (idx >= 0x41 && idx <= 0x5A) {
        idx += 0x20;
      }

      // Convert all whitespace chars to spaces
      if(idx >= 0x9 && idx <= 0x20) { idx = 0x20;} 
  
      // Only write the char if it is a space, a comma, a period, 
      // a number, or a letter (i.e. ignore all other chars)
      if (idx == 0x20 || idx == 0x2E || idx == 0x2C || 
          (idx >= 0x30 && idx <= 0x39) || 
          (idx >= 0x61 && idx <= 0x7A)) {
        fwrite(&idx, sizeof(char), 1, fout);
        totalChars++;
      }
    }
  }

  fclose(fin);
  fclose(fout);
}

/*****************************************************************************
 * Function: writeBit
 * Description: Writes a bit to a file
 * Inputs: fout (FILE*)- the file to write to
 *        bit (bool)- the bit to write
 * Outputs: None (writes to a file)
 * Time Complexity: O(1)
 * Space Complexity: O(1)
 * Usage: writeBit(fout, bit);
 *****************************************************************************/
void writeBit(FILE* fout, bool bit) {
  static unsigned char byte = 0;
  static unsigned char mask = 0x80;

  if (bit) {
    byte |= mask;
  }

  mask >>= 1;
  if (mask == 0) {
    fwrite(&byte, sizeof(unsigned char), 1, fout);
    byte = 0;
    mask = 0x80;
  }
}


/* -------------------------------------------------------------------------- */
//ENCODE/DECODE FUNCTION DEFINITIONS: 

/*****************************************************************************
 * Function: encodeFile
 * Description: Encodes a file using the Huffman table
 * Inputs: inputFilename (const char*)- the name of the file to read
 *        outputFilename (const char*)- the name of the file to write to
 *        table (HuffmanTable*)- the Huffman table to use for encoding
 * Outputs: None (writes to a file)
 * Time Complexity: ???
 * Space Complexity: ???
 * Usage: encodeFile(inputFilename, outputFilename, table);
 *****************************************************************************/
void encodeFile(const char* inputFilename, const char* outputFilename, 
                HuffmanTable* table) {
  FILE* fin = fopen(inputFilename, "r");
  FILE* fout = fopen(outputFilename, "wb");

  unsigned char buffer[CHUNK_SIZE];
  size_t totalChars = 0;
  while (true) {
    // Read in a chunk of the file
    size_t bytesRead = fread(buffer, sizeof(unsigned char), 
                             CHUNK_SIZE, fin);
    if (bytesRead == 0) {
      break;
    }
    // Encode the chunk
    for (size_t i = 0; i < bytesRead; i++) {
      unsigned char idx = buffer[i];
      // Write the code for the char to the file
      for (size_t j = 0; j < table->entries[idx].length; j++) {
        writeBit(fout, table->entries[idx].code[j]);
      }
      totalChars++;
    }
  }

  fclose(fin);
  fclose(fout);
}

/*****************************************************************************
 * Function: decode
 * Description: Decodes a file using the Huffman tree
 * Inputs: inputFilename (const char*)- the name of the file to read
 *         outputFilename (const char*)- the name of the file to write to
 *         treeFilename (const char*)- the name of the file containing the tree
 * Outputs: None (writes to a file)
 * Time Complexity: ???
 * Space Complexity: ???
 * Usage: decode(inputFilename, outputFilename, treeFilename);
 *****************************************************************************/
void decode(const char* inputFilename, const char* outputFilename, 
            const char* treeFilename){
  FILE* fin = fopen(inputFilename, "rb");
  FILE* fout = fopen(outputFilename, "w");
  FILE* tree = fopen(treeFilename, "rb");

  HuffmanTreeNode* huffmanTree = readTree(tree);
  HuffmanTreeNode* currentNode = huffmanTree;

  unsigned char buffer[CHUNK_SIZE];
  size_t totalChars = 0;
  while (true) {
    size_t bytesRead = fread(buffer, sizeof(unsigned char), 
                             CHUNK_SIZE, fin);
    if (bytesRead == 0) {
      break;
    }

    // Read each bit in the buffer
    for (size_t i = 0; i < bytesRead; i++) {
      unsigned char byte = buffer[i];
      for (int j = 0; j < 8; j++) {
        bool bit = byte & 0x80;
        byte <<= 1;
        
        // Traverse the tree until we reach a leaf node
        if (bit) {
          currentNode = currentNode->right;
        } else {
          currentNode = currentNode->left;
        }
        
        // If we reach a leaf node, write the char to the file and 
        // reset the current node to the root of the tree
        if (currentNode->left == NULL && currentNode->right == NULL) {
          fwrite(&currentNode->value, sizeof(unsigned char), 1, fout);
          currentNode = huffmanTree;
          totalChars++;
        }
      }
    }
  }
  // Print a newline at the end of the file to match spec. 
  // (This is not required)
  fwrite("\n", sizeof(unsigned char), 1, fout);

  fclose(fin);
  fclose(fout);
  fclose(tree);
  freeTree(huffmanTree);
}

