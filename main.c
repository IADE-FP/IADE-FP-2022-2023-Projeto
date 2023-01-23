#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief The player structure.
 *
 * This structure contains all the information about a registered player.
 */
typedef struct {
    char* name;        ///< The name of the player.
    int games_played;  ///< The number of games played by the player.
    int wins;          ///< The number of games won by the player.
} tPlayer, *pPlayer;

/**
 * @brief The in-game player structure.
 *
 * This structure contains all the information about a player in a game.
 */
typedef struct {
    pPlayer player;             ///< Pointer to the player structure.
    int* special_sequences;     ///< Array of special sequences.
    int num_special_sequences;  ///< The number of special sequences.
} tInGamePlayer, *pInGamePlayer;

/**
 * @brief The game structure.
 *
 * This structure contains all the information about the game.
 */
typedef struct {
    pPlayer* players;           ///< Array of registered players.
    int num_players;            ///< The number of registered players.
    int width;                  ///< The width of the board.
    int height;                 ///< The height of the board.
    int sequence_size;          ///< The size of the winning sequence.
    int* special_sequences;     ///< Array of special sequences.
    int num_special_sequences;  ///< The number of special sequences.
    pInGamePlayer player1;      ///< Pointer to the first player.
    pInGamePlayer player2;      ///< Pointer to the second player.
    pInGamePlayer** board;      ///< The board, with dimensions height x width.
} tGame, *pGame;

/**
 * @brief Create and initialize a new game.
 *
 * This function creates and initializes a new game, allocating memory for the
 * structure. It is the responsibility of the caller to free this memory.
 *
 * @return pGame Pointer to a malloc reserved tGame structure.
 */
pGame new_game() {
    pGame game = malloc(sizeof(tGame));
    game->players = NULL;
    game->num_players = 0;
    game->player1 = NULL;
    game->player2 = NULL;
    game->special_sequences = NULL;
    game->width = -1;
    game->height = -1;
    game->sequence_size = -1;
    game->num_special_sequences = 0;
    game->board = NULL;
    return game;
}

/**
 * @brief Frees the memory associated to a tGame.
 *
 * This function frees the memory associated to a tGame, and dependencies.
 *
 * @param game Pointer to a tGame structure.
 */
void free_game(pGame game) {
    for (int i = 0; i < game->num_players; i++) {
        free(game->players[i]->name);
        free(game->players[i]);
    }
    if (game->num_players > 0) {
        free(game->players);
    }
    if (game->player1 != NULL) {
        free(game->player1->special_sequences);
        free(game->player1);
    }
    if (game->player2 != NULL) {
        free(game->player2->special_sequences);
        free(game->player2);
    }
    if (game->special_sequences != NULL) {
        free(game->special_sequences);
    }
    if (game->board != NULL) {
        for (int r = 0; r < game->height; r++) {
            free(game->board[r]);
        }
        free(game->board);
    }
    free(game);
}

/**
 * @brief Get the player idx object
 *
 * This function returns the index of the player in the game->players array, or
 * -1 if not found.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 * @return int The index of the player in the game->players array, or -1 if not
 * found.
 */
int get_player_idx(pGame game, char* name) {
    for (int i = 0; i < game->num_players; i++) {
        if (strcmp(game->players[i]->name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Get the player object
 *
 * This function returns a pointer to a tPlayer structure, or NULL if the player
 * is not found.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 * @return pPlayer Pointer to a tPlayer structure, or NULL if not found.
 */
pPlayer get_player(pGame game, char* name) {
    int idx = get_player_idx(game, name);
    if (idx != -1) {
        return game->players[idx];
    }
    return NULL;
}

/**
 * @brief Check if a player exists in the game.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 * @return true If the player exists.
 * @return false If the player does not exist.
 */
bool has_player(pGame game, char* name) {
    return get_player(game, name) != NULL;
}

/**
 * @brief Add a player to the game.
 *
 * This function adds a player to the game, allocating memory for the player,
 * increasing the memory for the players array, and appending the player to the
 * end of the array.
 *
 * This function allocates memory for the player, and the player->name field. It
 * is the responsibility of the caller to free this memory.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 */
void add_player(pGame game, char* name) {
    game->num_players++;
    game->players = realloc(game->players, sizeof(pPlayer) * game->num_players);
    game->players[game->num_players - 1] = malloc(sizeof(tPlayer));
    game->players[game->num_players - 1]->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(game->players[game->num_players - 1]->name, name);
    game->players[game->num_players - 1]->games_played = 0;
    game->players[game->num_players - 1]->wins = 0;
}

/**
 * @brief Remove a player from the set of registered players.
 *
 * This functions frees memory associated with the player, and removes the
 * player from the collection of registered players.
 *
 * This function frees the memory associated with the player, and reallocates
 * memory for the players array. It is the responsibility of the caller to free
 * the array.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 */
void remove_player(pGame game, char* name) {
    int idx = get_player_idx(game, name);
    pPlayer player = game->players[idx];
    size_t num_players_to_move = game->num_players - idx - 1;
    if (num_players_to_move > 0) {
        memmove(&game->players[idx], &game->players[idx + 1], sizeof(pPlayer) * num_players_to_move);
    }
    free(player->name);
    free(player);
    game->num_players--;
    game->players = realloc(game->players, sizeof(pPlayer) * game->num_players);
    if (game->num_players == 0) {
        game->players = NULL;
    }
}

/**
 * @brief Check if a player is in the current game.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 * @return true If the player is in the game.
 * @return false If the player is not in the game.
 */
bool player_in_game(pGame game, char* name) {
    pPlayer player = get_player(game, name);
    return game->player1->player == player || game->player2->player == player;
}

/**
 * @brief Check if the game has any registered players.
 *
 * @param game Pointer to a tGame structure.
 * @return true If the game has any players.
 * @return false If the game has no players.
 */
bool has_players(pGame game) {
    return game->num_players > 0;
}

/**
 * @brief Comparison function for sorting players by name.
 *
 * @param p1 Pointer to a pointer to a tPlayer structure.
 * @param p2 Pointer to a pointer to a tPlayer structure.
 * @return int The result of the comparison between player names.
 */
int comp_players(pPlayer* p1, pPlayer* p2) {
    return strcmp((*p1)->name, (*p2)->name);
}

/**
 * @brief Check if there is a game in progress.
 *
 * @param game Pointer to a tGame structure.
 * @return true If there is a game in progress.
 * @return false If there is no game in progress.
 */
bool in_game(pGame game) {
    return game->board != NULL;
}

/**
 * @brief Check if the given dimensions are valid.
 *
 * The dimensions are valid if the height is greater than or equal to half of the width.
 *
 * @param width The width of the board.
 * @param height The height of the board.
 * @return true If the dimensions are valid.
 * @return false If the dimensions are not valid.
 */
bool valid_dimensions(int width, int height) {
    return height >= width / 2;
}

/**
 * @brief Check if the given winning sequence size is valid.
 *
 * The sequence size is valid if it is less than the width of the board.
 *
 * @param width The width of the board.
 * @param sequence_size The size of the sequence.
 * @return true If the sequence size is valid.
 * @return false If the sequence size is not valid.
 */
bool valid_sequence(int width, int sequence_size) {
    return width > sequence_size;
}

/**
 * @brief Check if the given special sequences are valid.
 *
 * The special sequences are valid if they are less than the winning sequence size.
 *
 * @param sequence_size The size of the winning sequence.
 * @param special_sequences Array of special sequence sizes.
 * @param num_special_sequences The number of special sequence sizes.
 * @return true If the special sequence sizes are valid.
 * @return false If the special sequence sizes are not valid.
 */
bool valid_special_sequences(int sequence_size, int* special_sequences, int num_special_sequences) {
    for (int i = 0; i < num_special_sequences; i++) {
        if (special_sequences[i] >= sequence_size) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Allocate memory for, and initialize, a new tInGamePlayer structure.
 *
 * This function allocates memory for a new tInGamePlayer structure, and
 * initializes it. It returns a pointer to the new structure in the player
 * parameter.
 *
 * This function allocates memory for the special sequences array, and is the
 * responsibility of the caller to free it.
 *
 * @param game Pointer to a tGame structure.
 * @param[out] player Pointer to a pInGamePlayer structure pointer.  It will be
 * set to point to the new tInGamePlayer structure.
 * @param name The name of the player.
 * @param special_sequences Array of special sequence sizes.
 * @param num_special_sequences The number of special sequence sizes.
 */
void start_game_player(pGame game, pInGamePlayer* player, char* name, int* special_sequences, int num_special_sequences) {
    (*player) = malloc(sizeof(tInGamePlayer));
    (*player)->player = get_player(game, name);
    (*player)->special_sequences = malloc(sizeof(int) * num_special_sequences);
    memcpy((*player)->special_sequences, special_sequences, sizeof(int) * num_special_sequences);
    (*player)->num_special_sequences = num_special_sequences;
}

/**
 * @brief Start a new game.
 *
 * This function starts a new game. It initializes the players, the board, and
 * the winning sequence size.
 *
 * This function allocates memory for the board, and for the special sequences
 * of the players. It is the responsibility of the caller to free this memory.
 *
 * @param game Pointer to a tGame structure.
 * @param player1_name The name of the first player.
 * @param player2_name The name of the second player.
 * @param width The width of the board.
 * @param height The height of the board.
 * @param sequence_size The size of the winning sequence.
 * @param special_sequences Array of special sequence sizes.
 * @param num_special_sequences The number of special sequence sizes.
 */
void start_game(pGame game, char* player1_name, char* player2_name, int width, int height, int sequence_size, int* special_sequences, int num_special_sequences) {
    start_game_player(game, &(game->player1), player1_name, special_sequences, num_special_sequences);
    start_game_player(game, &(game->player2), player2_name, special_sequences, num_special_sequences);
    game->sequence_size = sequence_size;
    game->special_sequences = malloc(sizeof(int) * num_special_sequences);
    memcpy(game->special_sequences, special_sequences, sizeof(int) * num_special_sequences);
    game->num_special_sequences = num_special_sequences;
    game->width = width;
    game->height = height;
    game->board = malloc(sizeof(pInGamePlayer*) * game->height);
    for (int r = 0; r < game->height; r++) {
        game->board[r] = malloc(sizeof(pInGamePlayer) * game->width);
        for (int c = 0; c < game->width; c++) {
            game->board[r][c] = NULL;
        }
    }
}

/**
 * @brief Build an array of unique special sequences from the current game.
 *
 * This function builds an array of unique special sequences from the current
 * game. It returns a pointer to the array, and sets the
 * num_unique_special_sequences parameter to the number of unique special
 * sequences.
 *
 * This function allocates memory for the array. It is the responsibility of the
 * caller to free the memory.
 *
 * @param game Pointer to a tGame structure.
 * @param num_unique_special_sequences Pointer to an int. It will be set to the
 * number of unique special sequences.
 * @return int* Pointer to an array of unique special sequences.
 */
int* get_unique_special_sequences(pGame game, int* num_unique_special_sequences) {
    int* unique_special_sequences = NULL;
    *num_unique_special_sequences = 0;
    for (int i = 0; i < game->num_special_sequences; i++) {
        bool found = false;
        for (int j = 0; j < *num_unique_special_sequences; j++) {
            if (game->special_sequences[i] == unique_special_sequences[j]) {
                found = true;
                break;
            }
        }
        if (!found) {
            (*num_unique_special_sequences)++;
            unique_special_sequences = realloc(unique_special_sequences, sizeof(int) * *num_unique_special_sequences);
            unique_special_sequences[*num_unique_special_sequences - 1] = game->special_sequences[i];
        }
    }
    return unique_special_sequences;
}

/**
 * @brief Get the in game player object.
 *
 * This function returns the in game player object for the player with the given
 * name.
 *
 * An inGamePlayer object is a structure that contains a pointer to the player
 * object and its special sequences.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 * @return pInGamePlayer Pointer to the in game player object.
 */
pInGamePlayer get_in_game_player(pGame game, char* name) {
    if (strcmp(game->player1->player->name, name) == 0) {
        return game->player1;
    }
    return game->player2;
}

/**
 * @brief Get the other InGamePlayer object.
 *
 * Returns the InGamePlayer object for the player with a different name.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the opposite player.
 * @return pInGamePlayer Pointer to an InGamePlayer object.
 */
pInGamePlayer get_other_in_game_player(pGame game, char* name) {
    if (strcmp(game->player1->player->name, name) == 0) {
        return game->player2;
    }
    return game->player1;
}

/**
 * @brief Validate the size of a special sequence.
 *
 * This function validates the size of a special sequence. It returns true if
 * the size is available to the player with the given name.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 * @param size The size of the special sequence.
 * @return true If the size is available to the player.
 * @return false If the size is not available to the player.
 */
bool valid_size(pGame game, char* name, int size) {
    if (size == 1) return true;
    if (size <= 0) return false;

    pInGamePlayer player = get_in_game_player(game, name);
    for (int i = 0; i < player->num_special_sequences; i++) {
        if (player->special_sequences[i] == size) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Get the starting column.
 *
 * This function returns the starting column for a sequence of the given size,
 * placed in the given column, and with the given direction.
 *
 * The stating column is the leftmost column where the sequence would be placed.
 *
 * @param size The size of the sequence.
 * @param column The column where the sequence is placed.
 * @param direction The direction of the sequence.
 * @return int The starting (leftmost) column.
 */
int get_starting_column(int size, int column, char* direction) {
    int dir = 1;
    if (direction != NULL) {
        dir = strcmp(direction, "D") == 0 ? 0 : -1;
    }
    int col = column - 1;
    int shift = (size - 1) * dir;
    return col + shift;
}

/**
 * @brief Check if a position is valid.
 *
 * This function checks if a position is valid. A position is valid if it is
 * within the board, and if all its pieces could be placed in the first (top)
 * row.
 *
 * @param game Pointer to a tGame structure.
 * @param size The size of the sequence.
 * @param column The column where the sequence is placed.
 * @param direction The direction of the sequence.
 * @return bool True if the position is valid, false otherwise.
 */
bool valid_position(pGame game, int size, int column, char* direction) {
    int col = get_starting_column(size, column, direction);
    if (col < 0 || col + size > game->width) {
        return false;
    }
    for (int c = col; c < col + size; c++) {
        if (game->board[0][c] != NULL) {
            return false;
        }
    }
    return true;
}

/**
 * @brief Remove a special sequence from a player.
 *
 * This function removes a special sequence size instance from a player.
 *
 * @param player Pointer to a tInGamePlayer structure.
 * @param size The size of the special sequence.
 */
void remove_special_sequence(pInGamePlayer player, int size) {
    for (int i = 0; i < player->num_special_sequences; i++) {
        if (player->special_sequences[i] == size) {
            int offset = player->num_special_sequences - i - 1;
            memmove(&(player->special_sequences[i]), &(player->special_sequences[i + 1]), offset * sizeof(int));
            player->num_special_sequences--;
            break;
        }
    }
}

/**
 * @brief Drop a sequence.
 *
 * This function drops a sequence of the given size, placed in the given column,
 * and with the given direction.
 *
 * The function returns the placement coordinates of the pieces of the sequence.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 * @param size The size of the sequence.
 * @param column The column where the sequence is placed.
 * @param direction The direction of the sequence.
 * @param[out] lines The line numbers of the pieces of the sequence.
 * @param[out] cols The column numbers of the pieces of the sequence.
 */
void drop(pGame game, char* name, int size, int column, char* direction, int* lines, int* columns) {
    pInGamePlayer player = get_in_game_player(game, name);
    int col = get_starting_column(size, column, direction);
    int idx = 0;
    for (int c = col; c < col + size; c++) {
        bool placed = false;
        for (int l = 0; l < game->height && !placed; l++) {
            if (game->board[l][c] != NULL) {
                game->board[l - 1][c] = player;
                lines[idx] = l - 1;
                columns[idx] = c;
                placed = true;
            }
        }
        if (!placed) {
            game->board[game->height - 1][c] = player;
            lines[idx] = game->height - 1;
            columns[idx] = c;
        }
    }
    if (size > 1) {
        remove_special_sequence(player, size);
    }
}

/**
 * @brief Count pieces in a given direction.
 *
 * This function counts the number of pieces of the given player in a given
 * direction, starting from the given line and column.
 *
 * The direction is given by the l_shift and c_shift parameters. For example, to
 * count the number of pieces in the direction of the top-left diagonal, the
 * l_shift and c_shift parameters should be -1 and -1, respectively.
 *
 * @param game Pointer to a tGame structure.
 * @param player Pointer to a tInGamePlayer structure.
 * @param line The line where the counting starts.
 * @param column The column where the counting starts.
 * @param l_shift The line shift.
 * @param c_shift The column shift.
 * @return int The number of pieces in the given direction.
 */
int count_pieces(pGame game, pInGamePlayer player, int line, int column, int l_shift, int c_shift) {
    int count = 0;
    int l = line;
    int c = column;
    while (l >= 0 && l < game->height && c >= 0 && c < game->width && game->board[l][c] == player) {
        if (!(l == line && c == column)) {
            count++;
        }
        l += l_shift;
        c += c_shift;
    }
    return count;
}

/**
 * @brief Check if a player won the game.
 *
 * This function checks if the given player wins. The player wins if he has a
 * sequence equal to or greater than the number of pieces required to win. The
 * given line and column indicate one of the positions recently placed by the
 * player.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 * @param line The line where the piece was placed.
 * @param column The column where the piece was placed.
 * @return bool True if the player wins, false otherwise.
 */
bool player_wins(pGame game, char* name, int line, int column) {
    pInGamePlayer player = get_in_game_player(game, name);
    // -1,-1 |       -1,0      | -1,+1
    // --------------------------------
    //  0,-1 |   line,column   |  0,+1
    // --------------------------------
    // +1,+1 |       +1,0      | -1,0
    for (int l_shift = 0; l_shift <= 1; l_shift++) {
        for (int c_shift = 0; c_shift <= 1; c_shift++) {
            if (l_shift == 0 && c_shift == 0) {
                continue;
            }
            int start_segment = count_pieces(game, player, line, column, l_shift, c_shift);
            int end_segment = count_pieces(game, player, line, column, -l_shift, -c_shift);
            if ((start_segment + 1 + end_segment) >= game->sequence_size) {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief Terminates the current game.
 *
 * This function terminates the current game. The function should be called when
 * a player wins or when the game is a draw.
 *
 * The second_name parameter may be NULL if one of the players wins. If the game
 * is a draw, no player records a win.
 *
 * @param game Pointer to a tGame structure.
 * @param first_name The name of the first player.
 * @param second_name The name of the second player.
 */
void game_over(pGame game, char* first_name, char* second_name) {
    pInGamePlayer player = get_in_game_player(game, first_name);
    if (second_name == NULL) {
        get_other_in_game_player(game, first_name)->player->wins++;
    }

    game->player1->player->games_played++;
    game->player2->player->games_played++;

    for (int l = 0; l < game->height; l++) {
        free(game->board[l]);
    }
    free(game->board);
    game->board = NULL;

    free(game->special_sequences);
    game->special_sequences = NULL;

    free(game->player1->special_sequences);
    free(game->player2->special_sequences);
    free(game->player1);
    free(game->player2);
    game->player1 = NULL;
    game->player2 = NULL;
}

/**
 * @brief Prints the number of special sequences of a given player.
 *
 * @param game Pointer to a tGame structure.
 * @param name The name of the player.
 * @return int The number of special sequences of the given player.
 */
void print_player_special_sequences(pGame game, pInGamePlayer player) {
    int num_unique_special_sequences;
    int* unique_special_sequences = get_unique_special_sequences(game, &num_unique_special_sequences);
    for (int i = 0; i < num_unique_special_sequences; i++) {
        int count = 0;
        for (int j = 0; j < player->num_special_sequences; j++) {
            if (player->special_sequences[j] == unique_special_sequences[i]) {
                count++;
            }
        }
        printf("%d %d\n", unique_special_sequences[i], count);
    }
    free(unique_special_sequences);
}

/**
 * @brief Saves the game to a file.
 *
 * @param game Pointer to a tGame structure.
 * @param filename The name of the file.
 */
void save_game(pGame game, char* filename) {
    FILE* fp = fopen(filename, "w");
    fprintf(fp, "%d\n", game->num_players);
    for (int i = 0; i < game->num_players; i++) {
        fprintf(fp, "%s %d %d\n", game->players[i]->name, game->players[i]->games_played, game->players[i]->wins);
    }
    if (in_game(game)) {
        fprintf(fp, "%d %d %d\n", game->height, game->width, game->sequence_size);
        fprintf(fp, "%d ", game->num_special_sequences);
        for (int i = 0; i < game->num_special_sequences; i++) {
            fprintf(fp, "%d ", game->special_sequences[i]);
        }
        fprintf(fp, "\n");
        fprintf(fp, "%s ", game->player1->player->name);
        for (int i = 0; i < game->player1->num_special_sequences; i++) {
            fprintf(fp, "%d ", game->player1->special_sequences[i]);
        }
        fprintf(fp, "\n");
        fprintf(fp, "%s ", game->player2->player->name);
        for (int i = 0; i < game->player2->num_special_sequences; i++) {
            fprintf(fp, "%d ", game->player2->special_sequences[i]);
        }
        fprintf(fp, "\n");
        for (int l = 0; l < game->height; l++) {
            for (int c = 0; c < game->width; c++) {
                if (game->board[l][c] == NULL) {
                    fprintf(fp, "0 ");
                } else if (game->board[l][c] == game->player1) {
                    fprintf(fp, "1 ");
                } else {
                    fprintf(fp, "2 ");
                }
            }
            fprintf(fp, "\n");
        }
    }
    fclose(fp);
}

/**
 * @brief Loads the record of a player in the current game from a file.
 *
 * This function allocates memory for the player. The caller is responsible for
 * freeing the memory.
 *
 * @param game Pointer to a tGame structure.
 * @param fp Pointer to a file.
 * @return Pointer to a tInGamePlayer structure.
 */
pInGamePlayer load_in_game_player(pGame game, FILE* fp) {
    pInGamePlayer player = malloc(sizeof(tInGamePlayer));
    player->num_special_sequences = 0;
    player->special_sequences = NULL;
    char* line = NULL;
    size_t len = 0;
    getline(&line, &len, fp);
    char* player_name = strtok(line, " ");
    player->player = get_player(game, player_name);
    char* special_sequence = strtok(NULL, " ");
    while (special_sequence != NULL && strcmp(special_sequence, " ") != 0) {
        player->num_special_sequences++;
        player->special_sequences = realloc(player->special_sequences, player->num_special_sequences * sizeof(int));
        player->special_sequences[player->num_special_sequences - 1] = atoi(special_sequence);
        special_sequence = strtok(NULL, " ");
    }
    free(line);
    return player;
}

/**
 * @brief Loads a game from a file.
 *
 * @param filename The name of the file.
 * @return pGame Pointer to a tGame structure.
 */
pGame load_game(char* filename) {
    FILE* fp = fopen(filename, "r");
    pGame game = new_game();
    fscanf(fp, "%d\n", &game->num_players);
    game->players = malloc(game->num_players * sizeof(pPlayer));
    char* line = NULL;
    size_t len = 0;
    for (int i = 0; i < game->num_players; i++) {
        game->players[i] = malloc(sizeof(tPlayer));
        getline(&line, &len, fp);
        char* name = strtok(line, " ");
        game->players[i]->name = malloc((strlen(name) + 1) * sizeof(char));
        strcpy(game->players[i]->name, name);
        game->players[i]->games_played = atoi(strtok(NULL, " "));
        game->players[i]->wins = atoi(strtok(NULL, " "));
        free(line);
        line = NULL;
    }
    fscanf(fp, "%d %d %d\n", &game->height, &game->width, &game->sequence_size);
    if (game->height != 0) {
        char* row_contents = NULL;
        size_t row_len = 0;

        // Special sequences
        getline(&row_contents, &row_len, fp);
        row_contents[strlen(row_contents) - 1] = '\0';
        game->num_special_sequences = atoi(strtok(row_contents, " "));
        game->special_sequences = malloc(game->num_special_sequences * sizeof(int));
        char* special_sequence = strtok(NULL, " ");
        int idx = 0;
        while (special_sequence != NULL && strcmp(special_sequence, " ") != 0) {
            game->special_sequences[idx] = atoi(special_sequence);
            idx++;
            special_sequence = strtok(NULL, " ");
        }
        free(row_contents);
        row_contents = NULL;

        // Players of the current game
        game->player1 = load_in_game_player(game, fp);
        game->player2 = load_in_game_player(game, fp);

        // Game board
        game->board = malloc(game->height * sizeof(pInGamePlayer*));
        for (int l = 0; l < game->height; l++) {
            getline(&row_contents, &row_len, fp);
            row_contents[strlen(row_contents) - 1] = '\0';
            game->board[l] = malloc(game->width * sizeof(pInGamePlayer));
            int c = 0;
            char* player = strtok(row_contents, " ");
            while (player != NULL) {
                int player_id = atoi(player);
                if (player_id == 0) {
                    game->board[l][c] = NULL;
                } else if (player_id == 1) {
                    game->board[l][c] = game->player1;
                } else {
                    game->board[l][c] = game->player2;
                }
                player = strtok(NULL, " ");
                c++;
            }
            free(row_contents);
            row_contents = NULL;
        }
    }
    fclose(fp);
    return game;
}

/**
 * @brief Executes the program.
 *
 * @return int 0 if the program terminates successfully.
 */
int main() {
    pGame game = new_game();

    char* line = NULL;
    size_t len = 0;

    while (true) {
        getline(&line, &len, stdin);
        line[strlen(line) - 1] = '\0';
        if (strlen(line) == 0) {
            break;
        }
        char* command = strtok(line, " ");
        if (strcmp(command, "RJ") == 0) {
            char* name = strtok(NULL, " ");
            if (has_player(game, name)) {
                printf("Jogador existente.\n");
            } else {
                add_player(game, name);
                printf("Jogador registado com sucesso.\n");
            }
        } else if (strcmp(command, "EJ") == 0) {
            char* name = strtok(NULL, " ");
            if (!has_player(game, name)) {
                printf("Jogador não existente.\n");
            } else if (in_game(game) && player_in_game(game, name)) {
                printf("Jogador participa no jogo em curso.\n");
            } else {
                remove_player(game, name);
                printf("Jogador removido com sucesso.\n");
            }
        } else if (strcmp(command, "LJ") == 0) {
            if (!has_players(game)) {
                printf("Não existem jogadores registados.\n");
            } else {
                pPlayer* players = malloc(sizeof(pPlayer) * game->num_players);
                memcpy(players, game->players, sizeof(pPlayer) * game->num_players);
                qsort(players, game->num_players, sizeof(pPlayer), (int (*)(const void*, const void*))comp_players);
                for (int i = 0; i < game->num_players; i++) {
                    printf("%s %d %d\n", players[i]->name, players[i]->games_played, players[i]->wins);
                }
                free(players);
            }
        } else if (strcmp(command, "IJ") == 0) {
            char* player1_name = strtok(NULL, " ");
            char* player2_name = strtok(NULL, " ");
            char* line2 = NULL;
            size_t len2 = 0;
            getline(&line2, &len2, stdin);
            int width = atoi(strtok(line2, " "));
            int height = atoi(strtok(NULL, " "));
            int sequence_size = atoi(strtok(NULL, " "));
            char* line3 = NULL;
            size_t len3 = 0;
            getline(&line3, &len3, stdin);
            char* special_sequence = strtok(line3, " ");
            int* special_sequences = NULL;
            int count = 0;
            while (special_sequence != NULL) {
                special_sequences = realloc(special_sequences, sizeof(int) * (count + 1));
                special_sequences[count] = atoi(special_sequence);
                special_sequence = strtok(NULL, " ");
                count++;
            }
            if (in_game(game)) {
                printf("Existe um jogo em curso.\n");
            } else if (!has_player(game, player1_name) || !has_player(game, player2_name)) {
                printf("Jogador não registado.\n");
            } else if (!valid_dimensions(width, height)) {
                printf("Dimensões de grelha inválidas.\n");
            } else if (!valid_sequence(width, sequence_size)) {
                printf("Tamanho de sequência inválido.\n");
            } else if (!valid_special_sequences(sequence_size, special_sequences, count)) {
                printf("Dimensões de peças especiais inválidas.\n");
            } else {
                start_game(game, player1_name, player2_name, width, height, sequence_size, special_sequences, count);
                if (strcmp(player1_name, player2_name) < 0) {
                    printf("Jogo iniciado entre %s e %s.\n", player1_name, player2_name);
                } else {
                    printf("Jogo iniciado entre %s e %s.\n", player2_name, player1_name);
                }
            }
            free(line2);
            free(line3);
            free(special_sequences);
        } else if (strcmp(command, "D") == 0) {
            char* player1_name = strtok(NULL, " ");
            char* player2_name = strtok(NULL, " ");
            if (!in_game(game)) {
                printf("Não existe jogo em curso.\n");
            } else if (!player_in_game(game, player1_name) || (player2_name != NULL && !player_in_game(game, player2_name))) {
                printf("Jogador não participa no jogo em curso.\n");
            } else {
                game_over(game, player1_name, player2_name);
                printf("Desistência com sucesso. Jogo terminado.\n");
            }
        } else if (strcmp(command, "DJ") == 0) {
            if (!in_game(game)) {
                printf("Não existe jogo em curso.\n");
            } else {
                printf("%d %d\n", game->width, game->height);
                printf("%s\n", game->player1->player->name);
                print_player_special_sequences(game, game->player1);
                printf("%s\n", game->player2->player->name);
                print_player_special_sequences(game, game->player2);
            }
        } else if (strcmp(command, "CP") == 0) {
            char* name = strtok(NULL, " ");
            int size = atoi(strtok(NULL, " "));
            int column = atoi(strtok(NULL, " "));
            char* direction = strtok(NULL, " ");

            if (!in_game(game)) {
                printf("Não existe jogo em curso.\n");
            } else if (!player_in_game(game, name)) {
                printf("Jogador não participa no jogo em curso.\n");
            } else if (!valid_size(game, name, size)) {
                printf("Tamanho de peça não disponível.\n");
            } else if (!valid_position(game, size, column, direction)) {
                printf("Posição irregular.\n");
            } else {
                int* lines = malloc(sizeof(int) * size);
                int* columns = malloc(sizeof(int) * size);
                drop(game, name, size, column, direction, lines, columns);
                bool player_won = false;
                for (int i = 0; i < size; i++) {
                    if (player_wins(game, name, lines[i], columns[i])) {
                        game_over(game, name, NULL);
                        player_won = true;
                        printf("Sequência conseguida. Jogo terminado.\n");
                        break;
                    }
                }
                if (!player_won) {
                    printf("Peça colocada.\n");
                }
                free(lines);
                free(columns);
            }
        } else if (strcmp(command, "VR") == 0) {
            if (!in_game(game)) {
                printf("Não existe jogo em curso.\n");
            } else {
                for (int r = 0; r < game->height; r++) {
                    for (int c = 0; c < game->width; c++) {
                        printf("%d %d ", r + 1, c + 1);
                        if (game->board[r][c] == NULL) {
                            printf("Vazio\n");
                        } else {
                            printf("%s\n", game->board[r][c]->player->name);
                        }
                    }
                }
            }
        } else if (strcmp(command, "G") == 0) {
            save_game(game, "game.data");
            printf("Jogo gravado.\n");
        } else if (strcmp(command, "L") == 0) {
            free_game(game);
            game = load_game("game.data");
            printf("Jogo carregado.\n");
        } else if (strcmp(command, "X") == 0) {
            if (!in_game(game)) {
                printf("Não existe jogo em curso.\n");
            } else {
                for (int l = 0; l < game->height; l++) {
                    for (int c = 0; c < game->width; c++) {
                        if (game->board[l][c] != NULL) {
                            printf("%8s", game->board[l][c]->player->name);
                        } else {
                            printf("%8s", "----");
                        }
                    }
                    printf("\n");
                }
            }
        } else {
            printf("Instrução inválida.\n");
        }
        free(line);
        line = NULL;
    }
    if (line != NULL) {
        free(line);
    }
    free_game(game);
    return 0;
}
