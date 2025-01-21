#pragma once

#include <M5Unified.h>
#include <math.h>
#include <stdio.h>
#include "common.hpp"
#include "eez/src/ui/ui.h"

// 定义常量
#define YIN_THRESHOLD 0.15 // 检测置信度阈值
#define SAMPLE_FREQ 16000  // 采样率
#define SAMPLES 2048       // 采样点数

// 全局变量
int16_t *sample;
lv_timer_t *pitch_timer;
lv_timer_t *pitch_btn_timer;

bool pitch_inited;

void pitch_detect(lv_timer_t *timer);
void pitch_btn_detect(lv_timer_t *timer);

void pitch_init()
{
  if (!pitch_inited)
  {
    M5.Mic.begin();
    sample = (int16_t *)ps_malloc(SAMPLES * sizeof(int16_t));
    lv_obj_clear_flag(objects.pitcher, LV_OBJ_FLAG_HIDDEN);
    pitch_timer = lv_timer_create(pitch_detect, 100, NULL);
    pitch_btn_timer = lv_timer_create(pitch_btn_detect, 20, NULL);
    pitch_inited = true;
  }
}

void pitch_deinit()
{
  if (pitch_inited)
  {
    lv_timer_delete(pitch_timer);
    lv_timer_delete(pitch_btn_timer);
    lv_obj_add_flag(objects.pitcher, LV_OBJ_FLAG_HIDDEN);
    M5.Mic.end();
    free(sample);
    pitch_inited = false;
  }
}

void pitch_btn_detect(lv_timer_t *timer){
  if (M5.BtnB.wasClicked())
  {
    nextFreq();
  }
}


// 差值函数
void yin_difference_function(const int16_t *audio_buffer, int buffer_size, float *difference)
{
  for (int tau = 0; tau < buffer_size / 2; tau++)
  {
    difference[tau] = 0;
    for (int i = 0; i < buffer_size / 2; i++)
    {
      float delta = (float)audio_buffer[i] - (float)audio_buffer[i + tau];
      difference[tau] += delta * delta;
    }
  }
}

// 累积归一化差值函数
void yin_cumulative_mean_normalized_difference(float *difference, int buffer_size)
{
  difference[0] = 1;
  float running_sum = 0;
  for (int tau = 1; tau < buffer_size / 2; tau++)
  {
    running_sum += difference[tau];
    difference[tau] *= tau / running_sum;
  }
}

// 查找第一个局部最小值
int yin_find_tau(float *difference, int buffer_size, float threshold)
{
  for (int tau = 1; tau < buffer_size / 2; tau++)
  {
    if (difference[tau] < threshold)
    {
      while (tau + 1 < buffer_size / 2 && difference[tau + 1] < difference[tau])
      {
        tau++;
      }
      return tau;
    }
  }
  return -1; // 未检测到基频
}

// 抛物线插值，提高精度
float yin_parabolic_interpolation(float *difference, int tau)
{
  float alpha = difference[tau - 1];
  float beta = difference[tau];
  float gamma = difference[tau + 1];
  return tau + (alpha - gamma) / (2 * (alpha - 2 * beta + gamma));
}

// YIN 算法主函数
float yin_detect_pitch(const int16_t *audio_buffer, int buffer_size)
{
  float difference[buffer_size / 2];
  yin_difference_function(audio_buffer, buffer_size, difference);
  yin_cumulative_mean_normalized_difference(difference, buffer_size);

  int tau = yin_find_tau(difference, buffer_size, YIN_THRESHOLD);
  if (tau == -1)
  {
    return -1; // 未检测到基频
  }

  float refined_tau = yin_parabolic_interpolation(difference, tau);
  return SAMPLE_FREQ / refined_tau;
}

void pitch_detect(lv_timer_t *timer)
{
  // 1. 采集音频数据
  M5.Mic.record(sample, SAMPLES, SAMPLE_FREQ);

  // 3. 检测基频
  float detectedPitch = yin_detect_pitch(sample, SAMPLES);

  // 6. 显示稳定后的频率
  static char freq_str[50];
  if (detectedPitch > 8000 || detectedPitch < 22) // 超出人耳可听范围
  {
    sprintf(freq_str, "----");
    lv_slider_set_value(objects.pitcher__cents, 0, LV_ANIM_ON);
  }
  else
  {
    const char *noteNames[] = {"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"};
    int n = round(12 * log2(detectedPitch / currentA4Freq()));
    float closestFrequency = currentA4Freq() * pow(2.0, n / 12.0);
    int noteIndex = (n + 69) % 12;
    int octave = (n + 69) / 12 - 1;
    float cents = 1200 * log2(detectedPitch / closestFrequency);
    cents = 5 * round(cents / 5);

    sprintf(freq_str, "%.1fhz/%s%d/%.0fcts", detectedPitch, noteNames[noteIndex], octave, cents);
    lv_roller_set_selected(objects.tune, noteIndex, LV_ANIM_ON);
    lv_slider_set_value(objects.pitcher__cents, cents, LV_ANIM_ON);
  }

  lv_label_set_text(objects.pitcher__freq, freq_str);

}
