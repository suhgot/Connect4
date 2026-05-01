#include "game.h"

/**
 * @file game.c
 * @brief Implements shared helper functions used across multiple modules.
 */

 /**
  * @brief Converts the player enum into the board symbol used for display.
  *
  * @param turn The current player turn value.
  * @return 'X' if the player is X, otherwise 'O'.
  */
char getPlayerChar(PlayerTurn turn) {
    return (turn == X) ? 'X' : 'O';
}

/**
 * @brief Switches from one player's turn to the other player's turn.
 *
 * @param turn The current player.
 * @return O if the current player is X, otherwise X.
 */
PlayerTurn switchTurn(PlayerTurn turn) {
    return (turn == X) ? O : X;
}
