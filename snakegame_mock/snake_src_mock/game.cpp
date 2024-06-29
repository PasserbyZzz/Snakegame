#include <string>
#include <iostream>
#include <cmath>

// For terminal delay
#include <chrono>
#include <thread>

#include <fstream>
#include <algorithm>

#include "game.h"

Game::Game()
{
    // Separate the screen to three windows
    this->mWindows.resize(3);
    initscr();
    // If there wasn't any key pressed don't wait for keypress
    nodelay(stdscr, true);
    // Turn on keypad control
    keypad(stdscr, true);
    // No echo for the key pressed
    noecho();
    // No cursor show
    curs_set(0);
    // Get screen and board parameters
    getmaxyx(stdscr, this->mScreenHeight, this->mScreenWidth);
    this->mGameBoardWidth = this->mScreenWidth - this->mInstructionWidth;
    this->mGameBoardHeight = this->mScreenHeight - this->mInformationHeight;

    this->createInformationBoard();
    this->createGameBoard();
    this->createInstructionBoard();

    // Initialize the leader board to be all zeros
    this->mLeaderBoard.assign(this->mNumLeaders, 0);
}

Game::~Game()
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        delwin(this->mWindows[i]);
    }
    endwin();
}

//创建简介栏
void Game::createInformationBoard()
{
    int startY = 0;
    int startX = 0;
    this->mWindows[0] = newwin(this->mInformationHeight, this->mScreenWidth, startY, startX);
}

//显示简介
void Game::renderInformationBoard() const
{
    mvwprintw(this->mWindows[0], 1, 1, "Welcome to The Snake Game!");
    mvwprintw(this->mWindows[0], 2, 1, "This is a mock version.");
    mvwprintw(this->mWindows[0], 3, 1, "Please fill in the blanks to make it work properly!!");
    mvwprintw(this->mWindows[0], 4, 1, "Implemented using C++ and libncurses library.");
    wrefresh(this->mWindows[0]);
}

//创建游戏栏
void Game::createGameBoard()
{
    int startY = this->mInformationHeight;
    int startX = 0;
    this->mWindows[1] = newwin(this->mScreenHeight - this->mInformationHeight, this->mScreenWidth - this->mInstructionWidth, startY, startX);
}

//显示游戏
void Game::renderGameBoard() const
{
    wrefresh(this->mWindows[1]);
}

//创建提示栏
void Game::createInstructionBoard()
{
    int startY = this->mInformationHeight;
    int startX = this->mScreenWidth - this->mInstructionWidth;
    this->mWindows[2] = newwin(this->mScreenHeight - this->mInformationHeight, this->mInstructionWidth, startY, startX);
}

//显示提示
void Game::renderInstructionBoard() const
{
    mvwprintw(this->mWindows[2], 1, 1, "Manual");

    mvwprintw(this->mWindows[2], 3, 1, "Up: W");
    mvwprintw(this->mWindows[2], 4, 1, "Down: S");
    mvwprintw(this->mWindows[2], 5, 1, "Left: A");
    mvwprintw(this->mWindows[2], 6, 1, "Right: D");

    mvwprintw(this->mWindows[2], 8, 1, "Difficulty");
    mvwprintw(this->mWindows[2], 11, 1, "Points");

    wrefresh(this->mWindows[2]);
}

//显示排行榜
void Game::renderLeaderBoard() const
{
    // If there is not too much space, skip rendering the leader board
    if (this->mScreenHeight - this->mInformationHeight - 14 - 2 < 3 * 2)
    {
        return;
    }
    mvwprintw(this->mWindows[2], 14, 1, "Leader Board");
    std::string pointString;
    std::string rank;
    for (int i = 0; i < std::min(this->mNumLeaders, this->mScreenHeight - this->mInformationHeight - 14 - 2); i ++)
    {
        pointString = std::to_string(this->mLeaderBoard[i]);
        rank = "#" + std::to_string(i + 1) + ":";
        mvwprintw(this->mWindows[2], 14 + (i + 1), 1, rank.c_str());
        mvwprintw(this->mWindows[2], 14 + (i + 1), 5, pointString.c_str());
    }
    wrefresh(this->mWindows[2]);
}

//游戏结束后跳出窗口，让玩家选择Restart还是Quit
bool Game::renderRestartMenu() const
{
    WINDOW * menu;
    int width = this->mGameBoardWidth * 0.5;
    int height = this->mGameBoardHeight * 0.5;
    int startX = this->mGameBoardWidth * 0.25;
    int startY = this->mGameBoardHeight * 0.25 + this->mInformationHeight;

    menu = newwin(height, width, startY, startX);
    box(menu, 0, 0);
    std::vector<std::string> menuItems = {"Restart", "Quit"};

    int index = 0;
    int offset = 4;
    mvwprintw(menu, 1, 1, "Your Final Score:");
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(menu, 2, 1, pointString.c_str());
    wattron(menu, A_STANDOUT);
    mvwprintw(menu, 0 + offset, 1, menuItems[0].c_str());
    wattroff(menu, A_STANDOUT);
    mvwprintw(menu, 1 + offset, 1, menuItems[1].c_str());

    wrefresh(menu);

    int key;
    while (true)
    {
        key = getch();
        switch(key)
        {
            case 'W':
            case 'w':
            case KEY_UP:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index --;
                index = (index < 0) ? menuItems.size() - 1 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
            case 'S':
            case 's':
            case KEY_DOWN:
            {
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                index ++;
                index = (index > menuItems.size() - 1) ? 0 : index;
                wattron(menu, A_STANDOUT);
                mvwprintw(menu, index + offset, 1, menuItems[index].c_str());
                wattroff(menu, A_STANDOUT);
                break;
            }
        }
        wrefresh(menu);
        if (key == ' ' || key == 10)
        {
            break;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    delwin(menu);

    if (index == 0)
    {
        return true;
    }
    else
    {
        return false;
    }

}

//修改分数，并显示在提示栏
void Game::renderPoints() const
{
    std::string pointString = std::to_string(this->mPoints);
    mvwprintw(this->mWindows[2], 12, 1, pointString.c_str());
    wrefresh(this->mWindows[2]);
}

//修改难度，并显示在提示栏
void Game::renderDifficulty() const
{
    std::string difficultyString = std::to_string(this->mDifficulty);
    mvwprintw(this->mWindows[2], 9, 1, difficultyString.c_str());
    wrefresh(this->mWindows[2]);
}

//初始化游戏界面
void Game::initializeGame()
{
    // allocate memory for a new snake
    this->mPtrSnake.reset(new Snake(this->mGameBoardWidth, this->mGameBoardHeight, this->mInitialSnakeLength));

    /* TODO
     * initialize the game pionts as zero
     * create a food at randome place
     * make the snake aware of the food
     * other initializations
     */

    //初始化分数
    this->mPoints = 0;
    this->renderPoints();

    //初始化难度
    this->mDifficulty = 0;
    this->renderDifficulty();

    //随机生成食物, 并且蛇已感知到该食物
    this->createRamdonFood();
    this->renderFood();
}

//随机生成食物
void Game::createRamdonFood()
{
    /* TODO
    * create a food at random places
    * make sure that the food doesn't overlap with the snake.
    */
    int foodX, foodY;
    do {
        foodX = (rand() % (this->mGameBoardWidth - 2)) + 1;
        foodY = (rand() % (this->mGameBoardHeight - 2)) + 1;
    }
    while (this->mPtrSnake->isPartOfSnake(foodX, foodY));

    SnakeBody food(foodX, foodY);
    this->mFood = food;
    this->mPtrSnake->senseFood(food);
}

//显示食物
void Game::renderFood() const
{
    mvwaddch(this->mWindows[1], this->mFood.getY(), this->mFood.getX(), this->mFoodSymbol);
    wrefresh(this->mWindows[1]);
}

//显示蛇
void Game::renderSnake() const
{
    int snakeLength = this->mPtrSnake->getLength();
    std::vector<SnakeBody>& snake = this->mPtrSnake->getSnake();
    for (int i = 0; i < snakeLength; i ++)
    {
        mvwaddch(this->mWindows[1], snake[i].getY(), snake[i].getX(), this->mSnakeSymbol);
    }
    wrefresh(this->mWindows[1]);
}

//玩家通过键盘控制蛇的移动方向
void Game::controlSnake() const
{
    int key;
    key = getch();
    switch(key)
    {
        case 'W':
        case 'w':
        case KEY_UP:
        {
            // TODO change the direction of the snake.
            if (this->mPtrSnake->getDirection() == Direction::Left || this->mPtrSnake->getDirection() == Direction::Right)
                this->mPtrSnake->changeDirection(Direction::Up);
            break;
        }
        case 'S':
        case 's':
        case KEY_DOWN:
        {
            // TODO change the direction of the snake.
            if (this->mPtrSnake->getDirection() == Direction::Left || this->mPtrSnake->getDirection() == Direction::Right)
                this->mPtrSnake->changeDirection(Direction::Down);
            break;
        }
        case 'A':
        case 'a':
        case KEY_LEFT:
        {
            // TODO change the direction of the snake.
            if (this->mPtrSnake->getDirection() == Direction::Up || this->mPtrSnake->getDirection() == Direction::Down)
                this->mPtrSnake->changeDirection(Direction::Left);
            break;
        }
        case 'D':
        case 'd':
        case KEY_RIGHT:
        {
            // TODO change the direction of the snake.
            if (this->mPtrSnake->getDirection() == Direction::Up || this->mPtrSnake->getDirection() == Direction::Down)
                this->mPtrSnake->changeDirection(Direction::Right);
            break;
        }
        default:
        {
            break;
        }
    }
}

//刷新窗口 绘制三个窗口
void Game::renderBoards() const
{
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        werase(this->mWindows[i]);
    }
    this->renderInformationBoard();
    this->renderGameBoard();
    this->renderInstructionBoard();
    for (int i = 0; i < this->mWindows.size(); i ++)
    {
        box(this->mWindows[i], 0, 0);
        wrefresh(this->mWindows[i]);
    }
    this->renderLeaderBoard();
}

//改变游戏难度，每得5分难度+1；分数被5整除的时候游戏加速
void Game::adjustDelay()
{
    this->mDifficulty = this->mPoints / 5;
    if (mPoints % 5 == 0)
    {
        this->mDelay = this->mBaseDelay * pow(0.75, this->mDifficulty);
    }
}

//运行游戏
void Game::runGame()
{
    bool moveSuccess;
    int key;
    while (true)
    {
        /* TODO
        * this is the main control loop of the game.
        * it keeps running a while loop, and does the following things:
        * 	1. process your keyboard input
        * 	2. clear the window
        * 	3. move the current snake forward
        * 	4. check if the snake has eaten the food after movement
        * 	5. check if the snake dies after the movement
        * 	6. make corresponding steps for the ``if conditions'' in 3 and 4.
        *   7. render the position of the food and snake in the new frame of window.
        *   8. update other game states and refresh the window
        */
        //从键盘读入方向
        this->controlSnake();

        //根据目前难度设置蛇的移动速率
        this->adjustDelay();

        //蛇移动
        //撞墙或者咬到自己 游戏结束
        if (this->mPtrSnake->checkCollision()) break;

        //否则向前移动
        //未吃到食物
        else if (!this->mPtrSnake->touchFood()) this->mPtrSnake->getSnake().pop_back();
        //吃到食物 再次生成食物，并且分数+1
        else {
            this->createRamdonFood();
            this->mPoints++;
        }

        //打印出蛇、食物、分数和难度
        this->renderSnake();
        this->renderFood();
        this->adjustDelay();
        this->renderDifficulty();
        this->renderPoints();

        std::this_thread::sleep_for(std::chrono::milliseconds(this->mDelay));
        this->renderBoards();
        refresh();
    }
}

//开始游戏
void Game::startGame()
{
    refresh();
    bool choice;
    while (true)
    {
        this->readLeaderBoard(); //读取历史排行
        this->renderBoards(); //绘制三个窗口
        this->initializeGame(); //初始化游戏
        this->runGame(); //运行游戏
        this->updateLeaderBoard(); //更新历史排行
        this->writeLeaderBoard(); //重写历史排行
        choice = this->renderRestartMenu(); //询问是否继续游戏
        if (choice == false)
        {
            break;
        }
    }
}

//从某个文件读取历史排行
// https://en.cppreference.com/w/cpp/io/basic_fstream
bool Game::readLeaderBoard()
{
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.in);
    if (!fhand.is_open())
    {
        return false;
    }
    int temp;
    int i = 0;
    while ((!fhand.eof()) && (i < mNumLeaders))
    {
        fhand.read(reinterpret_cast<char*>(&temp), sizeof(temp));
        this->mLeaderBoard[i] = temp;
        i ++;
    }
    fhand.close();
    return true;
}

//更新排行榜，若有更新，则返回true
bool Game::updateLeaderBoard()
{
    bool updated = false;
    int newScore = this->mPoints;
    for (int i = 0; i < this->mNumLeaders; i++)
    {
        if (this->mLeaderBoard[i] >= this->mPoints)
        {
            continue;
        }
        int oldScore = this->mLeaderBoard[i];
        this->mLeaderBoard[i] = newScore;
        newScore = oldScore;
        updated = true;
    }
    return updated;
}

//记录历史排行进某个文件中
bool Game::writeLeaderBoard()
{
    //trunc: clear the data file
    std::fstream fhand(this->mRecordBoardFilePath, fhand.binary | fhand.trunc | fhand.out);
    if (!fhand.is_open())
    {
        return false;
    }
    for (int i = 0; i < this->mNumLeaders; i ++)
    {
        fhand.write(reinterpret_cast<char*>(&this->mLeaderBoard[i]), sizeof(this->mLeaderBoard[i]));;
    }
    fhand.close();
    return true;
}








