/*
 * IFusion.cpp
 *
 *  Created on: 10 Jul 2011
 *      Author: tarik
 */
#include "core/IFusion.h"

using namespace ilixi;

IMaestro::AppRecord*
IMaestro::getAppRecord(FusionVector *appVector, int fusionID)
{
  AppRecord* temp = NULL;
  for (int i = 0; i < appVector->count; ++i)
    {
      temp = (AppRecord *) appVector->elements[i];
      if (temp->fusionID == fusionID)
        return temp;
    }
  return NULL;
}
