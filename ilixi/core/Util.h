#ifndef ILIXI_UTIL_H_
#define ILIXI_UTIL_H_

#include <directfb.h>
#include <string.h>
#include <direct/clock.h>
#include <direct/debug.h>

typedef struct
{
  int magic;

  int frames;
  float fps;
  long long fps_time;
  char fps_string[20];
} FPSData;

static inline void
fpsInit(FPSData *data)
{
  D_ASSERT( data != NULL);

  memset(data, 0, sizeof(FPSData));

  data->fps_time = direct_clock_get_millis();

  D_MAGIC_SET( data, FPSData);
}

static inline void
fpsCount(FPSData *data, int interval)
{
  long long diff;
  long long now = direct_clock_get_millis();

  D_MAGIC_ASSERT( data, FPSData);

  data->frames++;

  diff = now - data->fps_time;
  if (diff >= interval)
    {
      data->fps = data->frames * 1000 / (float) diff;

      snprintf(data->fps_string, sizeof(data->fps_string), "%.1f", data->fps);

      data->fps_time = now;
      data->frames = 0;
    }
}

#endif /* ILIXI_UTIL_H_ */
