#pragma once

#include <M5Unified.h>
#include <math.h>
#include <stdio.h>
#include "common.hpp"
#include "eez/src/ui/ui.h"

// 定义蜂鸣器的 GPIO 引脚（根据你的实际接线修改）
#define BUZZER_PIN 2

// 定义 PWM 频道和分辨率
#define PWM_CHANNEL 0
#define PWM_RESOLUTION 8 // 分辨率：8 位
#define PWM_FREQ 1000    // 初始频率（Hz）

bool buzzing;
bool sound_inited;
int level;

void sound_play(double freq)
{
  // 配置 PWM 频率、分辨率和通道
  ledcSetup(PWM_CHANNEL, PWM_FREQ, PWM_RESOLUTION);
  char name[20];
  lv_roller_get_selected_str(objects.tune, name, 10);
  lv_label_set_text_fmt(objects.sound__name, "%s%d/%ldHz", name, level, lround(freq));
  // 将 PWM 信号输出到蜂鸣器引脚
  ledcAttachPin(BUZZER_PIN, PWM_CHANNEL);
  ledcWriteTone(PWM_CHANNEL, freq);
}

void sound_stop()
{
  lv_label_set_text_fmt(objects.sound__name, "--");
  ledcWrite(PWM_CHANNEL, 0);
}

lv_timer_t *sound_btn_timer;

void sound_btn_detect(lv_timer_t *timer);

void sound_init()
{
  level = 4;
  if (!sound_inited)
  {
    buzzing = false;
    lv_label_set_text(objects.sound__play, LV_SYMBOL_PAUSE);
    lv_obj_clear_flag(objects.sound, LV_OBJ_FLAG_HIDDEN);
    sound_btn_timer = lv_timer_create(sound_btn_detect, 20, NULL);
    sound_inited = true;
  }
}

void sound_deinit()
{
  if (sound_inited)
  {
    lv_timer_delete(sound_btn_timer);
    lv_label_set_text(objects.sound__play, LV_SYMBOL_PAUSE);

    sound_stop();
    buzzing = false;

    lv_obj_add_flag(objects.sound, LV_OBJ_FLAG_HIDDEN);
    sound_inited = false;
  }
}

void sound_btn_detect(lv_timer_t *timer)
{
  if (M5.BtnA.wasClicked())
  {
    uint32_t sel = lv_roller_get_selected(objects.tune);
    lv_roller_set_selected(objects.tune, sel + 1, LV_ANIM_ON);
    if (buzzing)
    {
      sound_stop();
      sel = lv_roller_get_selected(objects.tune);
      sound_play(calculateFrequency(sel, level));
    }
  }
  if (M5.BtnA.wasHold())
  {
    if (!buzzing)
    {
      lv_label_set_text(objects.sound__play, LV_SYMBOL_PLAY);
      uint32_t sel = lv_roller_get_selected(objects.tune);
      sound_play(calculateFrequency(sel, level));
      buzzing = true;
    }
    else
    {
      sound_stop();
      buzzing = false;
    }
  }

  if (M5.BtnB.wasClicked())
  {
    nextFreq();
    if (buzzing)
    {
      uint32_t sel = lv_roller_get_selected(objects.tune);
      sound_stop();
      sound_play(calculateFrequency(sel, level));
    }
  }
  if (M5.BtnB.wasHold())
  {
    level++;
    if (level > 6)
    {
      level = 3;
    }
    if (buzzing)
    {
      uint32_t sel = lv_roller_get_selected(objects.tune);
      sound_stop();
      sound_play(calculateFrequency(sel, level));
    }
  }
}