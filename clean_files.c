
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

const char F_TABLE[] = "frequency.txt";
const char C_TABLE[] = "codes.txt";
const char C_FILE[] = "compressed.bin";
const char D_FILE[] = "decoded.txt";
const char T_FILE[] = "tree.txt";


int main(int argc, char *argv[]) {
  printf("This will delete the following files:\n%s\n%s\n%s\n%s\n%s\n", F_TABLE, C_TABLE, C_FILE, D_FILE, T_FILE);
  printf("Are you sure you want to continue? (y/n): ");
  char c;
  scanf("%c", &c);
  if (c == 'y') {
    remove(F_TABLE);
    remove(C_TABLE);
    remove(C_FILE);
    remove(D_FILE);
    remove(T_FILE);
    printf("Files deleted.\n");
  } else {
    printf("Files not deleted.\n");
  }
    return 0;
}
