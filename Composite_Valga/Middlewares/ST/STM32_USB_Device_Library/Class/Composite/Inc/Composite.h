/*
 * Composite.h
 *
 *  Created on: 19 mar. 2020
 *      Author: Valga-DeskPC
 */
#ifndef ST_STM32_USB_DEVICE_LIBRARY_CLASS_COMPOSITE_INC_COMPOSITE_H_
#define ST_STM32_USB_DEVICE_LIBRARY_CLASS_COMPOSITE_INC_COMPOSITE_H_

#include "..\..\HID\Inc\usbd_hid.h"
#include "..\..\CDC\Inc\usbd_cdc.h"
#include "usbd_ctlreq.h"
#include  "usbd_ioreq.h"

#define USB_COMPOSITE_CONFIG_DESC_SIZ                     92U

typedef struct
{
	void *hid;
	void *cdc;
}USBD_Composite_HandleTypeDef;

typedef struct _USBD_Comp_Itf
{
	void *CDC_ops;
	void *HID_ops;
} USBD_Comp_ItfTypeDef;

extern USBD_ClassTypeDef USBD_COMP;
#define USBD_COMP_CLASS    &USBD_COMP
/********************CDC**************************/
extern uint8_t  USBD_CDC_Init(USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx);

extern uint8_t  USBD_CDC_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx);

extern uint8_t  USBD_CDC_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req);

extern uint8_t  USBD_CDC_DataIn(USBD_HandleTypeDef *pdev,
                                uint8_t epnum);

extern uint8_t  USBD_CDC_DataOut(USBD_HandleTypeDef *pdev,
                                 uint8_t epnum);

extern uint8_t  USBD_CDC_EP0_RxReady(USBD_HandleTypeDef *pdev);

extern uint8_t  *USBD_CDC_GetFSCfgDesc(uint16_t *length);

extern uint8_t  *USBD_CDC_GetHSCfgDesc(uint16_t *length);

extern uint8_t  *USBD_CDC_GetOtherSpeedCfgDesc(uint16_t *length);

extern uint8_t  *USBD_CDC_GetOtherSpeedCfgDesc(uint16_t *length);

//extern uint8_t  *USBD_CDC_GetDeviceQualifierDescriptor(uint16_t *length);
/*********************************************************/
/********************HID**********************************/
extern uint8_t  USBD_HID_Init(USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx);

extern uint8_t  USBD_HID_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx);

extern uint8_t  USBD_HID_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req);

extern uint8_t  *USBD_HID_GetFSCfgDesc(uint16_t *length);

extern uint8_t  *USBD_HID_GetHSCfgDesc(uint16_t *length);

extern uint8_t  *USBD_HID_GetOtherSpeedCfgDesc(uint16_t *length);

extern uint8_t  *USBD_HID_GetDeviceQualifierDesc(uint16_t *length);

extern uint8_t  USBD_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
/*********************************************************/
uint8_t  USBD_Composite_RegisterInterface(USBD_HandleTypeDef   *pdev,
									USBD_Comp_ItfTypeDef *fops);
#endif /* ST_STM32_USB_DEVICE_LIBRARY_CLASS_COMPOSITE_INC_COMPOSITE_H_ */
