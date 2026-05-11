/*
 * bsp_led.h
 *
 *  Created on: May 16, 2024
 *      Author: zphu
 */

#ifndef MACBSP_INC_BSP_LED_H_

#define MACBSP_INC_BSP_LED_H_
#include "bsp_sys.h"

#define             LED_NUM             (30)
/****************************** LED 函数宏定义***************************************/
#define             macLED_Debug_OFF()                              HAL_GPIO_WritePin ( LED_DEBUG_GPIO_Port, LED_DEBUG_Pin , GPIO_PIN_RESET )
#define             macLED_Debug_ON()                               HAL_GPIO_WritePin ( LED_DEBUG_GPIO_Port, LED_DEBUG_Pin , GPIO_PIN_SET )

#define             macLED_Conscious_Judgment_OFF()                 HAL_GPIO_WritePin ( LED_CONSCIOUS_JUDGMENT_GPIO_Port, LED_CONSCIOUS_JUDGMENT_Pin , GPIO_PIN_RESET )
#define             macLED_Conscious_Judgment_ON()                  HAL_GPIO_WritePin ( LED_CONSCIOUS_JUDGMENT_GPIO_Port, LED_CONSCIOUS_JUDGMENT_Pin , GPIO_PIN_RESET )

#define             macLED_Sphygmoscopy_OFF()                       HAL_GPIO_WritePin ( LED_SPHYGMOSCOPY_GPIO_Port, LED_SPHYGMOSCOPY_Pin , GPIO_PIN_RESET )
#define             macLED_Sphygmoscopy_ON()                        HAL_GPIO_WritePin ( LED_SPHYGMOSCOPY_GPIO_Port, LED_SPHYGMOSCOPY_Pin , GPIO_PIN_RESET )

#define             macLED_Check_Breath_OFF()                       HAL_GPIO_WritePin ( LED_CHECK_BREATH_GPIO_Port, LED_CHECK_BREATH_Pin , GPIO_PIN_RESET )
#define             macLED_Check_Breath_ON()                        HAL_GPIO_WritePin ( LED_CHECK_BREATH_GPIO_Port, LED_CHECK_BREATH_Pin , GPIO_PIN_RESET )

#define             macLED_Emergency_Call_OFF()                     HAL_GPIO_WritePin ( LED_EMERGENCY_CALL_GPIO_Port, LED_EMERGENCY_CALL_Pin , GPIO_PIN_RESET )
#define             macLED_Emergency_Call_ON()                      HAL_GPIO_WritePin ( LED_EMERGENCY_CALL_GPIO_Port, LED_EMERGENCY_CALL_Pin , GPIO_PIN_RESET )

#define             macLED_Remove_Foreign_OFF()                     HAL_GPIO_WritePin ( LED_REMOVE_FOREIGN_GPIO_Port, LED_REMOVE_FOREIGN_Pin , GPIO_PIN_RESET )
#define             macLED_Remove_Foreign_ON()                      HAL_GPIO_WritePin ( LED_REMOVE_FOREIGN_GPIO_Port, LED_REMOVE_FOREIGN_Pin , GPIO_PIN_RESET )

#define             macLED_Body1_OFF()                              HAL_GPIO_WritePin ( LED_BODY1_GPIO_Port, LED_BODY1_Pin , GPIO_PIN_RESET )
#define             macLED_Body1_ON()                               HAL_GPIO_WritePin ( LED_BODY1_GPIO_Port, LED_BODY1_Pin , GPIO_PIN_SET )

#define             macLED_Body2_OFF()                              HAL_GPIO_WritePin ( LED_BODY2_GPIO_Port, LED_BODY2_Pin , GPIO_PIN_RESET )
#define             macLED_Body2_ON()                               HAL_GPIO_WritePin ( LED_BODY2_GPIO_Port, LED_BODY2_Pin , GPIO_PIN_SET )

#define             macLED_Body3_OFF()                              HAL_GPIO_WritePin ( LED_BODY3_GPIO_Port, LED_BODY3_Pin , GPIO_PIN_RESET )
#define             macLED_Body3_ON()                               HAL_GPIO_WritePin ( LED_BODY3_GPIO_Port, LED_BODY3_Pin , GPIO_PIN_SET )

#define             macLED_Body4_OFF()                              HAL_GPIO_WritePin ( LED_BODY4_GPIO_Port, LED_BODY4_Pin , GPIO_PIN_RESET )
#define             macLED_Body4_ON()                               HAL_GPIO_WritePin ( LED_BODY4_GPIO_Port, LED_BODY4_Pin , GPIO_PIN_SET )

#define             macLED_Body5_OFF()                              HAL_GPIO_WritePin ( LED_BODY5_GPIO_Port, LED_BODY5_Pin , GPIO_PIN_RESET )
#define             macLED_Body5_ON()                               HAL_GPIO_WritePin ( LED_BODY5_GPIO_Port, LED_BODY5_Pin , GPIO_PIN_SET )

#define             macLED_Body6_OFF()                              HAL_GPIO_WritePin ( LED_BODY6_GPIO_Port, LED_BODY6_Pin , GPIO_PIN_RESET )
#define             macLED_Body6_ON()                               HAL_GPIO_WritePin ( LED_BODY6_GPIO_Port, LED_BODY6_Pin , GPIO_PIN_SET )

#define             macLED_Body7_OFF()                              HAL_GPIO_WritePin ( LED_BODY7_GPIO_Port, LED_BODY7_Pin , GPIO_PIN_RESET )
#define             macLED_Body7_ON()                               HAL_GPIO_WritePin ( LED_BODY7_GPIO_Port, LED_BODY7_Pin , GPIO_PIN_SET )

#define             macLED_Reset_OFF()                              HAL_GPIO_WritePin ( LED_RESET_GPIO_Port, LED_RESET_Pin , GPIO_PIN_RESET )
#define             macLED_Reset_ON()                               HAL_GPIO_WritePin ( LED_RESET_GPIO_Port, LED_RESET_Pin , GPIO_PIN_SET )

#define             macLED_Competition_OFF()                        HAL_GPIO_WritePin ( LED_COMPETITION_GPIO_Port, LED_COMPETITION_Pin , GPIO_PIN_RESET )
#define             macLED_Competition_ON()                         HAL_GPIO_WritePin ( LED_COMPETITION_GPIO_Port, LED_COMPETITION_Pin , GPIO_PIN_SET )

#define             macLED_Assess_OFF()                             HAL_GPIO_WritePin ( LED_ASSESS_GPIO_Port, LED_ASSESS_Pin , GPIO_PIN_RESET )
#define             macLED_Assess_ON()                              HAL_GPIO_WritePin ( LED_ASSESS_GPIO_Port, LED_ASSESS_Pin , GPIO_PIN_SET )

#define             macLED_Train_OFF()                              HAL_GPIO_WritePin ( LED_TRAIN_GPIO_Port, LED_TRAIN_Pin , GPIO_PIN_RESET )
#define             macLED_Train_ON()                               HAL_GPIO_WritePin ( LED_TRAIN_GPIO_Port, LED_TRAIN_Pin , GPIO_PIN_SET )

#define             macLED_Minus_OFF()                              HAL_GPIO_WritePin ( LED_MINUS_GPIO_Port, LED_MINUS_Pin , GPIO_PIN_RESET )
#define             macLED_Minus_ON()                               HAL_GPIO_WritePin ( LED_MINUS_GPIO_Port, LED_MINUS_Pin , GPIO_PIN_SET )

#define             macLED_Plus_OFF()                               HAL_GPIO_WritePin ( LED_PLUS_GPIO_Port, LED_PLUS_Pin , GPIO_PIN_RESET )
#define             macLED_Plus_ON()                                HAL_GPIO_WritePin ( LED_PLUS_GPIO_Port, LED_PLUS_Pin , GPIO_PIN_SET )

#define             macLED_Setting_OFF()                            HAL_GPIO_WritePin ( LED_SETTING_GPIO_Port, LED_SETTING_Pin , GPIO_PIN_RESET )
#define             macLED_Setting_ON()                             HAL_GPIO_WritePin ( LED_SETTING_GPIO_Port, LED_SETTING_Pin , GPIO_PIN_SET )

#define             macLED_Printer_OFF()                            HAL_GPIO_WritePin ( LED_PRINTER_GPIO_Port, LED_PRINTER_Pin , GPIO_PIN_RESET )
#define             macLED_Printer_ON()                             HAL_GPIO_WritePin ( LED_PRINTER_GPIO_Port, LED_PRINTER_Pin , GPIO_PIN_RESET )

#define             macLED_Start_OFF()                              HAL_GPIO_WritePin ( LED_START_GPIO_Port, LED_START_Pin , GPIO_PIN_RESET )
#define             macLED_Start_ON()                               HAL_GPIO_WritePin ( LED_START_GPIO_Port, LED_START_Pin , GPIO_PIN_SET )
/*LED名称类型*/
typedef	enum
{
    LED_Name_Debug = (0x01),            /*! 调试LED */
    LED_Name_Consciousness_Judgment,    /*! 意识判断LED */
    LED_Name_Sphygmoscopy,              /*! 脉搏检查LED */
    LED_Name_Check_Breath,              /*! 呼吸检查LED */
    LED_Name_Emergency_Call,            /*! 急救呼叫LED */
    LED_Name_Remove_Foreign,            /*! 清除异物LED */
    LED_Name_Body1,
    LED_Name_Body2,
    LED_Name_Body3,
    LED_Name_Body4,
    LED_Name_Body5,
    LED_Name_Body6,
    LED_Name_Body7,
    LED_Name_Reset,                     /*! 复位LED */
    LED_Name_Competition,               /*! 竞赛LED */
    LED_Name_Assess,                    /*! 考核LED */
    LED_Name_Train,                     /*! 训练LED */
    LED_Name_Minus_Sign,                /*! 减号LED */
    LED_Name_Plus_Sign,                 /*! 加号LED */
    LED_Name_Setting,                   /*! 设置LED */
    LED_Name_Printer,                   /*! 打印LED */
    LED_Name_Start,                     /*! 启动LED */

}LED_Name_TypeDef;



/************************** PAD 函数声明********************************/
void LED_Init(void);
void LED_Out(int8_t ledName, int8_t ledState);
int8_t 	LED_GetNumber(void);
void LED_Off(int8_t ledName);
void LED_On(int8_t ledName);
void LED_Toggle(int8_t ledName);
void LED_Grad(int8_t ledName);
void LED_BlinkSetCycleDuty(int8_t ledName, int8_t Cycle, int8_t Duty);
void LED_Blink(int8_t ledName, int8_t cry, int8_t mute, int8_t repeat);
void LED_Fancy(int8_t mode);
void LED_DrvScan(void);
int ledTimer_Init(void);



#endif /* MACBSP_INC_BSP_LED_H_ */
