#include "Arduino.h"
#include <MySignals.h>

class MSPrint {
  public:

    bool temperature();
    void airflow();
    void ECG();
    void EMG();
    void GSR();
    bool pulsioximeter();
    void snore();
    void bodyPosition();
    void bloodPressure();
    void pulsioximeter_alarm();

};
