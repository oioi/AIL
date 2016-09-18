#include "main_widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   main_widget wgt {
      { elem::seven, elem::four, elem::two,
        elem::three, elem::five, elem::eight,
        elem::six,   elem::free, elem::one },

      { elem::one,  elem::two,   elem::three,
        elem::four, elem::free,  elem::five,
        elem::six,  elem::seven, elem::eight }
   };

   wgt.show();
   return a.exec();
}
