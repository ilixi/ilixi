/*
 Copyright 2010, 2011 Tarik Sekmen.

 All Rights Reserved.

 Written by Tarik Sekmen <tarik@ilixi.org>.

 This file is part of ilixi.

 ilixi is free software: you can redistribute it and/or modify
 it under the terms of the GNU Lesser General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 ilixi is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public License
 along with ilixi.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "StatusBar.h"
#include "ui/HBoxLayout.h"
#include "ui/VBoxLayout.h"
#include "graphics/Painter.h"
#include "core/Logger.h"
#include <sigc++/sigc++.h>
#include <string.h>

using namespace IMaestro;

const char* days[7] =
  { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };

const char* months[12] =
  { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
      "Nov", "Dec" };

DateThread::DateThread(StatusBar* parent) :
  Thread(), _parent(parent)
{
}

DateThread::~DateThread()
{
}

int
DateThread::run()
{
  while (1)
    {
      pthread_testcancel();
      _parent->updateTime();
      sleep(1);
    }
  return 0;
}

//*********************************************************

StatusBar::StatusBar(int argc, char* argv[]) :
  Application(argc, argv), _dateThread(NULL)
{
  setTitle("StatusBar");
  setBackgroundFilled(true);
  setMargins(5, 5, 10, 5);
  HBoxLayout* mainLayout = new HBoxLayout();
  mainLayout->setVerticalAlignment(Alignment::Middle);
  setLayout(mainLayout);

  _home = new ToolButton("Home");
  _home->setInputMethod(PointerInput);
  _home->setToolButtonStyle(ToolButton::IconOnly);
  _home->setIcon(ILIXI_DATADIR"statusbar/home.png", Size(32, 32));
  addWidget(_home);
  _home->sigClicked.connect(sigc::mem_fun(this, &StatusBar::showHome));

  _app = new Label("dfb_ilixi");
  _app->setLayoutAlignment(TextLayout::Left);
  _app->setXConstraint(ExpandingConstraint);
  _app->setFont(new Font(ILIXI_DATADIR"fonts/decker.ttf", 36));
  addWidget(_app);

  _time = new Label("00:00:00");
  _date = new Label("Fri 18 Nove");

  VBoxLayout* dateBox = new VBoxLayout();
  dateBox->addWidget(_time);
  dateBox->addWidget(_date);
  addWidget(dateBox);

  _dateThread = new DateThread(this);

  _shutdown = new ToolButton("Shutdown");
  _shutdown->setToolButtonStyle(ToolButton::IconOnly);
  _shutdown->setInputMethod(PointerInput);
  _shutdown->setIcon(ILIXI_DATADIR"statusbar/shutdown.png", Size(32, 32));
  _shutdown->setGeometry(500, 10, 100, 100);
  addWidget(_shutdown);

  _sDialog = new SDialog("Choose an action");

  _shutdown->sigClicked.connect(sigc::mem_fun(_sDialog, &SDialog::execute));
  _sDialog->sigFinished.connect(sigc::mem_fun(this, &StatusBar::handleDialog));

  sigVisible.connect(sigc::mem_fun(this, &StatusBar::onShow));
  sigHidden.connect(sigc::mem_fun(this, &StatusBar::onHide));
}

StatusBar::~StatusBar()
{
  delete _dateThread;
}

void
StatusBar::onHide()
{
  _dateThread->cancel();
}

void
StatusBar::onShow()
{
  _dateThread->start();
}

void
StatusBar::updateTime()
{
  struct timeval tv;
  struct tm* tm;
  gettimeofday(&tv, NULL);
  tm = localtime(&tv.tv_sec);

  char time[9];
  sprintf(time, "%02d:%02d:%02d", tm->tm_hour, tm->tm_min, tm->tm_sec);
  _time->setText(time);

  char date[12];
  sprintf(date, "%s %d %s ", days[tm->tm_wday], tm->tm_mday, months[tm->tm_mon]);
  _date->setText(date);
}

ReactionResult
StatusBar::maestroCB(MaestroMessage *msg, void *ctx)
{
  if (msg->senderID != 1)
    {
      ILOG_ERROR("Sender is not authorised!");
      return RS_REMOVE;
    }

  switch (msg->type)
    {
  case SwitchMode:
    if (msg->state == Visible)
      {
        ILOG_INFO("Received SwitchMode::Visible from Maestro.");
        show();
        return RS_OK;
      }
    else if (msg->state == Hidden)
      {
        ILOG_INFO("Received SwitchMode::Hidden from Maestro.");
        hide();
        return RS_OK;
      }
    else if (msg->state == Quit)
      {
        ILOG_INFO("Received SwitchMode::Quit from Maestro.");
        quit();
        return RS_OK;
      }
    else
      ILOG_ERROR("Message mode is not supported!");
    break;

  case Notification:

    if (msg->state == Visible)
      {
        AppRecord* app = getAppRecord(msg->appIndex);
        _currentAppID = msg->appIndex;
        _app->setText((std::string) app->title);

        if (_currentAppID == 3) // Notification comes from Home app.

          {
            _home->setDisabled();
            _sDialog->setKillVisibility(false);
          }
        else
          {
            _home->setEnabled();
            _sDialog->setKillVisibility(true);
            _sDialog->setAppName((std::string) app->title);
          }
      }
    break;

  default:
    ILOG_ERROR("Message type is not supported!");
    }
  return RS_DROP;
}

void
StatusBar::compose()
{
  Painter painter(this);
  painter.begin();
  painter.setBrush(Color(0, 0, 0, 127));
  painter.fillRectangle(0, 0, width(), height());
  painter.setPen(Color(0, 0, 0));
  painter.drawRectangle(0, 0, width(), height());
  painter.end();
}

void
StatusBar::handleDialog(int result)
{
  if (result == 0)
    return;
  else if (result == 2)
    {
      _sDialog->setKillVisibility(false);
      callMaestro(SwitchMode, Quit, _currentAppID);
      return;
    }
  else if (result == 5)
    quit();
}

void
StatusBar::showHome()
{
  callMaestro(SwitchMode, Visible, 4);
}

int
main(int argc, char* argv[])
{
  StatusBar app(argc, argv);
  app.exec();
  return 0;
}
