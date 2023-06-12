//Michał Plata and Joanna Maj 2023

/*This code allows the user to play console version of Minesweeper (on Windows) by revealing tiles, avoiding bombs, and using flags to mark potential bomb locations.
 The game state and board are updated based on user inputs, and the player can save and exit the game or load it.
 Keys used to play are:
 - arrow keys - for moving cursor around the board
 - right shift - for revealing tiles
 - right control - for flagging tiles (flagging a tile prohibits it from being revealed)
 - question mark - used for saving the game*/

//Including header files
#include <iostream>   // Provides input/output stream functionality
#include <string>     // Provides string manipulation functions and classes
#include <cmath>      // Provides mathematical functions and operations
#include <ctime>      // Provides functions for working with time and date
#include <cstdlib>    // Provides general-purpose functions (e.g., memory allocation, random number generation)
#include <fstream>    // Provides file input/output operations
#include <Windows.h>  // Provides Windows-specific functionality


// Defining keycodes for easier use and understanding
#define left_arrow 0x25           // Keycode for the left arrow key
#define right_arrow 0x27          // Keycode for the right arrow key
#define down_arrow 0x28           // Keycode for the down arrow key
#define up_arrow 0x26             // Keycode for the up arrow key
#define shift 0xA1                // Keycode for the shift key
#define control_right 0xA3        // Keycode for the right control key
#define question_mark 0xBF        // Keycode for the question mark key


// Structure describing a tile on the game board
struct tile {
    int value;             // The value of the tile (represents the number of adjacent bombs or a bomb itself)
    bool is_not_hidden;    // Indicates whether the tile is revealed or hidden
    bool is_flagged;       // Indicates whether the tile is flagged by the player
};


// Global variables
int height = 6;               // Height of the game board
int width = 6;                // Width of the game board
int game_end = 0;             // Indicates the game state (0: in progress, 1: won, 2: lost, 3: saved and exited)
int flags = 0;                // Number of remaining flags
int crsr_pos_y = 0;           // Current cursor position (Y coordinate)
int crsr_pos_x = 0;           // Current cursor position (X coordinate)
tile **board;                 // 2D array representing the game board (dynamically allocated in GenerateBoard function)

//Function used for defining array "board" with user input values which are checked for being in given range
bool GenerateBoard(){
    // Prompt the user to input the board height
    std::cout << "Input board height (6-32)" << std::endl;
    std::cin >> height;

    // Prompt the user to input the board width
    std::cout << "Input board width (6-32)" << std::endl;
    std::cin >> width;

    // Check if the input height or width is out of range (less than 6 or greater than 32)
    if(height < 6 || height > 32 || width < 6 || width > 32)
        return true; // Return true to indicate that board generation failed

    // Allocate memory for the board based on the input height and width
    board = new tile*[height];
    for(int i = 0; i < height; i++) {
        board[i] = new tile[width];
    }

    // Return false to indicate that board generation was successful
    return false;
}


//Function used for initializing and setting up all 'tiles' of array "board" with default values
void FillBoard(){
    // Iterate over each tile on the board
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            // Initialize the tile with default values: value = 0, is_not_hidden = false, is_flagged = false
            board[y][x] = {0, false, false};
        }
    }
}


//Function used to place bombs and change values of tiles around it (to indicate the bomb) with restricting it to the board size and omitting placed bombs
void PlaceBombs(int height_pos, int width_pos){
    board[height_pos][width_pos].value = 9;  // Assign a bomb value (9) to the specified tile

    // Update adjacent tiles to increment their values if they are not bombs
    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            // Skip the iteration if the adjacent tile is out of bounds
            if ((height_pos + i) < 0 || (width_pos + j) < 0) continue;
            if ((height_pos + i) > height - 1 || (width_pos + j) > width - 1) continue;

            // Skip the iteration if the adjacent tile is already a bomb
            if (board[height_pos + i][width_pos + j].value == 9) continue;

            // Increment the value of the adjacent tile
            board[height_pos + i][width_pos + j].value += 1;
        }
    }
}


//Draw random locations for bombs in given board size with omitting placing it more than once at given place
void RandomizeBombs(int bombs_amount){
    time_t t;
    int height_pos, width_pos;
    srand((unsigned)time(&t));

    // Repeat until all bombs are placed
    while (bombs_amount>0)
    {
        // Generate random positions within the board's dimensions
        height_pos = rand()%height;
        width_pos = rand()%width;

        // Check if the selected position already has a bomb
        if (board[height_pos][width_pos].value != 9)
        {
            // Place a bomb at the selected position
            PlaceBombs(height_pos, width_pos);
            bombs_amount--;
        }
    }
}


//Reveal tiles recursively
void TileReveal(int y, int x){
    // Check if the given coordinates are out of bounds
    if (y<0 || y>height-1) return;
    if (x<0 || x>width-1) return;

    // Check if the tile is already revealed, flagged, or has a non-zero value
    if (board[y][x].is_not_hidden==true) return;
    if (board[y][x].is_flagged==true) return;
    if (board[y][x].value != 9 && board[y][x].is_not_hidden==false) {
        // Set the tile as revealed if it isn't a bomb
        board[y][x].is_not_hidden = true;
    }
    if (board[y][x].value != 0) return;

    // Recursively reveal adjacent tiles
    TileReveal(y-1, x-1);  // Top-left
    TileReveal(y-1, x);    // Top
    TileReveal(y-1, x+1);  // Top-right
    TileReveal(y+1, x-1);  // Bottom-left
    TileReveal(y+1, x);    // Bottom
    TileReveal(y+1, x+1);  // Bottom-right
    TileReveal(y, x-1);    // Left
    TileReveal(y, x+1);    // Right
}

//Display whole board according to info stored in tiles
void BoardDisplay(){
    system ("cls");

    // Iterate over each row of the board
    for (int i = 0; i<height; i++)
    {
        // Iterate over each column of the board
        for (int j = 0; j<width; j++)
        {
            // Check if the current position is the cursor position
            if (i==crsr_pos_y && j==crsr_pos_x)
            {
                // Set console text attribute to highlight the cursor position
                SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE  ), 0x02);
                std::cout << "#";
            }
            else
            {
                // Check if the tile is revealed (not hidden)
                if (board[i][j].is_not_hidden==true)
                {
                    if (board[i][j].value==0){
                        std::cout << " "; // Print an empty space for empty tiles
                    }else{
                        // Set console text attribute to display the tile value
                        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE  ), 0x06);
                        std::cout << board[i][j].value;
                    }
                }
                // Check if the tile is hidden
                if (board[i][j].is_not_hidden==false){
                    if(board[i][j].is_flagged==true){
                        // Set console text attribute to display flagged hidden tiles
                        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE  ), 0x04);
                        std::cout << "#";
                    }else{
                        // Set console text attribute to display non-flagged hidden tiles
                        SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE  ), 0x09);
                        std::cout << "#";
                    }
                }
            }
        }
        std::cout<<std::endl;
    }

    // Reset console text attribute to default
    SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), 0x0F );

    // Display cursor position and remaining flags
    std::cout << "coursor position: " << std::endl << "Y: " << crsr_pos_y+1 << std::endl << "X: " << crsr_pos_x+1 << std::endl << "You have " << flags << " flags." << std::endl;
}


//Check key inputs and perform actions tied to them
void GameFunctions(){
    bool key_press=false;

    // Check if the shift key is pressed and handle tile reveal and game end conditions
    if ((GetKeyState(shift) & 0x8000)&&board[crsr_pos_y][crsr_pos_x].is_flagged==false)
    {
        if (board[crsr_pos_y][crsr_pos_x].value==9) game_end=2;

        TileReveal(crsr_pos_y, crsr_pos_x);
        BoardDisplay();
    }

    // Check if the right arrow key is pressed and update the cursor position
    if ((GetKeyState(right_arrow) & 0x8000) && crsr_pos_x<width-1){
         crsr_pos_x++;
         key_press=true;
    }

    // Check if the left arrow key is pressed and update the cursor position
    if ((GetKeyState(left_arrow) & 0x8000) && crsr_pos_x>0){
        crsr_pos_x--;
        key_press=true;
    }

    // Check if the down arrow key is pressed and update the cursor position
    if ((GetKeyState(down_arrow) & 0x8000) && crsr_pos_y<height-1){
        crsr_pos_y++;
        key_press=true;
    }

    // Check if the up arrow key is pressed and update the cursor position
    if ((GetKeyState(up_arrow) & 0x8000) && crsr_pos_y>0){
        crsr_pos_y--;
        key_press=true;
    }

    // Check if the right control key is pressed and toggle the flag state of the current tile
    if ((GetKeyState(control_right) & 0x8000)&&board[crsr_pos_y][crsr_pos_x].is_flagged==false&&board[crsr_pos_y][crsr_pos_x].is_not_hidden==false){
        board[crsr_pos_y][crsr_pos_x].is_flagged=true;
        key_press=true;
        flags--;
    }else if ((GetKeyState(control_right) & 0x8000)&&board[crsr_pos_y][crsr_pos_x].is_flagged==true){
        board[crsr_pos_y][crsr_pos_x].is_flagged=false;
        key_press=true;
        flags++;
    }

    // Check if the question mark key is pressed and handle game saving and exit
    if((GetKeyState(question_mark) & 0x8000)){
        char should_save;
        system ("cls");
        std::cout << "Do you want to save the game and exit? (y/n)" << std::endl;
        std::cin >> should_save;
        if(should_save=='y'){
            // Get the save file name from the user
            std::string filename;
            std::cout << "Input save name: " << std::endl;
            std::cin >> filename;
            filename += ".save";

            // Create a file stream and open the save file
            std::fstream file;
            file.open(filename, std::fstream::out);

            if( file.is_open() == true ){
                // Write game data to the save file
                file<<height<<','<<width<<','<<'\n';
                file<<crsr_pos_y<<','<<crsr_pos_x<<','<<'\n';
                file<<flags<<','<<'\n';
                file<<'v'<<','<<'\n';
                for(int i=0; i<height; i++){
                    for(int j=0; j<width; j++){
                        file<<board[i][j].value<<',';
                    }
                    file<<'\n';
                }
                file<<'h'<<','<<'\n';
                for(int i=0; i<height; i++){
                    for(int j=0; j<width; j++){
                        file<<board[i][j].is_not_hidden<<',';
                    }
                    file<<'\n';
                }
                file<<'f'<<','<<'\n';
                for(int i=0; i<height; i++){
                    for(int j=0; j<width; j++){
                        file<<board[i][j].is_flagged<<',';
                    }
                    file<<'\n';
                }

            }else{
                std::cout << "Save creation error";
                return;
            }

            file.close();
            game_end=3;
        }else{
            BoardDisplay();
            return;
        }
    }

    // If any key was pressed, update the board display
    if(key_press==true){
        BoardDisplay();
        key_press=false;
    }
}


//Function used for checking if winning condition haven't been met yet
bool WinningCondition()
{
    int points = 0;
    int revealed_tiles = 0;

    //Count flagged tiles
    for (int i = 0; i<height; i++){
        for (int j = 0; j<width; j++)
        {
            if(board[i][j].is_flagged==true&&board[i][j].value==9)
                points++;
        }
    }

    //Count revealed tiles
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){
            if(board[i][j].is_not_hidden==true)
                revealed_tiles++;
        }
    }

    //Return true or false depending on all bombs being flagged and all other tiles uncovered
    if (points==ceil((height*width)/10)&&revealed_tiles==height*width-((height*width)/10)) return true;
    return false;
}

int main(){
    std::string savename;
    std::fstream file;
    system("cls");
    //fragment used to determine if player does or does not want to load saved game
    char yes_no;
        std::cout<<"Do you want to load game? (y/n)"<<std::endl;
        std::cin>>yes_no;
    switch(yes_no){
        case 'y':
           std::cout << "Input save name: " << std::endl;
            std::cin >> savename;
            savename += ".save";

            file.open(savename, std::fstream::in);

            if (file.is_open()) {
                std::string line;
                getline(file, line); // Read the first line

                // Extract height and width from the first line
                size_t commaIndex = line.find(',');
                height = std::stoi(line.substr(0, commaIndex));
                width = std::stoi(line.substr(commaIndex + 1));

                // Create the board with the loaded dimensions
                board = new tile *[height];
                for (int i = 0; i < height; i++) {
                    board[i] = new tile[width];
                }

                // Read the cursor position
                getline(file, line);
                commaIndex = line.find(',');
                crsr_pos_y = std::stoi(line.substr(0, commaIndex));
                crsr_pos_x = std::stoi(line.substr(commaIndex + 1));

                // Read the flags count
                getline(file, line);
                flags = std::stoi(line);

                // Read the 'v' character to indicate the start of the board values
                getline(file, line);

                // Read the board values from the file
                for (int i = 0; i < height; i++) {
                    getline(file, line);
                    size_t start = 0;
                    for (int j = 0; j < width; j++) {
                        size_t commaIndex = line.find(',', start);
                        board[i][j].value = std::stoi(line.substr(start, commaIndex - start));
                        start = commaIndex + 1;
                    }
                }

                // Read the 'h' character to indicate the start of the hidden tiles
                getline(file, line);

                // Read the hidden tiles from the file
                for (int i = 0; i < height; i++) {
                    getline(file, line);
                    size_t start = 0;
                    for (int j = 0; j < width; j++) {
                        size_t commaIndex = line.find(',', start);
                        board[i][j].is_not_hidden = std::stoi(line.substr(start, commaIndex - start));
                        start = commaIndex + 1;
                    }
                }

                // Read the 'f' character to indicate the start of the flagged tiles
                getline(file, line);

                // Read the flagged tiles from the file
                for (int i = 0; i < height; i++) {
                    getline(file, line);
                    size_t start = 0;
                    for (int j = 0; j < width; j++) {
                        size_t commaIndex = line.find(',', start);
                        board[i][j].is_flagged = std::stoi(line.substr(start, commaIndex - start));
                        start = commaIndex + 1;
                    }
                }

                file.close();
                
                //Run the game
                game_end = 0;
                BoardDisplay();
                while(game_end==0){
                    Sleep(60);
                    GameFunctions();
                    if (WinningCondition()==true) game_end=1;
                }

                //Determine ending text
                if (game_end==1) std::cout << "You win!";
                if (game_end==2) std::cout << "You lose!";
                if (game_end==3) std::cout << "Game saved";
                system ("pause >nul");
            } else {
                std::cout << "Failed to load the save file";
            }
            break;
        case 'n':
            
            //Fragment used to generate new board with bombs in given bounds
            while(GenerateBoard()){
                std::cout<<"Incorrect board size"<<std::endl;
            }
            FillBoard();
            flags=(height*width)/10;
            RandomizeBombs(ceil(((height*width)/10)));
            
            //Run the game
            BoardDisplay();
            while(game_end==0){
                Sleep(60);
                GameFunctions();
                if (WinningCondition()==true) game_end=1;
            }

            //Determine ending text
            if (game_end==1) std::cout << "You win!";
            if (game_end==2) std::cout << "You lose!";
            if (game_end==3) std::cout << "Game saved";
            system ("pause >nul");
            break;

    }

    //Clear memory
    for(int i = 0; i<height; i++){
        delete[] board[i];
    }
    delete[] board;
return 0;
}