#ifndef BOARD_H
#define BOARD_H

#include "game.h"

/**
 * @file board.h
 * @brief Board-management and game-state evaluation function declarations.
 *
 * This module is responsible for:
 * - creating the board
 * - printing the board
 * - validating columns
 * - dropping pieces into columns
 * - checking whether the board is full
 * - determining whether the game has been won or tied
 *
 * Milestone 3 changes:
 * - board initialization now accepts a runtime board size n
 * - the original brute-force game-over routine is preserved as isGameOver_Slow
 * - a new optimized routine is provided as isGameOver_Fast
 */

 /**
  * @brief Creates and initializes an empty n x n board.
  *
  * @param n The requested board dimension.
  * @return A Board whose active area is n rows by n columns and whose cells are
  *         initialized to '.'.
  */
Board initializeBoard(int n);

/**
 * @brief Prints the top or bottom border line for the board.
 *
 * @param board The current board.
 */
void printBorder(Board board);

/**
 * @brief Prints the full board contents and column labels.
 *
 * @param board The current board.
 */
void printBoard(Board board);

/**
 * @brief Brute-force game-over check that scans the full board.
 *
 * This function preserves the original milestone-1 style behavior. It searches
 * the entire board for horizontal, vertical, and diagonal connect-4 patterns,
 * then checks for a tie if the board is full.
 *
 * @param board The current board.
 * @return The current win condition.
 */
WinCondition isGameOver_Slow(Board board);

/**
 * @brief Optimized game-over check based on the most recent move.
 *
 * Instead of scanning the entire board, this routine checks only the row,
 * column, and diagonals passing through the most recently placed piece.
 *
 * @param board The current board.
 * @param lastRow The row index of the most recent move.
 * @param lastCol The column index of the most recent move.
 * @param turn The player who made the most recent move.
 * @return The current win condition.
 */
WinCondition isGameOver_Fast(Board board, int lastRow, int lastCol, PlayerTurn turn);

/**
 * @brief Drops the current player's piece into the specified column.
 *
 * @param board The current board.
 * @param col The target column.
 * @param turn The current player.
 * @return A copy of the board with the new piece inserted if the column is not full.
 */
Board updBoard(Board board, int col, PlayerTurn turn);

/**
 * @brief Checks whether a column index is inside the active board range.
 *
 * @param board The current board.
 * @param col The column index to validate.
 * @return true if the column is valid, otherwise false.
 */
bool isValidColumn(Board board, int col);

/**
 * @brief Checks whether a column can accept another piece.
 *
 * @param board The current board.
 * @param col The target column.
 * @return true if the column is full, otherwise false.
 */
bool isColumnFull(Board board, int col);

/**
 * @brief Finds the row where a piece would land in a given column.
 *
 * @param board The current board.
 * @param col The target column.
 * @return The landing row index, or -1 if the column is full.
 */
int getNextOpenRow(Board board, int col);

/**
 * @brief Checks whether the entire board is full.
 *
 * @param board The current board.
 * @return true if no additional moves can be made, otherwise false.
 */
bool isBoardFull(Board board);

#endif
