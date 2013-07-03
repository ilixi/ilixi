/*
 * Settings.h
 *
 *  Created on: Jun 18, 2013
 *      Author: tarik
 */

#ifndef ILXSIP_SETTINGS_H_
#define ILXSIP_SETTINGS_H_

#include <ui/Frame.h>
#include <ui/PushButton.h>
#include <ui/LineInput.h>
#include <ui/ComboBox.h>

namespace ilixi
{

class Settings : public ContainerBase
{
public:
    /*!
     * Constructor.
     */
    Settings(Widget* parent = 0);

    /*!
     * Destructor.
     */
    virtual
    ~Settings();

protected:
    /*!
     * Does nothing.
     */
    virtual void
    compose(const PaintEvent& event);

private:
    //! This widget stores the server where account resides.
    LineInput* _server;
    //! This widget stores the transport protocol.
    ComboBox* _transport;
    //! This widget stores user name.
    LineInput* _user;
    //! This widget stores password.
    LineInput* _password;

    //! Resets to default settings.
    PushButton* _defaults;
    //! Apply settings.
    PushButton* _apply;

    //! Initialises settings values.
    void
    init();

    //! Applies settings values.
    void
    apply();
};

} /* namespace ilixi */
#endif /* ILXSIP_SETTINGS_H_ */
