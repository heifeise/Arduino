#ifndef READTEMPERATURE_H
#define READTEMPERATURE_H
#include <LiquidCrystal.h>
#include <DallasTemperature.h>
#include <OneWire.h>
#include"Arduino.h"
#define DQ A4
//OneWire G_onewire(DQ);
DallasTemperature G_temp(new OneWire(DQ));
class readtemperature {
  public:
    readtemperature();
    float readT();
 };
readtemperature::readtemperature()
{
   G_temp.begin();
}
float readtemperature::readT()
{
  G_temp.requestTemperatures();
  return G_temp.getTempCByIndex(0);
}
#endif
