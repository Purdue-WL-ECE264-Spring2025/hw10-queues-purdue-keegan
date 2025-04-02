#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include "queue.h"
#include "tile_game.h"

void enqueue(struct queue *q, struct game_state state) {
  uint64_t serial = serialize(state);
  insert_at_tail(&q->data, serial);
}

struct game_state dequeue(struct queue *q) {
  uint64_t serial = remove_from_head(&q->data);
  return deserialize(serial);
}

static bool is_goal(struct game_state state) {
  uint8_t solved[4][4] = {
      {1, 2, 3, 4},
      {5, 6, 7, 8},
      {9, 10, 11, 12},
      {13, 14, 15, 0}
  };
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      if (state.tiles[i][j] != solved[i][j]) {
	return false;
      }
    }
  }
  return true;
}

int number_of_moves(struct game_state start) {
  if (is_goal(start)) {
    return start.num_steps;
  }
  struct queue q;
  q.data.head = NULL;
  size_t visited_capacity = 1024;
  size_t visited_size = 0;
  uint64_t *visited = malloc(visited_capacity * sizeof(uint64_t));
  if (!visited) {
    exit(1);
  }
  uint64_t start_serial = serialize(start);
  visited[visited_size++] = start_serial;
  enqueue(&q, start);
  while (q.data.head != NULL) {
    struct game_state current = dequeue(&q);
    if (is_goal(current)) {
      free(visited);
      free_list(q.data);
      return current.num_steps;
    }

    struct game_state candidates[4];
    int num_candidates = 0;
    candidates[num_candidates] = current;
    move_up(&candidates[num_candidates]);
    if (candidates[num_candidates].num_steps != current.num_steps) {
      num_candidates++;
    }
    candidates[num_candidates] = current;
    move_down(&candidates[num_candidates]);
    if (candidates[num_candidates].num_steps != current.num_steps) {
      num_candidates++;
    }
    candidates[num_candidates] = current;
    move_left(&candidates[num_candidates]);
    if (candidates[num_candidates].num_steps != current.num_steps) {
      num_candidates++;
    }
    candidates[num_candidates] = current;
    move_right(&candidates[num_candidates]);
    if (candidates[num_candidates].num_steps != current.num_steps) {
      num_candidates++;
    }

    for (int i = 0; i < num_candidates; i++) {
      uint64_t serial = serialize(candidates[i]);
      bool already_visited = false;
      for (size_t j = 0; j < visited_size; j++) {
	if (visited[j] == serial) {
	  already_visited = true;
	  break;
	}
      }
      if (!already_visited) {
	if (visited_size == visited_capacity) {
	  visited_capacity *= 2;
	  visited = realloc(visited, visited_capacity * sizeof(uint64_t));
	  if (!visited) {
	    exit(1);
	  }
	}
	visited[visited_size++] = serial;
	enqueue(&q, candidates[i]);
      }
    }
  }
  free(visited);
  free_list(q.data);
  return -1;
}




























