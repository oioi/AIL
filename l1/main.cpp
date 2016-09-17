#include "main_widget.h"
#include <QApplication>

int main(int argc, char *argv[])
{
   QApplication a(argc, argv);

   main_widget wgt {
      { elem::two,   elem::three, elem::one,
        elem::four,  elem::five,  elem::six,
        elem::seven, elem::free,  elem::eight },

      { elem::one,   elem::two,   elem::three,
        elem::four,  elem::five,  elem::six,
        elem::seven, elem::eight, elem::free }
   };

   wgt.show();
   return a.exec();
}
