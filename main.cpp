#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace sf;

//--------------------------------------------------
//FUNCTION PROTOTYPES
//--------------------------------------------------

void enterName(RenderWindow& window);
void loadAssets();
void showMenu(RenderWindow& window);
void showPlayers(RenderWindow& window);
void drawMenu(RenderWindow& window);
void playGame(RenderWindow& window);
void initSnake();
void moveSnake();
void drawSnake(RenderWindow& window);
void handleGameInput(Keyboard::Key key);
void spawnFood();
void drawFood(RenderWindow& window);
void checkFoodCollision();
void drawScore(RenderWindow& window); 
void saveScore();
void showScores(RenderWindow& window);
void checkGameCollision();
void generateObstacles();
void drawObstacles(RenderWindow& window);
void checkObstacleCollision();
void showGameOver(RenderWindow& window);
void drawPauseScreen(RenderWindow& window);
void resetGame();

//--------------------------------------------------
//GLOBAL VARIABLES
//--------------------------------------------------

Font font;

Texture backgroundTexture;
Sprite* backgroundSprite = nullptr;

Music backgroundMusic;

SoundBuffer startBuffer;
SoundBuffer gameOverBuffer;
SoundBuffer eatBuffer;

Sound* startSound = nullptr;
Sound* gameOverSound = nullptr;
Sound* eatSound = nullptr;

Texture headTexture;
Texture bodyTexture;
Texture foodTexture;

Texture obstacleTexture; 

int* obstacleX = nullptr; 
int* obstacleY = nullptr;

int obstacleCount = 20; // Number of obstacles

int foodX = 0;
int foodY = 0;
int score = 0;

//size of the snake
int cellSize = 60; 

//movement speed of the snake in milliseconds
int moveDelay = 200;

////SFML clock to control snake movement timing
Clock moveClock; 

int gameState = 0;
int menuSelect = 0;

char playerName[50] = { '\0' };
int nameIndex = 0;

bool nameEntered = false;
bool scoreSaved = false;
bool gameStarted = false;
bool gamePaused = false;

//snake dynamic memory
int* snakeX = nullptr;
int* snakeY = nullptr;

int snakeSize = 5;
int direction = 1;

//--------------------------------------------------
//MAIN
//--------------------------------------------------

int main()
{
    //redering window and set size and title 
    RenderWindow window(VideoMode({ 1600u, 1600u }),
        "NOKIA SNAKE GAME -- Muhammad Nouman & Aqsa Ismail -- Press P to Pause During Gameplay");

	//calling function to load assets from project folder
    loadAssets(); 

	//main game loop
    while (window.isOpen())
    {
		//event handling
        while (const auto event = window.pollEvent())
        {
			//close window event
            if (event->is<Event::Closed>())
            {
                window.close();
            }

			//keyboard input handling
            if (const auto* keyPressed = event->getIf<Event::KeyPressed>())
            {
				//handle input based on game state
                if (gameState == 0)
                {
					//menu input handling
                    if (keyPressed->code == Keyboard::Key::Up)
                    {
                        menuSelect--;

                        if (menuSelect < 0)
                        {
                            menuSelect = 4;
                        }
                    }

                    if (keyPressed->code == Keyboard::Key::Down)
                    {
                        menuSelect++;

                        if (menuSelect > 4)
                        {
                            menuSelect = 0;
                        }
                    }

                    if (keyPressed->code == Keyboard::Key::Enter)
                    {
                        if (menuSelect == 0)
                        {
                            gameState = 1; 
                            resetGame();
                            initSnake();
                            spawnFood();

                            if (startSound != nullptr)
                            {
                                startSound->play();
                            }
                        }

                        else if (menuSelect == 1)
                        {
							gameState = 2; 
                            resetGame();
                            initSnake();
                            generateObstacles();
                            spawnFood();

                            if (startSound != nullptr)
                            {
                                startSound->play();
                            }
                        }

                        else if (menuSelect == 2)
                        {
                            gameState = 3; //registered players
                        }

                        else if (menuSelect == 3)
                        {
                            gameState = 4; //scores
                        }

                        else if (menuSelect == 4)
                        {
                            window.close();
                        }
                    }
                }

                //input handling of the game 
                //if game is started either normal or obstacle then call handlegameinput
                if (gameState == 1 || gameState == 2)
                {
					handleGameInput(keyPressed->code); //called function to handle game input
                }
            }
        }

		//game logic and rendering based on game state
        if (!nameEntered)
        {
			enterName(window); //call function to enter player name
        }
        else if (gameState == 0)
        {
			showMenu(window); //call function to show menu
        }
        else if (gameState == 3)
        {
			showPlayers(window); //call function to show registered players
        }
        else if (gameState == 4)
        {
			showScores(window); //call function to show scores
        }
        else if (gameState == 5)
        {
			showGameOver(window); //call function to show game over screen
        }
        else
        {
			playGame(window); //call function to play game (either normal or with obstacles)
        }
    }

    //--------------------------------------------------
    //MEMORY CLEANUP
    //--------------------------------------------------

    if (backgroundSprite != nullptr)
    {
        delete backgroundSprite;
        backgroundSprite = nullptr;
    }

    if (startSound != nullptr)
    {
        delete startSound;
        startSound = nullptr;
    }

    if (gameOverSound != nullptr)
    {
        delete gameOverSound;
        gameOverSound = nullptr;
    }

    if (eatSound != nullptr)
    {
        delete eatSound;
        eatSound = nullptr;
    }

    if (snakeX != nullptr)
    {
        delete[] snakeX;
        snakeX = nullptr;
    }

    if (snakeY != nullptr)
    {
        delete[] snakeY;
        snakeY = nullptr;
    }

    if (obstacleX != nullptr)
    {
        delete[] obstacleX;
        obstacleX = nullptr;
    }

    if (obstacleY != nullptr)
    {
        delete[] obstacleY;
        obstacleY = nullptr;
    }

    return 0;
}

//--------------------------------------------------
//LOAD ASSETS
//--------------------------------------------------

void loadAssets()
{
	//load all assets from project folder and handle errors if any asset fails to load
    if (!font.openFromFile("assets/font.ttf"))
    {
        cout << "Font Load Error" << endl;
    }

    if (!backgroundTexture.loadFromFile("assets/snake_background.png"))
    {
        cout << "Background Load Error" << endl;
    }

    if (!headTexture.loadFromFile("assets/snake_head.png"))
    {
        cout << "Snake Head Load Error" << endl;
    }

    if (!bodyTexture.loadFromFile("assets/snake_body.png"))
    {
        cout << "Snake Body Load Error" << endl;
    }

    if (!foodTexture.loadFromFile("assets/snake_food.png"))
    {
        cout << "Food Load Error" << endl;
    }
    if (!obstacleTexture.loadFromFile("assets/snake_obstacle.png"))
    {
        cout << "Obstacle Load Error" << endl;
    }

    backgroundSprite = new Sprite(backgroundTexture);

    //scale background
    backgroundSprite->setScale({ 2.0f, 2.0f });

	//load sounds
    if (!startBuffer.loadFromFile("assets/game_start.wav"))
    {
        cout << "Start Sound Error" << endl;
    }

    if (!gameOverBuffer.loadFromFile("assets/game_over.wav"))
    {
        cout << "Game Over Sound Error" << endl;
    }

    if (!eatBuffer.loadFromFile("assets/apple_crunch.wav"))
    {
        cout << "Eat Sound Error" << endl;
    }

    startSound = new Sound(startBuffer);
    gameOverSound = new Sound(gameOverBuffer);
    eatSound = new Sound(eatBuffer);

    if (!backgroundMusic.openFromFile("assets/background_music.wav"))
    {
        cout << "Music Load Error" << endl;
    }

	backgroundMusic.setLooping(true); //set music to loop
	backgroundMusic.play(); //play background music
     
    //create score files if missing

    ofstream write;

    write.open("normal_scores.txt", ios::app);
    write.close();

    write.open("obstacle_scores.txt", ios::app);
    write.close();

    write.open("user_record.txt", ios::app);
    write.close();
}

//--------------------------------------------------
//SHOW MENU
//--------------------------------------------------

void showMenu(RenderWindow& window)
{
    window.clear();

    if (backgroundSprite != nullptr)
    {
        window.draw(*backgroundSprite);
    }

    drawMenu(window);

    window.display();
}

//--------------------------------------------------
//ENTER NAME OF PLAYER
//--------------------------------------------------

void enterName(RenderWindow& window)
{
	//function to enter player name and handle input for name entry
    while (!nameEntered && window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }

			//handle text input for name entry
            if (const auto* textEntered = event->getIf<Event::TextEntered>())
            {
				char ch = static_cast<char>(textEntered->unicode); //convert unicode to char

                if (ch >= 32 && ch <= 126) 
                {
                    if (nameIndex < 49)
                    {
                        playerName[nameIndex] = ch;
                        nameIndex++;
                        playerName[nameIndex] = '\0';
                    }
                }
            }

            if (const auto* keyPressed = event->getIf<Event::KeyPressed>()) 
            {
                if (keyPressed->code == Keyboard::Key::Backspace)
                {
                    if (nameIndex > 0)
                    {
                        nameIndex--;
                        playerName[nameIndex] = '\0';
                    }
                }

                if (keyPressed->code == Keyboard::Key::Enter)
                {
                    if (playerName[0] != '\0')
                    {
                        ofstream write;

                        write.open("user_record.txt", ios::app);

                        if (write.is_open())
                        {
                            write << playerName << endl;
                            write.close();
                        }

                        nameEntered = true;
                    }
                }
            }
        }

        window.clear();

        if (backgroundSprite != nullptr)
        {
            window.draw(*backgroundSprite);
        }

		//display name entry prompt and current input

        Text title(font);
        title.setString("WELCOME TO SNAKE GAME");
        title.setCharacterSize(80);
        title.setPosition({ 280.f, 300.f });
		title.setFillColor(Color::Magenta);

        Text instruction(font);
        instruction.setString("ENTER YOUR NAME");
        instruction.setCharacterSize(50);
        instruction.setPosition({ 500.f, 500.f });
		instruction.setFillColor(Color::Magenta);

        RectangleShape box(Vector2f(600.f, 80.f));
        box.setPosition({ 500.f, 650.f });
        box.setFillColor(Color::Green);
        box.setOutlineThickness(3.f);
        box.setOutlineColor(Color::White);

        Text nameText(font);
        nameText.setString(playerName);
        nameText.setCharacterSize(40);
        nameText.setPosition({ 520.f, 665.f });
		nameText.setFillColor(Color::Black);

        window.draw(title);
        window.draw(instruction);
        window.draw(box);
        window.draw(nameText);

        window.display();
    }
}

//--------------------------------------------------
//DRAW MENU
//--------------------------------------------------

void drawMenu(RenderWindow& window)
{
    Text title(font);
    title.setString("NOKIA SNAKE GAME");
    title.setCharacterSize(90);
    title.setPosition({ 350.f, 200.f });

    Text normal(font);
    normal.setString("NORMAL MODE");
    normal.setCharacterSize(55);
    normal.setPosition({ 550.f, 600.f });

    Text obstacle(font);
    obstacle.setString("OBSTACLE MODE");
    obstacle.setCharacterSize(55);
    obstacle.setPosition({ 550.f, 720.f });

    Text players(font);
    players.setString("REGISTERED PLAYERS");
    players.setCharacterSize(55);
    players.setPosition({ 550.f, 840.f });

    Text high(font);
    high.setString("HIGH SCORES");
    high.setCharacterSize(55);
    high.setPosition({ 550.f, 960.f });

    Text exit(font);
    exit.setString("EXIT");
    exit.setCharacterSize(55);
    exit.setPosition({ 550.f, 1080.f });

    normal.setFillColor(Color::White);
    obstacle.setFillColor(Color::White);
    players.setFillColor(Color::White);
    high.setFillColor(Color::White);
    exit.setFillColor(Color::White);

    if (menuSelect == 0)
    {
        normal.setFillColor(Color::Red);
    }

    if (menuSelect == 1)
    {
        obstacle.setFillColor(Color::Red);
    }

    if (menuSelect == 2)
    {
        players.setFillColor(Color::Red);
    }

    if (menuSelect == 3)
    {
        high.setFillColor(Color::Red);
    }

    if (menuSelect == 4)
    {
        exit.setFillColor(Color::Red);
    }

    window.draw(title);
    window.draw(normal);
    window.draw(obstacle);
    window.draw(players);
    window.draw(high);
    window.draw(exit);
}

//--------------------------------------------------
//SHOW REGISTERED PLAYERS
//--------------------------------------------------

void showPlayers(RenderWindow& window)
{
    ifstream read;

    read.open("user_record.txt");

    char** names = new char* [100];

    for (int i = 0; i < 100; i++)
    {
        names[i] = new char[50];

        for (int j = 0; j < 50; j++)
        {
            names[i][j] = '\0';
        }
    }

    int totalPlayers = 0;

    if (read.is_open())
    {
        while (!read.eof() && totalPlayers < 100)
        {
            read.getline(names[totalPlayers], 50);

            if (read)
            {
                totalPlayers++;
            }
        }

        read.close();
    }

    while (gameState == 3 && window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<Event::KeyPressed>())
            {
                if (keyPressed->code == Keyboard::Key::Escape)
                {
                    gameState = 0;
                }
            }
        }

        window.clear();

        if (backgroundSprite != nullptr)
        {
            window.draw(*backgroundSprite);
        }

        Text title(font);

        title.setString("REGISTERED PLAYERS");
        title.setCharacterSize(70);
        title.setPosition({ 350.f, 150.f });
        title.setFillColor(Color::Black);

        window.draw(title);

        for (int i = 0; i < totalPlayers; i++)
        {
            char displayText[100] = { '\0' };
            char separator[3] = ". ";

            int index = 0;

            int number = i + 1;

            if (number >= 10)
            {
                displayText[index] = (number / 10) + '0';
                index++;

                displayText[index] = (number % 10) + '0';
                index++;
            }
            else
            {
                displayText[index] = number + '0';
                index++;
            }

            int sepIndex = 0;

            while (separator[sepIndex] != '\0')
            {
                displayText[index] = separator[sepIndex];

                index++;
                sepIndex++;
            }

            int j = 0;

            while (names[i][j] != '\0')
            {
                displayText[index] = names[i][j];

                index++;
                j++;
            }

            displayText[index] = '\0';

            Text player(font);

            player.setString(displayText);
            player.setCharacterSize(45);
            player.setFillColor(Color::Red);
            player.setPosition({ 450.f, 300.f + (i * 80.f) });

            window.draw(player);
        }

        Text back(font);

        back.setString("PRESS ESC TO RETURN");
        back.setCharacterSize(35);
        back.setPosition({ 500.f, 1400.f });
        back.setFillColor(Color::Magenta);

        window.draw(back);

        window.display();
    }

    for (int i = 0; i < 100; i++)
    {
        delete[] names[i];
        names[i] = nullptr;
    }

    delete[] names;
    names = nullptr;
}

//--------------------------------------------------
//SAVE SCORE
//--------------------------------------------------

void saveScore()
{
	if (scoreSaved)  // Check if the score has already been saved
	{
		return;  // Exit the function if the score is already saved
	}

    if (score < 0) // Check if the score is negative
    {
        return; // Exit the function if the score is not positive
    }

    ofstream write;

    if (gameState == 1) 
    {
        write.open("normal_scores.txt", ios::app);
    }
    else if (gameState == 2)
    {
        write.open("obstacle_scores.txt", ios::app);
    }

    if (write.is_open())
    {
        write << playerName << "," << score << endl;
        write.close();

        scoreSaved = true;
    }
}

//--------------------------------------------------
//SHOW SCORES
//--------------------------------------------------

void showScores(RenderWindow& window)
{
    int normalCount = 0;
    int obstacleCount = 0;

    char tempName[50] = { '\0' };
    int tempScore = 0;

    ifstream read;

    //count normal scores

    read.open("normal_scores.txt");

    if (read.is_open())
    {
        char line[100] = { '\0' };

        while (!read.eof())
        {
            read.getline(line, 100);

            if (read)
            {
                normalCount++;
            }
        }

        read.close();
    }

    //count obstacle scores

    read.open("obstacle_scores.txt");

    if (read.is_open())
    {
        char line[100] = { '\0' };

        while (!read.eof())
        {
            read.getline(line, 100);

            if (read)
            {
                obstacleCount++;
            }
        }

        read.close();
    }

    //dynamic memory for normal names and scores

    char** normalNames = nullptr;
    int* normalScores = nullptr;

    //dynamic memory for obstacle names and scores

    char** obstacleNames = nullptr;
    int* obstacleScores = nullptr;

	//allocate memory to store normal scores and names

    if (normalCount > 0)
    {
        normalNames = new char* [normalCount];
        normalScores = new int[normalCount];
    }

	//allocate memory to store obstacle scores and names

    if (obstacleCount > 0)
    {
        obstacleNames = new char* [obstacleCount];
        obstacleScores = new int[obstacleCount];
    }

    //read normal scores

    read.open("normal_scores.txt");

    if (read.is_open())
    {
        for (int i = 0; i < normalCount; i++)
        {
            char line[100] = { '\0' };

            read.getline(line, 100);

            int k = 0;
            int nameLength = 0;

            while (line[k] != ',' && line[k] != '\0')
            {
                tempName[nameLength] = line[k];

                nameLength++;
                k++;
            }

            tempName[nameLength] = '\0';

            if (line[k] == ',')
            {
                k++;
            }

            normalScores[i] = 0;

            while (line[k] != '\0')
            {
                normalScores[i] = normalScores[i] * 10 + (line[k] - '0');

                k++;
            }

            int length = 0;

            while (tempName[length] != '\0')
            {
                length++;
            }

            normalNames[i] = new char[length + 1];

            for (int j = 0; j < length; j++)
            {
                normalNames[i][j] = tempName[j];
            }

            normalNames[i][length] = '\0';
        }

        read.close();
    }

    //read obstacle scores

    read.open("obstacle_scores.txt");

    if (read.is_open())
    {
        for (int i = 0; i < obstacleCount; i++)
        {
            char line[100] = { '\0' };

            read.getline(line, 100);

            int k = 0;
            int nameLength = 0;

            while (line[k] != ',' && line[k] != '\0')
            {
                tempName[nameLength] = line[k];

                nameLength++;
                k++;
            }

            tempName[nameLength] = '\0';

            if (line[k] == ',')
            {
                k++;
            }

            obstacleScores[i] = 0;

            while (line[k] != '\0')
            {
                obstacleScores[i] = obstacleScores[i] * 10 + (line[k] - '0');

                k++;
            };

            int length = 0;

            while (tempName[length] != '\0')
            {
                length++;
            }

            obstacleNames[i] = new char[length + 1];

            for (int j = 0; j < length; j++)
            {
                obstacleNames[i][j] = tempName[j];
            }

            obstacleNames[i][length] = '\0';
        }

        read.close();
    }

	//sort normal scores in descending order
    for (int i = 0; i < normalCount - 1; i++)
    {
        for (int j = 0; j < normalCount - i - 1; j++)
        {
            if (normalScores[j] < normalScores[j + 1])
            {
                int tempScore = normalScores[j];
                normalScores[j] = normalScores[j + 1];
                normalScores[j + 1] = tempScore;

                char* tempName = normalNames[j];
                normalNames[j] = normalNames[j + 1];
                normalNames[j + 1] = tempName;
            }
        }
    }

	//sort obstacle scores in descending order
    for (int i = 0; i < obstacleCount - 1; i++)
    {
        for (int j = 0; j < obstacleCount - i - 1; j++)
        {
            if (obstacleScores[j] < obstacleScores[j + 1])
            {
                int tempScore = obstacleScores[j];
                obstacleScores[j] = obstacleScores[j + 1];
                obstacleScores[j + 1] = tempScore;

                char* tempName = obstacleNames[j];
                obstacleNames[j] = obstacleNames[j + 1];
                obstacleNames[j + 1] = tempName;
            }
        }
    }

	//display scores
    while (gameState == 4 && window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<Event::KeyPressed>())
            {
                if (keyPressed->code == Keyboard::Key::Escape)
                {
                    gameState = 0;
                }
            }
        }

        window.clear();

        if (backgroundSprite != nullptr)
        {
            window.draw(*backgroundSprite);
        }

        RectangleShape normalBox(Vector2f(650.f, 1050.f));
        normalBox.setPosition({ 100.f, 250.f });
        normalBox.setFillColor(Color(0, 100, 0, 180));
        normalBox.setOutlineThickness(5.f);
        normalBox.setOutlineColor(Color::White);

        RectangleShape obstacleBox(Vector2f(650.f, 1050.f));
        obstacleBox.setPosition({ 850.f, 250.f });
        obstacleBox.setFillColor(Color(100, 0, 0, 180));
        obstacleBox.setOutlineThickness(5.f);
        obstacleBox.setOutlineColor(Color::White);

        Text title(font);
        title.setString("HIGH SCORES");
        title.setCharacterSize(80);
        title.setPosition({ 590.f, 80.f });
        title.setFillColor(Color::Black);

        Text normalTitle(font);
        normalTitle.setString("NORMAL MODE");
        normalTitle.setCharacterSize(45);
        normalTitle.setPosition({ 250.f, 280.f });
        normalTitle.setFillColor(Color::White);

        Text obstacleTitle(font);
        obstacleTitle.setString("OBSTACLE MODE");
        obstacleTitle.setCharacterSize(45);
        obstacleTitle.setPosition({ 990.f, 280.f });
        obstacleTitle.setFillColor(Color::White);

        window.draw(normalBox);
        window.draw(obstacleBox);
        window.draw(title);
        window.draw(normalTitle);
        window.draw(obstacleTitle);

		// Display normal scores
        for (int i = 0; i < normalCount; i++)
        {
            char displayText[100] = { '\0' };

            int index = 0;

            int number = i + 1;

            if (number >= 10)
            {
                displayText[index] = (number / 10) + '0';
                index++;

                displayText[index] = (number % 10) + '0';
                index++;
            }
            else
            {
                displayText[index] = number + '0';
                index++;
            }

            displayText[index] = '.';
            index++;

            displayText[index] = ' ';
            index++;

            int j = 0;

            while (normalNames[i][j] != '\0')
            {
                displayText[index] = normalNames[i][j];
                index++;
                j++;
            }

            displayText[index] = ' ';
            index++;

            displayText[index] = '-';
            index++;

            displayText[index] = ' ';
            index++;

            int scoreNumber = normalScores[i];

            if (scoreNumber == 0)
            {
                displayText[index] = '0';
                index++;
            }
            else
            {
                char digits[10] = { '\0' };
                int digitCount = 0;

                while (scoreNumber > 0)
                {
                    digits[digitCount] = char((scoreNumber % 10) + '0');
                    scoreNumber = scoreNumber / 10;
                    digitCount++;
                }

                for (int k = digitCount - 1; k >= 0; k--)
                {
                    displayText[index] = digits[k];
                    index++;
                }
            }

            displayText[index] = '\0';

            Text line(font);
            line.setString(displayText);
            line.setCharacterSize(35);
            line.setPosition({ 150.f, 380.f + (i * 70.f) });
            line.setFillColor(Color::Yellow);

            window.draw(line);
        }

		// Display obstacle scores
        for (int i = 0; i < obstacleCount; i++)
        {
            char displayText[100] = { '\0' };

            int index = 0;

            int number = i + 1;

            if (number >= 10)
            {
                displayText[index] = (number / 10) + '0';
                index++;

                displayText[index] = (number % 10) + '0';
                index++;
            }
            else
            {
                displayText[index] = number + '0';
                index++;
            }

            displayText[index] = '.';
            index++;

            displayText[index] = ' ';
            index++;

            int j = 0;

            while (obstacleNames[i][j] != '\0')
            {
                displayText[index] = obstacleNames[i][j];
                index++;
                j++;
            }

            displayText[index] = ' ';
            index++;

            displayText[index] = '-';
            index++;

            displayText[index] = ' ';
            index++;

            int scoreNumber = obstacleScores[i];

            if (scoreNumber == 0)
            {
                displayText[index] = '0';
                index++;
            }
            else
            {
                char digits[10] = { '\0' };
                int digitCount = 0;

                while (scoreNumber > 0)
                {
                    digits[digitCount] = char((scoreNumber % 10) + '0');
                    scoreNumber = scoreNumber / 10;
                    digitCount++;
                }

                for (int k = digitCount - 1; k >= 0; k--)
                {
                    displayText[index] = digits[k];
                    index++;
                }
            }

            displayText[index] = '\0';

            Text line(font);
            line.setString(displayText);
            line.setCharacterSize(35);
            line.setPosition({ 900.f, 380.f + (i * 70.f) });
            line.setFillColor(Color::Cyan);

            window.draw(line);
        }

        Text back(font);
        back.setString("PRESS ESC TO RETURN");
        back.setCharacterSize(35);
        back.setPosition({ 520.f, 1400.f });
        back.setFillColor(Color::Magenta);

        window.draw(back);

        window.display();
    }

    if (normalNames != nullptr)
    {
        for (int i = 0; i < normalCount; i++)
        {
            delete[] normalNames[i];
            normalNames[i] = nullptr;
        }

        delete[] normalNames;
        normalNames = nullptr;
    }

    if (normalScores != nullptr)
    {
        delete[] normalScores;
        normalScores = nullptr;
    }

    if (obstacleNames != nullptr)
    {
        for (int i = 0; i < obstacleCount; i++)
        {
            delete[] obstacleNames[i];
            obstacleNames[i] = nullptr;
        }

        delete[] obstacleNames;
        obstacleNames = nullptr;
    }

    if (obstacleScores != nullptr)
    {
        delete[] obstacleScores;
        obstacleScores = nullptr;
    }
}

//--------------------------------------------------
//GAME PLACEHOLDER
//--------------------------------------------------

void playGame(RenderWindow& window)
{
    if (gamePaused == false)
    {
        if (moveClock.getElapsedTime().asMilliseconds() >= moveDelay)
        {
            moveSnake();

            checkFoodCollision();

            checkGameCollision();

            moveClock.restart();
        }
    }

    window.clear();

    if (backgroundSprite != nullptr)
    {
        window.draw(*backgroundSprite);
    }

    drawFood(window);

    drawObstacles(window);

    drawSnake(window);

    drawScore(window);

    if (gamePaused == true)
    {
        drawPauseScreen(window);
    }

    window.display();
}

//--------------------------------------------------
//INITIALIZE SNAKE
//--------------------------------------------------

void initSnake()
{
    snakeSize = 2;
    direction = 1;

    snakeX[0] = 13;
    snakeY[0] = 13;

    snakeX[1] = 12;
    snakeY[1] = 13;
}

//--------------------------------------------------
//HANDLE GAME INPUT
//--------------------------------------------------

void handleGameInput(Keyboard::Key key)
{
    //right

    if (key == Keyboard::Key::Right || key == Keyboard::Key::D)
    {
        //prevent reverse movement

        if (direction != 2)
        {
            direction = 1;
        }
    }

    //left

    if (key == Keyboard::Key::Left || key == Keyboard::Key::A)
    {
        if (direction != 1)
        {
            direction = 2;
        }
    }

    //up

    if (key == Keyboard::Key::Up || key == Keyboard::Key::W)
    {
        if (direction != 4)
        {
            direction = 3;
        }
    }

    //down

    if (key == Keyboard::Key::Down || key == Keyboard::Key::S)
    {
        if (direction != 3)
        {
            direction = 4;
        }
    }

    if (key == Keyboard::Key::P)
    {
        if (gamePaused == false)
        {
            gamePaused = true;
        }
        else
        {
            gamePaused = false;
        }
    }

    //escape back to menu

    if (key == Keyboard::Key::Escape)
    {
        saveScore();

        gameState = 0;
    }
}

//--------------------------------------------------
//MOVE SNAKE
//--------------------------------------------------

void moveSnake()
{
    //move body

    for (int i = snakeSize - 1; i > 0; i--)
    {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    //move head

    //right

    if (direction == 1)
    {
        snakeX[0]++;
    }

    //left

    if (direction == 2)
    {
        snakeX[0]--;
    }

    //up

    if (direction == 3)
    {
        snakeY[0]--;
    }

    //down

    if (direction == 4)
    {
        snakeY[0]++;
    }
}

//--------------------------------------------------
//DRAW SNAKE
//--------------------------------------------------

void drawSnake(RenderWindow& window)
{
    Sprite head(headTexture);
    Sprite body(bodyTexture);

    //scale sprites

    float headScaleX = float(cellSize) / float(headTexture.getSize().x);
    float headScaleY = float(cellSize) / float(headTexture.getSize().y);

    float bodyScaleX = float(cellSize) / float(bodyTexture.getSize().x);
    float bodyScaleY = float(cellSize) / float(bodyTexture.getSize().y);

    head.setScale({ headScaleX, headScaleY });
    body.setScale({ bodyScaleX, bodyScaleY });

    //set centered origin for proper rotation

    head.setOrigin({ float(headTexture.getSize().x) / 2.f, float(headTexture.getSize().y) / 2.f });
    body.setOrigin({ float(bodyTexture.getSize().x) / 2.f, float(bodyTexture.getSize().y) / 2.f });

    //rotate head according to direction

    if (direction == 1)
    {
        head.setRotation(degrees(0));
    }

    if (direction == 2)
    {
        head.setRotation(degrees(180));
    }

    if (direction == 3)
    {
        head.setRotation(degrees(270));
    }

    if (direction == 4)
    {
        head.setRotation(degrees(90));
    }

    //draw body

    for (int i = snakeSize - 1; i >= 1; i--)
    {
        if (snakeY[i] == snakeY[i - 1])
        {
            body.setRotation(degrees(0));
        }
        else
        {
            body.setRotation(degrees(90));
        }

        body.setPosition
        ({ float(snakeX[i] * cellSize) + float(cellSize) / 2.f,  float(snakeY[i] * cellSize) + float(cellSize) / 2.f });
        window.draw(body);
    }

    //draw head

    head.setPosition({ float(snakeX[0] * cellSize) + float(cellSize) / 2.f,  float(snakeY[0] * cellSize) + float(cellSize) / 2.f });
    window.draw(head);
}

//--------------------------------------------------
//SPAWN FOOD
//--------------------------------------------------

void spawnFood()
{
    bool validPosition = false;

    while (!validPosition)
    {
        foodX = rand() % 25;
        foodY = rand() % 25;

        validPosition = true;

        for (int i = 0; i < snakeSize; i++)
        {
            if (*(snakeX + i) == foodX &&
                *(snakeY + i) == foodY)
            {
                validPosition = false;
            }
        }
        if (gameState == 2)
        {
            for (int i = 0; i < obstacleCount; i++)
            {
                if (obstacleX[i] == foodX &&
                    obstacleY[i] == foodY)
                {
                    validPosition = false;
                }
            }
        }
    }
}

//--------------------------------------------------
//DRAW FOOD
//--------------------------------------------------

void drawFood(RenderWindow& window)
{
    Sprite* food = new Sprite(foodTexture);

    float foodScaleX = float(cellSize) / float(foodTexture.getSize().x);
    float foodScaleY = float(cellSize) / float(foodTexture.getSize().y);

    food->setScale({ foodScaleX, foodScaleY });

    food->setPosition({ float(foodX * cellSize),float(foodY * cellSize) });

    window.draw(*food);

    delete food;
    food = nullptr;
}

//--------------------------------------------------
//CHECK FOOD COLLISION
//--------------------------------------------------

void checkFoodCollision()
{
    if (*(snakeX + 0) == foodX &&
        *(snakeY + 0) == foodY)
    {
        if (snakeSize < 499)
        {
            snakeSize++;
        }

        score++;

        if (score % 5 == 0)
        {
            moveDelay = moveDelay - 20;

            if (moveDelay < 60)
            {
                moveDelay = 60;
            }
        }

        if (eatSound != nullptr)
        {
            eatSound->play();
        }

        spawnFood();
    }
}

//--------------------------------------------------
//DRAW SCORE
//--------------------------------------------------

void drawScore(RenderWindow& window)
{
    char scoreText[30] = "SCORE: ";

    int index = 0;

    while (scoreText[index] != '\0')
    {
        index++;
    }

    int number = score;

    if (number == 0)
    {
        scoreText[index] = '0';
        index++;
    }
    else
    {
        char digits[10] = { '\0' };

        int digitCount = 0;

        while (number > 0)
        {
            digits[digitCount] = char((number % 10) + '0');

            number = number / 10;

            digitCount++;
        }

        for (int i = digitCount - 1; i >= 0; i--)
        {
            scoreText[index] = digits[i];

            index++;
        }
    }

    scoreText[index] = '\0';

    Text text(font);
    text.setString(scoreText);
    text.setCharacterSize(45);
    text.setPosition({ 50.f, 40.f });
    text.setFillColor(Color::White);
    window.draw(text);
}

//--------------------------------------------------
//CHECK GAME COLLISION
//--------------------------------------------------

void checkGameCollision()
{
    //wall collision

    if (snakeX[0] < 0 || snakeX[0] >= 25 ||
        snakeY[0] < 0 || snakeY[0] >= 25)
    {
        saveScore();

        if (gameOverSound != nullptr)
        {
            gameOverSound->play();
        }

        gameState = 5;
    }

    //self collision

    for (int i = 1; i < snakeSize; i++)
    {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i])
        {
            saveScore();

            if (gameOverSound != nullptr)
            {
                gameOverSound->play();
            }

            gameState = 5;
        }
    }

    //check if snake has collision with obstacle 

    checkObstacleCollision();
}

//--------------------------------------------------
//SHOW GAME OVER
//--------------------------------------------------

void showGameOver(RenderWindow& window)
{
    while (gameState == 5 && window.isOpen())
    {
        while (const auto event = window.pollEvent())
        {
            if (event->is<Event::Closed>())
            {
                window.close();
            }

            if (const auto* keyPressed = event->getIf<Event::KeyPressed>())
            {
                if (keyPressed->code == Keyboard::Key::Escape)
                {
                    gameState = 0;
                }
            }
        }

        window.clear();

        if (backgroundSprite != nullptr)
        {
            window.draw(*backgroundSprite);
        }

        Text title(font);
        title.setString("GAME OVER");
        title.setCharacterSize(100);
        title.setPosition({ 500.f, 400.f });
        title.setFillColor(Color::Red);

        Text finalScore(font);

        char scoreText[50] = "FINAL SCORE: ";

        int index = 0;

        while (scoreText[index] != '\0')
        {
            index++;
        }

        int number = score;

        if (number == 0)
        {
            scoreText[index] = '0';
            index++;
        }
        else
        {
            char digits[10] = { '\0' };
            int digitCount = 0;

            while (number > 0)
            {
                digits[digitCount] = char((number % 10) + '0');
                number = number / 10;
                digitCount++;
            }

            for (int i = digitCount - 1; i >= 0; i--)
            {
                scoreText[index] = digits[i];
                index++;
            }
        }

        scoreText[index] = '\0';

        finalScore.setString(scoreText);
        finalScore.setCharacterSize(55);
        finalScore.setPosition({ 560.f, 600.f });
        finalScore.setFillColor(Color::White);

        Text back(font);
        back.setString("PRESS ESC TO RETURN MENU");
        back.setCharacterSize(45);
        back.setPosition({ 420.f, 780.f });
        back.setFillColor(Color::Magenta);

        window.draw(title);
        window.draw(finalScore);
        window.draw(back);

        window.display();
    }
}

//--------------------------------------------------
//GENERATE OBSTACLES
//--------------------------------------------------

void generateObstacles()
{
    for (int i = 0; i < obstacleCount; i++)
    {
        bool validPosition = false;

        while (!validPosition)
        {
            obstacleX[i] = rand() % 25;
            obstacleY[i] = rand() % 25;

            validPosition = true;

            for (int j = 0; j < snakeSize; j++)
            {
                if (obstacleX[i] == snakeX[j] && obstacleY[i] == snakeY[j])
                {
                    validPosition = false;
                }
            }

            for (int k = 0; k < i; k++)
            {
                if (obstacleX[i] == obstacleX[k] &&
                    obstacleY[i] == obstacleY[k])
                {
                    validPosition = false;
                }
            }
        }
    }
}

//--------------------------------------------------
//DRAW OBSTACLES
//--------------------------------------------------

void drawObstacles(RenderWindow& window)
{
    if (gameState == 2)
    {
        Sprite obstacle(obstacleTexture);

        float obstacleScaleX = float(cellSize) / float(obstacleTexture.getSize().x);
        float obstacleScaleY = float(cellSize) / float(obstacleTexture.getSize().y);

        obstacle.setScale({ obstacleScaleX * 1.4f,  obstacleScaleY * 1.4f });

        for (int i = 0; i < obstacleCount; i++)
        {
            obstacle.setPosition({ float(obstacleX[i] * cellSize), float(obstacleY[i] * cellSize) });

            window.draw(obstacle);
        }
    }
}

//--------------------------------------------------
//CHECK OBSTACLE COLLISION
//--------------------------------------------------

void checkObstacleCollision()
{
    if (gameState == 2)
    {
        for (int i = 0; i < obstacleCount; i++)
        {
            if (snakeX[0] == obstacleX[i] && snakeY[0] == obstacleY[i])
            {
                saveScore();

                if (gameOverSound != nullptr)
                {
                    gameOverSound->play();
                }

                gameState = 5;
            }
        }
    }
}

//--------------------------------------------------
//DRAW PAUSE SCREEN
//--------------------------------------------------

void drawPauseScreen(RenderWindow& window)
{
    RectangleShape box(Vector2f(700.f, 300.f));

    box.setPosition({ 450.f, 600.f });
    box.setFillColor(Color(0, 0, 0, 180));
    box.setOutlineThickness(5.f);
    box.setOutlineColor(Color::White);

    Text pauseTitle(font);

    pauseTitle.setString("GAME PAUSED");
    pauseTitle.setCharacterSize(70);
    pauseTitle.setPosition({ 560.f, 650.f });
    pauseTitle.setFillColor(Color::Yellow);

    Text resume(font);

    resume.setString("PRESS P TO RESUME");
    resume.setCharacterSize(40);
    resume.setPosition({ 560.f, 780.f });
    resume.setFillColor(Color::White);

    window.draw(box);
    window.draw(pauseTitle);
    window.draw(resume);
}

//--------------------------------------------------
//RESET GAME
//--------------------------------------------------

void resetGame()
{
    if (snakeX != nullptr)
    {
        delete[] snakeX;
        snakeX = nullptr;
    }

    if (snakeY != nullptr)
    {
        delete[] snakeY;
        snakeY = nullptr;
    }

    if (obstacleX != nullptr)
    {
        delete[] obstacleX;
        obstacleX = nullptr;
    }

    if (obstacleY != nullptr)
    {
        delete[] obstacleY;
        obstacleY = nullptr;
    }

    snakeX = new int[500];
    snakeY = new int[500];

    obstacleX = new int[obstacleCount];
    obstacleY = new int[obstacleCount];

    snakeSize = 2;
    direction = 1;

    score = 0;
    moveDelay = 200;

    scoreSaved = false;
    gamePaused = false;
}