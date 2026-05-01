#include <stdio.h>
#include "board.h"

/**
 * @file board.c
 * @brief Implements board creation, printing, move placement, and win checks.
 */

 /**
  * @brief Creates an empty board of size n x n.
  *
  * Every active cell is initialized to '.'. The board uses [col][row] indexing,
  * so the outer loop over rows and inner loop over columns simply fill the active
  * rectangular region that will be used during the game.
  *
  * @param n The board dimension selected by the user.
  * @return A fully initialized Board value.
  */
Board initializeBoard(int n) {
    Board board;
    board.num_rows = n;
    board.num_cols = n;

    for (int r = 0; r < board.num_rows; r++) {
        for (int c = 0; c < board.num_cols; c++) {
            board.cell[c][r] = '.';
        }
    }

    return board;
}

/**
 * @brief Prints a horizontal border sized to the active board width.
 *
 * @param board The current board.
 */
void printBorder(Board board) {
    for (int i = 0; i < board.num_cols + 2; i++) {
        printf("-");
    }
    printf("\n");
}

/**
 * @brief Prints the current board state to the console.
 *
 * The board is surrounded by vertical and horizontal borders. A row of column
 * labels is also printed underneath so the user can see which numeric input
 * corresponds to which column.
 *
 * @param board The current board.
 */
void printBoard(Board board) {
    printBorder(board);
    for (int r = 0; r < board.num_rows; r++) {
        printf("|");
        for (int c = 0; c < board.num_cols; c++) {
            printf("%c", board.cell[c][r]);
        }
        printf("|\n");
    }
    printBorder(board);

    printf(" ");
    for (int c = 0; c < board.num_cols; c++) {
        printf("%d", c % 10);
    }
    printf("\n");
}

/**
 * @brief Determines whether a column index is valid for the active board.
 *
 * @param board The current board.
 * @param col The column index to test.
 * @return true if col is between 0 and board.num_cols - 1, otherwise false.
 */
bool isValidColumn(Board board, int col) {
    return (col >= 0 && col < board.num_cols);
}

/**
 * @brief Determines whether the specified column is already full.
 *
 * A column is full when its topmost cell is no longer empty.
 *
 * @param board The current board.
 * @param col The column to test.
 * @return true if no additional piece can be inserted into the column.
 */
bool isColumnFull(Board board, int col) {
    return board.cell[col][0] != '.';
}

/**
 * @brief Finds the next open row within a given column.
 *
 * Because pieces fall downward, the search starts at the bottom row and moves
 * upward until an empty cell is found.
 *
 * @param board The current board.
 * @param col The target column.
 * @return The row index where the piece would land, or -1 if the column is full.
 */
int getNextOpenRow(Board board, int col) {
    for (int r = board.num_rows - 1; r >= 0; r--) {
        if (board.cell[col][r] == '.') {
            return r;
        }
    }
    return -1;
}

/**
 * @brief Checks whether every column is full.
 *
 * @param board The current board.
 * @return true if the game board has no legal moves remaining, otherwise false.
 */
bool isBoardFull(Board board) {
    for (int c = 0; c < board.num_cols; c++) {
        if (board.cell[c][0] == '.') {
            return false;
        }
    }
    return true;
}

/**
 * @brief Brute-force game-over check that scans the full board.
 *
 * This routine preserves the original slower behavior for comparison and
 * analysis. It checks every possible horizontal, vertical, and diagonal start
 * position that could contain a connect-4. If no winner is found, it then checks
 * whether the board is full to determine a tie.
 *
 * @param board The current board.
 * @return X_WIN, O_WIN, TIE, or NO_WINNER.
 */
WinCondition isGameOver_Slow(Board board) {
    for (int r = 0; r < board.num_rows; r++) {
        for (int c = 0; c <= board.num_cols - NUM_WIN; c++) {
            char start = board.cell[c][r];
            if (start != '.') {
                bool win = true;
                for (int k = 1; k < NUM_WIN; k++) {
                    if (board.cell[c + k][r] != start) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return (start == 'X') ? X_WIN : O_WIN;
                }
            }
        }
    }

    for (int c = 0; c < board.num_cols; c++) {
        for (int r = 0; r <= board.num_rows - NUM_WIN; r++) {
            char start = board.cell[c][r];
            if (start != '.') {
                bool win = true;
                for (int k = 1; k < NUM_WIN; k++) {
                    if (board.cell[c][r + k] != start) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return (start == 'X') ? X_WIN : O_WIN;
                }
            }
        }
    }

    for (int r = 0; r <= board.num_rows - NUM_WIN; r++) {
        for (int c = 0; c <= board.num_cols - NUM_WIN; c++) {
            char first = board.cell[c][r];
            if (first != '.') {
                bool win = true;
                for (int k = 1; k < NUM_WIN; k++) {
                    if (board.cell[c + k][r + k] != first) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return (first == 'X') ? X_WIN : O_WIN;
                }
            }
        }
    }

    for (int r = NUM_WIN - 1; r < board.num_rows; r++) {
        for (int c = 0; c <= board.num_cols - NUM_WIN; c++) {
            char first = board.cell[c][r];
            if (first != '.') {
                bool win = true;
                for (int k = 1; k < NUM_WIN; k++) {
                    if (board.cell[c + k][r - k] != first) {
                        win = false;
                        break;
                    }
                }
                if (win) {
                    return (first == 'X') ? X_WIN : O_WIN;
                }
            }
        }
    }

    if (isBoardFull(board)) {
        return TIE;
    }

    return NO_WINNER;
}

/**
 * @brief Counts consecutive matching pieces in one direction from a starting cell.
 *
 * The starting cell itself is not counted here. Instead, the function begins one
 * step away and continues as long as the same piece is found.
 *
 * @param board The current board.
 * @param row The starting row.
 * @param col The starting column.
 * @param dRow Row step for each move in the chosen direction.
 * @param dCol Column step for each move in the chosen direction.
 * @param piece The piece character being counted.
 * @return Number of consecutive matching pieces found.
 */
static int countDirection(Board board, int row, int col, int dRow, int dCol, char piece) {
    int count = 0;
    int r = row + dRow;
    int c = col + dCol;

    while (r >= 0 && r < board.num_rows &&
        c >= 0 && c < board.num_cols &&
        board.cell[c][r] == piece) {
        count++;
        r += dRow;
        c += dCol;
    }

    return count;
}

/**
 * @brief Optimized game-over check based only on the most recent move.
 *
 * Since only the latest move can create a new win, this function checks the four
 * relevant line directions that pass through that location:
 * - horizontal
 * - vertical
 * - diagonal descending
 * - diagonal ascending
 *
 * For each direction, it counts matching pieces on both sides of the latest move
 * and adds 1 for the placed piece itself. If any total reaches NUM_WIN, the game
 * is won. If no win is found, it then checks for a tie.
 *
 * @param board The current board.
 * @param lastRow The row of the most recent move.
 * @param lastCol The column of the most recent move.
 * @param turn The player who placed the most recent piece.
 * @return X_WIN, O_WIN, TIE, or NO_WINNER.
 */
WinCondition isGameOver_Fast(Board board, int lastRow, int lastCol, PlayerTurn turn) {
    char piece = getPlayerChar(turn);

    int horizontal = 1 + countDirection(board, lastRow, lastCol, 0, -1, piece)
        + countDirection(board, lastRow, lastCol, 0, 1, piece);
    if (horizontal >= NUM_WIN) {
        return (turn == X) ? X_WIN : O_WIN;
    }

    int vertical = 1 + countDirection(board, lastRow, lastCol, -1, 0, piece)
        + countDirection(board, lastRow, lastCol, 1, 0, piece);
    if (vertical >= NUM_WIN) {
        return (turn == X) ? X_WIN : O_WIN;
    }

    int diagDown = 1 + countDirection(board, lastRow, lastCol, -1, -1, piece)
        + countDirection(board, lastRow, lastCol, 1, 1, piece);
    if (diagDown >= NUM_WIN) {
        return (turn == X) ? X_WIN : O_WIN;
    }

    int diagUp = 1 + countDirection(board, lastRow, lastCol, 1, -1, piece)
        + countDirection(board, lastRow, lastCol, -1, 1, piece);
    if (diagUp >= NUM_WIN) {
        return (turn == X) ? X_WIN : O_WIN;
    }

    if (isBoardFull(board)) {
        return TIE;
    }

    return NO_WINNER;
}

/**
 * @brief Drops a player's piece into the requested column.
 *
 * This function first finds the next available row in the column and then writes
 * the correct piece symbol into that location.
 *
 * @param board The current board.
 * @param col The target column.
 * @param turn The current player.
 * @return A copy of the board with the move applied if possible.
 */
Board updBoard(Board board, int col, PlayerTurn turn) {
    int row = getNextOpenRow(board, col);
    if (row != -1) {
        board.cell[col][row] = getPlayerChar(turn);
    }
    return board;
}
