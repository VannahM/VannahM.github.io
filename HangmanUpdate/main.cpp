//
//  main.cpp
//
//  Driver program
//
//  Savannah Mattoon
//

#include "Hangman.h"
#include <iostream>
#include <iomanip>

using namespace std;

// ---------------------------------------------------------------------------------
// playOneGame
//
//  Creates a fresh Hangman instance and runs a full single game.
// ---------------------------------------------------------------------------------
void playOneGame() {
    Hangman game;

    game.setPlayer();
    game.setWord();
    game.createWordArray();
    game.createDashArray();
    game.game();
}

int main() {

    // Create a DisplayManager to handle the title and goodbye screens,
    // and an InputManager to handle the play again prompt.
    DisplayManager display;
    InputManager   input;

    do {
        // cin.ignore is used to pause so the player can read the screen
        // before the executioner selection prompt appears.
        cin.ignore();

        display.showTitleScreen();
        display.showExecutionerPrompt();

        playOneGame();

        display.showPlayAgainPrompt();

    } while (input.getPlayAgainChoice());

    display.showGoodbyeScreen();

    return 0;
}
