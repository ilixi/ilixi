/*
 * ImageDialog.h
 *
 *  Created on: 10 Nov 2011
 *      Author: tarik
 */

#ifndef IMAGEDIALOG_H_
#define IMAGEDIALOG_H_

#include "ui/Dialog.h"

class ImageDialog : public ilixi::Dialog
{
public:
  ImageDialog(const std::string& title, Widget* parent=0);

  virtual
  ~ImageDialog();

  void
  setImagePath(const std::string& path);

private:
  ilixi::Icon* _icon;
};

#endif /* IMAGEDIALOG_H_ */
