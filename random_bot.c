/*
 * random_bot.c - Example bot that makes random legal moves.
 *
 * Usage (matchmade):
 *   export BOT1_ID=your-bot-id
 *   export BOT1_KEY=your-bot-api-key
 *   ./random_bot
 *
 * Usage (continuous matchmade):
 *   export BOT1_ID=your-bot-id
 *   export BOT1_KEY=your-bot-api-key
 *   ./random_bot --continuous
 *
 * Usage (practice):
 *   export ROOM_ID=your-room-id
 *   export BOT1_ID=your-bot-id
 *   export BOT1_KEY=your-bot-api-key
 *   ./random_bot --practice
 */

#include <arena/arena.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Callbacks ────────────────────────────────────────────────── */

static void on_ready(void *ud)
{
    printf("[bot %.8s] Connected and ready.\n", (const char *)ud);
}

static void on_queue_entry(void *ud)
{
    printf("[bot %.8s] Joined queue, waiting for match...\n", (const char *)ud);
}

static void on_match_found(const char *game_id, void *ud)
{
    printf("[bot %.8s] Match found: %.8s...\n", (const char *)ud, game_id);
}

static void on_room_joined(const char *room_id, void *ud)
{
    printf("[bot %.8s] Joined room: %.8s...\n", (const char *)ud, room_id);
}

static void on_game_start(const arena_game_state_t *state, void *ud)
{
    printf("[bot %.8s] Game started. I am %s. %s moves first.\n",
        (const char *)ud,
        arena_side_str(state->my_side),
        arena_side_str(state->current_turn));
}

static void on_move(const arena_game_state_t *state, arena_move_t *move, void *ud)
{
    /* Pick a random piece */
    size_t pi = (size_t)rand() % state->legal_moves_count;
    const arena_piece_moves_t *piece = &state->legal_moves[pi];

    /* Pick a random destination */
    size_t mi = (size_t)rand() % piece->valid_moves_count;

    move->from_pos = piece->pos;
    move->to_pos   = piece->valid_moves[mi];

    /* FlipFour: set side for HAND placements */
    if (strcmp(piece->pos, "HAND") == 0)
        move->side = piece->name;
    else
        move->side = NULL;

    /* Amoeba: the same pos can appear as two legal-move entries (one per
     * splitting value), each with its own destinations. Echo the selected
     * entry's flag so the move matches the candidate list the server searches. */
    move->splitting = piece->has_splitting ? piece->splitting : false;

    printf("[bot %.8s] Move: %s -> %s\n", (const char *)ud, move->from_pos, move->to_pos);
}

static void on_game_end(const arena_game_end_t *state, void *ud)
{
    if (state->has_winner) {
        if (state->winner == state->my_side)
            printf("[bot %.8s] I won!\n", (const char *)ud);
        else
            printf("[bot %.8s] I lost.\n", (const char *)ud);
    } else {
        printf("[bot %.8s] Draw.\n", (const char *)ud);
    }
}

static void on_disconnect(const char *reason, void *ud)
{
    printf("[bot %.8s] Disconnected: %s\n", (const char *)ud, reason);
}

/* ── Main ─────────────────────────────────────────────────────── */

int main(int argc, char **argv)
{
    srand((unsigned)time(NULL));

    const char *bot_id  = getenv("BOT1_ID");
    const char *api_key = getenv("BOT1_KEY");
    if (!bot_id || !api_key) {
        fprintf(stderr, "Error: set BOT1_ID and BOT1_KEY environment variables.\n");
        return 1;
    }

    arena_bot_config_t cfg = {
        .bot_id   = bot_id,
        .api_key  = api_key,
        .callbacks = {
            .on_move       = on_move,
            .on_ready      = on_ready,
            .on_queue_entry = on_queue_entry,
            .on_match_found = on_match_found,
            .on_room_joined = on_room_joined,
            .on_game_start = on_game_start,
            .on_game_end   = on_game_end,
            .on_disconnect = on_disconnect,
        },
        .user_data = (void *)bot_id,
    };

    /* Parse game type from args (default: flipflop_3x3) */
    arena_game_type_t game_type = ARENA_GAME_FLIPFLOP_3X3;
    int practice_mode = 0;
    int continuous_mode = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--practice") == 0) {
            practice_mode = 1;
        } else if (strcmp(argv[i], "--continuous") == 0) {
            continuous_mode = 1;
        } else if (strcmp(argv[i], "--game") == 0 && i + 1 < argc) {
            i++;
            if (strcmp(argv[i], "flipflop_3x3") == 0)
                game_type = ARENA_GAME_FLIPFLOP_3X3;
            else if (strcmp(argv[i], "flipflop_5x5") == 0)
                game_type = ARENA_GAME_FLIPFLOP_5X5;
            else if (strcmp(argv[i], "flipfour") == 0)
                game_type = ARENA_GAME_FLIPFOUR;
            else if (strcmp(argv[i], "amoeba") == 0)
                game_type = ARENA_GAME_AMOEBA;
            else {
                fprintf(stderr, "Unknown game type: %s\n", argv[i]);
                return 1;
            }
        }
    }

    if (practice_mode && continuous_mode) {
        fprintf(stderr, "Error: --continuous cannot be used with --practice.\n");
        return 1;
    }

    printf("[bot %.8s] Starting %s mode, game: %s\n",
           bot_id,
           practice_mode ? "practice" :
               (continuous_mode ? "continuous matchmade" : "matchmade"),
           arena_game_type_str(game_type));

    int result;
    if (practice_mode) {
        result = arena_start_practice(&cfg, NULL);
    } else if (continuous_mode) {
        result = arena_start_continuous(&cfg, game_type);
    } else {
        result = arena_start(&cfg, game_type);
    }

    if (result != 0)
        fprintf(stderr, "[bot %.8s] Exited with error code %d\n", bot_id, result);

    return result;
}
