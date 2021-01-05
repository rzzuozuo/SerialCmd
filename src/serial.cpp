#include "fifo.hpp"
#include "serial.hpp"
extern "C"
{
#include "main.h"
#include "stm32f1xx_ll_usart.h"
}

Serial::Serial():
  USARTx(USART2)
  {
    
  }
  
  Serial::STATUS Serial::Init(f init)
  {
    STATUS status = ERROR;
    init();    
    status = OK;
    return status;
  }
  Serial::STATUS Serial::StartReception(void)
  {
    STATUS status = ERROR;
    LL_USART_EnableIT_RXNE(USARTx);
    LL_USART_EnableIT_ERROR(USARTx);
    status = OK;
    return status;
  }
  Serial::STATUS Serial::SendData(uint8_t* buffer,uint32_t size)
  {
    Buffer_t item;
    item.size = size;
    
    item.buffer = new uint8_t[size];
    for(int i = 0;i < size;++i)
    {
     item.buffer[i] = buffer[i];
    }
   return SendData(item);
  }
  Serial::STATUS Serial::SendData(Buffer_t item)
  {
    STATUS status = ERROR;
    if(!txBufferFifo.push(item))
        return status; 
    status = OK;
    return status;
  }
  
  USART_TypeDef* Serial::getInstance(void)
  {
    return USARTx;
  }
  
  
Serial serialCmd;


/**
  * @brief  Rx Transfer completed callback
  * @note   This example shows a simple way to report end of IT Rx transfer, and 
  *         you can add your own implementation.
  * @retval None
  */
void UART_CharReception_Callback(void)
{
  /* Read Received character. RXNE flag is cleared by reading of DR register */  
  uint8_t lastData = LL_USART_ReceiveData8(serialCmd.getInstance());
  serialCmd.rxBufferFifo.push(lastData);
}

/**
  * @brief  Function called for achieving next TX Byte sending
  * @retval None
  */
void UART_TXEmpty_Callback(void)
{
  USART_TypeDef* USARTx = serialCmd.getInstance();
  if(serialCmd.index == (serialCmd.txBuffer.size - 1))
  {
    /* Disable TXE interrupt */
    LL_USART_DisableIT_TXE(USARTx);
    
    /* Enable TC interrupt */
    LL_USART_EnableIT_TC(USARTx);
  }

  /* Fill DR with a new char */
  LL_USART_TransmitData8(USARTx, serialCmd.txBuffer.buffer[serialCmd.index++]);
}

/**
  * @brief  Function called at completion of last byte transmission
  * @retval None
  */
void UART_CharTransmitComplete_Callback(void)
{
  USART_TypeDef* USARTx = serialCmd.getInstance();
  if(serialCmd.index == serialCmd.txBuffer.size)
  {
    serialCmd.index = 0;
    delete serialCmd.txBuffer.buffer;
    serialCmd.txBuffer.buffer = NULL;
    if(serialCmd.txBufferFifo.isEmpty())
    {
    /* Disable TC interrupt */
    LL_USART_DisableIT_TC(USARTx);
    
    }
    else
    {
      
      serialCmd.txBufferFifo.pop(serialCmd.txBuffer);
      /* Fill DR with a new char */
      if(serialCmd.txBuffer.size != 0)
      {
        LL_USART_TransmitData8(USARTx, serialCmd.txBuffer.buffer[serialCmd.index++]);
      }
    }
  }
}

/**
  * @brief  UART error callbacks
  * @note   This example shows a simple way to report transfer error, and you can
  *         add your own implementation.
  * @retval None
  */
void UART_Error_Callback(void)
{
  __IO uint32_t sr_reg;

  /* Disable USARTx_IRQn */
  NVIC_DisableIRQ(USART2_IRQn);
  
  /* Error handling example :
    - Read USART SR register to identify flag that leads to IT raising
    - Perform corresponding error handling treatment according to flag
  */
  sr_reg = LL_USART_ReadReg(USART2, SR);
  if (sr_reg & LL_USART_SR_NE)
  {

  }
  else
  {

  }
}


void UART_Serial_Irq(void)
{
   if(LL_USART_IsActiveFlag_RXNE(USART2) && LL_USART_IsEnabledIT_RXNE(USART2))
  {
    /* RXNE flag will be cleared by reading of DR register (done in call) */
    /* Call function in charge of handling Character reception */
    UART_CharReception_Callback();
  }
  
  if(LL_USART_IsEnabledIT_TXE(USART2) && LL_USART_IsActiveFlag_TXE(USART2))
  {
    /* TXE flag will be automatically cleared when writing new data in DR register */
    
    /* Call function in charge of handling empty DR => will lead to transmission of next character */
    UART_TXEmpty_Callback();
  }
  
  if(LL_USART_IsEnabledIT_TC(USART2) && LL_USART_IsActiveFlag_TC(USART2))
  {
    /* Clear TC flag */
    LL_USART_ClearFlag_TC(USART2);
    /* Call function in charge of handling end of transmission of sent character
    and prepare next charcater transmission */
    UART_CharTransmitComplete_Callback();
  }
  
  if(LL_USART_IsEnabledIT_ERROR(USART2) && LL_USART_IsActiveFlag_NE(USART2))
  {
    /* Call Error function */
    UART_Error_Callback();
  }
}

void SerialInit(void)
{
  serialCmd.Init(MX_USART2_UART_Init);
  serialCmd.StartReception();
}

void SerialProcess(void)
{
  if(!serialCmd.rxBufferFifo.isEmpty())
  {
    char data;
    serialCmd.rxBufferFifo.pop((uint8_t&)data);
    static std::string rxString;
    if(data!='\n')
    {
      rxString.push_back(data);
    }
    else
    {
      serialCmd.rxList.push_back(rxString);
      rxString.erase();
    }
  }
  if(!serialCmd.rxList.empty())
  {
    std::string str = serialCmd.rxList.front();
    serialCmd.rxList.pop_front();
    str.
  }

}