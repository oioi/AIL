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

   connect(init_state, SIGNAL(state_changed()), SLOT(conditions_changed()));
   connect(goal_state, SIGNAL(state_changed()), SLOT(conditions_changed()));

   // Right controls - buttons and spinbox
   solve_button = new QPushButton {"&Solve", this};
   connect(solve_button, SIGNAL(clicked(bool)), SLOT(solve()));

   replay_button = new QPushButton {"&Replay", this};
   replay_button->setEnabled(false);
   connect(replay_button, SIGNAL(clicked(bool)), SLOT(replay()));

   maxdepth_box = new QSpinBox {this};
   maxdepth_box->setRange(1, 40);
   maxdepth_box->setValue(40);
   connect(maxdepth_box, SIGNAL(valueChanged(int)), SLOT(conditions_changed()));

   QVBoxLayout *controls_layout {new QVBoxLayout};
   controls_layout->setAlignment(Qt::AlignTop);
   controls_layout->setSpacing(10);

   controls_layout->addWidget(maxdepth_box);
   controls_layout->addWidget(solve_button);
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

   switch (laststate)
   {
      case state::init:
         maxdepth_box->setEnabled(true);
         solve_button->setEnabled(true);
         replay_button->setEnabled(false);
         break;

      case state::solved:
         maxdepth_box->setEnabled(true);
         solve_button->setEnabled(false);
         replay_button->setEnabled(true);
         break;

      case state::replay:
      case state::solving:
         maxdepth_box->setEnabled(false);
         solve_button->setEnabled(false);
         replay_button->setEnabled(false);
         break;

      case state::unsolved:
         maxdepth_box->setEnabled(true);
         solve_button->setEnabled(false);
         replay_button->setEnabled(false);
         break;
   }
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
   solver current {init_state->get_state(), goal_state->get_state(), (unsigned long) maxdepth_box->value()};
   last_solution = current.solve();

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
