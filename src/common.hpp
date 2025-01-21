#pragma once

#include "math.h"
#include "eez/src/ui/ui.h"

#define FREQS_NUM 8
char *a4FreqsName[] = {"415Hz", "432Hz", "440Hz", "441Hz", "442Hz", "442Hz", "443Hz", "445Hz"};
float a4Freqs[] = {415, 432, 440, 441, 442, 442, 443, 445};
int current_freq = 2;

float calculateFrequency(int i, int l)
{
  return a4Freqs[current_freq] * pow(2.0, (i-9) / 12.0) * pow(2.0, l-4);
}

int currentA4Freq()
{
  return a4Freqs[current_freq];
}

char *currentA4FreqName()
{
  return a4FreqsName[current_freq];
}

void nextFreq()
{
  current_freq = (current_freq + 1) % FREQS_NUM;
  lv_label_set_text(objects.a4freq, currentA4FreqName());
}