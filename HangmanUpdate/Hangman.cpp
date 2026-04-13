//
//  Hangman.cpp
//
//  Hangman implementation file
//
//  Savannah Mattoon
//

#include "Hangman.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cctype>
#include <unordered_set>
#include <sqlite3.h>

using namespace std;

// =================================================================================
//  DatabaseManager
// =================================================================================

//*************************************************************************************
//  Constructor / Destructor
//
//  Initialises the db handle to nullptr. The destructor calls close()
//*************************************************************************************
DatabaseManager::DatabaseManager() : db(nullptr) {}

DatabaseManager::~DatabaseManager() {
    close();
}

//*************************************************************************************
//  open
//
//  Opens a SQLite database file at 'dbPath'.
//  Returns true if the connection was established and false otherwise.
//*************************************************************************************
bool DatabaseManager::open(const std::string& dbPath) {
    int result = sqlite3_open(dbPath.c_str(), &db);

    if (result != SQLITE_OK) {
        cerr << "[Error: Could not open database: " << sqlite3_errmsg(db) << "]" << endl;
        db = nullptr;
        return false;
    }
    return true;
}

//*************************************************************************************
//  initializeWordTable
//
//  Creates the words table if it does not already exist.
//  The table stores only the word itself. Difficulty is determined at query
//  time by checking the length of the word.
//  easy   = 3 to 5 letters
//  medium = 6 to 7 letters
//  hard   = 8 or more letters
//  INSERT OR IGNORE prevents duplicate rows if the game is run multiple times.
//*************************************************************************************
void DatabaseManager::initializeWordTable() {
    if (!db) return;

    // Create a words table with just an id and the word itself
    const char* createTable =
        "CREATE TABLE IF NOT EXISTS words ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "word TEXT NOT NULL UNIQUE);";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, createTable, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "[Error creating table: " << errMsg << "]" << endl;
        sqlite3_free(errMsg);
        return;
    }

    // Seed with a variety of words across all three length ranges.
    // easy (3-5)
    // medium (6-7)
    // hard (8+)
    const char* seedWords =
        "INSERT OR IGNORE INTO words (word) VALUES"
        "('cat'),('dog'),('sun'),('hat'),('run'),"
        "('map'),('cup'),('bed'),('fox'),('pen'),"
        "('frog'),('drum'),('ship'),('wolf'),('tree'),"
        "('river'),('planet'),('bridge'),('castle'),('garden'),"
        "('frozen'),('mirror'),('rocket'),('jungle'),('bottle'),"
        "('blanket'),('cabinet'),('dolphin'),('feather'),('lantern'),"
        "('compass'),('harvest'),('pilgrim'),('thunder'),('sparrow'),"
        "('labyrinth'),('alchemy'),('fortress'),('skeleton'),('wanderer'),"
        "('champion'),('mystical'),('discover'),('owledge'),('pinnacle'),"
        "('brilliant'),('calendar'),('darkness'),('engineer'),('fragment');";

    if (sqlite3_exec(db, seedWords, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "[Error seeding words: " << errMsg << "]" << endl;
        sqlite3_free(errMsg);
    }
}

//*************************************************************************************
//  getRandomWord
//
//  Queries the words table for a random word filtered by length based on difficulty.
//  easy   = LENGTH between 3 and 5
//  medium = LENGTH between 6 and 7
//  hard   = LENGTH >= 8
//  ORDER BY RANDOM() lets SQLite handle selection. LIMIT 1 returns one result.
//  Falls back to any random word if no match is found.
//*************************************************************************************
std::string DatabaseManager::getRandomWord(const std::string& difficulty) const {
    if (!db) return "hangman";

    std::string word = "";

    // Build the length filter based on difficulty
    std::string lengthFilter;
    if (difficulty == "easy") {
        lengthFilter = "LENGTH(word) BETWEEN 3 AND 5";
    } else if (difficulty == "medium") {
        lengthFilter = "LENGTH(word) BETWEEN 6 AND 7";
    } else {
        lengthFilter = "LENGTH(word) >= 8";
    }

    std::string query =
        "SELECT word FROM words WHERE " + lengthFilter +
        " ORDER BY RANDOM() LIMIT 1;";

    auto callback = [](void* data, int, char** argv, char**) -> int {
        if (argv[0]) {
            *static_cast<std::string*>(data) = argv[0];
        }
        return 0;
    };

    char* errMsg = nullptr;
    if (sqlite3_exec(db, query.c_str(), callback, &word, &errMsg) != SQLITE_OK) {
        cerr << "[Error querying word: " << errMsg << "]" << endl;
        sqlite3_free(errMsg);
    }

    // Fall back to any random word if nothing matched the difficulty filter
    if (word.empty()) {
        const char* fallback = "SELECT word FROM words ORDER BY RANDOM() LIMIT 1;";
        sqlite3_exec(db, fallback, callback, &word, nullptr);
    }

    // Final safety fallback if the database is empty
    if (word.empty()) word = "hangman";

    return word;
}

//*************************************************************************************
//  close
//
//  Closes the SQLite connection and resets the handle to nullptr.
//*************************************************************************************
void DatabaseManager::close() {
    if (db) {
        sqlite3_close(db);
        db = nullptr;
    }
}

// =================================================================================
//  WordManager
// =================================================================================

//*************************************************************************************
//  setWordFromPlayer
//
//  Prompts the executioner (another human player) to type a word.
//  Stores the word in 'secretWord'. Extra blank lines are printed so the
//  guesser cannot see the word on screen after the executioner types it.
//*************************************************************************************
void WordManager::setWordFromPlayer() {
    cout << "[The executioner thinks of the word]..." << endl;
    cout << "(Executioner please enter a word): ";

    cin >> secretWord;

    // Scroll the word off screen so the other player cannot see it
    for (int i = 0; i < 24; ++i) {
        cout << endl;
    }
}

//*************************************************************************************
//  setWordFromDatabase
//
//  Asks DatabaseManager for a random word at the given difficulty level and
//  stores it in 'secretWord'.
//*************************************************************************************
void WordManager::setWordFromDatabase(DatabaseManager& dbManager,
                                      const std::string& difficulty) {
    cout << "[The executioner thinks of a word]..." << endl << endl;
    secretWord = dbManager.getRandomWord(difficulty);
}

//*************************************************************************************
//  getWord / getWordLength
//
//  Simple accessors that return the stored word and its character count.
//*************************************************************************************
std::string WordManager::getWord() const {
    return secretWord;
}

int WordManager::getWordLength() const {
    return static_cast<int>(secretWord.size());
}

// =================================================================================
//  DisplayManager
// =================================================================================

//*************************************************************************************
//  showTitleScreen
//
//  Renders the ASCII-art gallows and introductory story text.
//*************************************************************************************
void DisplayManager::showTitleScreen() const {
    cout << setw(24)  << "===================*=" << endl;
    cout << setw(23)  << "||  //             |";
    cout << setw(69)  << "--X---X---X---X---[ HANGMAN ]---X---X---X---X--" << endl;
    cout << setw(23)  << "|| //              |" << endl;
    cout << setw(23)  << "||//               |";
    cout << setw(75)  << "[You have been found guilty of a crime you did not commit.]" << endl;
    cout << setw(23)  << "||/                *";
    cout << setw(62)  << "[You are to be sent to the gallows before sunrise ";
    cout << "to be executed.]" << endl;
    cout << setw(25)  << "||               *   *";
    cout << setw(52)  << "[Your executioner is in a good mood, so ";
    cout << "he offers you a deal.]" << endl;
    cout << setw(26)  << "||              *     *";
    cout << setw(70)  << "[If you can guess his secret word you are free to leave;]" << endl;
    cout << setw(26)  << "||              *     *";
    cout << setw(61)  << "[fail and your unfortunate fate remains.]" << endl;
    cout << setw(25)  << "||               * * *";
    cout << setw(74)  << "[You must guess letter by letter until the word is revealed.]" << endl;
    cout << setw(5)   << "||";
    cout << setw(80)  << "[You are permitted 10 wrong guesses.]" << endl;
    cout << setw(5)   << "||";
    cout << setw(80)  << "[If you repeat a guess the executioner won't count ";
    cout << "it against you.]" << endl;
    cout << setw(5)   << "||" << endl;
    cout << setw(5)   << "||" << endl;
    cout << setw(5)   << "||" << endl;
    cout << setw(20)  << "-------------------" << endl;
}

//*************************************************************************************
//  showExecutionerPrompt
//
//  Displays the prompt asking the player to choose between computer and human
//  executioner.
//*************************************************************************************
void DisplayManager::showExecutionerPrompt() const {
    cout << setw(80) << "--X---X---X---[Choose your executioner]---X---X---X--" << endl;
    cout << setw(85) << "(Enter 'C' to play against the computer or 'P' for another player.)" << endl << endl;
    cout << "[Executioner]: ";
}

//*************************************************************************************
//  showExecutionerChoice
//
//  Confirms which executioner the player selected.
//*************************************************************************************
void DisplayManager::showExecutionerChoice(char executionerChoice) const {
    cout << endl;
    if (executionerChoice == 'P') {
        cout << setw(64) << "--X---X---X---[You have chosen another player as the ";
        cout << "executioner.]---X---X---X--" << endl << endl;
    } else {
        cout << setw(64) << "--X---X---X---[You have chosen the computer as the ";
        cout << "executioner.]---X---X---X--" << endl << endl;
    }
}

//*************************************************************************************
//  showInitialDashArray
//
//  Tells the player how many letters are in the word and displays the full
//  underscore (dash) array for the first time.
//*************************************************************************************
void DisplayManager::showInitialDashArray(const std::string& dashString) const {
    cout << "[The word is " << dashString.size() << " letters.]" << endl << endl;
    cout << "[Secret word]------->    ";
    cout << dashString;
    cout << endl << endl;
}

//*************************************************************************************
//  showDashArray
//
//  Displays the current state of the dash array after each guess.
//*************************************************************************************
void DisplayManager::showDashArray(const std::string& dashString) const {
    cout << "[Secret word]-------->  ";
    cout << dashString;
    cout << endl << endl;
}

//*************************************************************************************
//  showGuessedLetters
//
//  Prints all letters the player has guessed so far.
//*************************************************************************************
void DisplayManager::showGuessedLetters(const unordered_set<char>& guessedLetters) const {
    cout << "[Letters guessed]: ";
    for (char letter : guessedLetters) {
        cout << letter << ' ';
    }
    cout << endl << endl << endl;
}

//*************************************************************************************
//  showWrongGuessMessage
//
//  Notifies the player that their guess was incorrect and shows how many
//  wrong guesses remain.
//*************************************************************************************
void DisplayManager::showWrongGuessMessage(int remainingGuesses) const {
    cout << setw(56) << "--x---X---[Incorrect guess. You have [" << remainingGuesses;
    cout << "] Guesses left!]---X---x--" << endl << endl;
}

//*************************************************************************************
//  showWinScreen
//
//  Displays the victory message when the player guesses the word correctly.
//*************************************************************************************
void DisplayManager::showWinScreen() const {
    cout << endl << endl << endl << endl;
    cout << setw(18) << "*****" << endl;
    cout << setw(22) << "*************";
    cout << setw(49) << "[Congratulations!]" << endl;
    cout << setw(24) << "*****************";
    cout << setw(58) << "[You guessed the executioner's secret word.]" << endl;
    cout << setw(25) << "*******************";
    cout << setw(53) << "[Looks like you will live to see the sun rise ";
    cout << "another day!]" << endl;
    cout << setw(25) << "*******************" << endl;
    cout << setw(22) << "------------------------------" << endl;
    cout << setw(20) << "[YOU WIN!]" << endl << endl << endl << endl;
}

//*************************************************************************************
//  showLoseScreen
//
//  Displays the ASCII hangman figure and the losing message including the
//  secret word that the player failed to guess.
//*************************************************************************************
void DisplayManager::showLoseScreen(const std::string& secretWord) const {
    cout << setw(12) << "_____" << endl;
    cout << setw(13) << "|     |" << endl;
    cout << setw(16) << "___|_____|___" << endl;
    cout << setw(17) << "(___[_____]___)" << endl;
    cout << setw(13) << "[ x.x ]";
    cout << setw(68) << "[Sorry! You did not guess the executioner's word.]" << endl;
    cout << setw(12) << "[+++]";
    cout << setw(55) << "[The executioner tells you his secret word but ";
    cout << "you will be taking it to the grave.]" << endl;
    cout << setw(10) << "|";
    cout << setw(67) << "[Off to the gallows! DEAD MAN WALKING!]" << endl;
    cout << setw(16) << " |-____|____-|" << endl;
    cout << setw(16) << " |  ___|___  |" << endl;
    cout << setw(16) << " |   __|__   |" << endl;
    cout << setw(17) << "       |       " << endl;
    cout << setw(9)  << "-X-[ " << secretWord << " ]-X- " << endl << endl << endl;
}

//*************************************************************************************
//  showGoodbyeScreen
//
//  Displays the farewell ASCII art when the player quits the game.
//*************************************************************************************
void DisplayManager::showGoodbyeScreen() const {
    cout << endl << endl << endl;
    cout << setw(50) << "**" << endl;
    cout << setw(53) << "********" << endl;
    cout << setw(50) << "**" << endl;
    cout << setw(50) << "**" << endl;
    cout << setw(53) << "* **** *" << endl;
    cout << setw(56) << "*            *" << endl;
    cout << setw(57) << "+              +" << endl;
    cout << setw(57) << "|  [Goodbye!]  |" << endl;
    cout << setw(57) << "|   X X X X    |" << endl;
    cout << setw(57) << "|              |" << endl;
    cout << setw(57) << "|     ^ ^      |" << endl;
    cout << setw(57) << "|   ^ | | ^    |" << endl;
    cout << setw(68) << "----------|   |_/_/_/    |-----------" << endl;
    cout << setw(50) << "||" << endl;
    cout << setw(50) << "||" << endl;
    cout << setw(52) << "(^^^^)" << endl;
}

//*************************************************************************************
//  showDifficultyPrompt
//
//  Asks the player to choose a difficulty level before the computer picks a word.
//*************************************************************************************
void DisplayManager::showDifficultyPrompt() const {
    cout << setw(80) << "--X---X---X---[Choose a difficulty]---X---X---X--" << endl;
    cout << setw(60) << "[1] Easy   (3 - 5 letters)" << endl;
    cout << setw(60) << "[2] Medium (6 - 7 letters)" << endl;
    cout << setw(60) << "[3] Hard   (8 + letters  )" << endl << endl;
    cout << setw(50) << "(Enter 1, 2, or 3): ";
}

//*************************************************************************************
//  showPlayAgainPrompt
//
//  Asks the player whether they want to start a new game.
//*************************************************************************************
void DisplayManager::showPlayAgainPrompt() const {
    cout << setw(61) << "--X---X---X---X---[Would you like to play ";
    cout << "again?]---X---X---X---X--" << endl << endl << endl;
    cout << "(Please enter 'Y' to play or any other key to quit.): ";
}

// =================================================================================
//  InputManager
// =================================================================================

//*************************************************************************************
//  isValidLetter
//
//  Returns true if 'letter' is an alphabetic character (a-z or A-Z).
//*************************************************************************************
bool InputManager::isValidLetter(char letter) const {
    return isalpha(static_cast<unsigned char>(letter));
}

bool InputManager::isAlreadyGuessed(char letter,
                                    const unordered_set<char>& guessedLetters) const {
    // count() returns 1 if the lowercase letter exists in the set, 0 otherwise
    return guessedLetters.count(tolower(static_cast<unsigned char>(letter))) > 0;
}

//*************************************************************************************
//  getExecutionerChoice
//
//  Reads a character from the user and validates that it is either 'C' or 'P'
//  (case-insensitive). Loops until valid input is provided.
//*************************************************************************************
char InputManager::getExecutionerChoice() const {
    char input;
    cin >> input;
    char choice = static_cast<char>(toupper(static_cast<unsigned char>(input)));

    // Keep asking until the player enters a recognised option
    while (choice != 'P' && choice != 'C') {
        cout << "Invalid choice. Please choose 'C' or 'P': ";
        cin >> input;
        choice = static_cast<char>(toupper(static_cast<unsigned char>(input)));
    }
    return choice;
}

//*************************************************************************************
//  getLetterGuess
//
//  Prompts the player for a letter guess.
//  Validates that:
//    1. The input is a single alphabetic character.
//    2. The letter has not already been guessed (checked against guessedLetters).
//*************************************************************************************
char InputManager::getLetterGuess(const unordered_set<char>& guessedLetters) const {
    char guess;

    cout << "[Guess a letter]: ";
    cin >> guess;
    cout << endl;

    // Loop until the player provides a valid, unguessed alphabetic letter
    while (true) {
        if (!isValidLetter(guess)) {
            // Defensive check to reject non alphabetic characters
            cout << "[Invalid input. Please enter a letter (a-z).] " << endl << endl;
            cout << "[Guess]: ";
            cin >> guess;
            cout << endl;
        } else if (isAlreadyGuessed(guess, guessedLetters)) {
            cout << "[You have already guessed this letter. Try again.]" << endl << endl;
            cout << "[Guess]: ";
            cin >> guess;
            cout << endl;
        } else {
            break;  // Valid new alphabetic guess
        }
    }

    return guess;
}

//*************************************************************************************
//  getDifficultyChoice
//
//  Reads the player's difficulty selection and returns the matching string.
//  Loops until a valid option (1, 2, or 3) is entered.
//*************************************************************************************
std::string InputManager::getDifficultyChoice() const {
    char input;
    cin >> input;

    while (input != '1' && input != '2' && input != '3') {
        cout << "[Invalid choice. Please enter '1', '2', or '3']: ";
        cin >> input;
    }

    if (input == '1') return "easy";
    if (input == '2') return "medium";
    return "hard";
}

//*************************************************************************************
//  getPlayAgainChoice
//
//  Reads the player's play again response. Returns true only for 'Y' or 'y'.
//*************************************************************************************
bool InputManager::getPlayAgainChoice() const {
    char answer;
    cin >> answer;
    return toupper(static_cast<unsigned char>(answer)) == 'Y';
}

// =================================================================================
//  Hangman
// =================================================================================

//*************************************************************************************
//  Constructor
//
//  Initialises all game state counters and pointers to safe starting values.
//*************************************************************************************
Hangman::Hangman()
    : executionerType(' '),
      correctlyGuessedCount(0),
      matchesThisTurn(0),
      wrongGuessesRemaining(10),
      wordLength(0),
      difficulty("medium") {

    // The database file is created in the same folder as the running executable.
    // In Xcode this will be inside the build products directory automatically.
    if (dbManager.open("hangman.db")) {
        dbManager.initializeWordTable();
    }
}

//*************************************************************************************
//  setPlayer
//
//  Delegates input to InputManager and display to DisplayManager, then stores
//  the validated executioner choice in 'executionerType'.
//*************************************************************************************
void Hangman::setPlayer() {
    executionerType = inputManager.getExecutionerChoice();
    displayManager.showExecutionerChoice(executionerType);
}

//*************************************************************************************
//  setWord
//
//  If the executioner is a player prompt them to type a word.
//  If the executioner is the computer ask the player for a difficulty level
//  and retrieves a matching word from the database through WordManager.
//*************************************************************************************
void Hangman::setWord() {
    cout << endl;

    if (executionerType == 'P') {
        wordManager.setWordFromPlayer();
    } else {
        // Let the player choose a difficulty before the computer picks a word
        displayManager.showDifficultyPrompt();
        difficulty = inputManager.getDifficultyChoice();
        wordManager.setWordFromDatabase(dbManager, difficulty);
    }

    secretWord = wordManager.getWord();
    wordLength  = wordManager.getWordLength();
}

//*************************************************************************************
//  createWordArray
//
//  Assigns the secret word directly to 'wordString'.
//*************************************************************************************
void Hangman::createWordArray() {
    wordString = secretWord;
}

//*************************************************************************************
//  createDashArray
//
//  Initialises 'dashString' to a string of underscores the same length as the
//  secret word.
//*************************************************************************************
void Hangman::createDashArray() {
    // Construct a string of wordLength underscores. Each will be revealed as guessed
    dashString = std::string(wordLength, '_');

    displayManager.showInitialDashArray(dashString);
}

//*************************************************************************************
//  resetMatchCount
//
//  Resets the per turn match counter before processing each new guess.
//*************************************************************************************
void Hangman::resetMatchCount() {
    matchesThisTurn = 0;
}

//*************************************************************************************
//  recordGuess
//
//  Inserts the lowercase form of 'guess' into the unordered_set so that
//  duplicate detection via isAlreadyGuessed remains case insensitive.
//  Then asks DisplayManager to show the updated set.
//*************************************************************************************
void Hangman::recordGuess(char guess) {
    // Store lowercase so isAlreadyGuessed only needs to check one case
    guessedLetters.insert(tolower(static_cast<unsigned char>(guess)));
    displayManager.showGuessedLetters(guessedLetters);
}

//*************************************************************************************
//  checkAndRevealLetter
//
//  Scans 'wordString' for any position matching 'guess'.
//  For each match the character is written directly into 'dashString' at the
//  same index revealing it to the player.
//  Returns the number of positions where the guess was found this turn.
//*************************************************************************************
int Hangman::checkAndRevealLetter(char guess) {
    int matchCount = 0;

    for (int i = 0; i < wordLength; ++i) {
        // Compare case insensitively so 'A' matches 'a', etc.
        if (tolower(static_cast<unsigned char>(wordString[i])) ==
            tolower(static_cast<unsigned char>(guess))) {

            dashString[i] = wordString[i];  // Reveal the letter in the display string
            correctlyGuessedCount++;        // Track total revealed positions
            matchCount++;                   // Track matches for this specific guess
        }
    }
    return matchCount;
}

//*************************************************************************************
//  playerHasWon
//
//  Returns true when the number of correctly revealed positions equals the total
//  word length meaning every letter has been guessed.
//*************************************************************************************
bool Hangman::playerHasWon() const {
    return correctlyGuessedCount == wordLength;
}

//*************************************************************************************
//  playerHasLost
//
//  Returns true when the player has used all their allowed wrong guesses.
//*************************************************************************************
bool Hangman::playerHasLost() const {
    return wrongGuessesRemaining == 0;
}

//*************************************************************************************
//  game
//
//  Main game loop. Each iteration:
//    1. Resets the per turn match counter.
//    2. Gets a validated letter guess from InputManager.
//    3. Records the guess.
//    4. Checks whether the guess appears in the word and reveals any matches.
//    5. Updates the display.
//    6. Decrements wrong guess count if no matches were found.
//    7. Checks win/loss conditions and exits the loop if the game is over.
//*************************************************************************************
void Hangman::game() {
    do {
        resetMatchCount();

        char guess = inputManager.getLetterGuess(guessedLetters);

        cout << "---x----x----x----x----x----x----x----x----x----x----x----x---" << endl;

        // Record the guess before checking so duplicate detection stays consistent
        recordGuess(guess);

        // Check the guess against the word. matchesThisTurn holds the result
        matchesThisTurn = checkAndRevealLetter(guess);

        // Show the updated partially revealed word
        displayManager.showDashArray(dashString);

        if (matchesThisTurn == 0) {
            // If no match found this turn subtract one wrong guess and notify the player
            wrongGuessesRemaining--;
            displayManager.showWrongGuessMessage(wrongGuessesRemaining);
        }

        if (playerHasWon()) {
            displayManager.showWinScreen();
        }

        if (playerHasLost()) {
            displayManager.showLoseScreen(secretWord);
        }

    } while (!playerHasWon() && !playerHasLost());
    // Loop ends when the player wins (all letters revealed) or loses (no guesses left)
}
