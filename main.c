#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#include "game.h"
#include "board.h"
#include "human.h"
#include "ai.h"

/**
 * @file main.c
 * @brief Program entry point and high-level game/simulation control logic.
 *
 * This file coordinates:
 * - menu selection
 * - single human-played games
 * - computer-vs-computer simulations
 * - per-turn timing measurements
 * - summary reporting for multiple simulations
 */

 /**
  * @struct SimulationStats
  * @brief Accumulates the required metrics for computer-vs-computer simulations.
  */
typedef struct {
    int fastWins;
    int slowWins;
    int ties;
    long long fastTurns;
    long long slowTurns;
    long long totalTurns;
    double fastTotalMs;
    double slowTotalMs;
} SimulationStats;

/**
 * @brief Converts a start/end timespec pair into elapsed milliseconds.
 *
 * @param start The starting timestamp.
 * @param end The ending timestamp.
 * @return Elapsed time in milliseconds.
 */
static double elapsedMilliseconds(struct timespec start, struct timespec end) {
    return (end.tv_sec - start.tv_sec) * 1e3 + (end.tv_nsec - start.tv_nsec) / 1e6;
}

/**
 * @brief Prints the final outcome of a completed game.
 *
 * @param gameState The ending win condition.
 */
static void printGameResult(WinCondition gameState) {
    if (gameState == X_WIN) {
        printf("Player X wins!\n");
    }
    else if (gameState == O_WIN) {
        printf("Player O wins!\n");
    }
    else if (gameState == TIE) {
        printf("Game ended in a tie!\n");
    }
}

/**
 * @brief Runs a human-involved game until a win or tie occurs.
 *
 * Supported modes here are:
 * - HUMAN_VS_HUMAN
 * - HUMAN_VS_SLOW_COMPUTER
 *
 * After each move, the function uses isGameOver_Fast because the last move's
 * location is already known.
 *
 * @param mode The selected menu mode.
 * @param board The initialized board.
 * @return The final game outcome.
 */
static WinCondition playHumanGame(int mode, Board board) {
    PlayerTurn turn = X;
    WinCondition gameState = NO_WINNER;

    printBoard(board);

    while (gameState == NO_WINNER) {
        int col;
        int row;

        if (mode == HUMAN_VS_SLOW_COMPUTER && turn == O) {
            printf("Player %c's turn!\n", getPlayerChar(turn));
            col = getSlowComputerMove(board, turn);
            printf("Slow Computer chose column %d\n", col);
        }
        else {
            col = getHumanMove(board, turn);
        }

        row = getNextOpenRow(board, col);
        board = updBoard(board, col, turn);
        printBoard(board);

        gameState = isGameOver_Fast(board, row, col, turn);

        if (gameState == NO_WINNER) {
            turn = switchTurn(turn);
        }
    }

    printGameResult(gameState);
    return gameState;
}

/**
 * @brief Runs one Fast Computer vs. Slow Computer game.
 *
 * The function optionally prints each move if displayMoves is true. Timing is
 * measured for each turn and includes:
 * - move selection
 * - move placement
 * - game-over evaluation
 *
 * Printing is deliberately performed after timing stops so console output does
 * not distort the measured algorithm runtime.
 *
 * @param n The board dimension.
 * @param displayMoves Whether to print each move and board state.
 * @param fastMs Output accumulator for fast-computer elapsed milliseconds.
 * @param slowMs Output accumulator for slow-computer elapsed milliseconds.
 * @param fastTurns Output counter for fast-computer turns.
 * @param slowTurns Output counter for slow-computer turns.
 * @param gameTurns Output counter for total turns in this game.
 * @return The final game outcome.
 */
static WinCondition playOneComputerGame(int n, bool displayMoves,
    double* fastMs, double* slowMs,
    long long* fastTurns, long long* slowTurns,
    long long* gameTurns) {
    Board board = initializeBoard(n);
    PlayerTurn turn = X;
    WinCondition gameState = NO_WINNER;

    if (displayMoves) {
        printBoard(board);
    }

    while (gameState == NO_WINNER) {
        int col;
        int row;
        struct timespec start, end;
        double elapsedMs;

        clock_gettime(CLOCK_MONOTONIC, &start);

        if (turn == X) {
            col = getFastComputerMove(board, X);
        }
        else {
            col = getSlowComputerMove(board, O);
        }

        row = getNextOpenRow(board, col);
        board = updBoard(board, col, turn);

        if (turn == X) {
            gameState = isGameOver_Fast(board, row, col, turn);
        }
        else {
            gameState = isGameOver_Slow(board);
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsedMs = elapsedMilliseconds(start, end);

        if (turn == X) {
            *fastMs += elapsedMs;
            (*fastTurns)++;
        }
        else {
            *slowMs += elapsedMs;
            (*slowTurns)++;
        }
        (*gameTurns)++;

        if (displayMoves) {
            if (turn == X) {
                printf("Fast Computer (X) chose column %d\n", col);
            }
            else {
                printf("Slow Computer (O) chose column %d\n", col);
            }
            printBoard(board);
        }

        if (gameState == NO_WINNER) {
            turn = switchTurn(turn);
        }
    }

    if (displayMoves) {
        printGameResult(gameState);
    }

    return gameState;
}

/**
 * @brief Runs the requested number of Fast-vs-Slow computer simulations.
 *
 * If exactly one game is requested, every move is displayed. If more than one
 * game is requested, the function suppresses detailed move printing and instead
 * reports the required summary metrics after all simulations complete.
 *
 * @param n The board dimension.
 * @param numGames Number of games to simulate.
 */
static void runComputerSimulation(int n, int numGames) {
    SimulationStats stats = { 0 };

    for (int game = 1; game <= numGames; game++) {
        double fastMs = 0.0;
        double slowMs = 0.0;
        long long fastTurns = 0;
        long long slowTurns = 0;
        long long gameTurns = 0;
        bool displayMoves = (numGames == 1);

        WinCondition result = playOneComputerGame(n, displayMoves,
            &fastMs, &slowMs,
            &fastTurns, &slowTurns,
            &gameTurns);

        stats.fastTotalMs += fastMs;
        stats.slowTotalMs += slowMs;
        stats.fastTurns += fastTurns;
        stats.slowTurns += slowTurns;
        stats.totalTurns += gameTurns;

        if (result == X_WIN) {
            stats.fastWins++;
        }
        else if (result == O_WIN) {
            stats.slowWins++;
        }
        else {
            stats.ties++;
        }

        if (numGames > 1) {
            printf("Completed game %d of %d\n", game, numGames);
        }
    }

    if (numGames > 1) {
        double fastAvgTurnMs = (stats.fastTurns > 0) ? stats.fastTotalMs / stats.fastTurns : 0.0;
        double slowAvgTurnMs = (stats.slowTurns > 0) ? stats.slowTotalMs / stats.slowTurns : 0.0;
        double avgTurnsPerGame = (double)stats.totalTurns / numGames;

        printf("\nSimulation Summary\n");
        printf("------------------------------\n");
        printf("Fast Computer win %%: %.2f%%\n", 100.0 * stats.fastWins / numGames);
        printf("Slow Computer win %%: %.2f%%\n", 100.0 * stats.slowWins / numGames);
        printf("Tie %%: %.2f%%\n", 100.0 * stats.ties / numGames);

        printf("\nPerformance Metrics\n");
        printf("Fast Computer average compute time per turn: %.3f ms\n", fastAvgTurnMs);
        printf("Slow Computer average compute time per turn: %.3f ms\n", slowAvgTurnMs);
        printf("Total # of turns played by Fast Computer: %lld\n", stats.fastTurns);
        printf("Total # of turns played by Slow Computer: %lld\n", stats.slowTurns);
        printf("Average # of turns per game: %.2f\n", avgTurnsPerGame);
    }
}

/**
 * @brief Program entry point.
 *
 * The program:
 * 1. Greets the user.
 * 2. Prompts for the board size n.
 * 3. Prompts for the game mode.
 * 4. Runs either a human game or a computer simulation.
 *
 * @return EXIT_SUCCESS on normal termination.
 */
int main(void) {
    printf("Welcome to Connect 4!\n");

    int n = getBoardSize();
    int mode = getMenuChoice();

    if (mode == FAST_COMPUTER_VS_SLOW_COMPUTER) {
        int numGames = getNumSimulatedGames();
        runComputerSimulation(n, numGames);
    }
    else {
        Board board = initializeBoard(n);
        playHumanGame(mode, board);
    }

    return EXIT_SUCCESS;
}
