#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
using std::abs;
using std::cout;
using std::ifstream;
using std::istringstream;
using std::sort;
using std::string;
using std::vector;

// Etat Cellules
enum class State
{
    kEmpty,
    kObstacle,
    kClosed,
    kPath,
    kStart,
    kFinish
};

// Déplacement possibles
const int delta[4][2]{{-1, 0}, {0, -1}, {1, 0}, {0, 1}};

vector<State> ParseLine(string line)
{
    // Analyse caractère d'une ligne
    istringstream sline(line);
    int n;
    char c;
    vector<State> row;
    while (sline >> n >> c )
    {
        if (n == 0)
        {
            row.push_back(State::kEmpty);
        }
        else
        {
            row.push_back(State::kObstacle);
        }
    }
    return row;
}

vector<vector<State>> ReadBoardFile(string path)
{
    // Lit le fichier ligne par ligne
    ifstream myfile(path);
    vector<vector<State>> board{};
    if (myfile)
    {
        string line;
        while (getline(myfile, line))
        {
            vector<State> row = ParseLine(line);
            board.push_back(row);
        }
    }
    return board;
}

// Compare la f value de 2 cellule
// f = g + h
bool Compare(const vector<int> a, const vector<int> b)
{
    int f1 = a[2] + a[3];
    int f2 = b[2] + b[3];
    return f1 > f2;
}

void CellSort(vector<vector<int>> *v)
{
    sort(v->begin(), v->end(), Compare);
}

// Calcule la distance
int Heuristic(int x1, int y1, int x2, int y2)
{
    return abs(x2 - x1) + abs(y2 - y1);
}

// vérifie si la cellule fait partie de la grille et est libre
bool CheckValidCell(int x, int y, vector<vector<State>> &grid)
{
    bool on_grid_x = (x >= 0 && x < grid.size());
    bool on_grid_y = (y >= 0 && y < grid[0].size());
    if (on_grid_x && on_grid_y)
        return grid[x][y] == State::kEmpty;
    return false;
}

void AddToOpen(int x, int y, int g, int h, vector<vector<int>> &openlist, vector<vector<State>> &grid)
{
    // Ajoute à la liste des vecteurs ouverts et le déclare comme fermée
    openlist.push_back(vector<int>{x, y, g, h});
    grid[x][y] = State::kClosed;
}

void ExpandNeighbors(const vector<int> &current, int goal[2], vector<vector<int>> &openlist, vector<vector<State>> &grid)
{
    //Recherche le trajet optimal pour les cellules voisines

    // On obtient les valeurs de la cellule actuelle.
    int x = current[0];
    int y = current[1];

    // boucle sur les cellules potentielles
    for (auto &i : delta)
    {
        int x2 = x + i[0];
        int y2 = y + i[1];
        int g = current[2];
        // Vérifie que la cellule voisine n'est fermé
        if (CheckValidCell(x2, y2, grid))
        {
            // Increment g value and add neighbor to open list.
            g++;
            int h = Heuristic(x2, y2, goal[0], goal[1]);
            AddToOpen(x2, y2, g, h, openlist, grid);
        }
    }
}

vector<vector<State>> Search(vector<vector<State>> grid, int init[2], int goal[2])
{
    //Recherche du trajet optimal

    vector<vector<int>> open{};

    // initialisation
    int x = init[0];
    int y = init[1];
    int g = 0;
    int h = Heuristic(x, y, goal[0], goal[1]);
    AddToOpen(x, y, g, h, open, grid);

    while (!open.empty())
    {
        CellSort(&open);
        auto current = open.back();
        open.pop_back();
        x = current[0];
        y = current[1];
        grid[x][y] = State::kPath;

        // Vérifie si le programme est arrivé à destination
        if (x == goal[0] && y == goal[1])
        {
            grid[init[0]][init[1]] = State::kStart;
            grid[goal[0]][goal[1]] = State::kFinish;
            return grid;
        }
        else
        {
            ExpandNeighbors(current, goal, open, grid);
        }
    }

    cout << "No path found!" << std::endl;
    return std::vector<vector<State>>{};
}

string CellString(State cell)
{
    switch (cell)
    {
    case State::kObstacle:
        return "🚧   ";
    case State::kPath:
        return "🚗   ";
    case State::kStart:
        return "🚦   ";
    case State::kFinish:
        return "🏁   ";
    default:
        return "0   ";
    }
}

void PrintBoard(const vector<vector<State>> board)
{
    for (int i = 0; i < board.size(); i++)
    {
        for (int j = 0; j < board[i].size(); j++)
        {
            cout << CellString(board[i][j]);
        }
        cout << std::endl;
    }
}

int main()
{
    int init[2]{0, 0};
    int goal[2]{4, 11};
    auto board = ReadBoardFile("2.board");
    auto solution = Search(board, init, goal);
    PrintBoard(solution);
}