#ifndef LAYOUT_H_
#define LAYOUT_H_

#include <list>
#include <sawman.h>

class Layout
{
public:
  Layout(ISaWManManager* manager);

  virtual
  ~Layout();

  virtual DirectResult
  addWindow(SaWManWindowHandle window);

  virtual DirectResult
  removeWindow(SaWManWindowHandle window);

  virtual void
  relayout();

private:
  ISaWManManager* _manager;
  typedef std::list<SaWManWindowHandle> WindowList;
  WindowList _windows;
};

#endif /* LAYOUT_H_ */
