#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include "solver.h"
#include "game_widget.h"

#include <QWidget>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QPushButton>

enum class state
{
   init,
   solving,
   unsolved,
   solved,
   replay
};

class main_widget : public QWidget
{
   Q_OBJECT

   public:
      main_widget(const std::initializer_list<elem> &init_state_,
                  const std::initializer_list<elem> &goal_state_,
                  unsigned image_side = 96, QWidget *parent = nullptr);

   private slots:
      void solve();
      void replay();
      void conditions_changed() { change_state(state::init); }

   private:
      game_widget *init_state;
      game_widget *goal_state;
      QPlainTextEdit *textout;

      QPushButton *solve_button;
      QPushButton *replay_button;
      QSpinBox *maxdepth_box;

      state laststate {state::init};
      solution last_solution;

      bool eventFilter(QObject *watched, QEvent *event) override;
      void change_state(state newstate);
};

#endif // MAIN_WIDGET_H
