#ifndef GAME_WIDGET_H
#define GAME_WIDGET_H

#include "game.h"

#include <QFrame>
#include <QGridLayout>

class game_widget : public QFrame
{
   Q_OBJECT

   public:
      game_widget(const std::initializer_list<elem> &list, unsigned image_side_ = 96, QWidget *parent = nullptr);
      state_array get_state() const { return state; }

   protected:
      void dragEnterEvent(QDragEnterEvent *event) override;
      void dragMoveEvent(QDragMoveEvent *event) override;
      void dropEvent(QDropEvent *event) override;
      void mousePressEvent(QMouseEvent *event) override;

   signals:
      void state_changed();

   private:
      unsigned image_side;
      state_array state;

      const unsigned margin {10};
      const unsigned spacing {5};

      QGridLayout *layout;

      QPoint pos_to_index(const QPoint &source);
};

#endif // GAME_WIDGET_H
