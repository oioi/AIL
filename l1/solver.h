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
   unsigned long pathcost;

   std::array<node *, 4> childs;

   node(const state_array &state_, node *parent_, const action &res_action_ = {}, unsigned long depth_ = 0, unsigned long pathcost_ = 0) :
      state{}, parent{parent_}, res_action{res_action_}, depth{depth_}, pathcost{pathcost_}
   {
      state = state_;
      childs.fill(nullptr);
   }
};

enum class strategy_type
{
   width,
   depth
};

class strategy_queue
{
   public:
      virtual void insert(node *nd) = 0;
      virtual node * getnext() = 0;
      virtual bool empty() = 0;

      virtual ~strategy_queue() { }
};

class basic_strategy_queue : public strategy_queue
{
   public:
      bool empty() override { return node_queue.empty(); }

      node * getnext() override
      {
         node *nd = node_queue.front();
         node_queue.pop_front();
         return nd;
      }

   protected:
      std::deque<node *> node_queue;
};

class width_strategy_queue : public basic_strategy_queue
{
   public:
      void insert(node *nd) override { node_queue.push_back(nd); }
};

class depth_strategy_queue : public basic_strategy_queue
{
   public:
      void insert(node *nd) override { node_queue.push_front(nd); }
};

using solution = std::deque<action>;

struct solution_info
{
   unsigned expanded_nodes {};
   unsigned created_nodes  {};
   solution steps;
};

class solver
{
   public:
      solver(const state_array &init_state_, const state_array &goal_state_, strategy_type strat, unsigned long maxdepth_);
      solution_info solve();

      ~solver() {
         delete node_queue;

         // Every seen state stored in the hash-table. We're deleting nodes from
         // here, because if we're using DFS strategy, then tree can be long enough
         // to cause stack overflow when using recursive deletion in node's destructor.
         for (auto node : checked_states) delete node.second;
      }

   private:
      bool match_goal(node *exp) { return exp->state == goal_state; }
      uint64_t hash_state(const state_array &state);

      void expand_node(node *exp);
      node * create_node(node *parent, const action &res_action, unsigned long depth);

      unsigned long maxdepth;
      state_array init_state;
      state_array goal_state;

      unsigned expanded_nodes {0};
      unsigned created_nodes  {1}; // We always have init state.

      strategy_queue *node_queue;

      using statemap = std::unordered_map<uint64_t, node *>;
      static statemap checked_states;
      static const std::vector<std::vector<unsigned char>> possible_moves;
};

#endif // SOLVER_H
