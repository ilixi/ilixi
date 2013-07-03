/*
 * Dialer.h
 *
 *  Created on: Jun 18, 2013
 *      Author: tarik
 */

#ifndef ILXSIP_DIALER_H_
#define ILXSIP_DIALER_H_

#include <ui/Frame.h>
#include <ui/LineInput.h>
#include <ui/PushButton.h>

namespace ilixi
{

class Dialer : public Frame
{
public:
    Dialer(Widget* parent = 0);

    virtual
    ~Dialer();

    bool
    consumeKey(const DFBWindowEvent& event);

private:
    LineInput* _input;
    PushButton* _keyCall;

    PushButton* _key1;
    PushButton* _key2;
    PushButton* _key3;

    PushButton* _key4;
    PushButton* _key5;
    PushButton* _key6;

    PushButton* _key7;
    PushButton* _key8;
    PushButton* _key9;

    PushButton* _keyStar;
    PushButton* _key0;
    PushButton* _keyHash;

    void
    sendKey(const std::string& key);
};

} /* namespace ilixi */
#endif /* ILXSIP_DIALER_H_ */
