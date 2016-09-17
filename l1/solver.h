#ifndef SOLVER_H
#define SOLVER_H

#include "game.h"

#include <deque>
#include <vector>
#include <unordered_map>

struct compressed_state
{
   unsigned int l0: 4;
   unsigned int l1: 4;
   unsigned int l2: 4;

   unsigned int l3: 4;
   unsigned int l4: 4;
   unsigned int l5: 4;

   unsigned int l6: 4;
   unsigned int l7: 4;
   unsigned int l8: 4;
   unsigned int pad: 28;
};

struct action
{
   unsigned char from;
   unsigned char to;

   action(unsigned char from_ = 0, unsigned char to_ = 0) : from{from_}, to{to_} { }
   action(const action &other) : from{other.from}, to{other.to} { }
};

struct node
{
   state_array state;
   action res_action;
   node *parent;

   unsigned long depth;
   std::array<node *, 4> childs;

   node(const state_array &state_, node *parent_, const action &res_action_ = {}, unsigned long depth_ = 0) :
      state{}, parent{parent_}, res_action{res_action_}, depth{depth_}
   {
      state = state_;
      childs.fill(nullptr);
   }

   ~node() { for (auto ch : childs) delete ch; }
};

using solution = std::deque<action>;

class solver
{
   public:
      solver(const state_array &init_state_, const state_array &goal_state_, unsigned long maxdepth_) :
         maxdepth{maxdepth_}
      {
         init_state = init_state_;
         goal_state = goal_state_;
      }

      solution solve();

   private:
      bool match_goal(node *exp) { return exp->state == goal_state; }
      uint64_t hash_state(const state_array &state);

      void expand_node(node *exp);
      node * create_node(node *parent, const action &res_action, unsigned long depth = 0);

      unsigned long maxdepth;
      state_array init_state;
      state_array goal_state;

      using statemap = std::unordered_map<uint64_t, node *>;
      std::deque<node *> node_queue;
      statemap checked_states;

      static const std::vector<std::vector<unsigned char>> possible_moves;
};

#endif // SOLVER_H
