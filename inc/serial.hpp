

#ifndef FIFO_HPP
#define FIFO_HPP


#include <iostream>
#include <sstream>
#include <string>
using namespace std;
extern "C"{
#include "stm32f1xx_ll_usart.h"
}

class Serial
{
  typedef struct
  {
    uint8_t* buffer;
    uint32_t size;
  }Buffer_t;
  
  typedef enum
  {
    OK,
    BUSY,
    TIMEOUT,
    ERROR,
  }STATUS;
  typedef void (*f)(void);
public:
  Serial();
  USART_TypeDef* getInstance(){return USARTx;}
  bool ready(){return readyState;}
  void setReady(bool state){readyState = state;}
  STATUS Init(f init);
  STATUS StartReception(void);
  string rxStr;
  stringstream rxSStream;
  stringstream txSStream;
protected:  
  USART_TypeDef* USARTx;
  bool readyState;
};

#endif