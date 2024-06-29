#include <string>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "snake.h"


SnakeBody::SnakeBody()
{
}

SnakeBody::SnakeBody(int x, int y): mX(x), mY(y)
{
}

int SnakeBody::getX() const
{
    return mX;
}

int SnakeBody::getY() const
{
    return mY;
}

bool SnakeBody::operator == (const SnakeBody& snakeBody)
{
    // TODO overload the == operator for SnakeBody comparision.
    return this->getX() == snakeBody.getX() && this->getY() == snakeBody.getY();
}

Snake::Snake(int gameBoardWidth, int gameBoardHeight, int initialSnakeLength): mGameBoardWidth(gameBoardWidth), mGameBoardHeight(gameBoardHeight), mInitialSnakeLength(initialSnakeLength)
{
    this->initializeSnake();
    this->setRandomSeed();
}

void Snake::setRandomSeed()
{
    // use current time as seed for random generator
    std::srand(std::time(nullptr));
}

void Snake::initializeSnake()
{
    // Instead of using a random initialization algorithm 蛇初始位于正中间
    // We always put the snake at the center of the game mWindows
    int centerX = this->mGameBoardWidth / 2;
    int centerY = this->mGameBoardHeight / 2;

    //从正中间向下延伸蛇体
    for (int i = 0; i < this->mInitialSnakeLength; i ++)
    {
        this->mSnake.push_back(SnakeBody(centerX, centerY + i));
    }
    //初始行动方向 向上
    this->mDirection = Direction::Up;
}

bool Snake::isPartOfSnake(int x, int y)
{
    // TODO check if a given point with axis x, y is on the body of the snake.
    for (int i = 0; i < this->getSnake().size(); i++) {
        if (this->getSnake()[i] == SnakeBody(x, y))
            return true;
    }
    return false;
}

/*
 * Assumption:
 * Only the head would hit wall.
 */
bool Snake::hitWall()
{
    // TODO check if the snake has hit the wall
    int headX = this->getSnake()[0].getX(), headY = this->getSnake()[0].getY();

    switch (this->mDirection) {
        case Direction::Up:
            if (headY == 1) return true;
            break;
        case Direction::Down:
            if (headY == this->mGameBoardHeight - 2) return true;
            break;
        case Direction::Left:
            if (headX == 1) return true;
            break;
        case Direction::Right:
            if (headX == this->mGameBoardWidth - 2) return true;
            break;
    }
    return false;
}

/*
 * The snake head is overlapping with its body
 */
bool Snake::hitSelf()
{
    // TODO check if the snake has hit itself.
    int headX = this->mSnake[0].getX(), headY = this->mSnake[0].getY();

    switch (this->mDirection) {
        case Direction::Up:
            if (this->isPartOfSnake(headX, headY - 1)) return true;
            break;
        case Direction::Down:
            if (this->isPartOfSnake(headX, headY + 1)) return true;
            break;
        case Direction::Left:
            if (this->isPartOfSnake(headX - 1, headY)) return true;
            break;
        case Direction::Right:
            if (this->isPartOfSnake(headX + 1, headY)) return true;
            break;
    }
    return false;
}

//长头，并检测下一步是否碰到食物
bool Snake::touchFood() //为什么是bool型的
{
    SnakeBody newHead = this->createNewHead();

    if (this->mFood == newHead)
    {
        return true;
    }
    else
    {
        return false;
    }
}

//有食物出现在下一步前方
void Snake::senseFood(SnakeBody food) //干嘛用的
{
    this->mFood = food;
}

//返回储存蛇体的向量
std::vector<SnakeBody>& Snake::getSnake()
{
    return this->mSnake;
}

//为啥是bool型
bool Snake::changeDirection(Direction newDirection)
{
    switch (this->mDirection)
    {
        case Direction::Up:
        {
            // what you need to do when the current direction of the snake is Up
            // and the user inputs a new direction?  TODO
            //如果当前方向向上，则用户只能输入向左或向右的指令
            switch (newDirection) {
                case Direction::Left:
                case Direction::Right:
                    this->mDirection = newDirection;
            }
        }
        case Direction::Down:
        {
            // what you need to do when the current direction of the snake is Down
            // and the user inputs a new direction? TODO
            switch (newDirection) {
                case Direction::Left:
                case Direction::Right:
                    this->mDirection = newDirection;
            }
        }
        case Direction::Left:
        {
            // what you need to do when the current direction of the snake is Left
            // and the user inputs a new direction? TODO
            switch (newDirection) {
                case Direction::Up:
                case Direction::Down:
                    this->mDirection = newDirection;
            }
        }
        case Direction::Right:
        {
            // what you need to do when the current direction of the snake is Right
            // and the user inputs a new direction? TODO
            switch (newDirection) {
                case Direction::Up:
                case Direction::Down:
                    this->mDirection = newDirection;
            }
        }
    }

    return false;
}

//吃掉食物后原来是食物的地方生成新头
SnakeBody Snake::createNewHead()
{
    /* TODO
    * read the position of the current head
    * read the current heading direction
    * add the new head according to the direction
    * return the new head
    */
    int headX = this->mSnake[0].getX(), headY = this->mSnake[0].getY();

    switch (this->mDirection) {
        case Direction::Up:
            this->mSnake.insert(mSnake.begin(), SnakeBody(headX, headY - 1));
            break;
        case Direction::Down:
            this->mSnake.insert(mSnake.begin(), SnakeBody(headX, headY + 1));
            break;
        case Direction::Left:
            this->mSnake.insert(mSnake.begin(), SnakeBody(headX - 1, headY));
            break;
        case Direction::Right:
            this->mSnake.insert(mSnake.begin(), SnakeBody(headX + 1, headY));
            break;
    }

    SnakeBody newHead = this->mSnake[0];
    return newHead;
}

/*
 * If eat food, return true, otherwise return false
 */
bool Snake::moveFoward() //为什么是bool型
{
    /*
    * TODO
    * move the snake forward.
    * If eat food, return true, otherwise return false
    */
    //去尾
    //this->mSnake.pop_back();
    //长头
    this->touchFood();
    return true;
}

//检查下一步是否蛇头撞墙或者咬到自己
bool Snake::checkCollision()
{
    if (this->hitWall() || this->hitSelf())
    {
        return true;
    }
    else
    {
        return false;
    }
}

//获取蛇长，即分数-2
int Snake::getLength()
{
    return this->mSnake.size();
}

Direction Snake::getDirection()
{
    return this->mDirection;
}
