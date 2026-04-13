//
//  Hangman.h
//
//  Hangman header file
//
//  Savannah Mattoon
//

#ifndef HANGMAN_H
#define HANGMAN_H

#include <iostream>
#include <unordered_set>
#include <string>
#include <sqlite3.h>

// ---------------------------------------------------------------------------------
// DatabaseManager: Responsible for all SQLite database interactions.
// Handles database initialization, word table creation, seeding, and retrieval.
// ---------------------------------------------------------------------------------
class DatabaseManager {
public:
    DatabaseManager();
    ~DatabaseManager();

    // Opens the database at the given file path.
    // Returns true on success and false if the database could not be opened.
    bool open(const std::string& dbPath);

    // Creates the words table if it does not already exist then seeds it
    // with a default word list so the game works out of the box.
    void initializeWordTable();

    // Returns a random word from the words table matching the given difficulty.
    // Difficulty values: "easy", "medium", "hard"
    // Falls back to any random word if no match is found for the given difficulty.
    std::string getRandomWord(const std::string& difficulty) const;

    // Closes the database connection.
    void close();

private:
    sqlite3* db;  // Raw SQLite database handle managed by this class
};

// ---------------------------------------------------------------------------------
// WordManager: Responsible for storing and retrieving the secret word.
// Handles database based word selection and player entered word input.
// ---------------------------------------------------------------------------------
class WordManager {
public:
    // Sets the word from player input
    void setWordFromPlayer();

    // Sets the word by querying the database for a word at the given difficulty
    void setWordFromDatabase(DatabaseManager& dbManager, const std::string& difficulty);

    // Returns the secret word
    std::string getWord() const;

    // Returns the number of characters in the word
    int getWordLength() const;

private:
    std::string secretWord;  // The word to be guessed
};

// ---------------------------------------------------------------------------------
// DisplayManager: Responsible for all console output.
// Separates display logic from game logic so output can be changed independently.
// ---------------------------------------------------------------------------------
class DisplayManager {
public:
    // Displays the opening title screen
    void showTitleScreen() const;

    // Displays the executioner selection prompt
    void showExecutionerPrompt() const;

    // Displays the dash array representing the current guessed state of the word
    void showDashArray(const std::string& dashString) const;

    // Displays the list of letters already guessed
    void showGuessedLetters(const std::unordered_set<char>& guessedLetters) const;

    // Displays the word length hint and initial dash array
    void showInitialDashArray(const std::string& dashString) const;

    // Displays a message confirming the executioner choice
    void showExecutionerChoice(char executionerChoice) const;

    // Displays a wrong guess message with remaining guesses
    void showWrongGuessMessage(int remainingGuesses) const;

    // Displays the winning screen
    void showWinScreen() const;

    // Displays the losing screen including the secret word
    void showLoseScreen(const std::string& secretWord) const;

    // Displays the goodbye screen
    void showGoodbyeScreen() const;

    // Displays the play again prompt
    void showPlayAgainPrompt() const;

    // Displays the difficulty selection prompt
    void showDifficultyPrompt() const;
};

// ---------------------------------------------------------------------------------
// InputManager: Responsible for reading and validating user input.
// Separates input handling from game logic for easier maintenance and testing.
// ---------------------------------------------------------------------------------
class InputManager {
public:
    // Prompts user to choose an executioner ('C' or 'P') and returns valid choice
    char getExecutionerChoice() const;

    // Prompts user for a letter guess and returns a valid, unguessed alphabetic char
    char getLetterGuess(const std::unordered_set<char>& guessedLetters) const;

    // Prompts user to play again and returns true if they choose yes
    bool getPlayAgainChoice() const;

    // Prompts the player to choose a difficulty and returns "easy", "medium", or "hard"
    std::string getDifficultyChoice() const;

private:
    // Returns true if the letter has already been guessed
    bool isAlreadyGuessed(char letter,
                          const std::unordered_set<char>& guessedLetters) const;

    // Returns true if the character is a valid alphabetic letter
    bool isValidLetter(char letter) const;
};

// ---------------------------------------------------------------------------------
// Hangman: Controls overall gameplay flow.
// Delegates word management, display, and input to their respective classes.
// Manages the game state such as correct guesses, wrong guesses, and the dash array.
// ---------------------------------------------------------------------------------
class Hangman {
public:
    Hangman();

    // Sets up the executioner choice ('C' for computer, 'P' for player)
    void setPlayer();

    // Sets the secret word based on the executioner type
    void setWord();

    // Allocates the word character array from the secret word
    void createWordArray();

    // Allocates and displays the initial dash array (underscores)
    void createDashArray();

    // Runs the main game loop until the player wins or loses
    void game();

private:
    // Checks if guess appears in the word array and updates the dash array.
    // Returns the number of positions where the guess was found.
    int checkAndRevealLetter(char guess);

    // Adds a guessed letter to the guessed letters list
    void recordGuess(char guess);

    // Resets the per turn match counter to zero
    void resetMatchCount();

    // Returns true if all letters have been correctly guessed
    bool playerHasWon() const;

    // Returns true if no wrong guesses remain
    bool playerHasLost() const;

    char executionerType;        // 'C' = computer 'P' = another player
    int correctlyGuessedCount;   // Total correct letter positions revealed so far
    int matchesThisTurn;         // Number of times the current guess matched this turn
    int wrongGuessesRemaining;   // Number of wrong guesses left (starts at 10)
    std::string wordString;      // The secret word characters (replaces raw char* wordArray)
    int wordLength;              // Number of characters in the secret word
    std::string dashString;      // Partially revealed word display (replaces raw char* dashArray)
    std::string secretWord;      // The word to be guessed
    std::string difficulty;      // Difficulty level for word selection: easy, medium, or hard
    std::unordered_set<char> guessedLetters;  // Letters already guessed

    DatabaseManager dbManager;
    WordManager  wordManager;
    DisplayManager displayManager;
    InputManager inputManager;
};

#endif /* HANGMAN_H */
