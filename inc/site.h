#ifndef __SITE_H
#define __SITE_H
#include <algorithm>
#include <iostream>
#include <cmath>

using namespace std;

class Site 
{
      unsigned int I;
      unsigned int J;

    public:

      Site(unsigned int i, unsigned int j): I(i), J(j) {}
      unsigned int i() const { return I; }
      unsigned int j() const { return J; }

      int manhattanTo(const Site *w) const 
      {
         unsigned a = (I > w->i()) ? (I - w->i()) : (w->i() - I);
         unsigned b = (J > w->j()) ? (J - w->j()) : (w->j() - J);
         return (a + b) ; 
      }
      bool equals(const Site *w) const { return (manhattanTo(w) == 0); }

      friend ostream& operator<<(ostream& os, Site *site);
};

#endif
