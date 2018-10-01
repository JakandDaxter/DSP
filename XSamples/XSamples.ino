// file: Lab2_dither_average.ino
// created by: Clark Hochgraf 150901
// modified by: David Orlicki 170817
// purpose: drive and read dither-averaged LM61 readings  

#include <MsTimer2.h>

const int DAC0 = 3, DAC1 = 4, DAC2 = 5, LM61 = A0;
const int TSAMP_MSEC = 100;
const int NUM_SAMPLES = 256;

volatile boolean sampleFlag = false;
int nSmpl = 1;
float sample, samplemV, sampleC;

//**********************************************************************
void setup()
{
  configureArduino();
  Serial.println("Enter 'g' to go .....");
  while (Serial.read() != 'g'); // spin until 'g' entry
  MsTimer2::set(TSAMP_MSEC, ISR_Sample); // Set sample msec, ISR name
  MsTimer2::start(); // start running the Timer   
} // setup()

//**********************************************************************
void loop()
{
  syncSample();
  sample = analogReadDitherAve();  // Rdith = 200
  samplemV = sample+600.0; //1 CV per milivolt, offset by 600
  sampleC = (samplemV-600.0)/10.0; //10 milivolts per degree C
  //sample = analogRead(LM61);
  displayData(); 
  if (++nSmpl > NUM_SAMPLES) MsTimer2::stop(); 
} // loop()

//**********************************************************************
float analogReadDitherAve(void)
{
  const int NUM_SUB_SMPL = 1;
  int ramp;
  float sum = 0.0;
  for (int i = 0; i < NUM_SUB_SMPL; i++)
  {
    ramp = 0; // ramp 0..7 using Modulo function
    digitalWrite(DAC0, (ramp & B00000001)); //Bit mask
    digitalWrite(DAC1, (ramp & B00000010)); //Bit mask
    digitalWrite(DAC2, (ramp & B00000100)); //Bit mask
    sum += analogRead(LM61);
  }
  return sum/NUM_SUB_SMPL; // averaged subsamples   
}
//**********************************************************************
void configureArduino(void)
{
  pinMode(DAC0,OUTPUT); digitalWrite(DAC0,LOW);
  pinMode(DAC1,OUTPUT); digitalWrite(DAC1,LOW);
  pinMode(DAC2,OUTPUT); digitalWrite(DAC2,LOW);
  analogRead(LM61); // read and discard to prime ADC registers
  Serial.begin(9600);   
}
//**********************************************************************
//**********************************************************************
void displayData(void) {  
  if (nSmpl == 1) Serial.print("\nn\tsample\tsamplemV\tsampleC\n");
  Serial.print(nSmpl);  Serial.print('\t');
  Serial.print(sample);  Serial.print('\t');
  Serial.print(samplemV);  Serial.print('\t');
  Serial.print(sampleC); Serial.print('\n');}
void syncSample(void)
{
  while(sampleFlag == false);
  sampleFlag = false;
}
//void ISR_Sample() {}
void ISR_Sample() {sampleFlag = true;}
