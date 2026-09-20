#include <iostream>
#include <string>

using namespace std; //For the Standard Library

//Function Declarations
void showheader();


//Main Function
int main(){

    //Let's keep main() limited to function calls so we can stay modular
    //this will make it easier for us to expand the project later for MO3
    showheader();

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
