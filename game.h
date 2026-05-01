#ifndef GAME_H
#define GAME_H

#include <stdbool.h>

/**
 * @file game.h
 * @brief Shared game-wide types, constants, and helper function declarations.
 *
 * This header defines the common data structures used across the Connect 4
 * program, including:
 * - board size limits
 * - win length
 * - player turn values
 * - game result values
 * - menu mode values
 * - the Board struct used by every module
 *
 * Milestone 3 changes:
 * - the board is no longer hard-coded to a fixed size
 * - the board is now an n x n grid chosen at runtime
 * - a third menu mode was added for Fast Computer vs. Slow Computer
 */

 /** @brief Maximum supported board dimension for an n x n board. */
#define MAX_SIZE 128

/** @brief Minimum supported board dimension for an n x n board. */
#define MIN_SIZE 4

/** @brief Number of connected pieces required to win. */
#define NUM_WIN 4

/**
 * @enum PlayerTurn
 * @brief Represents which player's turn it is.
 */
typedef enum { X, O } PlayerTurn;

/**
 * @enum WinCondition
 * @brief Represents the current game outcome.
 */
typedef enum { X_WIN, O_WIN, NO_WINNER, TIE } WinCondition;

/**
 * @enum GameMode
 * @brief Represents the menu option selected by the user.
 *
 * HUMAN_VS_HUMAN
 *   Two human players alternate moves.
 *
 * HUMAN_VS_SLOW_COMPUTER
 *   A human plays against the slower baseline computer player.
 *
 * FAST_COMPUTER_VS_SLOW_COMPUTER
 *   The optimized fast computer simulates games against the slow computer.
 */
typedef enum {
    HUMAN_VS_HUMAN = 1,
    HUMAN_VS_SLOW_COMPUTER = 2,
    FAST_COMPUTER_VS_SLOW_COMPUTER = 3
} GameMode;

/**
 * @struct Board
 * @brief Stores the active game board and its runtime dimensions.
 *
 * The full array is allocated at MAX_SIZE x MAX_SIZE so the program can support
 * any valid board up to that limit. Only the sub-region defined by num_rows and
 * num_cols is considered active during play.
 *
 * The project uses board.cell[col][row] indexing, meaning the first index is
 * the column and the second index is the row.
 */
typedef struct {
    char cell[MAX_SIZE][MAX_SIZE];
    int num_rows;
    int num_cols;
} Board;

/**
 * @brief Converts a PlayerTurn value into the printable board symbol.
 *
 * @param turn The current player enum value.
 * @return 'X' when turn is X, otherwise 'O'.
 */
char getPlayerChar(PlayerTurn turn);

/**
 * @brief Switches to the other player's turn.
 *
 * @param turn The current player.
 * @return O if turn is X, otherwise X.
 */
PlayerTurn switchTurn(PlayerTurn turn);

#endif
