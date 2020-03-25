/*
 * Composite.c
 *
 *  Created on: 19 mar. 2020
 *      Author: Valga-DeskPC
 */

#include "..\Inc\Composite.h"
#include "stm32wbxx_hal_def.h"



static uint8_t  USBD_Composite_Init(USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx);

static uint8_t  USBD_Composite_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx);

static uint8_t  USBD_Composite_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req);

static uint8_t  USBD_Composite_DataIn(USBD_HandleTypeDef *pdev,
                                uint8_t epnum);

static uint8_t  USBD_Composite_DataOut(USBD_HandleTypeDef *pdev,
                                 uint8_t epnum);

static uint8_t  USBD_Composite_EP0_RxReady(USBD_HandleTypeDef *pdev);

static uint8_t  *USBD_Composite_GetFSCfgDesc(uint16_t *length);

//static uint8_t  *USBD_Composite_GetHSCfgDesc(uint16_t *length);

//static uint8_t  *USBD_Composite_GetOtherSpeedCfgDesc(uint16_t *length);

//static uint8_t  *USBD_Composite_GetOtherSpeedCfgDesc(uint16_t *length);

static uint8_t  *USBD_Composite_GetDeviceQualifierDescriptor(uint16_t *length);

USBD_Comp_ItfTypeDef Composite_Operators;

/* USB Standard Device Descriptor */
__ALIGN_BEGIN static uint8_t USBD_Composite_DeviceQualifierDesc[USB_LEN_DEV_QUALIFIER_DESC] __ALIGN_END =
{
  USB_LEN_DEV_QUALIFIER_DESC,				//bLength
  USB_DESC_TYPE_DEVICE_QUALIFIER,			//bDescriptorType
  0x00,										//Specification Release 1
  0x02,										//Specification Release 2
  0x00,										//bDeviceClass
  0x00,										//bDeviceSubClass
  0x00,										//bDeviceProtocol
  0x40,										//bMaxPacketSize0
  0x01,										//bNumConfigurations
  0x00,										//bReserved
};



USBD_ClassTypeDef USBD_COMP =
{
  USBD_Composite_Init,
  USBD_Composite_DeInit,
  USBD_Composite_Setup,
  NULL,                 /* EP0_TxSent, */
  USBD_Composite_EP0_RxReady,
  USBD_Composite_DataIn,
  USBD_Composite_DataOut,
  NULL,
  NULL,
  NULL,
  NULL,					//USBD_CDC_GetHSCfgDesc,
  USBD_Composite_GetFSCfgDesc,
  NULL, 				//USBD_Composite_GetOtherSpeedCfgDesc,
  USBD_Composite_GetDeviceQualifierDescriptor,
};

/* USB CDC device Configuration Descriptor */
__ALIGN_BEGIN uint8_t USBD_Composite_CfgFSDesc[USB_COMPOSITE_CONFIG_DESC_SIZ] __ALIGN_END =
{
  /*Configuration Descriptor*/
  0x09,   /* bLength: Configuration Descriptor size */
  USB_DESC_TYPE_CONFIGURATION,      /* bDescriptorType: Configuration */
  USB_COMPOSITE_CONFIG_DESC_SIZ,                /* wTotalLength:no of returned bytes */
  0x00,
  /*0x02*/0X03,   /* bNumInterfaces: 2 interface */ /*3 iNTERFACES = 2CDC + 1HID*/
  0x01,   /* bConfigurationValue: Configuration value */
  0x04,   /* iConfiguration: Index of string descriptor describing the configuration */
  0xE0,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 0 mA */

  /*---------------------------------------------------------------------------*/
  /************** Descriptor of HID interface ****************/
  /* 09 */
  0x09,         /*bLength: Interface Descriptor size*/
  USB_DESC_TYPE_INTERFACE,/*bDescriptorType: Interface descriptor type*/
  0x00,         /*bInterfaceNumber: Number of Interface*/
  0x00,         /*bAlternateSetting: Alternate setting*/
  0x01,         /*bNumEndpoints*/
  0x03,         /*bInterfaceClass: HID*/
  0x01,         /*bInterfaceSubClass : 1=BOOT, 0=no boot*/
  0x01,         /*nInterfaceProtocol : 0=none, 1=keyboard, 2=mouse*/
  0X05,            /*iInterface: Index of string descriptor*/
  /******************** Descriptor of keyboard HID ********************/
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
  /******************** Descriptor of Keyboard endpoint ********************/
  /* 27 */
  0x07,          /*bLength: Endpoint Descriptor size*/
  USB_DESC_TYPE_ENDPOINT, /*bDescriptorType:*/

  HID_EPIN_ADDR,     /*bEndpointAddress: Endpoint Address (IN)*/
  0x03,          /*bmAttributes: Interrupt endpoint*/
  HID_EPIN_SIZE, /*wMaxPacketSize: 4 Byte max */
  0x00,
  HID_FS_BINTERVAL,          /*bInterval: Polling Interval */
  /* 34 */
  /***********************CDC********************************/
  /*Interface Descriptor */
  0x09,   /* bLength: Interface Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: Interface */
  /* Interface descriptor type */
  0x01,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x01,   /* bNumEndpoints: One endpoints used */
  0x02,   /* bInterfaceClass: Communication Interface Class */
  0x02,   /* bInterfaceSubClass: Abstract Control Model */
  0x01,   /* bInterfaceProtocol: Common AT commands */
  0x04,   /* iInterface: */

  /*Header Functional Descriptor*/
  0x05,   /* bLength: Endpoint Descriptor size */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x00,   /* bDescriptorSubtype: Header Func Desc */
  0x10,   /* bcdCDC: spec release number */
  0x01,

  /*Call Management Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x01,   /* bDescriptorSubtype: Call Management Func Desc */
  0x00,   /* bmCapabilities: D0+D1 */
  0x01,   /* bDataInterface: 1 */

  /*ACM Functional Descriptor*/
  0x04,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x02,   /* bDescriptorSubtype: Abstract Control Management desc */
  0x02,   /* bmCapabilities */

  /*Union Functional Descriptor*/
  0x05,   /* bFunctionLength */
  0x24,   /* bDescriptorType: CS_INTERFACE */
  0x06,   /* bDescriptorSubtype: Union func desc */
  0x01,   /* bMasterInterface: Communication class interface */
  0x02,   /* bSlaveInterface0: Data Class Interface */

  /*Endpoint 2 Descriptor*/
  0x07,                           /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,   /* bDescriptorType: Endpoint */
  CDC_CMD_EP,                     /* bEndpointAddress */
  0x03,                           /* bmAttributes: Interrupt */
  LOBYTE(CDC_CMD_PACKET_SIZE),     /* wMaxPacketSize: */
  HIBYTE(CDC_CMD_PACKET_SIZE),
  CDC_FS_BINTERVAL,                           /* bInterval: */
  /*---------------------------------------------------------------------------*/

  /*Data class interface descriptor*/
  0x09,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_INTERFACE,  /* bDescriptorType: */
  0x02,   /* bInterfaceNumber: Number of Interface */
  0x00,   /* bAlternateSetting: Alternate setting */
  0x02,   /* bNumEndpoints: Two endpoints used */
  0x0A,   /* bInterfaceClass: CDC */
  0x00,   /* bInterfaceSubClass: */
  0x00,   /* bInterfaceProtocol: */
  0x04,   /* iInterface: */

  /*Endpoint OUT Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_OUT_EP,                        /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
  0x00,                              /* bInterval: ignore for Bulk transfer */

  /*Endpoint IN Descriptor*/
  0x07,   /* bLength: Endpoint Descriptor size */
  USB_DESC_TYPE_ENDPOINT,      /* bDescriptorType: Endpoint */
  CDC_IN_EP,                         /* bEndpointAddress */
  0x02,                              /* bmAttributes: Bulk */
  LOBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),  /* wMaxPacketSize: */
  HIBYTE(CDC_DATA_FS_MAX_PACKET_SIZE),
  0x00,                               /* bInterval: ignore for Bulk transfer */

  /*****************************************************************************/
} ;
/*****************************************************************/
//__ALIGN_BEGIN static uint8_t USBD_HID_Desc[USB_HID_DESC_SIZ]  __ALIGN_END  =
//{
//  /* 18 */
//  0x09,         /*bLength: HID Descriptor size*/
//  HID_DESCRIPTOR_TYPE, /*bDescriptorType: HID*/
//  0x11,         /*bcdHID: HID Class Spec release number*/
//  0x01,
//  0x00,         /*bCountryCode: Hardware target country*/
//  0x01,         /*bNumDescriptors: Number of HID class descriptors to follow*/
//  0x22,         /*bDescriptorType*/
//  HID_KEYBOARD_REPORT_DESC_SIZE,/*wItemLength: Total length of Report descriptor*/
//  0x00,
//};
//
//__ALIGN_BEGIN static uint8_t HID_KEYBOARD_ReportDesc[HID_KEYBOARD_REPORT_DESC_SIZE]  __ALIGN_END =
//{
//		0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
//		0x09, 0x06,        // Usage (Keyboard)
//		0xA1, 0x01,        // Collection (Application)
//		0x85, 0x01,        //   Report ID (1)
//		0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
//		0x19, 0xE0,        //   Usage Minimum (0xE0)
//		0x29, 0xE7,        //   Usage Maximum (0xE7)
//		0x15, 0x00,        //   Logical Minimum (0)
//		0x25, 0x01,        //   Logical Maximum (1)
//		0x75, 0x01,        //   Report Size (1)
//		0x95, 0x08,        //   Report Count (8)
//		0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
//		0x75, 0x08,        //   Report Size (8)
//		0x95, 0x01,        //   Report Count (1)
//		0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
//		0x05, 0x07,        //   Usage Page (Kbrd/Keypad)
//		0x19, 0x00,        //   Usage Minimum (0x00)
//		0x29, 0x65,        //   Usage Maximum (0x65)
//		0x15, 0x00,        //   Logical Minimum (0)
//		0x25, 0x65,        //   Logical Maximum (101)
//		0x75, 0x08,        //   Report Size (8)
//		0x95, 0x05,        //   Report Count (5)
//		0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
//		0xC0,              // End Collection
//		0x05, 0x0C,        // Usage Page (Consumer)
//		0x09, 0x01,        // Usage (Consumer Control)
//		0xA1, 0x01,        // Collection (Application)
//		0x85, 0x02,        //   Report ID (2)
//		0x19, 0x00,        //   Usage Minimum (Unassigned)
//		0x2A, 0x3C, 0x02,  //   Usage Maximum (AC Format)
//		0x15, 0x00,        //   Logical Minimum (0)
//		0x26, 0x3C, 0x02,  //   Logical Maximum (572)
//		0x95, 0x01,        //   Report Count (1)
//		0x75, 0x10,        //   Report Size (16)
//		0x81, 0x00,        //   Input (Data,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
//		0xC0,              // End Collection
//		0x05, 0x01,        // Usage Page (Generic Desktop Ctrls)
//		0x09, 0x80,        // Usage (Sys Control)
//		0xA1, 0x01,        // Collection (Application)
//		0x85, 0x03,        //   Report ID (3)
//		0x19, 0x81,        //   Usage Minimum (Sys Power Down)
//		0x29, 0x83,        //   Usage Maximum (Sys Wake Up)
//		0x15, 0x00,        //   Logical Minimum (0)
//		0x25, 0x01,        //   Logical Maximum (1)
//		0x75, 0x01,        //   Report Size (1)
//		0x95, 0x03,        //   Report Count (3)
//		0x81, 0x02,        //   Input (Data,Var,Abs,No Wrap,Linear,Preferred State,No Null Position)
//		0x95, 0x05,        //   Report Count (5)
//		0x81, 0x01,        //   Input (Const,Array,Abs,No Wrap,Linear,Preferred State,No Null Position)
//		0xC0,              // End Collection
//		0x06, 0x01, 0xFF,  // Usage Page (Vendor Defined 0xFF01)
//		0x09, 0x01,        // Usage (0x01)
//		0xA1, 0x01,        // Collection (Application)
//		0x85, 0x04,        //   Report ID (4)
//		0x95, 0x01,        //   Report Count (1)
//		0x75, 0x08,        //   Report Size (8)
//		0x15, 0x01,        //   Logical Minimum (1)
//		0x25, 0x0A,        //   Logical Maximum (10)
//		0x09, 0x20,        //   Usage (0x20)
//		0xB1, 0x03,        //   Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
//		0x09, 0x23,        //   Usage (0x23)
//		0xB1, 0x03,        //   Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
//		0x25, 0x4F,        //   Logical Maximum (79)
//		0x09, 0x21,        //   Usage (0x21)
//		0xB1, 0x03,        //   Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
//		0x25, 0x30,        //   Logical Maximum (48)
//		0x09, 0x22,        //   Usage (0x22)
//		0xB1, 0x03,        //   Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
//		0x95, 0x03,        //   Report Count (3)
//		0x09, 0x24,        //   Usage (0x24)
//		0xB1, 0x03,        //   Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
//		0xC0,              // End Collection
//		0x06, 0x01, 0xFF,  // Usage Page (Vendor Defined 0xFF01)
//		0x09, 0x01,        // Usage (0x01)
//		0xA1, 0x01,        // Collection (Application)
//		0x85, 0x05,        //   Report ID (5)
//		0x95, 0x01,        //   Report Count (1)
//		0x75, 0x08,        //   Report Size (8)
//		0x15, 0x01,        //   Logical Minimum (1)
//		0x25, 0x0A,        //   Logical Maximum (10)
//		0x09, 0x20,        //   Usage (0x20)
//		0xB1, 0x03,        //   Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
//		0x09, 0x23,        //   Usage (0x23)
//		0xB1, 0x03,        //   Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
//		0x25, 0x4F,        //   Logical Maximum (79)
//		0x09, 0x21,        //   Usage (0x21)
//		0xB1, 0x03,        //   Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
//		0x25, 0x30,        //   Logical Maximum (48)
//		0x09, 0x22,        //   Usage (0x22)
//		0xB1, 0x03,        //   Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
//		0x95, 0x03,        //   Report Count (3)
//		0x09, 0x24,        //   Usage (0x24)
//		0xB1, 0x03,        //   Feature (Const,Var,Abs,No Wrap,Linear,Preferred State,No Null Position,Non-volatile)
//		0xC0,              // End Collection
//};

/*****************************************************************/
static uint8_t  USBD_Composite_Init(USBD_HandleTypeDef *pdev,
                              uint8_t cfgidx)
{
	pdev->pClassData = USBD_malloc_Comp(sizeof(USBD_Composite_HandleTypeDef)); //Cambiar despues por static alloc

	if(USBD_HID_Init(pdev, cfgidx))
		return USBD_FAIL;
	else if (USBD_CDC_Init(pdev,cfgidx))
		return USBD_FAIL;
	else
		return USBD_OK;
}

static uint8_t  USBD_Composite_DeInit(USBD_HandleTypeDef *pdev,
                                uint8_t cfgidx)
{
	USBD_CDC_DeInit(pdev,cfgidx);
	USBD_HID_DeInit(pdev, cfgidx);
	return USBD_OK;
}

static uint8_t  USBD_Composite_Setup(USBD_HandleTypeDef *pdev,
                               USBD_SetupReqTypedef *req)
{
	if(req->wIndex == 0)
		return USBD_HID_Setup(pdev, req);
	else
		return USBD_CDC_Setup(pdev,req);
//	if(USBD_HID_Setup(pdev, req))
//		return USBD_FAIL;
////	else if(USBD_CDC_Setup(pdev,req))
////		return USBD_FAIL;
//	else
//		return USBD_OK;
}

static uint8_t  USBD_Composite_DataIn(USBD_HandleTypeDef *pdev,
                                uint8_t epnum)
{
	if(epnum == (HID_EPIN_ADDR & 0x0F))
		return USBD_HID_DataIn(pdev, epnum);
	else
		return USBD_CDC_DataIn(pdev, epnum);
}

static uint8_t  USBD_Composite_DataOut(USBD_HandleTypeDef *pdev,
                                 uint8_t epnum)
{
	return USBD_CDC_DataOut(pdev, epnum);
}

static uint8_t  USBD_Composite_EP0_RxReady(USBD_HandleTypeDef *pdev)
{
	return USBD_CDC_EP0_RxReady(pdev);
}

static uint8_t  *USBD_Composite_GetFSCfgDesc(uint16_t *length)
{
	*length = sizeof(USBD_Composite_CfgFSDesc);
	return USBD_Composite_CfgFSDesc;
}

static uint8_t  *USBD_Composite_GetDeviceQualifierDescriptor(uint16_t *length)
{
	*length = sizeof(USBD_Composite_DeviceQualifierDesc);
	return USBD_Composite_DeviceQualifierDesc;
}

uint8_t  USBD_Composite_RegisterInterface(USBD_HandleTypeDef   *pdev,
									USBD_Comp_ItfTypeDef *fops)
{
  uint8_t  ret = USBD_FAIL;
  fops->HID_ops = NULL;
  if (fops != NULL)
  {
	  pdev->pUserData = fops;
    ret = USBD_OK;
  }

  return ret;
}
