//This program takes in an image and perfoms simple visual effect operations on it, before the proccessed image is output to a new file.
//Import standard libraries
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

//Declaration of data type synonyms
typedef unsigned char Byte;
typedef int16_t Int2;
typedef int32_t Int4;

//Useful global constants
const Byte byteLength = 8;
const Byte channels = 3;

enum {Filetype1=0, Filetype2=1, Size=2, PixelDataIndex=10, Width=18, Height=22, BitsPerPixel=28, Compression=30};
enum {inFile=1, outFile=2};
const Byte kSize = 3;

//Image header object definition
struct ImageHeader{
    Byte byte1;
    Byte byte2;
    Int4 size;
    Int4 width;
    Int4 height;
    Int4 bitsPerPixel;
    Int4 compression;
    Int4 colourDepth;
    Int4 pixelDataIndex;
};

typedef struct ImageHeader ImageHeader;

//IO FUNCTIONS

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

//Packs 4 consecutive bytes in an array into a single 4 byte integer
//Converts from little endian to big endian (LSB at lowest address)
Int4 packBytes(Byte headerCopy[], int startIndex, int length){
    Int4 bytes = 0;
    Byte nextByte;
    for (int i = 0; i < length; i++){
        nextByte = headerCopy[i + startIndex];
        bytes = bytes | (nextByte << (byteLength * i));
    }
    return bytes;
}

//Gets the first %headerSize% bytes from the image file and copy into an array
//Represents the header containing image metadata
void getHeaderData(const char fileName[], Byte headerSize, Byte headerCopy[headerSize]){
    FILE *in = fopenCheck(fileName,"rb");
    fread(headerCopy, sizeof(Byte),headerSize, in);
    fclose(in);
}

//Check the provided file is a bitmap
void bitmapCheck(Byte headerCopy[]){
    if (headerCopy[0] != 'B' || headerCopy[1] != 'M') {
        printf("Please use a bitmap file\n");
        exit(1);
    }
}

//Extracts useful metadata from the header bytes and loads into an ImageHeader structure
void parseHeader(Byte headerCopy[], ImageHeader *header){
    header->byte1 = headerCopy[0];
    header->byte2 = headerCopy[1];
    header->size = packBytes(headerCopy, Size, 4);
    header->pixelDataIndex = packBytes(headerCopy, PixelDataIndex, 4);
    header->width = packBytes(headerCopy, Width, 4);
    header->height = packBytes(headerCopy, Height, 4);
    header->bitsPerPixel = packBytes(headerCopy, BitsPerPixel, 2);
    header->compression = packBytes(headerCopy, Compression, 4);
}

//Validates image specificiation to ensure file in a suitable format to be processed
void validateImage(ImageHeader *header){
    if (header->compression != 0){
        printf("Please use an uncompressed bitmap file\n");
        exit(1);
    }
}

//Retrieves the byte pixel array from the image file and loads i
void getPixelArray(const char fileName[], int pixelArrayIndex, int pixelArraySize, Byte rawPixelArray[]){
    FILE *in = fopenCheck(fileName,"rb");
    fseek(in, pixelArrayIndex, SEEK_SET);
    fread(rawPixelArray, pixelArraySize, sizeof(Byte), in);
    fclose(in);
}

//Parses the byte array represneting pixel data into a more convinient array struture for simplified processesing
void parseRawPixelArray(int height, int width, Byte pixels[height][width][channels], Byte const rawPixelArray[], ImageHeader *header){
    const Byte bytesPerPixel = header->bitsPerPixel / 8;
    const Byte padding = 4;
    const Byte paddingLength = (header->width * bytesPerPixel) % padding;
    int index = 0;
    for(int i = height - 1; i >= 0; i--){
        for(int j = 0; j < width; j++){
            for (Byte k = 0; k < channels; k++) pixels[i][j][k] = rawPixelArray[index + k];
            index += bytesPerPixel;
        }
        index += paddingLength;
    }
}

//Converts an array of processed pixels back into a one dimensional pixel array following the bitmap specificiation
void generateRawPixelArray(int height, int width, Byte const pixels[height][width][channels], Byte rawPixelArray[], ImageHeader *header){
    const Byte bytesPerPixel = header->bitsPerPixel / 8;
    const Byte padding = 4;
    const Byte paddingLength = (header->width * bytesPerPixel) % padding;
    int index = 0;

    for(int i = height - 1; i >= 0; i--){
        for(int j = 0; j < width; j++){
            for (Byte k = 0; k < channels; k++) rawPixelArray[index + k] = pixels[i][j][k];
            index += bytesPerPixel;
        }
        index += paddingLength;
    }
}

//Writes bytes to new bitmap file
void writeToFile(const char fileName[], int headerSize, Byte header[], int pixelArraySize, Byte pixelArray[]){
    FILE *out = fopenCheck(fileName,"wb");
    for (int i = 0; i < headerSize; i++) fputc(header[i], out);
    for (int i = 0; i < pixelArraySize; i++) fputc(pixelArray[i], out);
    fclose(out);
}
//IMAGE PROCESSING FUNCTIONS

//Copies the contents of one array to another
void copyPixels(int height, int width, Byte pixels1[height][width][channels], Byte pixels2[height][width][channels]){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for (int k = 0; k < channels; k++) pixels2[i][j][k] = pixels1[i][j][k];
        }
    }
}

//Flips the image around a centrally a X-axis
void flipX(int height, int width, Byte pixels[height][width][channels]){
    Byte pixelsCopy[height][width][channels];
    int upperBound = height - 1;
    copyPixels(height, width, pixels, pixelsCopy);
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for (int k = 0; k < channels; k++) pixels[i][j][k] = pixelsCopy[upperBound - i][j][k];
        }
    }
}

//Flips the image around a centrally a Y-axis
void flipY(int height, int width, Byte pixels[height][width][channels]){
    Byte pixelsCopy[height][width][channels];
    int upperBound = width - 1;
    copyPixels(height, width, pixels, pixelsCopy);
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for (int k = 0; k < channels; k++) pixels[i][j][k] = pixelsCopy[i][upperBound - j][k];
        }
    }
}

//Darkens every pixel in the image across all colour channels
void darken(int height, int width, Byte pixels[height][width][channels], float scaling){
    scaling /= 100;
    scaling = 1 - scaling;
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for (Byte k = 0; k < channels; k++) pixels[i][j][k] *= scaling;
        }
    }
}

//Brigtens every pixel in the image across all colour channels
void brighten(int height, int width, Byte pixels[height][width][channels], float scaling){
    scaling /= 100;
    const int maxBrightness = 255;
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for (Byte k = 0; k < channels; k++) pixels[i][j][k] += (maxBrightness - pixels[i][j][k]) * scaling;
        }
    }
}

//Converts a colour image into greyscale
void greyscale(int height, int width, Byte pixels[height][width][channels]){
    int sum;
    int average;
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            sum = 0;
            for (Byte k = 0; k < channels; k++) sum += pixels[i][j][k];
            average = sum / channels;
            for (Byte k = 0; k < channels; k++) pixels[i][j][k] = average;
        }
    }
}

//Inverts all colours
void invert(int height, int width, Byte pixels[height][width][channels]){
    const Byte maxBrightness = 255;
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for (int k = 0; k < channels; k++) pixels[i][j][k] = maxBrightness - pixels[i][j][k];
        }
    }
}

//Calculates new pixel value for a single pixel in blurred image
Byte blurKernel (int height, int width, Byte pixels[height][width][channels], int size, int i, int j, int k){
    int lowerX = j - size;
    int lowerY = i - size;
    int upperX = j + size;
    int upperY = i + size;
    if (lowerX < 0) lowerX = 0;
    if (lowerY < 0) lowerY = 0;
    if (upperX >= width) upperX = width - 1;
    if (upperY >= height) upperY = height - 1;
    int sum = 0;
    int pixelCount = 0;
    for (int y = lowerY; y <= upperY; y++){
        for (int x = lowerX; x <= upperX; x++){
            sum += pixels[y][x][k];
            pixelCount++;
        }
    }
    int averageColour = sum / pixelCount;
    return averageColour;

}

//Blurs the image
void blur(int height, int width, Byte pixels[height][width][channels], int size){
    Byte pixelsCopy[height][width][channels];
    copyPixels(height, width, pixels, pixelsCopy);
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for (Byte k = 0; k < channels; k++){
                pixels[i][j][k] = blurKernel(height, width, pixelsCopy, size, i, j, k);
            }
        }
    }
}

//Converts a pixel array of unsigned bytes into a pixel array of 2-byte signed integers
void byteToInt(int height, int width, Byte pixels1[height][width][channels], Int2 pixels2[height][width][channels]){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for (int k = 0; k < channels; k++) pixels2[i][j][k] = pixels1[i][j][k];
        }
    }
}

//Copies a pixel array of 2-byte signed integers into another array
void copySignedVals(int height, int width, Int2 pixels1[height][width][channels], Int2 pixels2[height][width][channels]){
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for (int k = 0; k < channels; k++) pixels2[i][j][k] = pixels1[i][j][k];
        }
    }
}

//Convolves a single pixel with a kernel
Int2 edgeKernel(int height, int width, Int2 pixels[height][width][channels], Int2 kernel[kSize][kSize], int y, int x, int k){
    int sum = 0;
    for (int i = 0; i < kSize ; i++){
        for (int j = 0; j < kSize; j++){
            sum += (pixels[y + i - 1][x + j - 1][k] * kernel[i][j]);
        }
    }
    int averageColour = sum / (kSize * kSize);
    return averageColour;
}

//Performs kernel convolution across an entire image and all colour channels
void edgeConvolution(int height, int width, Int2 pixels[height][width][channels], Int2 kernel[kSize][kSize]){
    Int2 pixelsCopy[height][width][channels];
    copySignedVals(height, width, pixels, pixelsCopy);
    for (int i = 1; i < height - 1; i++){
        for (int j = 1; j < width - 1; j++){
            for (int k = 0; k < channels; k++) {
                pixels[i][j][k] = edgeKernel(height, width, pixelsCopy, kernel, i, j, k);
            }
        }
    }
}

//Perorms Sobel edge detection across the image
void edges(int height, int width, Byte pixels[height][width][channels]){
    greyscale(height, width, pixels);

    Int2 gradX[height][width][channels];
    byteToInt(height, width, pixels, gradX);
    Int2 xKernel[3][3] = {{-1,0,1},{-2,0,2},{-1,0,1}};
    edgeConvolution(height, width, gradX, xKernel);


    Int2 gradY[height][width][channels];
    byteToInt(height, width, pixels, gradY);
    Int2 yKernel[3][3] = {{-1,-2,-1},{0,0,0},{1,2,1}};
    edgeConvolution(height, width, gradY, yKernel);

    for (int i = 1; i < height - 1; i++){
        for (int j = 1; j < width - 1; j++){
            for (int k = 0; k < channels; k++){
                 pixels[i][j][k] = sqrt((gradY[i][j][k] * gradY[i][j][k]) + (gradY[i][j][k] * gradY[i][j][k]));
            }
        }
    }

}

//Checks if the parameter following an effect is valid
//Returns a valid parameter or 0 otherwise
int parseNum(char arg[]){
    int result = 0;
    if (arg != NULL) {
        result = atoi(arg);
        for (int i = 0; i < strlen(arg);i++){
            if (arg[i] < '0' || arg[i] > '9') {
                result = 0;
                break;
            }
        }
        if (result > 100 || result < 1) result = 0;
    }
    return result;
}

//Checks whether an effect argument is valid or not
bool validArg(char arg[]){
    bool result = false;
    const Byte numOfFunctions = 3;
    char validStrs[3][10] = {"darken","brighten","blur"};
    for (int i = 0; i < numOfFunctions; i++){
        if (strcmp(arg,validStrs[i]) == 0) {
            result = true;
            break;
        }
    }
    return result;
}

//Produces an error message in the case that there is a syntax error with the user's program call
void invalidArg(char arg[]) {
    printf("\"%s\" is an invalid effect or has an invalid effect parameter\n", arg);
    exit(1);
}

//Calls the effects in the order specified in the program arguments
void effectsChain(int height, int width, Byte pixels[height][width][channels], ImageHeader *header, int argNum, char *args[argNum]){
    const Byte standardArgs = 3;
    for (int i = standardArgs; i < argNum; i++){
        if(strcmp(args[i], "flipX") == 0) flipX(height, width, pixels);
        else if(strcmp(args[i], "flipY") == 0) flipY(height, width, pixels);
        else if(strcmp(args[i], "greyscale") == 0) greyscale(height, width, pixels);
        else if(strcmp(args[i], "invert") == 0) invert(height, width, pixels);
        else if(strcmp(args[i], "edges") == 0) edges(height, width, pixels);
        else if(parseNum(args[i + 1]) != 0 && validArg(args[i])){
            if(strcmp(args[i], "darken") == 0) darken(height, width, pixels, parseNum(args[i + 1]));
            else if(strcmp(args[i], "brighten") == 0) brighten(height, width, pixels, parseNum(args[i + 1]));
            else if(strcmp(args[i], "blur") == 0) blur(height, width, pixels, parseNum(args[i + 1]));
            i++;
        } else invalidArg(args[i]);
    }
}

//Main pipeline
//Calls the main functions related to importing, applying effects and outputting
void proccessImage(int argNum, char *args[argNum]){
    ImageHeader headerData;
    ImageHeader *header = &headerData;

    Byte headerSize = 54;
    Byte headerCopy[headerSize];

    getHeaderData(args[inFile], headerSize, headerCopy);
    bitmapCheck(headerCopy);
    parseHeader(headerCopy, header);
    validateImage(header);

    const int rawPixelArraySize = header->size - header->pixelDataIndex;
    Byte rawPixelArray[rawPixelArraySize];
    getPixelArray(args[inFile], header->pixelDataIndex, rawPixelArraySize, rawPixelArray);

    Byte pixels[header->height][header->width][channels];
    parseRawPixelArray(header->height, header->width, pixels, rawPixelArray, header);

    effectsChain(header->height, header->width, pixels, header, argNum, args);

    Byte newRawPixelArray[rawPixelArraySize];
    generateRawPixelArray(header->height, header->width, pixels, newRawPixelArray, header);
    writeToFile(args[outFile], header->pixelDataIndex, headerCopy, rawPixelArraySize, newRawPixelArray);

    printf("SUCCESS: %s -> %s\n", args[inFile], args[outFile]);

}

//TESTING FUNCTIONS

//Tests the functionality of the parseNum function
void testParseNum(){
    assert(parseNum("4") == 4);
    assert(parseNum("37") == 37);
    assert(parseNum("100") == 100);
    assert(parseNum("101") == 0);
    assert(parseNum("0") == 0);
    assert(parseNum("-1") == 0);
    assert(parseNum("4f") == 0);
    assert(parseNum("f4") == 0);
    assert(parseNum("/&%^&") == 0);
    assert(parseNum("ab") == 0);

}

//Automates the checking of 2 pixel arrays
//One array is the hand calculated (predicted) result of an effect function
//The other is the actual result of an effect function
bool checkPixels(int height, int width, Byte correct[height][width][channels], Byte test[height][width][channels]){
    bool valid = true;
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            for (int k = 0; k < channels; k++) {
                if (correct[i][j][k] != test[i][j][k]) {
                    valid = false;
                    break;
                }
            }
        }
    }
    return valid;
}

//Tests the flipX effect function
void testFlipX(){
    Byte pixels[2][1][3] = {{{0, 0, 0}},{{125, 255, 63}}};
    Byte correct[2][1][3] = {{{125,255,63}},{{0, 0, 0}}};
    flipX(2,1,pixels);
    assert(checkPixels(2, 1, correct, pixels));
}

//Tests the flipY effect function
void testFlipY(){
    Byte pixels[1][2][3] = {{{0, 0, 0},{125, 255, 63}}};
    Byte correct[1][2][3] = {{{125,255,63},{0, 0, 0}}};
    flipY(1,2,pixels);
    assert(checkPixels(1, 2, correct, pixels));
}

//Tests the greyscale effect function
void testGreyscale(){
    Byte pixel[1][1][3] = {{{43,254,137}}};
    Byte correct[1][1][3] = {{{144,144,144}}};
    greyscale(1,1,pixel);
    assert(checkPixels(1, 1, correct, pixel));
}

//Tests the darken effect function
void testDarken(){
    Byte pixel[1][1][3] = {{{0, 255, 125}}};
    Byte correct[1][1][3] = {{{0,89,43}}};
    darken(1,1,pixel,65);
    assert(checkPixels(1, 1, correct, pixel));
}

//Tests the brighten effect function
void testBrighten(){
    Byte pixel[1][1][3] = {{{0, 255, 125}}};
    Byte correct[1][1][3] = {{{191,255,222}}};
    brighten(1,1,pixel,75);
    assert(checkPixels(1, 1, correct, pixel));
}

//Tests the invert effect function
void testInvert(){
    Byte pixel[1][1][3] = {{{0, 255, 64}}};
    Byte correct[1][1][3] = {{{255, 0, 191}}};
    invert(1,1,pixel);
    assert(checkPixels(1, 1, correct, pixel));
}

//Test the blur effect function
void testBlur(){
    Byte pixels[2][2][3] = {{{255,0,0},{24,1,1}},{{95,2,2},{183,3,3}}};
    Byte correct[2][2][3] = {{{139,1,1},{139,1,1}},{{139,1,1},{139,1,1}}};
    blur(2,2,pixels,1);
    assert(checkPixels(2, 2, correct, pixels));
}

//MANAGEMENT FUNCTIONS

//Calls all tests
void testAll(){
    testParseNum();
    testFlipX();
    testFlipY();
    testGreyscale();
    testDarken();
    testBrighten();
    testInvert();
    testBlur();
    printf("All tests passed.\n");
}

//Entry point to the program
int main(int argNum, char *args[argNum]){
    setbuf(stdout,NULL);
    if (argNum > 1) proccessImage(argNum, args);
    else testAll();
    return 0;
}
