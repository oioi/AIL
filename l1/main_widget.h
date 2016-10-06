#ifndef MAIN_WIDGET_H
#define MAIN_WIDGET_H

#include "solver.h"
#include "game_widget.h"
#include "solve_control.h"

#include <QWidget>
#include <QPlainTextEdit>
#include <QPushButton>

enum class state
{
   init,
   solving,
   unsolved,
   solved,
   replay
};

struct buttons_en_state
{
   bool strat_en;
   bool depth_en;
   bool solve_en;
   bool replay_en;
};

class main_widget : public QWidget
{
   Q_OBJECT

   public:
      main_widget(const std::initializer_list<elem> &init_state_,
                  const std::initializer_list<elem> &goal_state_,
                  unsigned image_side = 96, QWidget *parent = nullptr);

   public slots:
      void solve();
      void replay();
      void conditions_changed() { change_state(state::init); }

   private:
      bool eventFilter(QObject *watched, QEvent *event) override;
      void change_state(state newstate);

      game_widget *init_state;
      game_widget *goal_state;
      QPlainTextEdit *textout;

      solve_control *solve_cnt;
      QPushButton *replay_button;

      state laststate {state::init};
      solution_info last_solution;

      static std::vector<buttons_en_state> buttons_state;
};

#endif // MAIN_WIDGET_H
