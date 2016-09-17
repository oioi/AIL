#include <game_widget.h>
#include "solver.h"

#include <QtWidgets>

game_widget::game_widget(const std::initializer_list<elem> &list, unsigned image_side_, QWidget *parent) :
   QFrame(parent), image_side{image_side_}
{
   constexpr int boxsize = side * side;
   std::array<bool, boxsize> check;
   check.fill(false);

   if (boxsize != list.size())
      throw std::runtime_error {"Wrong list element count"};

   int i = 0;
   for (auto e : list)
   {
      if (true == check[static_cast<int>(e)])
         throw std::runtime_error {"Duplicated elements in the list"};
      check[static_cast<int>(e)] = true;
      state[i++] = e;
   }

   setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
   setAcceptDrops(true);

   layout = new QGridLayout(this);
   layout->setMargin(margin);
   layout->setSpacing(spacing);

   QString filename {"./images/number"};
   i = -1;

   for (auto e : state)
   {
      i++;
      if (0 == static_cast<int>(e)) continue;

      QLabel *icon {new QLabel(this)};
      QPixmap pixmap {filename + QString::number(static_cast<int>(e)) + ".png"};
      icon->setPixmap(pixmap.scaled(image_side, image_side, Qt::KeepAspectRatio, Qt::SmoothTransformation));
      icon->setAttribute(Qt::WA_DeleteOnClose);
      layout->addWidget(icon, i / side, i % side);
   }
}

void game_widget::dragEnterEvent(QDragEnterEvent *event)
{
   // Do drag'n'drop only within the same widget
   if (event->mimeData()->hasFormat("application/x-gicon-dnddata") && this == event->source())
   {
      event->setDropAction(Qt::MoveAction);
      event->accept();
   }
   else event->ignore();
}

void game_widget::dragMoveEvent(QDragMoveEvent *event)
{
   // DO drag'n'drop only within the same widget
   if (event->mimeData()->hasFormat("application/x-gicon-dnddata") && this == event->source())
   {
      event->setDropAction(Qt::MoveAction);
      event->accept();
   }
   else event->ignore();
}

QPoint game_widget::pos_to_index(const QPoint &source)
{
   QPoint target {source};

   target.rx() -= margin;
   target.ry() -= margin;
   if (0 > target.x()) target.rx() = 0;
   if (0 > target.y()) target.ry() = 0;

   // Built-in / operator for points rounds to the nearest. We don't want that.
   target.rx() /= image_side + spacing;
   target.ry() /= image_side + spacing;
   if (side - 1 < target.x()) target.rx() = side - 1;
   if (side - 1 < target.y()) target.ry() = side - 1;

   return target;
}

void game_widget::mousePressEvent(QMouseEvent *event)
{
   QLabel *child {static_cast<QLabel *>(childAt(event->pos()))};
   if (!child) return;

   QPixmap pixmap {*child->pixmap()};
   QByteArray itemdata;
   QDataStream datastream(&itemdata, QIODevice::WriteOnly);

   QPoint source {pos_to_index(event->pos())};
   datastream << pixmap << source;
   QMimeData *mimedata {new QMimeData};
   mimedata->setData("application/x-gicon-dnddata", itemdata);

   QDrag *drag {new QDrag(this)};
   drag->setMimeData(mimedata);
   drag->setPixmap(pixmap);
   drag->setHotSpot(event->pos() - child->pos());

   QPixmap temp_pixmap = pixmap;
   QPainter painter;
   painter.begin(&temp_pixmap);
   painter.fillRect(pixmap.rect(), QColor(127, 127, 127, 127));
   painter.end();

   child->setPixmap(temp_pixmap);
   if (Qt::MoveAction == drag->exec(Qt::CopyAction | Qt::MoveAction, Qt::CopyAction)) child->close();
   else { child->show(); child->setPixmap(pixmap); }
}

void game_widget::dropEvent(QDropEvent *event)
{
   if (!event->mimeData()->hasFormat("application/x-gicon-dnddata")) { event->ignore(); return; }

   QByteArray itemdata = event->mimeData()->data("application/x-gicon-dnddata");
   QDataStream datastream(&itemdata, QIODevice::ReadOnly);

   QPoint source;
   QPixmap pixmap;
   datastream >> pixmap >> source;

   QLabel *icon {new QLabel(this)};
   icon->setPixmap(pixmap);
   icon->setAttribute(Qt::WA_DeleteOnClose);

   QPoint target {pos_to_index(event->pos())};
   QLayoutItem *child {layout->itemAtPosition(target.y(), target.x())};
   if (nullptr != child)
   {
      // Nothing else can be here, right?
      QLabel *cu_icon {static_cast<QLabel *>(child->widget())};
      layout->removeWidget(cu_icon);
      layout->addWidget(cu_icon, source.y(), source.x());
   }

   layout->addWidget(icon, target.y(), target.x());
   std::swap(state[source.y() * side + source.x()], state[target.y() * side + target.x()]);
   if (source.y() * side + source.x() != target.y() * side + target.x()) emit state_changed();

   if(this == event->source())
   {
      event->setDropAction(Qt::MoveAction);
      event->accept();
   }
   else event->acceptProposedAction();
}
