#include "main_widget.h"
#include "solver.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>

#include <QTime>

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

   connect(init_state, SIGNAL(state_changed()), SLOT(game_changed()));
   connect(goal_state, SIGNAL(state_changed()), SLOT(game_changed()));

   QVBoxLayout *buttons_layout {new QVBoxLayout};
   solve_button = new QPushButton{"&Solve"};

   buttons_layout->addWidget(solve_button);
   connect(solve_button, SIGNAL(clicked(bool)), SLOT(solve()));

   gw_layout->addLayout(buttons_layout);
   QVBoxLayout *main_layout {new QVBoxLayout};

   textout = new QPlainTextEdit;
   textout->setReadOnly(true);

   main_layout->addLayout(gw_layout);
   main_layout->addWidget(textout);
   setLayout(main_layout);
}

bool main_widget::eventFilter(QObject *, QEvent *event)
{
   if (event->type() == QEvent::MouseButtonPress) {
      if (laststate == state::solving || laststate == state::replay) return true; }
   return false;
}

void main_widget::change_state(state newstate)
{
   switch (newstate)
   {
      case state::init:
         solve_button->setEnabled(true); break;

      case state::replay:
      case state::solving:
      case state::solved:
         solve_button->setEnabled(false); break;
   }
}

void main_widget::solve()
{
   change_state(state::solving);
   solver current {init_state->get_state(), goal_state->get_state(), 2048};
   last_solution = current.solve();

   change_state(state::solved);
   QTime time {QTime::currentTime()};

   if (last_solution.empty()) textout->appendPlainText(time.toString("hh:mm:ss") + "\tNo solution can be found.");
   else textout->appendPlainText(time.toString("hh:mm:ss") +  "\tSolution found in " +
                                 QString::number(last_solution.size()) + " steps.");
}
