#include "main_widget.h"
#include "solve_control.h"

#include <QVBoxLayout>

solve_control::solve_control(main_widget *parent) : QWidget(parent)
{
   QStringList lst;
   lst << "DFS" << "BFS";

   strategy_selection = new QComboBox {this};
   strategy_selection->addItems(lst);
   connect(strategy_selection, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
           parent, &main_widget::conditions_changed);
   connect(strategy_selection, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged),
           this, &solve_control::strategy_changed);

   solve_button = new QPushButton {"&Solve", this};
   connect(solve_button, &QPushButton::clicked, parent, &main_widget::solve);

   maxdepth_box = new QSpinBox {this};
   maxdepth_box->setRange(1, max_depth_val);
   maxdepth_box->setValue(max_depth_val);
   connect(maxdepth_box, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged),
           parent, &main_widget::conditions_changed);

   QVBoxLayout *layout = new QVBoxLayout {this};
   layout->addWidget(strategy_selection);
   layout->addWidget(solve_button);
   layout->addWidget(maxdepth_box);
}

void solve_control::strategy_changed()
{
   if (0 == strategy_selection->currentIndex()) maxdepth_box->setVisible(true);
   else maxdepth_box->setVisible(false);
}
