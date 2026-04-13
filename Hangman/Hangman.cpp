
//
//  Hangman.cpp
//
//  Hangman implementation file
//
//  Savannah Mattoon
//

#include "Hangman.h"
#include <iostream>
#include <cstring>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <vector>


using namespace std;

//Constructor
Hangman::Hangman(){
    
}

//*************************************************************************************
//                               * setPlayer *
//
//  Takes user input for executioner choice and sets 'player' to choice. An
//  output statement excutes depending on what the user chooses.
//  .
//
//
//*************************************************************************************
void Hangman::setPlayer(){
    
    char executioner;
    
//    Gets user input and sets player to toupper choice
    cin>>executioner;
    player=toupper(executioner);
    
//   Executes if choice is nether 'P' or 'C'
    if(player!='P'&&player!='C'){
        
        do{
           cout<<"Invalid choice. Please choose 'C' or 'P' :";
           cin>>executioner;
           player=toupper(executioner);
            
        }while(player!='C'&&player!='P');
        
     }
  
//    Outputs if 'P' is chosen
    if(player=='P'){
        cout<<endl;
        cout<<setw(64)<<"--X---X---X---[You have chosen another player as the ";
        cout<<"executioner.]---X---X---X--"<<endl<<endl;
        
    }
    
//    Outputs if 'C' is chosen
    if(player=='C'){
        cout<<endl;
        cout<<setw(64)<<"--X---X---X---[You have chosen the computer as the ";
        cout<<"executioner.]---X---X---X--"<<endl<<endl;
        
    }
    
}

//*************************************************************************************
//                               * setWord *
//
//  Depending on executioner choice the word is set by another player or by computer.
// 'Word' is set to chosen word. setSize is called to get length of word.
//
//
//*************************************************************************************
void Hangman::setWord(){
    
   
    cout<<endl;
//    String variable to hold a word
    std::string w;
   
//    If choice was 'P' executioner is prompted to enter word and 'Word' is set to chosen word
    if(player=='P'){
        
        cout<<"[The executioner thinks of the word]..."<<endl;
        cout<<"(Executioner please enter a word): ";
       
        cin>>w;
        word=w;
        
        cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
        cout<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl<<endl;
       
        
    }
    
//    If 'C' a random word is set to 'word'
    if(player=='C'){
        
       srand(time(0));
       string line;
        
        cout<<"[The exectioner thinks of a word]... "<<endl<<endl<<endl<<endl;
        
//     Random number is generated and assigned to 'num'.
      int num=(rand()%800);
        
        ifstream inputFile;
        inputFile.open("/Users/bammam/Desktop/words.txt");
        

       for(int i=0; i<num; i++){
            
           inputFile>>line;
          
       }

        inputFile.close();
        word=line;
        
    }

    setSize();

}

//*************************************************************************************
//                               * setSize *
//
//  Counts the size of the chosen word using a for loop and assigns the number
//  to arraySize member.
//
//
//*************************************************************************************
void Hangman::setSize(){
    
    int count=0;
    
    for(int i=0; word[i]!='\0'; ++i){
        
        count++;
        
    }
    arraySize=count;
    
}

//*************************************************************************************
//                               * createWordArray *
//
//  Dynamically creates an array for the chosen word using arraySize as size and
//  arrPtr member. A for loop is used to assign the string to the arrPtr member.
//
//
//*************************************************************************************
void Hangman::createWordArray(){
    
    this->arrPtr=new char[arraySize];
    
    for(int i=0; i<arraySize; i++){
        
        arrPtr[i]=word[i];
        
    }
}

//*************************************************************************************
//                               * createDashArray *
//  Dynamically creates a dash array for the chosen word using arraySize as size
//  and dashArrayPtr member. A for loop is used to assign the string to the arrPtr
//  member.
//
//
//*************************************************************************************
void Hangman::createDashArray(){
    
    cout<<"[The word is "<<arraySize<<" letters.]"<<endl<<endl;
    cout<<"[Secret word]------->    ";
  
    this->dashArrayPtr=new char[arraySize];
    
    for(int i=0; i<arraySize; i++){
        
//        Assigns dashes per letter into array and outputs array
        dashArrayPtr[i]='_';
        cout<<dashArrayPtr[i];
    }
    
    cout<<endl<<endl;
}

//*************************************************************************************
//                               * displayDashArray *
//
//  Outputs the dash array. Uses for loop to output array each time a letter
//  is guessed.
//
//
//*************************************************************************************
void Hangman::displayDashArray(){
    
    cout<<"[Secret word]-------->  ";
   for(int i=0; i<arraySize; i++){
       cout<<dashArrayPtr[i];
   }
    cout<<endl<<endl;
    
}

//*************************************************************************************
//                               * game *
//
//  User is prompted to guess letters. If a letter has already been guessed user
//  is promted to
//  enter a different guess. Do while loop executes until user has guessed all letters
//  or until they get 10 wrong guesses
//
//*************************************************************************************
void Hangman::game(){
    
    char guess;
    
   
    do{
//      calls function to reset 'check' counter.
        resetCheck();
        
        
        cout<<"[Guess a letter]: ";
        cin>>guess;
        cout<<endl;
        cout<<"---x----x----x----x----x----x----x----x----x----x----x----x---"<<endl;
        
//        for loop that checks vector for existing guessed letters.
        for(int i=0; i<vect.size(); i++){

//           If 'guess' is already in vect do while loop executes
            if(tolower(vect.at(i))==guess||toupper(vect.at(i))==guess){
            
            do{
                cout<<"[You have already guessed this letter. try again.]"<<endl<<endl;
                cout<<"[Guess]:";
                cin>>guess;
                
            }while(tolower(vect.at(i))==guess||toupper(vect.at(i))==guess);

        }
        
  }
        
       addGuessToVect(guess);
       
     
        for(int i=0; i<arraySize; i++){

//           Checks dash array for any instance of 'guess'
            if(toupper(dashArrayPtr[i]==guess)||tolower(dashArrayPtr[i])==guess){
            
               do{
                  
                   cout<<"[You have already guessed this letter. try again.]"<<endl<<endl;
                   cout<<"[Guess]:";
                   cin>>guess;
                   
                    
               
               }while(toupper(dashArrayPtr[i])==guess||tolower(dashArrayPtr[i])==guess);

            }
        
        }
        
        
        

       for(int i=0; i<arraySize; i++){
           
//        Checks if guess is in arrayPtr
            if(arrPtr[i]==toupper(guess)||arrPtr[i]==tolower(guess)){
                dashArrayPtr[i]=arrPtr[i];
                
                count+=1; //To keep track of correctly guessed letter
                check+=1; //To show instances of guessed letter were found
            }
       }
        

        displayDashArray();
        

        if(check==0){
            wrong-=1; //Decrements 1 from number of alotted wrong guesses.
            cout<<setw(56)<<"--x---X---[Incorrect guess. You have ["<<wrong;
            cout<<"] Guesses left!]---X---x--"<<endl<<endl;
        }

//     Calls function to display winning message
        if(count==arraySize){
            displayWon();
        }

//     Calls function to display losing message if 'wrong' is zero.
        if(wrong==0){
            
            displayLost();
        }
        
      
       
        
    }while(count!=arraySize&&wrong!=0);
//Loop executes while count does not equal array size or while wrong guesses does not equal zero
    
 }
    
//*************************************************************************************
//                               * resetCheck *
//
//  Resets 'check' counter to zero
//
//*************************************************************************************
void Hangman::resetCheck(){
    
    check=0;
    
}

//*************************************************************************************
//                               * addGuessToVect *
// Takes in a char variable containing the guessed letter and adds it to the vect
// and then displays the vector.
//
//*************************************************************************************
void Hangman::addGuessToVect(char g){
    
   
    cout<<"[Letters guessed]: ";
   

    vect.push_back(g); //Adds letter to vector
    
    for(int i=0; i<vect.size();++i){
        std::cout<<vect.at(i)<<' ';
    }
    cout<<endl<<endl<<endl;
    
}

//*************************************************************************************
//                               * displayWon *
//
//  Displays winning message
//
//*************************************************************************************
void Hangman::displayWon(){
    
    cout<<endl<<endl<<endl<<endl;
    cout<<setw(18)<<"*****"<<endl;
    cout<<setw(22)<<"*************";
    cout<<setw(49)<<"[Congratulations!]"<<endl;
    cout<<setw(24)<<"*****************";
    cout<<setw(58)<<"[You guessed the executioners secret word.]"<<endl;
    cout<<setw(25)<<"*******************";
    cout<<setw(53)<<"[Looks like you will live to see the sun rise ";
    cout<<"another day!]"<<endl;    cout<<setw(25)<<"*******************"<<endl;
    cout<<setw(22)<<"------------------------------"<<endl;
    cout<<setw(20)<<"[YOU WIN!]"<<endl<<endl<<endl<<endl;
    
}

//*************************************************************************************
//                               * displayLost*
//
//  Displays losing message
//
//*************************************************************************************
void Hangman::displayLost(){
    
    cout<<setw(12)<<"_____"<<endl;
    cout<<setw(13)<<"|     |"<<endl;
    cout<<setw(16)<<"___|_____|___"<<endl;
    cout<<setw(17)<<"(___[_____]___)"<<endl;
    cout<<setw(13)<<"[ x.x ]";
    cout<<setw(68)<<"[Sorry! You did not guess the executioner's word.]"<<endl;
    cout<<setw(12)<<"[+++]";
    cout<<setw(55)<<"[The executioner tells you his secret word but ";
    cout<<"you will be taking it to the grave.]"<<endl;
    cout<<setw(10)<<"|";
    cout<<setw(67)<<"[Off to the gallows! DEAD MAN WALKING!]"<<endl;
    cout<<setw(16)<<" |-____|____-|"<<endl;
    cout<<setw(16)<<" |  ___|___  |"<<endl;
    cout<<setw(16)<<" |   __|__   |"<<endl;
    cout<<setw(17)<<"       |       "<<endl;
    cout<<setw(9)<<"-X-[ "<<word<<" ]-X- "<<endl<<endl<<endl;
    
}

//Destructor
Hangman::~Hangman(){
    delete[] this->arrPtr;
    delete[] this->dashArrayPtr;
  
    
}
