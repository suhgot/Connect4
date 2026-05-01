#ifndef AI_H
#define AI_H

#include "game.h"

/**
 * @file ai.h
 * @brief Declarations for the slow and fast computer move-selection algorithms.
 */

int getSlowComputerMove(Board board, PlayerTurn computerTurn);

int getFastComputerMove(Board board, PlayerTurn computerTurn);

#endif