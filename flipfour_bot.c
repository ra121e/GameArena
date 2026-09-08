/*
 * flipfour_bot.c - Example bot configured for flipfour.
 */

#include <arena/arena.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void on_move(const arena_game_state_t *state, arena_move_t *move, void *ud)
{
    (void)ud;
    if (state->legal_moves_count == 0) {
        move->from_pos = "";
        move->to_pos = "";
        move->side = NULL;
        move->splitting = false;
        return;
    }

    const arena_piece_moves_t *piece = &state->legal_moves[0];
    if (piece->valid_moves_count == 0) {
        move->from_pos = piece->pos;
        move->to_pos = "";
        move->side = NULL;
        move->splitting = false;
        return;
    }

    move->from_pos = piece->pos;
    move->to_pos = piece->valid_moves[0];
    if (strcmp(piece->pos, "HAND") == 0)
        move->side = piece->name;
    else
        move->side = NULL;
    move->splitting = piece->has_splitting ? piece->splitting : false;
}

int main(void)
{
    const char *bot_id = getenv("BOT3_ID");
    const char *api_key = getenv("BOT3_KEY");
    if (!bot_id || !api_key) {
        fprintf(stderr, "Error: set BOT3_ID and BOT3_KEY environment variables.\n");
        return 1;
    }

    arena_bot_config_t cfg = {
        .bot_id = bot_id,
        .api_key = api_key,
        .callbacks = { .on_move = on_move },
        .user_data = (void *)bot_id,
    };

    return arena_start(&cfg, ARENA_GAME_FLIPFOUR);
}
