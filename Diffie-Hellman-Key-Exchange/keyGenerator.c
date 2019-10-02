/*
Toy implementation of a Diffie-Hellman key generation (https://en.wikipedia.org/wiki/Diffie%E2%80%93Hellman_key_exchange)
Type ./keyGenerator for information on how to use this program
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <assert.h>

//Converts an array of charecters to a number
long charsToNum(const char arg[], const char baseChar){
    long multiplier = 1;
    long total = 0;
    int argLength = strlen(arg);
    int asciiDiff;

    for (int i = (argLength - 1); i >= 0; i--){
        asciiDiff = arg[i] - baseChar;
        total += asciiDiff * multiplier;
        multiplier *= 10;
    }
    return total;
}

//Efficiently computes (x ^ y) mod p
//Algorithm adapted from https://en.wikipedia.org/wiki/Modular_exponentiation#Right-to-left_binary_method
int modPower(long base, unsigned long exponent, long modulus){
    int evaluated = 1;
    base = base % modulus;
    while (exponent > 0){
        if (exponent % 2 == 1){
            evaluated = (evaluated * base) % modulus;
        }
        exponent = exponent >> 1;
        base = (base * base) % modulus;
    }
    return evaluated;
}

//Displays guide to using the program
void displayInstructions(){
    printf("SYNTAX: ./keyGenerator [UPPERCASE PASSWORD] [OPTIONAL - PUBLIC KEY]\n");
    printf("[PASSWORD] -> Uppercase letters only\n");
    printf("[PUBLIC KEY] -> Integer only\n");
    printf("Enter a secret password know only to you to generate a public key, this can be shared safely over a potentially unsafe channel with another trusted party, who should share their public key with you.\n");
    printf("Then enter your secret password again followed by the other public key to generate a shared private key known only to you and the holder of the other secret password.\n");
}

//Tests the modPower function with static numbers
void testModPower(long prime){
    prime = 1705829;
    assert(modPower(24, 2015, prime) == 1250396);
    assert(modPower(22, 2015, prime) == 1019425);
    assert(modPower(658, 23432, prime) == 614209);
}

//Tests key generation by simulating a key exchange
void testKeyGen(long modBase, long prime){
    int password1 = 8493564;
    int password2 = 346102;

    int public1 = modPower(modBase, password1, prime);
    int public2 = modPower(modBase, password2, prime);
    int private1 = modPower(public2, password1, prime);
    int private2 = modPower(public1, password2, prime);
    assert(private1 == private2);
}

//Produces private and public keys
void generateKey(int n, char *args[], long modBase, const long prime){
    long key;
    const long password = charsToNum(args[1], 'A');

    if (n == 2){
        printf("Public Key (exchange with other trusted party): ");
    } else {
        modBase = charsToNum(args[2], '0');
        printf("Private Key (shared secret for future communication): ");
    }

    key = modPower(modBase, password, prime);
    printf("%ld\n", key);
}

//Entry point to program - calls functions based on number of arguments provided
int main(int n, char *args[n]) {
    setbuf(stdout, NULL);

    const long prime = 2106945901;
    long modBase = 2;

    if (n >= 2){
        generateKey(n, args, modBase, prime);
    }
    else {
        displayInstructions();
        testModPower(prime);
        testKeyGen(modBase,prime);
        printf("All tests passed.\n");
    }
    return 0;
}
