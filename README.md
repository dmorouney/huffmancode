*********************************************************
* ALL CODE in this project was created by:
* Robert Morouney (069001422)
*********************************************************


Huffman Encoding / Decoding

This folder contains 4 programs.  
  - encode.c : The program encodes a file whose name is supplied to stdin.  Durring this 
               process encode.c creates the following files:
                 * tree.txt : This file contains meta data used to reconstruct the Huffman Tree
                              durring decoding.
                 * frequency.txt : This file contains 39 lines each of which contain a character
                                   then a colon, and finally the frequency represented as an 
                                   unsigned integer. The format of each line is:
                                      "%c:%d\n",character,frequency
                 * codes.txt : This file contains 39 lines where each line is an ASCII character
                               followed by a colon and then the binary representation of the 
                               Huffman code generated for that specific character.
                 * .clean.tmp : This temporary file is created during the encoding process.  This file 
                                is the input file stripped of all but the 39 characters which are 
                                accepted for encoding.  In this process all uppercase characters are 
                                replaced with lowercase characters and all white space is replaced with 
                                regular spaces.  This file is removed automatically.
                 * compressed.bin : This final file is the Huffman encoded representation of our input
                                    file.

  - decode.c : This program reads a Huffman tree from tree.txt and then uses the tree to decode the data in 
              compressed.bin. The decoded data is placed in 'decoded.txt'

  - drawTree.c : This program reads a Huffman tree from tree.txt (cointained in the same folder) then 
                 writes a graphiz string to stdout.  This string, when piped to dot (graphiz) creates 
                 a PNG which is a visual representation of the tree being used to decode/encode the current
                 file. This was super helpful when debugging. 
                 To use this progam, first compile then run from a shell with the following command:
                    ./drawTree | dot -Tpng -o tree.png
                 *Note: This requires graphiz to be installed. An example is in the folder as tree_example.png

  - clean_files.c : This program simply removes any files generated durring encoding and decoding.  This is
                    helpful when running multiple input files as it cleans up the generated files which 
                    have a requirement to be uniformly named which can make it difficult when running multi[ple inputs 


  General usage is as follows:
    + Compile:
          $ gcc -Wall -o encode encode.c;
          $ gcc -Wall -o decode decode.c
          $ gcc -Wall -o drawTree drawTree.c;
          $ gcc -Wall -o cleanUp clean_files.c;

    + Use:
          $ ./encode
            Enter filename: input.txt
            - open and view frequency.txt
            - open and view codes.txt
            - inspect compressed.bin
          $ ./drawTree | dot -Tpng -o tree.png 
            - open and view tree.png
          $ ./decode 
            - open and view decoded.txt
          $ ./cleanUp 
            - start again with a new file.


**NOTES TO MARKER**

As per the spec the code is provided in separate files which are each single files for compilation. 
This is suboptimal as most of the code is re-used and should be in separate header/source files. 
Normally this would be the case and compilation for each of the programs would use separate compiler
flags and a make file.  This was not done for ease of marking. 



