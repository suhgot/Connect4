#ifndef HUMAN_H
#define HUMAN_H

#include "game.h"

/**
 * @file human.h
 * @brief User-input helper declarations for menu choices, board size, and human moves.
 */

 /**
  * @brief Prompts the user for a valid board size n.
  *
  * @return An integer n such that MIN_SIZE <= n <= MAX_SIZE.
  */
int getBoardSize(void);

/**
 * @brief Prompts the user to select a menu option.
 *
 * @return One of the GameMode integer values.
 */
int getMenuChoice(void);

/**
 * @brief Prompts the user for the number of simulated computer games.
 *
 * @return A positive integer representing the number of games to simulate.
 */
int getNumSimulatedGames(void);

/**
 * @brief Prompts the current human player for a legal move.
 *
 * @param board The current board.
 * @param turn The player whose turn it is.
 * @return A valid non-full column index.
 */
int getHumanMove(Board board, PlayerTurn turn);

/**
 * @brief Clears unread characters from standard input after invalid input.
 */
void clearInputBuffer(void);

#endif
