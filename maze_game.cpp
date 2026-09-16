#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <conio.h>
#include <windows.h>
#include <cmath>

using namespace std;

// ------------------------------------------------------------
// CONSTANTS
// ------------------------------------------------------------

const int ROWS = 15;
const int COLS = 30;

// ------------------------------------------------------------
// PLAYER CLASS
// ------------------------------------------------------------

class Player {
public:
    int r;
    int c;
    int score;

    Player() {
        r = 1;
        c = 1;
        score = 100;
    }
};

// ------------------------------------------------------------
// ENEMY CLASS
// ------------------------------------------------------------

class Enemy {
public:
    int r;
    int c;
    bool alive;

    Enemy(int rr = 0, int cc = 0) {
        r = rr;
        c = cc;
        alive = true;
    }
};

// ------------------------------------------------------------
// DECISION TREE NODE
// ------------------------------------------------------------

class DecisionNode {
public:
    string question;
    string action;

    DecisionNode* yes;
    DecisionNode* no;

    DecisionNode(string q = "", string a = "") {
        question = q;
        action = a;
        yes = nullptr;
        no = nullptr;
    }
};

// ------------------------------------------------------------
// MAZE GAME CLASS
// ------------------------------------------------------------

class MazeGame {

private:

    char board[ROWS][COLS];

    Player player;

    vector<Enemy> enemies;

    // Stack for Undo
    stack<pair<int, int>> undoStack;

    // Treasure position
    int tr;
    int tc;

    // Enemy Decision Tree
    DecisionNode* enemyDecisionTree;

    // --------------------------------------------------------
    // Delete Decision Tree
    // --------------------------------------------------------

    void deleteDecisionTree(DecisionNode* node) {

        if (node == nullptr)
            return;

        deleteDecisionTree(node->yes);
        deleteDecisionTree(node->no);

        delete node;
    }

    // --------------------------------------------------------
    // Build Enemy Decision Tree
    // --------------------------------------------------------

    void buildDecisionTree() {

        /*
              Is Player Near?
                 /       \
               YES       NO
               /          \
        Is Treasure     Is Enemy
        Far Away?      Near Treasure?
          /   \          /      \
        YES   NO        YES      NO
         |     |         |        |
       CHASE  GUARD    GUARD    PATROL
        */

        enemyDecisionTree =
            new DecisionNode("Player is near?");

        enemyDecisionTree->yes =
            new DecisionNode("Treasure is far away?", "CHASE");

        enemyDecisionTree->no =
            new DecisionNode("Enemy is near treasure?");

        enemyDecisionTree->yes->yes =
            new DecisionNode("", "CHASE");

        enemyDecisionTree->yes->no =
            new DecisionNode("", "GUARD");

        enemyDecisionTree->no->yes =
            new DecisionNode("", "GUARD");

        enemyDecisionTree->no->no =
            new DecisionNode("", "PATROL");
    }

    // --------------------------------------------------------
    // Evaluate Enemy Decision Tree
    // --------------------------------------------------------

    string evaluateDecisionTree(Enemy &e) {

        if (enemyDecisionTree == nullptr)
            return "PATROL";

        int playerDistance =
            abs(e.r - player.r) + abs(e.c - player.c);

        int treasureDistance =
            abs(e.r - tr) + abs(e.c - tc);

        DecisionNode* node = enemyDecisionTree;

        // Root: Is player near?
        if (playerDistance <= 5) {

            node = node->yes;

            // Is treasure far away?
            if (treasureDistance > 2)
                node = node->yes;
            else
                node = node->no;

        }
        else {

            node = node->no;

            // Is enemy near treasure?
            if (treasureDistance <= 2)
                node = node->yes;
            else
                node = node->no;
        }

        return node->action;
    }

    // --------------------------------------------------------
    // Save Game Result
    // --------------------------------------------------------

    void saveGameResult(string result) {

        ofstream file("game_history.txt", ios::app);

        if (!file) {
            cout << "\nUnable to open game history file.\n";
            return;
        }

        time_t now = time(0);

        file << "----------------------------------------\n";
        file << "Game Result: " << result << "\n";
        file << "Score: " << player.score << "\n";
        file << "Date: " << ctime(&now);

        file.close();
    }

public:

    // --------------------------------------------------------
    // CONSTRUCTOR
    // --------------------------------------------------------

    MazeGame() {

        srand((unsigned)time(0));

        enemyDecisionTree = nullptr;

        buildDecisionTree();

        init();
    }

    // --------------------------------------------------------
    // DESTRUCTOR
    // --------------------------------------------------------

    ~MazeGame() {

        deleteDecisionTree(enemyDecisionTree);
    }

    // --------------------------------------------------------
    // INITIALIZE GAME
    // --------------------------------------------------------

    void init() {

        // Fill board with walls
        for (int i = 0; i < ROWS; i++) {

            for (int j = 0; j < COLS; j++) {

                board[i][j] = '#';
            }
        }

        // ----------------------------------------------------
        // Create guaranteed path
        // ----------------------------------------------------

        int r = 1;
        int c = 1;

        board[r][c] = ' ';

        while (r < ROWS - 2 || c < COLS - 2) {

            if (r == ROWS - 2) {

                c++;
            }
            else if (c == COLS - 2) {

                r++;
            }
            else if (rand() % 2) {

                r++;
            }
            else {

                c++;
            }

            board[r][c] = ' ';
        }

        // ----------------------------------------------------
        // Randomly open additional cells
        // ----------------------------------------------------

        for (int i = 1; i < ROWS - 1; i++) {

            for (int j = 1; j < COLS - 1; j++) {

                if (rand() % 100 < 40) {

                    board[i][j] = ' ';
                }
            }
        }

        // ----------------------------------------------------
        // Treasure
        // ----------------------------------------------------

        tr = ROWS - 2;
        tc = COLS - 2;

        board[tr][tc] = 'T';

        // ----------------------------------------------------
        // Traps
        // ----------------------------------------------------

        int traps = 0;

        while (traps < 14) {

            int rr = rand() % (ROWS - 2) + 1;
            int cc = rand() % (COLS - 2) + 1;

            // Don't place trap on player start or treasure
            if ((rr == 1 && cc == 1) ||
                (rr == tr && cc == tc)) {

                continue;
            }

            if (board[rr][cc] == ' ') {

                board[rr][cc] = 'X';

                traps++;
            }
        }

        // ----------------------------------------------------
        // Create enemies
        // ----------------------------------------------------

        enemies.clear();

        enemies.push_back(Enemy(ROWS - 2, 1));
        enemies.push_back(Enemy(1, COLS - 2));

        // Reset player
        player.r = 1;
        player.c = 1;
        player.score = 100;

        // Clear undo history
        while (!undoStack.empty())
            undoStack.pop();
    }

    // --------------------------------------------------------
    // DRAW BOARD
    // --------------------------------------------------------

    void draw() {

        system("cls");

        cout << "========================================\n";
        cout << "              MAZE GAME\n";
        cout << "========================================\n";

        cout << "Score: " << player.score << "\n\n";

        for (int i = 0; i < ROWS; i++) {

            for (int j = 0; j < COLS; j++) {

                // Player
                if (player.r == i && player.c == j) {

                    cout << "P";
                    continue;
                }

                bool printed = false;

                // Enemies
                for (auto &e : enemies) {

                    if (e.alive &&
                        e.r == i &&
                        e.c == j) {

                        cout << "E";

                        printed = true;

                        break;
                    }
                }

                if (!printed)
                    cout << board[i][j];
            }

            cout << "\n";
        }

        cout << "\nLegend:\n";
        cout << "P = Player\n";
        cout << "E = Enemy\n";
        cout << "X = Trap\n";
        cout << "T = Treasure\n";
        cout << "# = Wall\n";

        cout << "\nControls:\n";
        cout << "Arrow Keys = Move\n";
        cout << "H = BFS Hint\n";
        cout << "U = Undo\n";
        cout << "Q = Quit\n";
    }

    // --------------------------------------------------------
    // CHECK VALID POSITION
    // --------------------------------------------------------

    bool valid(int r, int c) {

        return r >= 0 &&
               r < ROWS &&
               c >= 0 &&
               c < COLS &&
               board[r][c] != '#';
    }

    // --------------------------------------------------------
    // BFS HINT
    // --------------------------------------------------------

    void bfsHint() {

        bool visited[ROWS][COLS] = {};

        pair<int, int> parent[ROWS][COLS];

        queue<pair<int, int>> q;

        q.push({player.r, player.c});

        visited[player.r][player.c] = true;

        int dr[4] = {-1, 1, 0, 0};
        int dc[4] = {0, 0, -1, 1};

        // ----------------------------------------------------
        // BFS
        // ----------------------------------------------------

        while (!q.empty()) {

            pair<int, int> current = q.front();

            q.pop();

            if (current.first == tr &&
                current.second == tc) {

                break;
            }

            for (int k = 0; k < 4; k++) {

                int nr = current.first + dr[k];
                int nc = current.second + dc[k];

                if (valid(nr, nc) &&
                    !visited[nr][nc]) {

                    visited[nr][nc] = true;

                    parent[nr][nc] = current;

                    q.push({nr, nc});
                }
            }
        }

        // ----------------------------------------------------
        // No path
        // ----------------------------------------------------

        if (!visited[tr][tc]) {

            cout << "\nNo path found.\n";

            _getch();

            return;
        }

        // ----------------------------------------------------
        // Reconstruct path
        // ----------------------------------------------------

        vector<pair<int, int>> path;

        int r = tr;
        int c = tc;

        while (!(r == player.r &&
                 c == player.c)) {

            path.push_back({r, c});

            pair<int, int> p = parent[r][c];

            r = p.first;
            c = p.second;
        }

        // Reverse direction of path
        auto next = path.back();

        int nr = next.first;
        int nc = next.second;

        cout << "\nBFS Hint: ";

        if (nr < player.r)
            cout << "MOVE UP";

        else if (nr > player.r)
            cout << "MOVE DOWN";

        else if (nc < player.c)
            cout << "MOVE LEFT";

        else if (nc > player.c)
            cout << "MOVE RIGHT";

        cout << "\n";

        cout << "Next cell: ("
             << nr << ", "
             << nc << ")";

        cout << "\nShortest path length: "
             << path.size();

        _getch();
    }

    // --------------------------------------------------------
    // MOVE ENEMY
    // --------------------------------------------------------

    void moveEnemies() {

        for (auto &e : enemies) {

            if (!e.alive)
                continue;

            // Get decision from actual decision tree
            string decision = evaluateDecisionTree(e);

            int nr = e.r;
            int nc = e.c;

            // ------------------------------------------------
            // CHASE
            // ------------------------------------------------

            if (decision == "CHASE") {

                if (player.r < e.r)
                    nr--;

                else if (player.r > e.r)
                    nr++;

                else if (player.c < e.c)
                    nc--;

                else if (player.c > e.c)
                    nc++;
            }

            // ------------------------------------------------
            // GUARD
            // ------------------------------------------------

            else if (decision == "GUARD") {

                if (tr < e.r)
                    nr--;

                else if (tr > e.r)
                    nr++;

                else if (tc < e.c)
                    nc--;

                else if (tc > e.c)
                    nc++;
            }

            // ------------------------------------------------
            // PATROL
            // ------------------------------------------------

            else {

                int direction = rand() % 4;

                int dr[4] = {-1, 1, 0, 0};
                int dc[4] = {0, 0, -1, 1};

                nr += dr[direction];
                nc += dc[direction];
            }

            // ------------------------------------------------
            // Move only if valid
            // ------------------------------------------------

            if (valid(nr, nc)) {

                e.r = nr;
                e.c = nc;
            }

            // ------------------------------------------------
            // Trap kills enemy
            // ------------------------------------------------

            if (board[e.r][e.c] == 'X') {

                e.alive = false;
            }
        }
    }

    // --------------------------------------------------------
    // CHECK PLAYER CAUGHT
    // --------------------------------------------------------

    bool playerCaught() {

        for (auto &e : enemies) {

            if (e.alive &&
                e.r == player.r &&
                e.c == player.c) {

                return true;
            }
        }

        return false;
    }

    // --------------------------------------------------------
    // SHOW DECISION TREE INFORMATION
    // --------------------------------------------------------

    void showEnemyAI() {

        cout << "\nEnemy AI uses a Decision Tree:\n\n";

        cout << "                Player Near?\n";
        cout << "                /          \\\n";
        cout << "              YES           NO\n";
        cout << "              /              \\\n";
        cout << "     Treasure Far?       Near Treasure?\n";
        cout << "       /     \\             /       \\\n";
        cout << "     YES     NO          YES        NO\n";
        cout << "      |       |           |          |\n";
        cout << "    CHASE    GUARD       GUARD     PATROL\n";

        _getch();
    }

    // --------------------------------------------------------
    // PLAY GAME
    // --------------------------------------------------------

    void play() {

        while (true) {

            draw();

            // ------------------------------------------------
            // Win condition
            // ------------------------------------------------

            if (player.r == tr &&
                player.c == tc) {

                cout << "\n\nYOU WIN!\n";

                cout << "Final Score: "
                     << player.score << "\n";

                saveGameResult("WIN");

                break;
            }

            // ------------------------------------------------
            // Enemy collision
            // ------------------------------------------------

            if (playerCaught()) {

                cout << "\n\nPLAYER IS CAUGHT!\n";

                cout << "Final Score: "
                     << player.score << "\n";

                saveGameResult("LOSE");

                break;
            }

            int key = _getch();

            // ------------------------------------------------
            // Quit
            // ------------------------------------------------

            if (key == 'q' || key == 'Q') {

                saveGameResult("QUIT");

                break;
            }

            // ------------------------------------------------
            // BFS Hint
            // ------------------------------------------------

            if (key == 'h' || key == 'H') {

                bfsHint();

                continue;
            }

            // ------------------------------------------------
            // Show Decision Tree
            // ------------------------------------------------

            if (key == 'd' || key == 'D') {

                showEnemyAI();

                continue;
            }

            // ------------------------------------------------
            // Undo
            // ------------------------------------------------

            if (key == 'u' || key == 'U') {

                if (!undoStack.empty()) {

                    player.r =
                        undoStack.top().first;

                    player.c =
                        undoStack.top().second;

                    undoStack.pop();

                }

                continue;
            }

            // ------------------------------------------------
            // Arrow Keys
            // ------------------------------------------------

            if (key == 224) {

                int r = player.r;
                int c = player.c;

                key = _getch();

                if (key == 72)
                    r--;          // Up

                else if (key == 80)
                    r++;          // Down

                else if (key == 75)
                    c--;          // Left

                else if (key == 77)
                    c++;          // Right

                // ------------------------------------------------
                // Only save position if movement is valid
                // ------------------------------------------------

                if (valid(r, c)) {

                    // Save previous position for Undo
                    undoStack.push(
                        {player.r, player.c}
                    );

                    player.r = r;
                    player.c = c;

                    // ------------------------------------------------
                    // Trap
                    // ------------------------------------------------

                    if (board[r][c] == 'X') {

                        int penalty =
                            5 + rand() % 10;

                        player.score -= penalty;

                        cout << "\nTrap!\n";
                        cout << "Score penalty: "
                             << penalty << "\n";

                        // Trap disappears after activation
                        board[r][c] = ' ';

                        _getch();
                    }

                    // ------------------------------------------------
                    // Move enemies after player moves
                    // ------------------------------------------------

                    moveEnemies();
                }
            }
        }
    }
};

// ------------------------------------------------------------
// MAIN FUNCTION
// ------------------------------------------------------------

int main() {

    char again = 'Y';

    while (again == 'Y' ||
           again == 'y') {

        MazeGame game;

        game.play();

        cout << "\nPlay Again? (Y/N): ";

        cin >> again;
    }

    cout << "\nThanks for playing!\n";

    return 0;
}
