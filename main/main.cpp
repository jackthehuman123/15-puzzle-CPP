#include <iostream>
#include <numeric>
#include <cassert>
#include <utility>
#include "Random.h"

constexpr int g_consoleLines{ 25 };

class Direction
{
public:
    
    enum Type
    {
        up,
        down,
        left,
        right,
        maxDirections
    };

    static constexpr std::array<std::string_view, maxDirections> directionNames{"up", "down", "left", "right"};
    static constexpr std::array<Type, maxDirections> directionTypes{up, down, left, right};

private:
    Type m_type{};

public:
    Direction(Type type)
    :   m_type {type}
    {}

    Type getType() const {return m_type;}

    static Direction random() 
    {
        return Direction{directionTypes[static_cast<std::size_t>(Random::get(0, 3))]};
    }

    Direction operator-() const
    {
        switch(m_type)
        {
            case up: return Direction{ down };
            case down: return Direction{ up };
            case left: return Direction{ right };
            case right: return Direction{ left };
            default: break;
        }
        assert(0 && "Unsupported direction was passed!");
        return Direction{ up };
    } 
    
    friend std::ostream& operator<<(std::ostream& out, const Direction& direction)
    {
        out << directionNames[direction.m_type];
        return out;
    }
};

struct Point
{
    int x{};
    int y{};

    friend bool operator==(const Point& p1, const Point& p2)
    {
        return ((p1.x == p2.x) && (p1.y == p2.y));
    }   
    
    friend bool operator!=(const Point& p1, const Point& p2)
    {
        return !(p1 == p2);
    }

    Point getAdjacentPoint(const Direction& direction) const
    {
        switch(direction.getType())
        {
            case Direction::up:     return Point{ x , y + 1 };
            case Direction::down:   return Point{ x , y - 1 };
            case Direction::left:   return Point{ x + 1 , y };
            case Direction::right:  return Point{ x - 1 , y };
            default:                break; 
        }
    
        assert(0 && "Invalid direction entered");
        return Point{ x , y };
    }
};

class Tile
{
private:
    int m_value{};

public:
    Tile() = default;
    explicit Tile(int value)
    :   m_value {value}
    {
    }

    bool isEmpty() const {return m_value == 0;}
    int getNum() const {return m_value;}
    friend std::ostream& operator<<(std::ostream& out, const Tile& tile)
    {   
        if(tile.isEmpty())
        {
            out << "   ";
            return out;
        }
    
        if (tile.getNum() < 10)
            out << " ";
    
        out << " " << tile.getNum() << " ";
        return out;
    }
};

class Board
{
private:
    static constexpr int s_size {4};
    Tile m_board[s_size][s_size]
    {
        Tile{1}, Tile{2}, Tile{3}, Tile{4},
        Tile{5}, Tile{6}, Tile{7}, Tile{8},
        Tile{9}, Tile{10}, Tile{11}, Tile{12},
        Tile{13}, Tile{14}, Tile{15}, Tile{0} 
    };

public:
    Board() = default;

    static bool validPoint(const Point& p)
    {
        return (p.x >= 0 && p.x < s_size)
            && (p.y >= 0 && p.y < s_size);
    }

    Point findEmptyTile() const
    {
        //Make sure row = y , col = x
        for (int y{0}; y < s_size; y++)
        {
            for (int x{0}; x < s_size; ++x)
            {
                if (m_board[y][x].isEmpty())
                    return Point { x , y };
            }
        }
        assert(0 && "Empty point is unfounded");
        return {-1 , -1}; 
    }

    void swapTile(Point p1, Point p2)
    {   
        //Make sure row = y , col = x
        std::swap(m_board[p1.y][p1.x] , m_board[p2.y][p2.x]);
    }

    bool moveTile(Direction direction)
    {   
        Point empty{findEmptyTile()};
        Point adj{empty.getAdjacentPoint(direction)};

        if (!validPoint(adj))
            return false;
        
        swapTile(adj, empty);
        return true;
    }

    friend bool operator==(const Board& b1, const Board& b2)
    {
        bool isEqual{true};
        for (int i{0}; i < 4; ++i)
        {
            for (int j{0}; j < 4; ++j)
            {
                if (b1.m_board[i][j].getNum() != b2.m_board[i][j].getNum())
                    isEqual = false;
            }
        }
        return isEqual;
    }

    friend std::ostream& operator<<(std::ostream& out, const Board& board)
    {   
        for (int i = 0; i < g_consoleLines; ++i)
            std::cout << '\n';
    
        for (const auto& arow : board.m_board)
        {
            for (const auto& e : arow)
                out << e;
    
            out << '\n';
        }
        return out;
    }
};

namespace UserInput 
{   
    void ignoreLine()
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }

    //no need for error handling since we validate the input 
    //and that all input are char, so no type mismatch
    bool validInput(char ch)
    {
        return ch == 'w'
            || ch == 'a'
            || ch == 's'
            || ch == 'd'
            || ch == 'q';
    }

    char getInput()
    {  
        char c{};
        std::cin >> c;
        //online one character to be extracted
        ignoreLine();
        return c;
    }

    char getCommandFromUser()
    {   
        char input{};
        //Will be an inf loop if the input is invalid
        //since !validInput(*invalidinput*) will evaluates to true, which makes the loop infinite
        //until a valid input is entered, then !validInput(*validinput*) evaluates to false 
        while (!validInput(input))
            input = getInput();

        return input;
    }

    Direction convert(char input)
    {
        switch (input)
        {
        case 'w': return Direction{ Direction::up };
        case 's': return Direction{ Direction::down };
        case 'a': return Direction{ Direction::left };
        case 'd': return Direction{ Direction::right };
        }

        assert(0 && "Unsupported direction was passed!");
        return Direction{ Direction::up };
    }
}

int main() 
{  
    Board board{};
    Board board2(board);

    for (int i{0}; i < 1; ++i)
    {
        [[maybe_unused]]bool shuffle{board.moveTile(Direction::random())};
    }

    std::cout << board;

    while(true)
    {
        if (board == board2)
        {
            std::cout << "\n\nYou won!\n\n";
            return 0;
        }

        char input{UserInput::getCommandFromUser()};
    
        if(input == 'q')
        {
            std::cout << "\n\nBye!\n\n";
            return 0;
        }

        Direction direction{UserInput::convert(input)};
    
        bool userMoved{board.moveTile(direction)};
        //Just print it in the loop lol
        if (userMoved)
            std::cout << board;
        
    }
    return 0;
}   
