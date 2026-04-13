//
// Hangman.cpp
//
// Driver program
//
// Savannah Mattoon
//

#include "Hangman.h"
#include <iostream>
#include <cstdlib>
#include <iomanip>

using namespace std;

void callClass();

int main(){
    
    char answer;
    
    do{
        
       
        cout<<setw(24)<<"===================*="<<endl;
        cout<<setw(23)<<"||  //             |";
        cout<<setw(69)<<"--X---X---X---X---[ HANGMAN ]---X---X---X---X--"<<endl;
        cout<<setw(23)<<"|| //              |"<<endl;
        cout<<setw(23)<<"||//               |";
        cout<<setw(75)<<"[You have been found guilty of a crime you did not commit.]"<<endl;
        cout<<setw(23)<<"||/                *";
        cout<<setw(62)<<"[You are to be sent to the gallows before sunrise ";
        cout<<"to be executed.]"<<endl;
        cout<<setw(25)<<"||               *   *";
        cout<<setw(52)<<"[Your executioner is in a good mood, so ";
        cout<<"he offers you a deal.]"<<endl;
        cout<<setw(26)<<"||              *     *";
        cout<<setw(70)<<"[If you can guess his secret word you are free to leave;]"<<endl;
        cout<<setw(26)<<"||              *     *";
        cout<<setw(61)<<"[fail and your unforutante fate remains.]"<<endl;
        cout<<setw(25)<<"||               * * *";
        cout<<setw(74)<<"[You must guess letter by letter until the word is revealed.]"<<endl;
        cout<<setw(5)<<"||";
        cout<<setw(80)<<"[You are permitted 10 wrong guesses.]"<<endl;
        cout<<setw(5)<<"||";
        cout<<setw(80)<<"[If you repeat a guess the executioner won't count ";
        cout<<"it against you.]"<<endl;
        cout<<setw(5)<<"||"<<endl;
        cout<<setw(5)<<"||"<<endl;
        cout<<setw(5)<<"||"<<endl;
        cout<<setw(20)<<"-------------------"<<endl;
        
        cin.get();
        
        cout<<setw(80)<<"--X---X---X---[Choose your executioner]---X---X---X--"<<endl;
        cout<<setw(85)<<"(Enter 'C' to play against the computer or 'P' for another player.)"<<endl<<endl;
        cout<<"[Executioner]: ";
        

        callClass();
        
    
        cout<<setw(61)<<"--X---X---X---X---[Would you like to play ";
        cout<<"again?]---X---X---X---X--"<<endl<<endl<<endl;
        cout<<"(Please enter 'Y' to play or any other key to quit.): ";
        cin>>answer;
    
        
    
    
    }while(toupper(answer)=='Y');
    
              
//   Quit game display
    cout<<endl<<endl<<endl;
    cout<<setw(50)<<"**"<<endl;
    cout<<setw(53)<<"********"<<endl;
    cout<<setw(50)<<"**"<<endl;
    cout<<setw(50)<<"**"<<endl;
    cout<<setw(53)<<"* **** *"<<endl;
    cout<<setw(56)<<"*            *"<<endl;
    cout<<setw(57)<<"+              +"<<endl;
    cout<<setw(57)<<"|  [Goodbye!]  |"<<endl;
    cout<<setw(57)<<"|   X X X X    |"<<endl;
    cout<<setw(57)<<"|              |"<<endl;
    cout<<setw(57)<<"|     ^ ^      |"<<endl;
    cout<<setw(57)<<"|   ^ | | ^    |"<<endl;
    cout<<setw(68)<<"----------|   |_/_/_/    |-----------"<<endl;
    cout<<setw(50)<<"||"<<endl;
    cout<<setw(50)<<"||"<<endl;
    cout<<setw(52)<<"(^^^^)"<<endl;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    return 0;
}

void callClass(){
    
    Hangman call;
    
    call.setPlayer();
    call.setWord();
    call.createWordArray();
    call.createDashArray();
    call.game();
    
}
