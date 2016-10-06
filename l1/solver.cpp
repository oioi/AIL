#include "solver.h"

solver::statemap solver::checked_states;
const std::vector<std::vector<unsigned char>> solver::possible_moves {
   {1, 3},
   {0, 2, 4},
   {1, 5},
   {0, 4, 6},
   {1, 3, 5, 7},
   {2, 4, 8},
   {3, 7},
   {4, 6, 8},
   {5, 7}
};

solver::solver(const state_array &init_state_, const state_array &goal_state_, strategy_type strategy_, int maxdepth_)
   : maxdepth{maxdepth_}, strategy{strategy_}
{
   init_state = init_state_;
   goal_state = goal_state_;

   checked_states.clear();
   checked_states.reserve(max_combinations);

   switch(strategy)
   {
      case strategy_type::depth: node_queue = new depth_strategy_queue; break;
      case strategy_type::width: node_queue = new width_strategy_queue; break;

      case strategy_type::astar_h1:
      case strategy_type::astar_h2:
         node_queue = new priority_queue;
         break;
   }
}

uint64_t solver::hash_state(const state_array &state)
{
   uint64_t ret;
   compressed_state *cstate = reinterpret_cast<compressed_state *>(&ret);
   cstate->pad = 0;

   cstate->l0 = static_cast<unsigned>(state[0]);
   cstate->l1 = static_cast<unsigned>(state[1]);
   cstate->l2 = static_cast<unsigned>(state[2]);

   cstate->l3 = static_cast<unsigned>(state[3]);
   cstate->l4 = static_cast<unsigned>(state[4]);
   cstate->l5 = static_cast<unsigned>(state[5]);

   cstate->l6 = static_cast<unsigned>(state[6]);
   cstate->l7 = static_cast<unsigned>(state[7]);
   cstate->l8 = static_cast<unsigned>(state[8]);

   return ret;
}

int solver::h1cost(const state_array &state)
{
   int cost {};
   for (int i = 0; i < side * side; i++) {
      if (state[i] != goal_state[i]) cost++;
   }

   return cost;
}

int solver::h2cost(const state_array &state)
{
   int cost {};
   for (int d,j, i = 0; i < side * side; i++)
   {
      for (j = 0; j < side * side; j++) {
         if (state[i] == goal_state[j]) break;
      }

      d = i > j ? i - j : j - i;
      cost += d / side + d % side;
   }

   return cost;
}

node * solver::create_node(node *parent, const action &res_action, int depth)
{
   state_array new_state = parent->state;
   std::swap(new_state[res_action.from], new_state[res_action.to]);
   uint64_t hash = hash_state(new_state);

   statemap::iterator it = checked_states.find(hash); 

   // NOTE: i am not sure that this is right behaviour in all cases.
   // for BFS it's right, because if we've already seen some state
   // we can find it again only at lower level of tree.
   if (checked_states.end() != it) return nullptr;

   int hcost {};
   if (strategy_type::astar_h1 == strategy) hcost = depth + h1cost(new_state);
   if (strategy_type::astar_h2 == strategy) hcost = depth + h2cost(new_state);

   node *nptr {new node {new_state, parent, res_action, depth, hcost}};
   checked_states[hash] = nptr;
   node_queue->insert(nptr);

   created_nodes++;
   return nptr;
}

void solver::expand_node(node *exp)
{
   expanded_nodes++;
   int depth = exp->depth + 1;
   if (0 != maxdepth && depth > maxdepth) return;

   unsigned char target {};
   for (auto e : exp->state) {
      if (e == elem::free) break; target++; }

   unsigned i {};
   for (auto source : possible_moves[target])
      exp->childs[i++] = create_node(exp, {source, target}, depth);
}

solution_info solver::solve()
{
   solution_info ret;
   node *init_node {new node {init_state, nullptr}};

   checked_states[hash_state(init_node->state)] = init_node;
   expand_node(init_node);

   for (;;)
   {
      if (node_queue->empty()) break;
      node *exp = node_queue->getnext();

      if (match_goal(exp))
      {
         for (; nullptr != exp->parent; exp = exp->parent)
            ret.steps.push_front(exp->res_action);
         break;
      }

      expand_node(exp);
   }

   ret.expanded_nodes = expanded_nodes;
   ret.created_nodes = created_nodes;
   return ret;
}
