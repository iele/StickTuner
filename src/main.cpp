#include <M5Unified.h>
#include <math.h>
#include <stdio.h>

#include "lv_port_disp.h"
#include "eez/src/ui/ui.h"

#include "pitch.hpp"
#include "sound.hpp"

int current_mode;
char* mode[] = {"Auto", "Sound"};

void battery_detect(lv_timer_t *timer)
{
  lv_label_set_text_fmt(objects.battery, "%.0d%%", M5.Power.getBatteryLevel());
}

// 初始化
void setup()
{
  auto cfg = M5.config();
  M5.begin(cfg);
  M5.Display.setRotation(1);

  lv_init();
  lv_port_disp_init();
  ui_init();

  battery_detect(NULL);
  lv_timer_create(battery_detect, 5000, NULL);

  pitch_init();
  current_mode = 0;
  lv_label_set_text(objects.mode, mode[current_mode]);
}

void loop()
{

  M5.update();

  if (M5.BtnPWR.wasClicked())
  {
    switch (current_mode)
    {
    case 0:
      current_mode = 1;
      pitch_deinit();
      sound_init();
      break;
    case 1:
      current_mode = 0;
      sound_deinit();
      pitch_init();
      break;
    }
    lv_label_set_text(objects.mode, mode[current_mode]);
  }

  uint32_t wait_ms = lv_timer_handler();
  M5.delay(20);
  lv_tick_inc(20);
  ui_tick();
}