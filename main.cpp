#include <iostream>
#include <array>
#include <deque>

#include <unordered_map>
#include <vector>

enum class elem : char
{
   free = 0,
   one,
   two,
   three,
   four,
   five,
   six,
   seven,
   eight
};

struct action
{
   unsigned char from;
   unsigned char to;

   action(unsigned char from_ = 0, unsigned char to_ = 0) : from{from_}, to{to_} { }
   action(const action &other) : from{other.from}, to{other.to} { }
};

using state_array = std::array<elem, 9>;

struct node
{
   state_array state;
   node *parent;
   action paction;

   unsigned long depth;
   unsigned long pathcost;

   std::array<node *, 4> childs;
      
   node(const state_array &state_, node *parent_, const action &paction_ = {}, unsigned long depth_ = 0, unsigned long pathcost_ = 0) :
      state{}, parent{parent_}, paction{paction_}, depth{depth_}, pathcost{pathcost_}
   {
      state = state_;
      childs.fill(nullptr);
   }

   ~node() { for (auto ch : childs) delete ch; }     
};

std::deque<node *> node_queue;
using statemap = std::unordered_map<uint64_t, node *>;
statemap checked_states;

unsigned long nodecount {}, maxd {};
uint64_t hash;

bool match_goal(node *exp)
{
   static const state_array goal_state { {
      elem::one,  elem::two,   elem::three,
      elem::four, elem::free,  elem::five,
      elem::six,  elem::seven, elem::eight
   } };

   return exp->state == goal_state;
}

struct hashed_state
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

uint64_t hash_state(const state_array &state)
{
   static uint64_t ret;
   static hashed_state *hstate = reinterpret_cast<hashed_state *>(&ret);
   hstate->pad = 0;
   
   hstate->l0 = static_cast<unsigned>(state[0]);
   hstate->l1 = static_cast<unsigned>(state[1]);
   hstate->l2 = static_cast<unsigned>(state[2]);

   hstate->l3 = static_cast<unsigned>(state[3]);
   hstate->l4 = static_cast<unsigned>(state[4]);
   hstate->l5 = static_cast<unsigned>(state[5]);

   hstate->l6 = static_cast<unsigned>(state[6]);
   hstate->l7 = static_cast<unsigned>(state[7]);
   hstate->l8 = static_cast<unsigned>(state[8]);

   return ret;
}

node * create_node(const state_array &state, node *parent, const action &paction, unsigned long depth = 0, unsigned long pathcost = 0)
{
   state_array new_state = parent->state;
   std::swap(new_state[paction.from], new_state[paction.to]); 
   hash = hash_state(new_state);

   statemap::iterator it = checked_states.find(hash);
   if (checked_states.end() == it)
   {
      node *nptr = new node {new_state, parent, paction, depth, pathcost};
      checked_states[hash] = nptr;

      node_queue.push_back(nptr);
      nodecount++;
      return nptr;
   }

   if (pathcost >= it->second->pathcost) return nullptr;
 
   // This should not happen as we're going through the tree by width.
   std::cerr << "Loop occured for state: " << hash << std::endl 
             << "   Current pathcost: " << (unsigned int) pathcost << std::endl
             << "   Seen pathcost: " << (unsigned int) it->second->pathcost << std::endl;
   getchar();

   return nullptr;
}

void expand(node *exp, unsigned long maxdepth)
{
   static const std::vector<std::vector<unsigned char>> possible_moves {
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
   
   unsigned long depth = exp->depth + 1;
   if (depth > maxd) maxd = depth;
   if (depth > maxdepth) return;

   unsigned char target = 0;
   unsigned long pathcost = exp->pathcost + 1;

   for (auto e : exp->state) {
      if (e == elem::free) break; target++; }

   int i = 0;
   for (auto source : possible_moves[target])
      exp->childs[i++] = create_node(exp->state, exp, {source, target}, depth, pathcost);
}

int main(int argc, char *argv[])
{
   unsigned long maxdepth = 2048;
   
   state_array init_state { {
      elem::seven, elem::four, elem::two,
      elem::three, elem::five, elem::eight,
      elem::six,   elem::free, elem::one 
   } };

   /*
   state_array init_state { {
      elem::five,  elem::eight, elem::three,
      elem::four,  elem::free,  elem::two,
      elem::seven, elem::six,   elem::one 
   } }; */

   node init_node {init_state, nullptr};
   expand(&init_node, maxdepth);

   for (;;)
   {
      if (node_queue.empty()) {
         std::cerr << "No solution can be found." << std::endl; break; }

      node *exp = node_queue.front();
      node_queue.pop_front();

      if (match_goal(exp)) {
         std::cerr << "Solution found." << std::endl;
         std::cerr << "Steps: " << exp->depth << std::endl << std::endl;
         break;
      }

      expand(exp, maxdepth);
   }

   std::cerr << "Created nodes: " << nodecount << std::endl
             << "Maximum depth:" << maxd << std::endl;

   getchar();
   return 0;
}