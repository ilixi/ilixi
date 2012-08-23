/*
 * Sound.h
 *
 *  Created on: Aug 23, 2012
 *      Author: tarik
 */

#ifndef ILIXI_SOUND_H_
#define ILIXI_SOUND_H_

extern "C"
{
#include <fusionsound.h>
}

namespace ilixi
{

class Sound
{
public:
    Sound();

    virtual
    ~Sound();

    void
    getMasterAmplitude(float* left, float* right);

private:
    IFusionSound* _sound;
};

} /* namespace ilixi */
#endif /* ILIXI_SOUND_H_ */
