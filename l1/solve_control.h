#ifndef SOLVE_CONTROL_H
#define SOLVE_CONTROL_H

#include <QComboBox>
#include <QPushButton>
#include <QSpinBox>

enum { max_depth_val = 70000 };

class main_widget;

class solve_control : public QWidget
{
   Q_OBJECT

   public:
      solve_control(main_widget *parent);

      int get_depth() const { return maxdepth_box->value(); }
      int get_strategy() const { return strategy_selection->currentIndex(); }

      void set_solve_enabled(bool enabled)  { solve_button->setEnabled(enabled); }
      void set_depth_enabled(bool enabled)  { maxdepth_box->setEnabled(enabled); }
      void set_select_enabled(bool enabled) { strategy_selection->setEnabled(enabled); }

   private slots:
      void strategy_changed();

   private:
      QComboBox *strategy_selection;
      QPushButton *solve_button;
      QSpinBox *maxdepth_box;
};

#endif // SOLVE_CONTROL_H
