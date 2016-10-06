#include "main_widget.h"
#include "solver.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include <QTime>

std::vector<buttons_en_state> main_widget::buttons_state {
   { true,  true,  true,  false }, // init
   { false, false, false, false }, // solving
   { true,  true,  false, false }, // unsolved
   { true,  true,  false, true  }, // solved
   { false, false, false, false }  // replay
};

main_widget::main_widget(const std::initializer_list<elem> &init_state_, const std::initializer_list<elem> &goal_state_,
                         unsigned image_side, QWidget *parent) :
   QWidget(parent)
{
   QHBoxLayout *gw_layout {new QHBoxLayout};
   init_state = new game_widget{init_state_, image_side, this};
   goal_state = new game_widget{goal_state_, image_side, this};

   init_state->installEventFilter(this);
   goal_state->installEventFilter(this);

   gw_layout->addWidget(init_state);
   gw_layout->addWidget(goal_state);

   connect(init_state, &game_widget::state_changed, this, &main_widget::conditions_changed);
   connect(goal_state, &game_widget::state_changed, this, &main_widget::conditions_changed);

   // Right controls - buttons and spinbox
   solve_cnt = new solve_control {this};

   replay_button = new QPushButton {"&Replay", this};
   replay_button->setEnabled(false);
   connect(replay_button, &QPushButton::clicked, this, &main_widget::replay);

   QVBoxLayout *controls_layout {new QVBoxLayout};
   controls_layout->setAlignment(Qt::AlignTop);
   controls_layout->setSpacing(10);

   controls_layout->addWidget(solve_cnt);
   controls_layout->addWidget(replay_button);
   gw_layout->addLayout(controls_layout);

   // Text area
   textout = new QPlainTextEdit {this};
   textout->setReadOnly(true);

   // Main layout
   QVBoxLayout *main_layout {new QVBoxLayout(this)};
   main_layout->addLayout(gw_layout);
   main_layout->addWidget(textout);
}

bool main_widget::eventFilter(QObject *, QEvent *event)
{
   if (QEvent::MouseButtonPress == event->type()) {
      if (laststate == state::solving || laststate == state::replay) return true; }
   return false;
}

void main_widget::change_state(state newstate)
{
   laststate = newstate;
   buttons_en_state &bs = buttons_state[static_cast<int>(laststate)];

   solve_cnt->set_select_enabled(bs.strat_en);
   solve_cnt->set_depth_enabled(bs.depth_en);
   solve_cnt->set_solve_enabled(bs.solve_en);
   replay_button->setEnabled(bs.replay_en);
}

void main_widget::replay()
{
   change_state(state::replay);
   init_state->replay(last_solution.steps, 500);
   change_state(state::solved);
}

void main_widget::solve()
{
   change_state(state::solving);

   try {
      solver current {init_state->get_state(), goal_state->get_state(),
                      static_cast<strategy_type>(solve_cnt->get_strategy()), solve_cnt->get_depth()};
      last_solution = current.solve();
   }
   catch (std::exception &exc)
   {
      textout->appendPlainText("Exception thrown: " + QString(exc.what()));
      change_state(state::unsolved);
      return;
   }

   QTime time {QTime::currentTime()};
   QString timestr {time.toString()};

   if (last_solution.steps.empty())
   {
      textout->appendPlainText(timestr + "\tNo solution can be found.");
      change_state(state::unsolved);
   }

   else
   {
      textout->appendPlainText(timestr +  "\tSolution found with " + QString::number(last_solution.steps.size()) + " steps.");
      change_state(state::solved);
   }

   textout->appendPlainText("\tExpanded nodes: " + QString::number(last_solution.expanded_nodes) +
                            "\n\tCreated nodes:    " + QString::number(last_solution.created_nodes) + '\n');
}
