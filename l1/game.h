#ifndef GAME_H
#define GAME_H

#include <array>

enum class elem : unsigned char
{
   one = 0,
   two,
   three,
   four,
   five,
   six,
   seven,
   eight,
   free
};

enum {
   side = 3,
   max_combinations = 181440
};

using state_array = std::array<elem, side * side>;

#endif // GAME_H
