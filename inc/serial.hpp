

#ifndef FIFO_HPP
#define FIFO_HPP

#include <vector>
#include "fifo.hpp"
#include <string>
#include <list>
#include <deque>

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
  STATUS Init(f init);
  STATUS StartReception(void);
  STATUS SendData(uint8_t* buffer,uint32_t size);
  STATUS SendData(Buffer_t);
  USART_TypeDef* getInstance(void);
  std::deque<std::string> rxList;
  Buffer_t txBuffer;
  uint32_t index;
  Fifo<uint8_t,128> rxBufferFifo;
  Fifo<Buffer_t,16> txBufferFifo;
protected:
  
  
  USART_TypeDef* USARTx;
};

#endif