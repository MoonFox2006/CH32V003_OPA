#define USE_OPO

#include <ch32v00x.h>
#include "Arduino.h"
#include "TM1637.h"

#ifdef USE_OPO
constexpr uint8_t AIN_PIN = PD4; // OPO
#else
constexpr uint8_t AIN_PIN = PC4;
#endif

TM1637<PC2, PC1> tm1637;
volatile uint16_t adcAvg = 0;

void setup() {
  pinMode(AIN_PIN, INPUT_ANALOG);
#ifdef USE_OPO
  pinMode(PA2, INPUT); // OPP0
  pinMode(PA1, INPUT); // OPN0
  EXTEN->EXTEN_CTR &= ~(EXTEN_OPA_PSEL | EXTEN_OPA_NSEL);
  EXTEN->EXTEN_CTR |= EXTEN_OPA_EN;
#endif
  NVIC_EnableIRQ(ADC_IRQn);
  ADC1->CTLR1 |= ADC_EOCIE;
#ifdef USE_OPO
  ADC1->SAMPTR2 |= (0x07 << (3 * 7)); // ADC_SampleTime_241Cycles
#else
  ADC1->SAMPTR2 |= (0x07 << (3 * 2)); // ADC_SampleTime_241Cycles
#endif
  ADC1->RSQR3 &= ~0x1F;
#ifdef USE_OPO
  ADC1->RSQR3 |= 7;
#else
  ADC1->RSQR3 |= 2;
#endif
  ADC1->CTLR2 |= (ADC_SWSTART | ADC_EXTTRIG | ADC_CONT);
  tm1637.begin();
}

void loop() {
  delay(50);
  tm1637 = adcAvg;
}

extern "C" void __attribute__((interrupt("WCH-Interrupt-fast"))) ADC1_IRQHandler() {
  constexpr uint8_t SAMPLE_COUNT = 32;

  static uint16_t samples[SAMPLE_COUNT];
  static uint32_t avg = 0;
  static int8_t pos = -SAMPLE_COUNT;

  if (ADC1->STATR & ADC_EOC) {
    uint16_t data = ADC1->RDATAR;

    if (pos < 0) {
      samples[SAMPLE_COUNT + pos] = data;
    } else {
      avg -= samples[pos];
      samples[pos] = data;
    }
    avg += data;
    if (++pos >= SAMPLE_COUNT)
      pos = 0;
    if (pos < 0)
      adcAvg = avg / (SAMPLE_COUNT + pos);
    else
      adcAvg = avg / SAMPLE_COUNT;
    ADC1->STATR = ~ADC_EOC;
  }
}
