Lucas O'Dowd-Jones - Optional Coursework
Huffman Coding Generator

For this week's optional coursework I created a program which takes in a file and produces a Huffman coding (https://en.wikipedia.org/wiki/Huffman_coding)
This coding generates a coding from a byte to a unique prefix
Huffman coding takes advantage of the fact that some bytes are more common than others in binary files, for example in text files the bytes referring to ASCII charecter codes are much more frequent than other byte values, and within that certain charecters are more common than others in the english language
My program can input any binary file and output the Huffman coding for each byte to the terminal
Huffman coding has been proven to produce the optimal prefix code

My program also calculates the maximum reduction in file size using this code optimally (ie: each prefix is as tighly packed as possible within the output bytes)
This calculation does not take into account including the coding map from prefixes to bytes needed for decompression as the size of this map would vary with each differnent implementation of compression

Decompression could take place by mapping each prefix back to a Byte and reconstucting the original file

This implementation operates as follows:
    1) The file is loaded into memory
    2) The program counts the number of occourences of each indvidual byte (an array of 256 elements is used to record this as the value of the byte could range from 0 -> 255)
    3) Each byte and frequency is represented as a single node in an array (bytes with a frequency of 0, ie did not occour in the file, are ignored)
    4) These nodes are sorted within the array into acending order of frequency by a quicksort (as there are up to 256 elements simpler sorts are unsuitable)
    5) The nodes are linked into an ordered singly linked list (as this structure allows us to quickly remove elements from the front of the array and insert into the middle)
    6)  a) Remove the 2 nodes with the smallest frequency from the front of the node list
        b) Create a new internal node with a frequency which is the sum of the 2 nodes which have just been removed
        c) Add this new internal node back onto the node list in the correct position to maintain order
        d) Repeat until only one node is remaining (the root node of the Huffman Tree)
    7) The Huffman Tree is traversed generating the coding for each leaf (representing a byte value) and the mapping is displayed to the terminal

Testing:
For the testing of this program as each function is dependent on the previous function I have created a single test function which simulates the running of the entire program
I have used set inputs which I have hand calculated the expected results of for each stage in the computation
While I would usually prefer completely isolated testing, in this instance it makes more sense to simulate a single run of the program and still test the results of each function indvidually

Syntax:
$./huffman [FILENAME]
Displays the Huffman coding of [FILENAME] to the terminal

$./huffman
Runs the automated test encoding

