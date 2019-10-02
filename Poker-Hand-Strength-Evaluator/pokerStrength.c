#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

//Card structure definition
struct card{
    int value;
    char suit;
};

//Define stuct card synonym
typedef struct card Card;

//Rank structure definition
struct rank{
    int type;
    int cardValue;
};

//Define struct card synonym
typedef struct rank Rank;

//Define hand rank constants
enum {HighCard=1, Pair=2, TwoPair=3, ThreeOfAKind=4, Straight=5, Flush=6, FullHouse=7, FourOfAKind=8, StraightFlush=9};

//Define court card constants
enum {J=11, Q=12, K=13, A=14};

const int binSize = 15;
const char suits[] = {'H','S','C','D'};


//Display the contents of the deck
void printDeck(int size, Card deck[]) {
    for (int i = 0; i < size; i++){
        if (i != 0) printf(", ");
        printf("%d%c", deck[i].value, deck[i].suit);
    }
    printf("\n");
}

//Add one to the permutation pointers
void incrementPointers(int pointers[], int current, int topValue){
    if (pointers[current] < topValue - 1) {
        pointers[current] += 1;
    } else {
        incrementPointers(pointers, current - 1, pointers[current]);
        pointers[current] = pointers[current - 1] + 1;
    }
}

//Display the hand of a permutation
void getHandFromPointers(Card outputArray[], int pointerLength, const int pointers[], const Card deck[]){
    for (int i = 0; i < pointerLength; i++) outputArray[i] = deck[pointers[i]];
}

//Set the values of all the cards in the deck
void initialiseDeck(int size, Card deck[]){
    for (int i = 0; i < size; i++){
        deck[i].suit = suits[i / 13];
        deck[i].value = 2 + (i % 13);
    }
}

//Checks if any of the cards to remove match the ith card in the deck
bool isCardFound(Card deck[], Card toRemove[], int removeLength, int i){
    bool cardFound = false;
    for (int j = 0; j < removeLength; j++){
        if (deck[i].value == toRemove[j].value && deck[i].suit == toRemove[j].suit){
            cardFound = true;
            break;
        }
    }
    return cardFound;
}

//Removes all the cards in an array from a source array
void removeCardsFromDeck(int removeLength, Card toRemove[], int deckLength, Card sourceDeck[], Card newDeck[]){
    int cardsFound = 0;
    for (int i = 0; i < deckLength; i++){
        if (isCardFound(sourceDeck, toRemove, removeLength, i)) cardsFound++;
        else newDeck[i-cardsFound] = sourceDeck[i];
    }
}

//Set inital the values of permutation pointers
void initialisePointers(int size, int pointers[]){
    for (int i = 0; i < size; i++) pointers[i] = i;
}

//Sort hand into 'bins'
//Counts how many times each card value from 2 -> Ace(14) appears in the hand
void createBins(Card hand[5], int bins[binSize]){
    const int handSize = 5;
    for (int i = 0; i < binSize; i++) bins[i] = 0;
    for (int i = 0; i < handSize; i++) bins[hand[i].value] += 1;
}

//Uses values from binAnalysis() to obtain the greatest rank for the hand
//Detects high card, pair, 2 pair, 3 of a kind, full house and 4 of a kind
Rank classifyBinRank(Rank binRank, int maxSize, int numOfLargeBins, int pairMax, int highCardValue){
    if (maxSize == 4) binRank.type = FourOfAKind;
    else if ((maxSize == 3) && (numOfLargeBins == 2)) binRank.type = FullHouse;
    else if (maxSize == 3) binRank.type = ThreeOfAKind;
    else if (numOfLargeBins == 2) {
        binRank.type = TwoPair;
        binRank.cardValue = pairMax;
    }
    else if (maxSize == 2) binRank.type = Pair;
    else if (maxSize == 1) {
        binRank.type = HighCard;
        binRank.cardValue = highCardValue;
    }
    return binRank;
}

//Iterates through values in bins generating useful values for determining the best rank of the hand
Rank binAnalysis(Card hand[5], int bins[binSize]){
    Rank binRank;
    int maxSize = 0;
    int numOfLargeBins = 0;
    int pairMax = 0;
    int highCardValue = 0;

    for (int i = binSize - 1; i >= 0; i--){
        if (maxSize < bins[i]){
            maxSize = bins[i];
            binRank.cardValue = i;
        }
        if (bins[i] >= 2) {
            numOfLargeBins++;
            if (i > pairMax) pairMax = i;
        }
        if ((highCardValue == 0) && (bins[i] != 0)) highCardValue = i;
    }

    binRank = classifyBinRank(binRank, maxSize, numOfLargeBins, pairMax, highCardValue);

    return binRank;
}

//Checks if a flush is present in the hand
Rank checkFlush(Card hand[5]){
    Rank flushRank = {Flush, hand[0].value};
    for (int i = 1; i < 5; i++){
        if (hand[i].suit == hand[0].suit){
            if (hand[i].value > flushRank.cardValue) flushRank.cardValue = hand[i].value;
        } else {
            flushRank.type = 0;
            break;
        }
    }
    return flushRank;
}

//Checks if a straight is present in the hand
Rank checkStraight(int bins[binSize]){
    Rank straightRank = {0, 0};
    int upperBound;
    int lowerBound;

    for (upperBound = binSize - 1; bins[upperBound] == 0; upperBound--){}
    for (lowerBound = 2; bins[lowerBound] == 0; lowerBound++){}

    if ((upperBound - lowerBound) == 4){
        straightRank.type = Straight;
        straightRank.cardValue = upperBound;
    }

    return straightRank;
}

//Determines which of 2 ranks is better (or if they tie)
int compareRanks(Rank rank1, Rank rank2){
    int bestRank;
    if (rank1.type > rank2.type) bestRank = 1;
    else if (rank1.type < rank2.type) bestRank = 2;
    else {
        if (rank1.cardValue > rank2.cardValue) bestRank = 1;
        else if (rank1.cardValue < rank2.cardValue) bestRank = 2;
        else bestRank = 0;
    }
    return bestRank;
}

//Calcualtes the best rank and card values for the hand
Rank bestRank(Card hand[5]){
    int bins[binSize];
    createBins(hand, bins);

    Rank finalRank;

    Rank binRank = binAnalysis(hand, bins);
    Rank flushRank = checkFlush(hand);

    if (compareRanks(binRank, flushRank) == 1) finalRank = binRank;
    else finalRank = flushRank;

    if (binRank.type == HighCard){
        Rank straightRank = checkStraight(bins);
        if ((straightRank.type != 0) && (flushRank.type != 0)) {
            finalRank.type = StraightFlush;
            finalRank.cardValue = straightRank.cardValue;
        } else {
            if (compareRanks(straightRank, finalRank) == 1) finalRank = straightRank;
        }
    }

    return finalRank;
}

//Concatenate the contents of array2 to the end of array1 and save into outputArray
void concatArray(Card outputArray[], int size1 ,Card array1[size1], int size2, Card array2[size2]) {
    for (int i = 0; i < size1; i++) outputArray[i] = array1[i];
    for (int i = 0; i < size2; i++) outputArray[i + size1] = array2[i];
}

//Calculates the best ranking card combination of the hole cards and the community cards
Rank bestRankFromFullHand(Card sevenCardHand[7]){
    const int fullHandSize = 7;
    const int playableHandSize = 5;
    int pointers[playableHandSize];
    initialisePointers(playableHandSize, pointers);

    Card fiveCardHand[playableHandSize];
    Rank topRank = {0,0};
    Rank tempRank;

    while (pointers[0] < (fullHandSize - playableHandSize)){
        getHandFromPointers(fiveCardHand, playableHandSize, pointers, sevenCardHand);
        tempRank = bestRank(fiveCardHand);
        if (compareRanks(tempRank,topRank) == 1) topRank = tempRank;

        incrementPointers(pointers, playableHandSize - 1, fullHandSize);
    }
    return topRank;
}

//Displays the hand's strength as the percentage of possible hands it beats
void displayResult(float numberOfWins, float numberOfTrials, float numberOfTies){
    float winRate = (numberOfWins / numberOfTrials) * 100;
    float tieRate = (numberOfTies / numberOfTrials) * 100;
    float lossRate = 100 - winRate - tieRate;
    printf("Win - %.2f%%\n", winRate);
    printf("Split Pot - %.2f%%\n", tieRate);
    printf("Loss - %.2f%%\n", lossRate);
}

//Iterates through all the potential hole cards other players may have
//For each potential opposing hand -> determines whether the player would win, lose, or tie (same strength hand)
void checkAllOpponentHands(Card deck[], Card potCards[], Rank playerRank){
    const int deckLength = 45;
    const int numOfOpponentCards = 2;
    int pointers[numOfOpponentCards];
    initialisePointers(numOfOpponentCards, pointers);

    Rank opponentRank;
    Card opponentCards[numOfOpponentCards];
    Card opponentFullHand[7];

    int playerWins = 0;
    int ties = 0;

    //Known number of combinations of opposing hands
    //45 * 44 = 990
    // 45 choose 2 = 990
    const int cardCombintions = 990;
    for (int i = 0; i < cardCombintions; i++){
        getHandFromPointers(opponentCards, numOfOpponentCards, pointers, deck);
        concatArray(opponentFullHand, numOfOpponentCards, opponentCards, 5, potCards);
        opponentRank = bestRankFromFullHand(opponentFullHand);
        if (compareRanks(playerRank, opponentRank) == 1) playerWins++;
        else if (compareRanks(playerRank, opponentRank) == 0) ties++;
        if (pointers[0] < (deckLength - numOfOpponentCards)) incrementPointers(pointers, numOfOpponentCards - 1, deckLength);
    }
    displayResult(playerWins, cardCombintions, ties);
}

//Evaluates the strength of a player's hole cards given all 5 community cards
void handStrength(Card handCards[2], Card potCards[5]){
    const int fullHandSize = 7;
    Card playerCards[fullHandSize];

    concatArray(playerCards, 2, handCards, 5, potCards);
    Rank playerRank = bestRankFromFullHand(playerCards);

    const int deckLength = 52;
    Card fullDeck[deckLength];
    initialiseDeck(deckLength, fullDeck);

    Card deck[deckLength - fullHandSize];
    removeCardsFromDeck(fullHandSize, playerCards, deckLength, fullDeck, deck);

    checkAllOpponentHands(deck, potCards, playerRank);
}

//Converts a charecter card value to an integer
int parseValue(char digit){
    int digitValue = -1;
    if ((digit >= '2') && (digit <= '9')) digitValue = digit - '0';
    else if (digit == 'J') digitValue = J;
    else if (digit == 'Q') digitValue = Q;
    else if (digit == 'K') digitValue = K;
    else if (digit == 'A') digitValue = A;
    return digitValue;
}

//Evaluates whether a suit charecter provided by the user is valid
bool parseSuit(char letter){
    bool validSuit = false;
    for (int j = 0; j < 4; j++){
        if(letter == suits[j]){
            validSuit = true;
            break;
        }
    }
    return validSuit;
}

//Parses an individual card from user input
bool parseCard(int argNum, char *args[argNum], int argIndex, int cardIndex, Card cardArray[]){
    bool validCard = true;
    int charOffset = 0;
    if ((args[argIndex][0] == '1') && (args[argIndex][1]=='0')){
        cardArray[cardIndex].value = 10;
        charOffset = 1;
    }
    else if (parseValue(args[argIndex][0]) > -1) cardArray[cardIndex].value = parseValue(args[argIndex][0]);
    else validCard = false;

    if (parseSuit(args[argIndex][1 + charOffset])) cardArray[cardIndex].suit = args[argIndex][1 + charOffset];
    else validCard = false;

    return validCard;
}

//Parses all 7 cards provided by the user input
bool parseHand(Card handCards[2], Card potCards[5], int argNum, char *args[argNum]){
    bool validArgs = true;
    const int offset = 3;
    for (int i = 0; (i < 2) && (validArgs); i++){
        validArgs = parseCard(argNum, args, i + 1, i, handCards);
    }
    for (int i = 0; (i < 5) && (validArgs); i++){
        validArgs = parseCard(argNum, args, i + offset, i, potCards);
    }

    return validArgs;
}

//Calcualtes the strength of a user-provided set of cards
void userHand(int argNum, char *args[argNum]){
    const int handCardsSize = 2;
    const int potCardsSize = 5;
    Card handCards[handCardsSize];
    Card potCards[potCardsSize];
    if (parseHand(handCards, potCards, argNum, args)){
        handStrength(handCards, potCards);
    }
    else printf("Invalid arguments\n");
}

//Test the permutation generation functionality
void testPermutations(){
    const int deckLength = 52;
    Card deck[deckLength];
    initialiseDeck(deckLength, deck);

    const int subsetLength = 7;
    int pointers[subsetLength];
    initialisePointers(subsetLength, pointers);

    int counter = 1;
    while (pointers[0] < (deckLength - subsetLength)){
        incrementPointers(pointers, subsetLength - 1, deckLength);
        counter++;
    }

    //Checks that the known number of 7 card permutations from a 52 card deck are generated
    // 52!/((7!)(52 - 7)!) = 133784560 permutations
    assert(counter == 133784560);
}

//Tests the card removal functionality
void testRemoveCards(){
    const int deckLength = 52;
    Card deck[deckLength];
    initialiseDeck(deckLength, deck);

    Card handCards[] = {{J,'H'},{Q,'S'}};

    printDeck(52, deck);
    Card freeCards[50];
    removeCardsFromDeck(2, handCards, 52, deck, freeCards);
    printDeck(50, freeCards);
    //Perform visual inspection - new deck should be missing 10H and 10S
}

//Tests the hand strength classifier on examples for all hand types
void testBestRank(){

    Card handCards1[5] = {{6,'H'},{4,'H'},{5,'H'},{J,'H'},{8,'C'}};
    Rank strength = bestRank(handCards1);
    assert(strength.type == HighCard && strength.cardValue == 11);

    Card handCards2[5] = {{6,'H'},{4,'H'},{4,'C'},{J,'H'},{8,'C'}};
    strength = bestRank(handCards2);
    assert(strength.type == Pair && strength.cardValue == 4);

    Card handCards3[5] = {{6,'H'},{4,'H'},{4,'C'},{6,'S'},{8,'C'}};
    strength = bestRank(handCards3);
    assert(strength.type == TwoPair && strength.cardValue == 6);

    Card handCards4[5] = {{7,'H'},{4,'H'},{7,'C'},{7,'S'},{8,'C'}};
    strength = bestRank(handCards4);
    assert(strength.type == ThreeOfAKind && strength.cardValue == 7);

    Card handCards5[5] = {{4,'H'},{5,'H'},{6,'C'},{7,'S'},{8,'C'}};
    strength = bestRank(handCards5);
    assert(strength.type == Straight && strength.cardValue == 8);

    Card handCards6[5] = {{7,'H'},{4,'H'},{3,'H'},{J,'H'},{8,'H'}};
    strength = bestRank(handCards6);
    assert(strength.type == Flush && strength.cardValue == 11);

    Card handCards7[5] = {{3,'H'},{4,'H'},{3,'C'},{3,'S'},{4,'C'}};
    strength = bestRank(handCards7);
    assert(strength.type == FullHouse && strength.cardValue == 3);

    Card handCards8[5] = {{Q,'H'},{4,'H'},{Q,'C'},{Q,'S'},{Q,'D'}};
    strength = bestRank(handCards8);
    assert(strength.type == FourOfAKind && strength.cardValue == 12);

    Card handCards9[5] = {{7,'H'},{8,'H'},{9,'H'},{10,'H'},{J,'H'}};
    strength = bestRank(handCards9);
    assert(strength.type == StraightFlush && strength.cardValue == 11);

}

//Tests the 7 card hand strength classifier on examples
void testBestRankFromFullHand() {
    Card fullHand1[7] = {{6,'H'},{4,'H'},{5,'H'},{K,'H'},{8,'C'},{2,'S'},{J,'D'}};
    Rank strength = bestRankFromFullHand(fullHand1);
    assert(strength.type == HighCard && strength.cardValue == K);

    Card fullHand2[7] = {{6,'H'},{4,'H'},{5,'H'},{J,'H'},{8,'C'},{4,'S'},{2,'D'}};
    strength = bestRankFromFullHand(fullHand2);
    assert(strength.type == Pair && strength.cardValue == 4);

    Card fullHand3[7] = {{2,'H'},{4,'H'},{5,'H'},{7,'H'},{8,'C'},{4,'S'},{7,'D'}};
    strength = bestRankFromFullHand(fullHand3);
    assert(strength.type == TwoPair && strength.cardValue == 7);

    Card fullHand4[7] = {{6,'H'},{4,'H'},{5,'H'},{J,'H'},{8,'C'},{J,'S'},{J,'D'}};
    strength = bestRankFromFullHand(fullHand4);
    assert(strength.type == ThreeOfAKind && strength.cardValue == J);

    Card fullHand5[7] = {{4,'H'},{2,'H'},{5,'H'},{3,'H'},{6,'C'},{J,'S'},{J,'D'}};
    strength = bestRankFromFullHand(fullHand5);
    assert(strength.type == Straight && strength.cardValue == 6);

    Card fullHand6[7] = {{4,'H'},{2,'H'},{5,'H'},{3,'H'},{6,'C'},{J,'H'},{J,'D'}};
    strength = bestRankFromFullHand(fullHand6);
    assert(strength.type == Flush && strength.cardValue == J);

    Card fullHand7[7] = {{4,'H'},{4,'D'},{5,'H'},{3,'H'},{5,'C'},{J,'H'},{5,'D'}};
    strength = bestRankFromFullHand(fullHand7);
    assert(strength.type == FullHouse && strength.cardValue == 5);

    Card fullHand8[7] = {{3,'S'},{3,'C'},{5,'H'},{3,'H'},{6,'C'},{J,'H'},{3,'D'}};
    strength = bestRankFromFullHand(fullHand8);
    assert(strength.type == FourOfAKind && strength.cardValue == 3);

    Card fullHand9[7] = {{J,'D'},{8,'D'},{9,'D'},{3,'H'},{2,'D'},{Q,'D'},{10,'D'}};
    strength = bestRankFromFullHand(fullHand9);
    assert(strength.type == StraightFlush && strength.cardValue == Q);
}

//Run automated testing
void test(){
    testPermutations();
    testRemoveCards();
    testBestRank();
    testBestRankFromFullHand();
    printf("All tests passed\n");
}

//Entry point to program
//Captures user input and passes to relevant functions
int main(int argNum, char *args[argNum]){
    setbuf(stdout, NULL);
    if (argNum == 1) test();
    else if (argNum == 8){
        userHand(argNum, args);
    } else printf("Invalid number of arguments provided\n");


    return 0;
}
