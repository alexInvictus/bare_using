/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#define _C_MAIN_
#include "all.h"
/* printff重定向函数重定向串口1 ---------------------------------------------------------*/
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif 
PUTCHAR_PROTOTYPE
{
HAL_UART_Transmit(&huart3 , (uint8_t *)&ch, 1, 0xFFFF);
return ch;
} 

int main(void)
{
  system_init();
  while (1)
  {
  switch(Command_State)
	{
		case Read_State: 			//读取状态  该模式下读取eeprom里面的小车ID号(只执行一次)		
				           Eeprom_Read();			
		                   Send_Id();
                           Command_State=Wait_State;	
							 break;	
	 
		case Wait_State:                                     //等待命令状态  (所有暂停点的判断case)
			
			    if(packflag_3==1)
					{
					 if((Rx_buff_3[4]=='f')&&(Rx_buff_3[5]=='w'))
					 {
						 Command_State=Run_Ahead_State;              //起始位置收到go前进模式
					 }
					 else if((Rx_buff_3[4]=='b')&&(Rx_buff_3[5]=='k'))
					 {
						 Command_State=Run_Back_State;               //起始位置收到bk返回模式
					 }
					  else if((Rx_buff_3[4]=='r')&&(Rx_buff_3[5]=='k'))
					 {
						 Command_State=Run_Ruku_State;               //起始位置收到rk前进模式
					 }
                      else if((Rx_buff_3[4]=='o')&&(Rx_buff_3[5]=='k')) 
					 {
						 Command_State=Store_State;
					 }				 
					   packflag_3=0;
					   USART_RX_STA_3=0;
					 }				
					     break;
	  
		case Store_State:                                                        //等待地图信息并分析的状态 换存地图。
                                if(Uart_Store()==1)
								{
							//	printf("a");
								packflag_3=0;
                                USART_RX_STA_3=0;									
		                        Command_State=Analyse_State;
								}			
		           break;
		
    case Analyse_State:		 
			    if(Key_Start==1)
				{
					HAL_UART_Transmit(&huart3,(u8*)Tsk,4,1000);                        //完成分析后按下按钮，发送指令给中控表示启动任务
					HAL_UART_Transmit(&huart3,(u8*)Fw,3,1000);
					HAL_UART_Receive_IT(&huart3, (u8 *)aRxBuffer_3, 1);
		            Command_State=Wait_State;
	            }
				  if(Key_Stop==1)
				{
					HAL_UART_Transmit(&huart3,(u8*)Tsk,4,1000);                        //完成分析后按下按钮，发送指令给中控表示启动任务
					HAL_UART_Transmit(&huart3,(u8*)Bk,3,1000);
					HAL_UART_Receive_IT(&huart3, (u8 *)aRxBuffer_3, 1);
		            Command_State=Wait_State;
	            }	
                 if((Rx_buff_33[4]=='w')&&(Rx_buff_33[5]=='t'))
					{
						 Command_State=Run_Wt_State;                                     //直接判断是否去取料点。收到去取料点地图去取料点
					}
				 else if((Rx_buff_33[4]=='r')&&(Rx_buff_33[5]=='k'))
					{
						 Command_State=Run_Ruku_State;                                     //是否是入库点，是的话直接入库
					}
					     break;
   				
	  case Run_Ahead_State:                                                    //解析完地图，正常向前循迹的状态
			   
		       Motor_Ahead();

		           break;
		case Run_Wt_State:
			   
		       Motor_Ahead_Wait();
		
		           break;
		case Run_Back_State:                                                     //到达终点时，切换的向后循迹的状态
			    
		       Motor_Back(); 		      
		      
		           break;
		
		case Stop_State:
			
					    Stop();
					    Command_State=Wait_State; 
		           break;
		
		case Run_Ruku_State:
			
			     Motor_Ruku();
		      
		           break;
		
		default:
			
		           break;      
		}
 }
}





#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}

#endif
 
/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
