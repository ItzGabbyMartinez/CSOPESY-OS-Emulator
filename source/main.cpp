#include <iostream>
#include <string>
#include <cstdlib>

using namespace std; //For the Standard Library

//Function Declarations
void showheader();
void runcli();
void readcommand(const string& command);
// void clearscreen();   


//Main Function
int main(){

    //Let's keep main() limited to function calls so we can stay modular
    //this will make it easier for us to expand the project later for MO3
    showheader();
    runcli();

    return 0;
}

void showheader() {
    cout << R"(
  ______     _______.  ______   .______    _______     _______.____    ____ 
 /      |   /       | /  __  \  |   _  \  |   ____|   /       |\   \  /   / 
|  ,----'  |   (----`|  |  |  | |  |_)  | |  |__     |   (----` \   \/   /  
|  |        \   \    |  |  |  | |   ___/  |   __|     \   \      \_    _/   
|  `----.----)   |   |  `--'  | |  |      |  |____.----)   |       |  |     
 \______|_______/     \______/  | _|      |_______|_______/        |__|     
)" << endl;
}

void runcli(){
    string command;

    while (true){
        cout << "Enter a command: ";
        getline(cin, command);

        if (command == "clear") {
            // clearscreen(); to be implemented (clears + reprints header)
        }
        else if (command == "exit") {
            exit(0);          
        }
        else {
            readcommand(command);   
        }
    }
}

//Function responsible for handling the 5 required commands 
void readcommand(const string& command){
    if (command == "initialize") {
        cout << "Initialize command recognized. Doing something." << endl;
    }
    else if (command == "screen") {
        cout << "Screen command recognized. Doing something." << endl;
    }
    else if (command == "scheduler-start") {
        cout << "Scheduler-start command recognized. Doing something." << endl;
    }
    else if (command == "scheduler-stop") {
        cout << "Scheduler-stop command recognized. Doing something." << endl;
    }
    else if (command == "report-util") {
        cout << "Report-util command recognized. Doing something." << endl;
    }
    else {
        cout << command << " is not recognized as a command." << endl;
    }
}

void clearscreen(){

}