//
// Hangman.h
//
// Hangman header file
//
// Savannah Mattoon
//

#ifndef HANGMAN_H
#define HANGMAN_H

#include <iostream>
#include <vector>


class Hangman{
    
private:

    char player;
    int count=0;
    int check=0;
    int wrong=10;
    char *arrPtr;
    int arraySize;
    char *dashArrayPtr;
    std::string word;
    std::vector<unsigned char>vect;

public:
    
    Hangman();
    void game();
    void setWord();
    void setSize();
    void setPlayer();
    void displayWon();
    void checkGuess();
    void resetCheck();
    void displayLost();
    void createWordArray();
    void createDashArray();
    void displayDashArray();
    void addGuessToVect(char);
    ~Hangman();
    
};


#endif /* Hangman_h */
