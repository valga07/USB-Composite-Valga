/**
  ******************************************************************************
  * @file    usbd_hid.c
  * @author  MCD Application Team
  * @brief   This file provides the HID core functions.
  *
  * @verbatim
  *
  *          ===================================================================
  *                                HID Class  Description
  *          ===================================================================
  *           This module manages the HID class V1.11 following the "Device Class Definition
  *           for Human Interface Devices (HID) Version 1.11 Jun 27, 2001".
  *           This driver implements the following aspects of the specification:
  *             - The Boot Interface Subclass
  *             - The Mouse protocol
  *             - Usage Page : Generic Desktop
  *             - Usage : Joystick
  *             - Collection : Application
  *
  * @note     In HS mode and when the DMA is used, all variables and data structures
  *           dealing with the DMA during the transaction process should be 32-bit aligned.
  *
  *
  *  @endverbatim
  *
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2015 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                      www.st.com/SLA0044
  *
  ******************************************************************************
  */

/* BSPDependencies
- "stm32xxxxx_{eval}{discovery}{nucleo_144}.c"
- "stm32xxxxx_{eval}{discovery}_io.c"
EndBSPDependencies */

/* Includes ------------------------------------------------------------------*/
#include "..\Inc\usbd_hid.h"
#include "usbd_ctlreq.h"
#include "..\..\Composite\Inc\Composite.h"


/** @addtogroup STM32_USB_DEVICE_LIBRARY
  * @{
  */


/** @defgroup USBD_HID
  * @brief usbd core module
  * @{
  */

/** @defgroup USBD_HID_Private_TypesDefinitions
  * @{
  */
/**
  * @}
  */


/** @defgroup USBD_HID_Private_Defines
  * @{
  */

/**
  * @}
  */


/** @defgroup USBD_HID_Private_Macros
  * @{
  */
/**
  * @}
  */




/** @defgroup USBD_HID_Private_FunctionPrototypes
  * @{
  */


 uint8_t  USBD_HID_Init(USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx);

 uint8_t  USBD_HID_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx);

 uint8_t  USBD_HID_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req);

 uint8_t  *USBD_HID_GetFSCfgDesc(uint16_t *length);

 uint8_t  *USBD_HID_GetHSCfgDesc(uint16_t *length);

 uint8_t  *USBD_HID_GetOtherSpeedCfgDesc(uint16_t *length);

 uint8_t  *USBD_HID_GetDeviceQualifierDesc(uint16_t *length);

 uint8_t  USBD_HID_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
/**
  * @}
  */

/** @defgroup USBD_HID_Private_Variables
  * @{
  */

USBD_ClassTypeDef  USBD_HID =
{
  USBD_HID_Init,
  USBD_HID_DeInit,
  USBD_HID_Setup,
  NULL, /*EP0_TxSent*/
  NULL, /*EP0_RxReady*/
  USBD_HID_DataIn, /*DataIn*/
  NULL, /*DataOut*/
  NULL, /*SOF */
  NULL,
  NULL,
  USBD_HID_GetHSCfgDesc,
  USBD_HID_GetFSCfgDesc,
  USBD_HID_GetOtherSpeedCfgDesc,
  USBD_HID_GetDeviceQualifierDesc,
};

/* USB HID device FS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CfgFSDesc[USB_HID_CONFIG_DESC_SIZ]  __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /************** Descriptor of Joystick Mouse interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x01,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x01,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_KEYBOARD_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

  HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_EPIN_SIZE, /*wMaxPacketSize: 4 Byte max */
  0x00,
  HID_FS_BINTERVAL,          /*bInterval: Polling Interval */
  /* 34 */
};

/* USB HID device HS Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_CfgHSDesc[USB_HID_CONFIG_DESC_SIZ]  __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /************** Descriptor of Joystick Mouse interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x01,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x02,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_KEYBOARD_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

  HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_EPIN_SIZE, /*wMaxPacketSize: 4 Byte max */
  0x00,
  HID_HS_BINTERVAL,          /*bInterval: Polling Interval */
  /* 34 */
};

/* USB HID device Other Speed Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_OtherSpeedCfgDesc[USB_HID_CONFIG_DESC_SIZ]  __ALIGN_END =
{
  0x09, /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION, /* bDescriptorType: Configuration */
  USB_HID_CONFIG_DESC_SIZ,
  /* wTotalLength: Bytes returned */
  0x00,
  0x01,         /*bNumInterfaces: 1 interface*/
  0x01,         /*bConfigurationValue: Configuration value*/
  0x00,         /*iConfiguration: Index of string descriptor describing
  the configuration*/
  0xE0,         /*bmAttributes: bus powered and Support Remote Wake-up */
  0x32,         /*MaxPower 100 mA: this current is used for detecting Vbus*/

  /************** Descriptor of Joystick Mouse interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x01,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x02,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of Joystick Mouse HID ********************/
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_KEYBOARD_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
  /******************** Descriptor of Mouse endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

  HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_EPIN_SIZE, /*wMaxPacketSize: 4 Byte max */
  0x00,
  HID_FS_BINTERVAL,          /*bInterval: Polling Interval */
  /* 34 */
};


/* USB HID device Configuration Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ]  __ALIGN_END  =
{
  /* 18 */
  0x09,         /*bLength: HID Descriptor size*/
  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
  0x11,         /*bcdHID: HID Class Spec release number*/
  0x01,
  0x00,         /*bCountryCode: Hardware target country*/
  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
  0x22,         /*bDescriptorType*/
  HID_KEYBOARD_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
  0x00,
};

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_HID_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC]  __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,
  USB_DESC_TYPE_DEVICE_QUALIFIER,
  0x00,
  0x02,
  0x00,
  0x00,
  0x00,
  0x40,
  0x01,
  0x00,
};

__ALIGN_BEGIN static uint8_t HID_KEYBOARD_ReportDesc[HID_KEYBOARD_REPORT_DESC_SIZE]  __ALIGN_END =
{
	     0x05    ,//bSize: 0x01, bType: Global, bTag: Usage Page
	     0x01    ,//Usage Page(Generic Desktop Controls )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x06    ,//Usage(Keyboard)
	     0xA1    ,//bSize: 0x01, bType: Main, bTag: Collection
	     0x01    ,//Collection(Application )
	     0x85    ,//bSize: 0x01, bType: Global, bTag: Report ID
	     0x01    ,//Report ID(0x1 )
	     0x05    ,//bSize: 0x01, bType: Global, bTag: Usage Page
	     0x07    ,//Usage Page(Keyboard/Keypad )
	     0x19    ,//bSize: 0x01, bType: Local, bTag: Usage Minimum
	     0xE0    ,//Usage Minimum(0xE0 )
	     0x29    ,//bSize: 0x01, bType: Local, bTag: Usage Maximum
	     0xE7    ,//Usage Maximum(0xE7 )
	     0x15    ,//bSize: 0x01, bType: Global, bTag: Logical Minimum
	     0x00    ,//Logical Minimum(0x0 )
	     0x25    ,//bSize: 0x01, bType: Global, bTag: Logical Maximum
	     0x01    ,//Logical Maximum(0x1 )
	     0x75    ,//bSize: 0x01, bType: Global, bTag: Report Size
	     0x01    ,//Report Size(0x1 )
	     0x95    ,//bSize: 0x01, bType: Global, bTag: Report Count
	     0x08    ,//Report Count(0x8 )
	     0x81    ,//bSize: 0x01, bType: Main, bTag: Input
	     0x02    ,//Input(Data, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
	     0x75    ,//bSize: 0x01, bType: Global, bTag: Report Size
	     0x08    ,//Report Size(0x8 )
	     0x95    ,//bSize: 0x01, bType: Global, bTag: Report Count
	     0x01    ,//Report Count(0x1 )
	     0x81    ,//bSize: 0x01, bType: Main, bTag: Input
	     0x01    ,//Input(Constant, Array, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
	     0x05    ,//bSize: 0x01, bType: Global, bTag: Usage Page
	     0x07    ,//Usage Page(Keyboard/Keypad )
	     0x19    ,//bSize: 0x01, bType: Local, bTag: Usage Minimum
	     0x00    ,//Usage Minimum(0x0 )
	     0x29    ,//bSize: 0x01, bType: Local, bTag: Usage Maximum
	     0x65    ,//Usage Maximum(0x65 )
	     0x15    ,//bSize: 0x01, bType: Global, bTag: Logical Minimum
	     0x00    ,//Logical Minimum(0x0 )
	     0x25    ,//bSize: 0x01, bType: Global, bTag: Logical Maximum
	     0x65    ,//Logical Maximum(0x65 )
	     0x75    ,//bSize: 0x01, bType: Global, bTag: Report Size
	     0x08    ,//Report Size(0x8 )
	     0x95    ,//bSize: 0x01, bType: Global, bTag: Report Count
	     0x05    ,//Report Count(0x5 )
	     0x81    ,//bSize: 0x01, bType: Main, bTag: Input
	     0x00    ,//Input(Data, Array, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
	     0xC0    ,//bSize: 0x00, bType: Main, bTag: End Collection
	     0x05    ,//bSize: 0x01, bType: Global, bTag: Usage Page
	     0x0C    ,//Usage Page(Consumer )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x01    ,//Usage(Consumer Control)
	     0xA1    ,//bSize: 0x01, bType: Main, bTag: Collection
	     0x01    ,//Collection(Application )
	     0x85    ,//bSize: 0x01, bType: Global, bTag: Report ID
	     0x02    ,//Report ID(0x2 )
	     0x19    ,//bSize: 0x01, bType: Local, bTag: Usage Minimum
	     0x00    ,//Usage Minimum(0x0 )
	     0x2A    ,//bSize: 0x02, bType: Local, bTag: Usage Maximum
	     0x3C,
	     0x02 ,//Usage Maximum(0x23C )
	     0x15    ,//bSize: 0x01, bType: Global, bTag: Logical Minimum
	     0x00    ,//Logical Minimum(0x0 )
	     0x26    ,//bSize: 0x02, bType: Global, bTag: Logical Maximum
	     0x3C,
	     0x02 ,//Logical Maximum(0x23C )
	     0x95    ,//bSize: 0x01, bType: Global, bTag: Report Count
	     0x01    ,//Report Count(0x1 )
	     0x75    ,//bSize: 0x01, bType: Global, bTag: Report Size
	     0x10    ,//Report Size(0x10 )
	     0x81    ,//bSize: 0x01, bType: Main, bTag: Input
	     0x00    ,//Input(Data, Array, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
	     0xC0    ,//bSize: 0x00, bType: Main, bTag: End Collection
	     0x05    ,//bSize: 0x01, bType: Global, bTag: Usage Page
	     0x01    ,//Usage Page(Generic Desktop Controls )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x80    ,//Usage(System Control)
	     0xA1    ,//bSize: 0x01, bType: Main, bTag: Collection
	     0x01    ,//Collection(Application )
	     0x85    ,//bSize: 0x01, bType: Global, bTag: Report ID
	     0x03    ,//Report ID(0x3 )
	     0x19    ,//bSize: 0x01, bType: Local, bTag: Usage Minimum
	     0x81    ,//Usage Minimum(0x81 )
	     0x29    ,//bSize: 0x01, bType: Local, bTag: Usage Maximum
	     0x83    ,//Usage Maximum(0x83 )
	     0x15    ,//bSize: 0x01, bType: Global, bTag: Logical Minimum
	     0x00    ,//Logical Minimum(0x0 )
	     0x25    ,//bSize: 0x01, bType: Global, bTag: Logical Maximum
	     0x01    ,//Logical Maximum(0x1 )
	     0x75    ,//bSize: 0x01, bType: Global, bTag: Report Size
	     0x01    ,//Report Size(0x1 )
	     0x95    ,//bSize: 0x01, bType: Global, bTag: Report Count
	     0x03    ,//Report Count(0x3 )
	     0x81    ,//bSize: 0x01, bType: Main, bTag: Input
	     0x02    ,//Input(Data, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
	     0x95    ,//bSize: 0x01, bType: Global, bTag: Report Count
	     0x05    ,//Report Count(0x5 )
	     0x81    ,//bSize: 0x01, bType: Main, bTag: Input
	     0x01    ,//Input(Constant, Array, Absolute, No Wrap, Linear, Preferred State, No Null Position, Bit Field)
	     0xC0    ,//bSize: 0x00, bType: Main, bTag: End Collection
	     0x06    ,//bSize: 0x02, bType: Global, bTag: Usage Page
	     0x01,
	     0xFF ,//Usage Page(Undefined )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x01    ,//Usage(1)
	     0xA1    ,//bSize: 0x01, bType: Main, bTag: Collection
	     0x01    ,//Collection(Application )
	     0x85    ,//bSize: 0x01, bType: Global, bTag: Report ID
	     0x04    ,//Report ID(0x4 )
	     0x95    ,//bSize: 0x01, bType: Global, bTag: Report Count
	     0x01    ,//Report Count(0x1 )
	     0x75    ,//bSize: 0x01, bType: Global, bTag: Report Size
	     0x08    ,//Report Size(0x8 )
	     0x15    ,//bSize: 0x01, bType: Global, bTag: Logical Minimum
	     0x01    ,//Logical Minimum(0x1 )
	     0x25    ,//bSize: 0x01, bType: Global, bTag: Logical Maximum
	     0x0A    ,//Logical Maximum(0xA )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x20    ,//Usage(32)
	     0xB1    ,//bSize: 0x01, bType: Main, bTag: Feature
	     0x03    ,//Feature(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x23    ,//Usage(35)
	     0xB1    ,//bSize: 0x01, bType: Main, bTag: Feature
	     0x03    ,//Feature(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	     0x25    ,//bSize: 0x01, bType: Global, bTag: Logical Maximum
	     0x4F    ,//Logical Maximum(0x4F )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x21    ,//Usage(33)
	     0xB1    ,//bSize: 0x01, bType: Main, bTag: Feature
	     0x03    ,//Feature(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	     0x25    ,//bSize: 0x01, bType: Global, bTag: Logical Maximum
	     0x30    ,//Logical Maximum(0x30 )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x22    ,//Usage(34)
	     0xB1    ,//bSize: 0x01, bType: Main, bTag: Feature
	     0x03    ,//Feature(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	     0x95    ,//bSize: 0x01, bType: Global, bTag: Report Count
	     0x03    ,//Report Count(0x3 )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x24    ,//Usage(36)
	     0xB1    ,//bSize: 0x01, bType: Main, bTag: Feature
	     0x03    ,//Feature(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	     0xC0    ,//bSize: 0x00, bType: Main, bTag: End Collection
	     0x06    ,//bSize: 0x02, bType: Global, bTag: Usage Page
	     0x01,
	     0xFF ,//Usage Page(Undefined )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x01    ,//Usage(1)
	     0xA1    ,//bSize: 0x01, bType: Main, bTag: Collection
	     0x01    ,//Collection(Application )
	     0x85    ,//bSize: 0x01, bType: Global, bTag: Report ID
	     0x05    ,//Report ID(0x5 )
	     0x95    ,//bSize: 0x01, bType: Global, bTag: Report Count
	     0x01    ,//Report Count(0x1 )
	     0x75    ,//bSize: 0x01, bType: Global, bTag: Report Size
	     0x08    ,//Report Size(0x8 )
	     0x15    ,//bSize: 0x01, bType: Global, bTag: Logical Minimum
	     0x01    ,//Logical Minimum(0x1 )
	     0x25    ,//bSize: 0x01, bType: Global, bTag: Logical Maximum
	     0x0A    ,//Logical Maximum(0xA )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x20    ,//Usage(32)
	     0xB1    ,//bSize: 0x01, bType: Main, bTag: Feature
	     0x03    ,//Feature(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x23    ,//Usage(35)
	     0xB1    ,//bSize: 0x01, bType: Main, bTag: Feature
	     0x03    ,//Feature(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	     0x25    ,//bSize: 0x01, bType: Global, bTag: Logical Maximum
	     0x4F    ,//Logical Maximum(0x4F )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x21    ,//Usage(33)
	     0xB1    ,//bSize: 0x01, bType: Main, bTag: Feature
	     0x03    ,//Feature(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	     0x25    ,//bSize: 0x01, bType: Global, bTag: Logical Maximum
	     0x30    ,//Logical Maximum(0x30 )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x22    ,//Usage(34)
	     0xB1    ,//bSize: 0x01, bType: Main, bTag: Feature
	     0x03    ,//Feature(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	     0x95    ,//bSize: 0x01, bType: Global, bTag: Report Count
	     0x03    ,//Report Count(0x3 )
	     0x09    ,//bSize: 0x01, bType: Local, bTag: Usage
	     0x24    ,//Usage(36)
	     0xB1    ,//bSize: 0x01, bType: Main, bTag: Feature
	     0x03    ,//Feature(Constant, Variable, Absolute, No Wrap, Linear, Preferred State, No Null Position, Non VolatileBit Field)
	     0xC0    //bSize: 0x00, bType: Main, bTag: End Collection
};
//{
//  0x05,   0x01,
//  0x09,   0x02,
//  0xA1,   0x01,
//  0x09,   0x01,
//
//  0xA1,   0x00,
//  0x05,   0x09,
//  0x19,   0x01,
//  0x29,   0x03,
//
//  0x15,   0x00,
//  0x25,   0x01,
//  0x95,   0x03,
//  0x75,   0x01,
//
//  0x81,   0x02,
//  0x95,   0x01,
//  0x75,   0x05,
//  0x81,   0x01,
//
//  0x05,   0x01,
//  0x09,   0x30,
//  0x09,   0x31,
//  0x09,   0x38,
//
//  0x15,   0x81,
//  0x25,   0x7F,
//  0x75,   0x08,
//  0x95,   0x03,
//
//  0x81,   0x06,
//  0xC0,   0x09,
//  0x3c,   0x05,
//  0xff,   0x09,
//
//  0x01,   0x15,
//  0x00,   0x25,
//  0x01,   0x75,
//  0x01,   0x95,
//
//  0x02,   0xb1,
//  0x22,   0x75,
//  0x06,   0x95,
//  0x01,   0xb1,
//
//  0x01,   0xc0
//};

/**
  * @}
  */

/** @defgroup USBD_HID_Private_Functions
  * @{
  */

/**
  * @brief  USBD_HID_Init
  *         Initialize the HID interface
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
 uint8_t  USBD_HID_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
  USBD_Composite_HandleTypeDef *compHandle;
  USBD_HID_HandleTypeDef *hhid;
  /* Open EP IN */
  USBD_LL_OpenEP(pdev, HID_EPIN_ADDR, USBD_EP_TYPE_INTR, HID_EPIN_SIZE);
  pdev->ep_in[HID_EPIN_ADDR & 0xFU].is_used = 1U;

  //pdev->pClassData = USBD_malloc(sizeof(USBD_HID_HandleTypeDef));
  compHandle = (USBD_Composite_HandleTypeDef *)pdev->pClassData;
  compHandle->hid = USBD_malloc_HID(sizeof(USBD_HID_HandleTypeDef));
  hhid = (USBD_HID_HandleTypeDef *)compHandle->hid;

  if (compHandle->hid== NULL)
  {
    return USBD_FAIL;
  }

  hhid->state = HID_IDLE;

  return USBD_OK;
}

/**
  * @brief  USBD_HID_Init
  *         DeInitialize the HID layer
  * @param  pdev: device instance
  * @param  cfgidx: Configuration index
  * @retval status
  */
 uint8_t  USBD_HID_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx)
{
  USBD_Composite_HandleTypeDef *compHandle;
  compHandle = (USBD_Composite_HandleTypeDef *)pdev->pClassData;
  /* Close HID EPs */
  USBD_LL_CloseEP(pdev, HID_EPIN_ADDR);
  pdev->ep_in[HID_EPIN_ADDR & 0xFU].is_used = 0U;

  /* FRee allocated memory */
  if (compHandle->hid != NULL)
  {
    USBD_free(compHandle->hid);
    compHandle->hid = NULL;
  }

  return USBD_OK;
}

/**
  * @brief  USBD_HID_Setup
  *         Handle the HID specific requests
  * @param  pdev: instance
  * @param  req: usb requests
  * @retval status
  */
 uint8_t  USBD_HID_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req)
{
  USBD_Composite_HandleTypeDef *compHandle;
  compHandle = (USBD_Composite_HandleTypeDef *)pdev->pClassData;
  USBD_HID_HandleTypeDef *hhid = (USBD_HID_HandleTypeDef *) compHandle->hid;
  uint16_t len = 0U;
  uint8_t *pbuf = NULL;
  uint16_t status_info = 0U;
  USBD_StatusTypeDef ret = USBD_OK;

  switch (req->bmRequest & USB_REQ_TYPE_MASK)
  {
    case USB_REQ_TYPE_CLASS :
      switch (req->bRequest)
      {
        case HID_REQ_SET_PROTOCOL:
          hhid->Protocol = (uint8_t)(req->wValue);
          break;

        case HID_REQ_GET_PROTOCOL:
          USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->Protocol, 1U);
          break;

        case HID_REQ_SET_IDLE:
          hhid->IdleState = (uint8_t)(req->wValue >> 8);
          break;

        case HID_REQ_GET_IDLE:
          USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->IdleState, 1U);
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;
    case USB_REQ_TYPE_STANDARD:
      switch (req->bRequest)
      {
        case USB_REQ_GET_STATUS:
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            USBD_CtlSendData(pdev, (uint8_t *)(void *)&status_info, 2U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_GET_DESCRIPTOR:
          if (req->wValue >> 8 == HID_REPORT_DESC)
          {
            len = MIN(HID_KEYBOARD_REPORT_DESC_SIZE, req->wLength);
            pbuf = HID_KEYBOARD_ReportDesc;
          }
          else if (req->wValue >> 8 == HID_DESCRIPTOR_TYPE)
          {
            pbuf = USBD_HID_Desc;
            len = MIN(USB_HID_DESC_SIZ, req->wLength);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
            break;
          }
          USBD_CtlSendData(pdev, pbuf, len);
          break;

        case USB_REQ_GET_INTERFACE :
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            USBD_CtlSendData(pdev, (uint8_t *)(void *)&hhid->AltSetting, 1U);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        case USB_REQ_SET_INTERFACE :
          if (pdev->dev_state == USBD_STATE_CONFIGURED)
          {
            hhid->AltSetting = (uint8_t)(req->wValue);
          }
          else
          {
            USBD_CtlError(pdev, req);
            ret = USBD_FAIL;
          }
          break;

        default:
          USBD_CtlError(pdev, req);
          ret = USBD_FAIL;
          break;
      }
      break;

    default:
      USBD_CtlError(pdev, req);
      ret = USBD_FAIL;
      break;
  }

  return ret;
}

/**
  * @brief  USBD_HID_SendReport
  *         Send HID Report
  * @param  pdev: device instance
  * @param  buff: pointer to report
  * @retval status
  */
uint8_t USBD_HID_SendReport(USBD_HandleTypeDef  *pdev,
                            uint8_t *report,
                            uint16_t len)
{
  USBD_Composite_HandleTypeDef *compHandle;
  compHandle = (USBD_Composite_HandleTypeDef *)pdev->pClassData;
  USBD_HID_HandleTypeDef     *hhid = (USBD_HID_HandleTypeDef *)compHandle->hid;

  if (pdev->dev_state == USBD_STATE_CONFIGURED)
  {
    if (hhid->state == HID_IDLE)
    {
      hhid->state = HID_BUSY;
      USBD_LL_Transmit(pdev,
                       HID_EPIN_ADDR,
                       report,
                       len);
    }
  }
  return USBD_OK;
}

/**
  * @brief  USBD_HID_GetPollingInterval
  *         return polling interval from endpoint descriptor
  * @param  pdev: device instance
  * @retval polling interval
  */
uint32_t USBD_HID_GetPollingInterval(USBD_HandleTypeDef *pdev)
{
  uint32_t polling_interval = 0U;

  /* HIGH-speed endpoints */
  if (pdev->dev_speed == USBD_SPEED_HIGH)
  {
    /* Sets the data transfer polling interval for high speed transfers.
     Values between 1..16 are allowed. Values correspond to interval
     of 2 ^ (bInterval-1). This option (8 ms, corresponds to HID_HS_BINTERVAL */
    polling_interval = (((1U << (HID_HS_BINTERVAL - 1U))) / 8U);
  }
  else   /* LOW and FULL-speed endpoints */
  {
    /* Sets the data transfer polling interval for low and full
    speed transfers */
    polling_interval =  HID_FS_BINTERVAL;
  }

  return ((uint32_t)(polling_interval));
}

/**
  * @brief  USBD_HID_GetCfgFSDesc
  *         return FS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
 uint8_t  *USBD_HID_GetFSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_HID_CfgFSDesc);
  return USBD_HID_CfgFSDesc;
}

/**
  * @brief  USBD_HID_GetCfgHSDesc
  *         return HS configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
 uint8_t  *USBD_HID_GetHSCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_HID_CfgHSDesc);
  return USBD_HID_CfgHSDesc;
}

/**
  * @brief  USBD_HID_GetOtherSpeedCfgDesc
  *         return other speed configuration descriptor
  * @param  speed : current device speed
  * @param  length : pointer data length
  * @retval pointer to descriptor buffer
  */
 uint8_t  *USBD_HID_GetOtherSpeedCfgDesc(uint16_t *length)
{
  *length = sizeof(USBD_HID_OtherSpeedCfgDesc);
  return USBD_HID_OtherSpeedCfgDesc;
}

 extern HIDLOP_TransferHandler hHIDTransfer;

/**
  * @brief  USBD_HID_DataIn
  *         handle data IN Stage
  * @param  pdev: device instance
  * @param  epnum: endpoint index
  * @retval status
  */
 uint8_t  USBD_HID_DataIn(USBD_HandleTypeDef *pdev,
                                uint8_t epnum)
{
  USBD_Composite_HandleTypeDef *compHandle;
  compHandle = (USBD_Composite_HandleTypeDef *)pdev->pClassData;
  /* Ensure that the FIFO is empty before a new transfer, this condition could
  be caused by  a new transfer before the end of the previous transfer */
  ((USBD_HID_HandleTypeDef *)compHandle->hid)->state = HID_IDLE;
  hHIDTransfer.SendNextChar(pdev, &hHIDTransfer);
  return USBD_OK;
}


/**
* @brief  DeviceQualifierDescriptor
*         return Device Qualifier descriptor
* @param  length : pointer data length
* @retval pointer to descriptor buffer
*/
 uint8_t  *USBD_HID_GetDeviceQualifierDesc(uint16_t *length)
{
  *length = sizeof(USBD_HID_DeviceQualifierDesc);
  return USBD_HID_DeviceQualifierDesc;
}

/**
  * @}
  */


/**
  * @}
  */

 static void Ascii2Keyboard(uint8_t *KeyBoardBuff, uint8_t AsciiVal);
 __weak void SendNextCharCallBack(USBD_HandleTypeDef *pdev, HIDLOP_TransferHandler *hTransf);
 __weak void TransferCompletedCallBack(void *ptr);

 HIDLOP_TransferHandler hHIDTransfer =
 {
 	.HID_StateMachine = LOP_IDLE,
 	.TxBuffer = NULL,
 	.MessageSize = 0,
 	.RemainingSize = 0,
 	.TransferCompletedCallBack = TransferCompletedCallBack,
 	.SendNextChar = SendNextCharCallBack
 };
 uint8_t BufferSend[8] ={1,0,0,0,0,0,0,0};


 __weak void SendNextCharCallBack(USBD_HandleTypeDef *pdev, HIDLOP_TransferHandler *hTransf)
 {
	 if(hTransf->RemainingSize == 0)
	 {
		 hTransf->HID_StateMachine = LOP_IDLE;
		 hTransf->MessageSize = 0;
		 hTransf->TransferCompletedCallBack(NULL);
	 }
	 else
	 {
		 Ascii2Keyboard(BufferSend, hTransf->TxBuffer[hTransf->MessageSize - hTransf->RemainingSize]);
		 USBD_HID_SendReport(pdev, BufferSend, 8);
		 hTransf->RemainingSize--;
	 }
 }

 __weak void TransferCompletedCallBack(void *ptr)
 {
	 UNUSED(ptr);
 }

 HIDLOP_FSM SendMessageHID (USBD_HandleTypeDef *pdev, uint8_t *Buffer, uint32_t SizeOfMsg)
 {
 	if(hHIDTransfer.HID_StateMachine != LOP_IDLE)
 		return LOP_BUSY;
 	if(!SizeOfMsg)
 		return LOP_IDLE;
 	else if(SizeOfMsg == 1)
 	{
 		Ascii2Keyboard(BufferSend, Buffer[0]);
 		USBD_HID_SendReport(pdev, BufferSend, 8);
 		return LOP_IDLE;
 	}
 	else
 	{
 		hHIDTransfer.HID_StateMachine = LOP_BUSY;
 		hHIDTransfer.TxBuffer = Buffer;
 		hHIDTransfer.MessageSize = SizeOfMsg;
 		Ascii2Keyboard(BufferSend, Buffer[0]);
 		USBD_HID_SendReport(pdev, BufferSend, 8);
 		hHIDTransfer.RemainingSize = SizeOfMsg - 1;
 		return LOP_OK;
 	}
 }

 static void Ascii2Keyboard(uint8_t *KeyBoardBuff, uint8_t AsciiVal)
 {
 	const uint8_t ascii2kbNumbers [10] = {KEY_0_CPARENTHESIS, KEY_1_EXCLAMATION_MARK, KEY_2_AT,
 		KEY_3_NUMBER_SIGN, KEY_4_DOLLAR, KEY_5_PERCENT, KEY_6_CARET, KEY_7_AMPERSAND,
 		KEY_8_ASTERISK, KEY_9_OPARENTHESIS};
 		if((AsciiVal >= 65) && (AsciiVal <= 90))
 		{
 			KeyBoardBuff[2] = KEY_LEFTSHIFT;
 			KeyBoardBuff[4] = AsciiVal - 61;
 		}
 		else if((AsciiVal >= 97) && (AsciiVal <= 122))
 		{
 			KeyBoardBuff[2] = 0x00;
 			KeyBoardBuff[4] = AsciiVal - 93;
 		}
 		else if((AsciiVal >= 48) && (AsciiVal <= 57))
 		{
 			KeyBoardBuff[2] = 0x00;
 			KeyBoardBuff[4] = ascii2kbNumbers[AsciiVal - 48];
 		}
 		else if((AsciiVal == 0x0D) || (AsciiVal == 0x0A))
 		{
 			KeyBoardBuff[2] = 0x00;
 			KeyBoardBuff[4] = KEY_ENTER;
 			hHIDTransfer.RemainingSize = 0;
 		}
 		else
 			switch(AsciiVal)
 			{
 				case(' '):
 					KeyBoardBuff[2] = 0x00;
 					KeyBoardBuff[4] = KEY_SPACEBAR;
 						break;
 				case('!'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 					KeyBoardBuff[4] = ascii2kbNumbers[1];
 						break;
 				case('@'):
 					KeyBoardBuff[2] = KEY_RIGHTALT;
 					KeyBoardBuff[4] = KEY_Q;
 						break;
 				case('#'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 					KeyBoardBuff[4] = ascii2kbNumbers[3];
 						break;
 				case('$'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 					KeyBoardBuff[4] = ascii2kbNumbers[4];
 						break;
 				case('%'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 					KeyBoardBuff[4] = ascii2kbNumbers[5];
 						break;
 				case('&'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 					KeyBoardBuff[4] = ascii2kbNumbers[6];
 						break;
 				case('/'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 					KeyBoardBuff[4] = ascii2kbNumbers[7];
 						break;
 				case('('):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 					KeyBoardBuff[4] = ascii2kbNumbers[8];
 						break;
 				case(')'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 					KeyBoardBuff[4] = ascii2kbNumbers[9];
 						break;
 				case('='):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 					KeyBoardBuff[4] = ascii2kbNumbers[0];
 						break;
 				case('-'):
 					KeyBoardBuff[2] = 0x00;
 				//KeyBoardBuff[4] = KEY_MINUS_UNDERSCORE;
 				KeyBoardBuff[4] = KEY_SLASH_QUESTION;
 						break;
 				case('_'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 				//KeyBoardBuff[4] = KEY_MINUS_UNDERSCORE;
 				KeyBoardBuff[4] = KEY_SLASH_QUESTION;
 						break;
 				case('"'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 				KeyBoardBuff[4] = ascii2kbNumbers[2];
 						break;
 				case('?'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 				KeyBoardBuff[4] = KEY_EQUAL_PLUS;
 						break;
 				case('['):
 					KeyBoardBuff[2] = 0x00;
 				KeyBoardBuff[4] = KEY_OBRACKET_AND_OBRACE;
 						break;
 				case('{'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 				KeyBoardBuff[4] = KEY_OBRACKET_AND_OBRACE;
 						break;
 				case(']'):
 					KeyBoardBuff[2] = 0x00;
 				KeyBoardBuff[4] = KEY_CBRACKET_AND_CBRACE;
 						break;
 				case('}'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 				KeyBoardBuff[4] = KEY_CBRACKET_AND_CBRACE;
 						break;
 				case('*'):
 					KeyBoardBuff[2] = 0x00;
 				KeyBoardBuff[4] = KEY_KEYPAD_ASTERIKS;
 						break;
 				case('+'):
 					KeyBoardBuff[2] = 0x00;
 				KeyBoardBuff[4] = KEY_KEYPAD_PLUS;
 						break;
 				case('.'):
 					KeyBoardBuff[2] = 0x00;
 				KeyBoardBuff[4] = KEY_DOT_GREATER;
 						break;
 				case(':'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 				KeyBoardBuff[4] = KEY_DOT_GREATER;
 						break;
 				case(';'):
 					KeyBoardBuff[2] = KEY_LEFTSHIFT;
 				KeyBoardBuff[4] = KEY_COMMA_AND_LESS;
 						break;
 				default:
 					KeyBoardBuff[2] = 0x00;
 					KeyBoardBuff[4] = KEY_KEYPAD_PERCENT;
 						break;

 			}
 }

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
