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

enum { side = 3 };

using state_array = std::array<elem, side * side>;

#endif // GAME_H
