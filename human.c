#include <stdio.h>
#include "human.h"
#include "board.h"
#include "game.h"

/**
 * @file human.c
 * @brief Implements user input handling for board size, menus, and human moves.
 */

 /**
  * @brief Discards any remaining characters in stdin until newline or EOF.
  *
  * This is used after failed scanf calls so the next prompt starts with a clean
  * input buffer.
  */
void clearInputBuffer(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

/**
 * @brief Prompts the user for the board dimension n.
 *
 * The project requires a square board with 4 <= n <= 128. This function loops
 * until a valid integer in that range is entered.
 *
 * @return A validated board dimension.
 */
int getBoardSize(void) {
    int n;

    while (true) {
        printf("Enter n (Board size will be n x n): ");
        if (scanf("%d", &n) != 1) {
            printf("Invalid input. Please enter an integer.\n");
            clearInputBuffer();
            continue;
        }

        if (n < MIN_SIZE || n > MAX_SIZE) {
            printf("Invalid size. Enter %d <= n <= %d.\n", MIN_SIZE, MAX_SIZE);
            continue;
        }

        return n;
    }
}

/**
 * @brief Prompts the user to choose one of the supported menu modes.
 *
 * @return 1, 2, or 3 depending on the validated user selection.
 */
int getMenuChoice(void) {
    int choice;

    while (true) {
        printf("1. Human vs. Human\n");
        printf("2. Human vs. Slow Computer\n");
        printf("3. Fast Computer vs. Slow Computer\n");
        printf("Select Choice (1-3): ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter 1, 2, or 3.\n");
            clearInputBuffer();
            continue;
        }

        if (choice >= 1 && choice <= 3) {
            return choice;
        }

        printf("Invalid choice. Please enter 1, 2, or 3.\n");
    }
}

/**
 * @brief Prompts for the number of simulated games in computer-vs-computer mode.
 *
 * @return A positive integer representing how many games to simulate.
 */
int getNumSimulatedGames(void) {
    int numGames;

    while (true) {
        printf("Enter # of simulated games?: ");
        if (scanf("%d", &numGames) != 1) {
            printf("Invalid input. Please enter a positive integer.\n");
            clearInputBuffer();
            continue;
        }

        if (numGames >= 1) {
            return numGames;
        }

        printf("Please enter a value of at least 1.\n");
    }
}

/**
 * @brief Prompts a human player to enter a legal move.
 *
 * The function repeatedly asks for input until the entered column:
 * - is numeric
 * - lies within the active board range
 * - is not already full
 *
 * @param board The current board.
 * @param turn The human player whose turn it is.
 * @return A valid column index.
 */
int getHumanMove(Board board, PlayerTurn turn) {
    int userInput;

    while (true) {
        printf("Player %c's turn!\nEnter column (0-%d): ", getPlayerChar(turn), board.num_cols - 1);

        if (scanf("%d", &userInput) != 1) {
            printf("Invalid input. Please enter a number.\n");
            clearInputBuffer();
            continue;
        }

        if (!isValidColumn(board, userInput)) {
            printf("Invalid column. Please enter a number between 0 and %d.\n", board.num_cols - 1);
            continue;
        }

        if (isColumnFull(board, userInput)) {
            printf("Column %d is full! Choose another column.\n", userInput);
            continue;
        }

        return userInput;
    }
}
