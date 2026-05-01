#include <limits.h>
#include "ai.h"
#include "board.h"

/**
 * @file ai.c
 * @brief Implements the slow and fast computer-player algorithms.
 *
 * Milestone 3 Task 3.3 improvement:
 * The Fast Computer avoids expensive full-board scans during move selection.
 * It checks only local lines near each candidate move and limits directional
 * scans to the maximum useful Connect-4 length.
 */

#define WIN_SCORE          100000000
#define BLOCK_SCORE         90000000
#define THREE_SCORE            50000
#define TWO_SCORE               1000
#define CENTER_SCORE              20
#define OPEN_END_SCORE           250
#define OPP_THREAT_SCORE        2000

static int absInt(int value) {
    return (value < 0) ? -value : value;
}

static int isInsideBoard(const Board* board, int row, int col) {
    return row >= 0 && row < board->num_rows &&
        col >= 0 && col < board->num_cols;
}

/**
 * @brief Counts consecutive pieces in one direction.
 *
 * This version is used by the slow player and may scan farther than needed.
 */
static int countDirection(Board board, int row, int col,
    int dRow, int dCol, char piece) {
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
 * @brief Counts matching pieces, but stops once Connect-4 information is enough.
 *
 * For the Fast Computer, scanning beyond NUM_WIN - 1 is unnecessary because
 * any line length >= NUM_WIN already means a winning line.
 */
static int countDirectionLimited(const Board* board, int row, int col,
    int dRow, int dCol, char piece) {
    int count = 0;
    int r = row + dRow;
    int c = col + dCol;

    while (count < NUM_WIN - 1 &&
        isInsideBoard(board, r, c) &&
        board->cell[c][r] == piece) {
        count++;
        r += dRow;
        c += dCol;
    }

    return count;
}

/**
 * @brief Checks whether the next cell after a line is open.
 */
static int hasOpenEnd(const Board* board, int row, int col,
    int dRow, int dCol, char piece) {
    int r = row + dRow;
    int c = col + dCol;

    while (isInsideBoard(board, r, c) && board->cell[c][r] == piece) {
        r += dRow;
        c += dCol;
    }

    return isInsideBoard(board, r, c) && board->cell[c][r] == '.';
}

/**
 * @brief Returns the total line length through a move in one axis.
 */
static int lineLengthLimited(const Board* board, int row, int col,
    int dRow, int dCol, char piece) {
    return 1 +
        countDirectionLimited(board, row, col, dRow, dCol, piece) +
        countDirectionLimited(board, row, col, -dRow, -dCol, piece);
}

/**
 * @brief Fast local win check for a candidate move.
 *
 * This avoids calling isGameOver_Slow or scanning the entire board.
 */
static int createsWinLocal(const Board* board, int row, int col, char piece) {
    if (lineLengthLimited(board, row, col, 0, 1, piece) >= NUM_WIN) {
        return 1;
    }

    if (lineLengthLimited(board, row, col, 1, 0, piece) >= NUM_WIN) {
        return 1;
    }

    if (lineLengthLimited(board, row, col, 1, 1, piece) >= NUM_WIN) {
        return 1;
    }

    if (lineLengthLimited(board, row, col, 1, -1, piece) >= NUM_WIN) {
        return 1;
    }

    return 0;
}

/**
 * @brief Scores one direction through the candidate move.
 */
static int scoreDirection(const Board* board, int row, int col,
    int dRow, int dCol, char piece) {
    int length = lineLengthLimited(board, row, col, dRow, dCol, piece);

    int openEnds = 0;
    openEnds += hasOpenEnd(board, row, col, dRow, dCol, piece);
    openEnds += hasOpenEnd(board, row, col, -dRow, -dCol, piece);

    if (length >= NUM_WIN) {
        return WIN_SCORE;
    }

    if (length == 3) {
        return THREE_SCORE + openEnds * OPEN_END_SCORE;
    }

    if (length == 2) {
        return TWO_SCORE + openEnds * OPEN_END_SCORE;
    }

    return 10 + openEnds;
}

/**
 * @brief Gives higher value to center columns.
 */
static int centerPreference(const Board* board, int col) {
    int center = board->num_cols / 2;
    return board->num_cols - absInt(col - center);
}

/**
 * @brief Evaluates a candidate move for the Fast Computer.
 *
 * The scoring is local and bounded. It rewards:
 * - immediate wins
 * - strong 3-in-a-row and 2-in-a-row extensions
 * - open-ended lines
 * - center control
 * - blocking opponent local threats
 */
static int scoreFastCandidate(Board* board, int row, int col,
    PlayerTurn computerTurn) {
    char myPiece = getPlayerChar(computerTurn);
    char oppPiece = getPlayerChar(switchTurn(computerTurn));

    int score = 0;

    board->cell[col][row] = myPiece;

    score += scoreDirection(board, row, col, 0, 1, myPiece);
    score += scoreDirection(board, row, col, 1, 0, myPiece);
    score += scoreDirection(board, row, col, 1, 1, myPiece);
    score += scoreDirection(board, row, col, 1, -1, myPiece);

    board->cell[col][row] = oppPiece;

    score += scoreDirection(board, row, col, 0, 1, oppPiece) / 2;
    score += scoreDirection(board, row, col, 1, 0, oppPiece) / 2;
    score += scoreDirection(board, row, col, 1, 1, oppPiece) / 2;
    score += scoreDirection(board, row, col, 1, -1, oppPiece) / 2;

    score += centerPreference(board, col) * CENTER_SCORE;

    board->cell[col][row] = '.';

    return score;
}

/**
 * @brief Finds an immediate winning column using bounded local checks.
 */
static int findImmediateMoveFast(Board* board, PlayerTurn testTurn) {
    char piece = getPlayerChar(testTurn);

    for (int col = 0; col < board->num_cols; col++) {
        if (!isColumnFull(*board, col)) {
            int row = getNextOpenRow(*board, col);

            board->cell[col][row] = piece;
            int wins = createsWinLocal(board, row, col, piece);
            board->cell[col][row] = '.';

            if (wins) {
                return col;
            }
        }
    }

    return -1;
}

/**
 * @brief Measures the strongest local line created by a temporary move.
 */
static int evaluateMove(Board board, int row, int col, char piece) {
    int horizontal = 1 + countDirection(board, row, col, 0, -1, piece)
        + countDirection(board, row, col, 0, 1, piece);

    int vertical = 1 + countDirection(board, row, col, -1, 0, piece)
        + countDirection(board, row, col, 1, 0, piece);

    int diagDown = 1 + countDirection(board, row, col, -1, -1, piece)
        + countDirection(board, row, col, 1, 1, piece);

    int diagUp = 1 + countDirection(board, row, col, 1, -1, piece)
        + countDirection(board, row, col, -1, 1, piece);

    int max = horizontal;

    if (vertical > max) {
        max = vertical;
    }

    if (diagDown > max) {
        max = diagDown;
    }

    if (diagUp > max) {
        max = diagUp;
    }

    return max;
}

/**
 * @brief Finds an immediate winning move for the slow player logic.
 */
static int findImmediateMoveSlow(Board board, PlayerTurn testTurn) {
    for (int col = 0; col < board.num_cols; col++) {
        if (!isColumnFull(board, col)) {
            Board temp = updBoard(board, col, testTurn);
            WinCondition result = isGameOver_Slow(temp);

            if ((testTurn == X && result == X_WIN) ||
                (testTurn == O && result == O_WIN)) {
                return col;
            }
        }
    }

    return -1;
}

/**
 * @brief Finds the move that extends the slow player's longest line.
 */
static int findBestExtensionMoveSlow(Board board, PlayerTurn computerTurn) {
    int bestCol = -1;
    int bestScore = -1;
    char piece = getPlayerChar(computerTurn);

    for (int col = 0; col < board.num_cols; col++) {
        if (!isColumnFull(board, col)) {
            int row = getNextOpenRow(board, col);
            Board temp = board;
            temp.cell[col][row] = piece;

            int score = evaluateMove(temp, row, col, piece);

            if (score > bestScore) {
                bestScore = score;
                bestCol = col;
            }
        }
    }

    return bestCol;
}

/**
 * @brief Computes a move for the slower baseline computer player.
 */
int getSlowComputerMove(Board board, PlayerTurn computerTurn) {
    PlayerTurn opponentTurn = switchTurn(computerTurn);

    int move = findImmediateMoveSlow(board, computerTurn);
    if (move != -1) {
        return move;
    }

    move = findImmediateMoveSlow(board, opponentTurn);
    if (move != -1) {
        return move;
    }

    move = findBestExtensionMoveSlow(board, computerTurn);
    if (move != -1) {
        return move;
    }

    for (int col = 0; col < board.num_cols; col++) {
        if (!isColumnFull(board, col)) {
            return col;
        }
    }

    return -1;
}

/**
 * @brief Computes a move for the optimized fast computer player.
 *
 * Strategy:
 * 1. Win immediately if possible.
 * 2. Block the opponent's immediate win.
 * 3. Otherwise evaluate each legal move using bounded local scoring.
 *
 * Runtime for one turn:
 * - Scans each column once for immediate win: O(n)
 * - Scans each column once for immediate block: O(n)
 * - Scores each legal column with constant-size local checks: O(n)
 * Therefore the Fast Computer move decision is O(n).
 */
int getFastComputerMove(Board board, PlayerTurn computerTurn) {
    PlayerTurn opponentTurn = switchTurn(computerTurn);

    int move = findImmediateMoveFast(&board, computerTurn);
    if (move != -1) {
        return move;
    }

    move = findImmediateMoveFast(&board, opponentTurn);
    if (move != -1) {
        return move;
    }

    int bestCol = -1;
    int bestScore = INT_MIN;

    for (int col = 0; col < board.num_cols; col++) {
        if (!isColumnFull(board, col)) {
            int row = getNextOpenRow(board, col);
            int score = scoreFastCandidate(&board, row, col, computerTurn);

            if (score > bestScore) {
                bestScore = score;
                bestCol = col;
            }
        }
    }

    return bestCol;
}