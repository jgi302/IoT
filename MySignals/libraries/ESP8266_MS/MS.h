#include "Arduino.h"

class ESP8266_MS {
  public:

    /** 
     * Verify ESP8266 startup. 
     *
     * Send command "AT"
     * 
     * @retval true - alive.
     * @retval false - dead.
     */
    bool startup(void);
    
bool firmware(void);

    /**
     * Restart ESP8266. 
     *
     * Send command "AT+RST"
     * This method will take 3 seconds or more. 
     *
     * @retval true - success.
     * @retval false - failure.
     */
    bool restart(void);
    
    /**
     * Set ESP8266 UART configuration.
     *
     * Send command "AT+UART_CUR=<baudrate>,<databits>,<stopbits>,<parity>,<flow control>"
     *
     * @retval true - success.
     * @retval false - failure.
    */
    bool setUart(uint32_t baud = 115200);

    /**
     * Set ESP8266 operation mode to staion. 
     * 
     * Send command "AT+CWMOD_CUR=1"
     *
     * @retval true - success.
     * @retval false - failure.
     */
    bool setOprToStation(void);

    bool joinAP(String ssid, String pwd);

    bool getAP(void);

    bool singleConnection(void);

    bool startTCP(String type, String addr, uint32_t port);

    bool tcpStatus(void);

    bool tcpSend(String msg, uint32_t len);

    bool tcpSend(const uint8_t *buffer, uint32_t len);

  private:

    /* 
     * Empty the buffer or UART RX.
     */
    void serial_empty(void);

    /* 
     * Recvive data from uart. Return all received data if target found or timeout. 
     */
    String recvString(String target, uint32_t timeout = 1000);

    /* 
     * Recvive data from uart. Return all received data if target found or timeout. 
     */
    String recvString(String target1, String target2, uint32_t timeout = 1000);

String recvString(String target1, String target2, String target3, uint32_t timeout);

    /* 
     * Recvive data from uart and search first target. Return true if target found, false for timeout.
     */
    bool recvFind(String target, uint32_t timeout = 1000);

};
