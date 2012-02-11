#include "Layout.h"
#include <algorithm>

Layout::Layout(ISaWManManager* manager) :
  _manager(manager)
{
}

Layout::~Layout()
{
}

DirectResult
Layout::addWindow(SaWManWindowHandle window)
{
  _manager->Lock(_manager);
  _windows.push_back(window);
  _manager->InsertWindow(_manager, window, SAWMAN_WINDOW_NONE, SWMWR_TOP);
  relayout();
  _manager->Unlock(_manager);
  D_INFO("Layout::addWindow\n");
  return DR_OK;
}

DirectResult
Layout::removeWindow(SaWManWindowHandle window)
{
  _manager->Lock(_manager);
  WindowList::iterator it = std::find(_windows.begin(), _windows.end(), window);
  if (window == *it)
    _windows.erase(it);

  _manager->RemoveWindow(_manager, window);
  relayout();
  _manager->Unlock(_manager);
  D_INFO("Layout::removeWindow\n");
  return DR_OK;
}

void
Layout::relayout()
{
  _manager->Lock(_manager);
  _manager->QueueUpdate(_manager, DWSC_MIDDLE, NULL);
  _manager->Unlock(_manager);
}
