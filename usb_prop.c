#include "usb_lib.h"
#include "usb_conf.h"
#include "usb_prop.h"
#include "usb_pwr.h"
#include "hw_config.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"

uint8_t Request = 0;

/* -------------------------------------------------------------------------- */
/*  Structures initializations */
/* -------------------------------------------------------------------------- */

DEVICE Device_Table =
{
	EP_NUM,
	1
};

DEVICE_PROP Device_Property =
{
    Virtual_Com_Port_init,
    Virtual_Com_Port_Reset,
    Virtual_Com_Port_Status_In,
    Virtual_Com_Port_Status_Out,
    Virtual_Com_Port_Data_Setup,
    Virtual_Com_Port_NoData_Setup,
    Virtual_Com_Port_Get_Interface_Setting,
    Virtual_Com_Port_GetDeviceDescriptor,
    Virtual_Com_Port_GetConfigDescriptor,
    Virtual_Com_Port_GetStringDescriptor,
    0,
    0x40 /*MAX PACKET SIZE*/
};

USER_STANDARD_REQUESTS User_Standard_Requests =
{
    Virtual_Com_Port_GetConfiguration,
    Virtual_Com_Port_SetConfiguration,
    Virtual_Com_Port_GetInterface,
    Virtual_Com_Port_SetInterface,
    Virtual_Com_Port_GetStatus,
    Virtual_Com_Port_ClearFeature,
    Virtual_Com_Port_SetEndPointFeature,
    Virtual_Com_Port_SetDeviceFeature,
    Virtual_Com_Port_SetDeviceAddress
};

ONE_DESCRIPTOR Device_Descriptor =
{
	(uint8_t*)Virtual_Com_Port_DeviceDescriptor,
	VIRTUAL_COM_PORT_SIZ_DEVICE_DESC
};

ONE_DESCRIPTOR Config_Descriptor =
{
    (uint8_t*)Virtual_Com_Port_ConfigDescriptor,
    VIRTUAL_COM_PORT_SIZ_CONFIG_DESC
};

ONE_DESCRIPTOR String_Descriptor[4] =
{
    {(uint8_t*)Virtual_Com_Port_StringLangID, VIRTUAL_COM_PORT_SIZ_STRING_LANGID},
    {(uint8_t*)Virtual_Com_Port_StringVendor, VIRTUAL_COM_PORT_SIZ_STRING_VENDOR},
    {(uint8_t*)Virtual_Com_Port_StringProduct, VIRTUAL_COM_PORT_SIZ_STRING_PRODUCT},
    {(uint8_t*)Virtual_Com_Port_StringSerial, VIRTUAL_COM_PORT_SIZ_STRING_SERIAL}
};

/*******************************************************************************
* Function Name  : Virtual_Com_Port_init.
* Description    : Virtual COM Port Mouse init routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Virtual_Com_Port_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;

	/* Update the serial number string descriptor with the data from the unique
	ID*/
	Get_SerialNum();

	pInformation->Current_Configuration = 0;

	/* Connect the device */
	PowerOn();

	/* Perform basic device initialization operations */
	USB_SIL_Init();

	/* configure the USART to the default settings */
	USARTx_Config_Default(&USB_USART1_Data);
	USARTx_Config_Default(&USB_USART2_Data);
	USARTx_Config_Default(&USB_USART3_Data);
	
	//configure pins for usart3
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	bDeviceState = UNCONNECTED;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Reset
* Description    : Virtual_Com_Port Mouse reset routine
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Virtual_Com_Port_Reset(void)
{
	/* Set Virtual_Com_Port DEVICE as not configured */
	pInformation->Current_Configuration = 0;

	/* Current Feature initialization */
	pInformation->Current_Feature = Virtual_Com_Port_ConfigDescriptor[7];

	/* Set Virtual_Com_Port DEVICE with the default Interface*/
	pInformation->Current_Interface = 0;

#ifdef STM32F10X_CL     
	/* EP0 is already configured by USB_SIL_Init() function */
  
	/* Init EP1 IN as Bulk endpoint */
	OTG_DEV_EP_Init(EP1_IN, OTG_DEV_EP_TYPE_BULK, VIRTUAL_COM_PORT_DATA_SIZE);
  
	/* Init EP2 IN as Interrupt endpoint */
	OTG_DEV_EP_Init(EP2_IN, OTG_DEV_EP_TYPE_INT, VIRTUAL_COM_PORT_INT_SIZE);

	/* Init EP3 OUT as Bulk endpoint */
	OTG_DEV_EP_Init(EP3_OUT, OTG_DEV_EP_TYPE_BULK, VIRTUAL_COM_PORT_DATA_SIZE);  
#else 

	SetBTABLE(BTABLE_ADDRESS);

	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_STALL);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	SetEPRxValid(ENDP0);

	//port1, endpoint 1 control
	SetEPType(ENDP1, EP_INTERRUPT);
	SetEPTxAddr(ENDP1, ENDP1_TXADDR);
	SetEPRxStatus(ENDP1, EP_RX_DIS);
	SetEPTxStatus(ENDP1, EP_TX_NAK);
	//endpoint 2 data
	SetEPType(ENDP2, EP_BULK);
	SetEPTxAddr(ENDP2, ENDP2_TXADDR);
	SetEPRxAddr(ENDP2, ENDP2_RXADDR);
	SetEPRxCount(ENDP2, VIRTUAL_COM_PORT_DATA_SIZE);
	SetEPTxStatus(ENDP2, EP_TX_NAK);
	SetEPRxStatus(ENDP2, EP_RX_VALID);

	//port2, endpoint 3 control
	SetEPType(ENDP3, EP_INTERRUPT);
	SetEPTxAddr(ENDP3, ENDP3_TXADDR);
	SetEPRxStatus(ENDP3, EP_RX_DIS);
	SetEPTxStatus(ENDP3, EP_TX_NAK);
	//endpoint 4 data
	SetEPType(ENDP4, EP_BULK);
	SetEPTxAddr(ENDP4, ENDP4_TXADDR);
	SetEPRxAddr(ENDP4, ENDP4_RXADDR);
	SetEPRxCount(ENDP4, VIRTUAL_COM_PORT_DATA_SIZE);
	SetEPTxStatus(ENDP4, EP_TX_NAK);
	SetEPRxStatus(ENDP4, EP_RX_VALID);
	
	//port3, endpoint 5 control
	SetEPType(ENDP5, EP_INTERRUPT);
	SetEPTxAddr(ENDP5, ENDP5_TXADDR);
	SetEPRxStatus(ENDP5, EP_RX_DIS);
	SetEPTxStatus(ENDP5, EP_TX_NAK);
	//endpoint 6 data
	SetEPType(ENDP6, EP_BULK);
	SetEPTxAddr(ENDP6, ENDP6_TXADDR);
	SetEPRxAddr(ENDP6, ENDP6_RXADDR);
	SetEPRxCount(ENDP6, VIRTUAL_COM_PORT_DATA_SIZE);
	SetEPTxStatus(ENDP6, EP_TX_NAK);
	SetEPRxStatus(ENDP6, EP_RX_VALID);


	/* Set this device to response on default address */
	SetDeviceAddress(0);

#endif /* STM32F10X_CL */

	bDeviceState = ATTACHED;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_SetConfiguration.
* Description    : Udpade the device state to configured.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Virtual_Com_Port_SetConfiguration(void)
{
  DEVICE_INFO *pInfo = &Device_Info;

  if (pInfo->Current_Configuration != 0)
  {
    /* Device configured */
    bDeviceState = CONFIGURED;
  }
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_SetConfiguration.
* Description    : Udpade the device state to addressed.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Virtual_Com_Port_SetDeviceAddress (void)
{
  bDeviceState = ADDRESSED;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Status_In.
* Description    : Virtual COM Port Status In Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Virtual_Com_Port_Status_In(void)
{
	if (Request == SET_LINE_CODING)
	{
		if (pInformation->USBwIndex0 == 0)
			USARTx_Config(&USB_USART1_Data);
		else if (pInformation->USBwIndex0 == 2)
			USARTx_Config(&USB_USART2_Data);
		else if (pInformation->USBwIndex0 == 4)
			USARTx_Config(&USB_USART3_Data);
		Request = 0;
	}
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Status_Out
* Description    : Virtual COM Port Status OUT Routine.
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void Virtual_Com_Port_Status_Out(void)
{}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Data_Setup
* Description    : handle the data class specific requests
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Virtual_Com_Port_Data_Setup(uint8_t RequestNo)
{
	uint8_t    *(*CopyRoutine)(uint16_t);

	CopyRoutine = NULL;

	if (RequestNo == GET_LINE_CODING)
	{
		if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
		{
			if (pInformation->USBwIndex0 == 0)
				CopyRoutine = Virtual_Com_Port_GetLineCoding1;
			else if (pInformation->USBwIndex0 == 2)
				CopyRoutine = Virtual_Com_Port_GetLineCoding2;
			else if (pInformation->USBwIndex0 == 4)
				CopyRoutine = Virtual_Com_Port_GetLineCoding3;
		}
	}
	else if (RequestNo == SET_LINE_CODING)
	{
		if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
		{
			if(pInformation->USBwIndex0 == 0)
				CopyRoutine = Virtual_Com_Port_SetLineCoding1;
			else if(pInformation->USBwIndex0 == 2)
				CopyRoutine = Virtual_Com_Port_SetLineCoding2;
			else if(pInformation->USBwIndex0 == 4)
				CopyRoutine = Virtual_Com_Port_SetLineCoding3;
		}
		Request = SET_LINE_CODING;
	}

	if (CopyRoutine == NULL)
	{
		return USB_UNSUPPORT;
	}

	pInformation->Ctrl_Info.CopyData = CopyRoutine;
	pInformation->Ctrl_Info.Usb_wOffset = 0;
	(*CopyRoutine)(0);
	return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_NoData_Setup.
* Description    : handle the no data class specific requests.
* Input          : Request Nb.
* Output         : None.
* Return         : USB_UNSUPPORT or USB_SUCCESS.
*******************************************************************************/
RESULT Virtual_Com_Port_NoData_Setup(uint8_t RequestNo)
{

  if (Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT))
  {
    if (RequestNo == SET_COMM_FEATURE)
    {
      return USB_SUCCESS;
    }
    else if (RequestNo == SET_CONTROL_LINE_STATE)
    {
      return USB_SUCCESS;
    }
  }

  return USB_UNSUPPORT;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetDeviceDescriptor.
* Description    : Gets the device descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the device descriptor.
*******************************************************************************/
uint8_t *Virtual_Com_Port_GetDeviceDescriptor(uint16_t Length)
{
  return Standard_GetDescriptorData(Length, &Device_Descriptor);
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetConfigDescriptor.
* Description    : get the configuration descriptor.
* Input          : Length.
* Output         : None.
* Return         : The address of the configuration descriptor.
*******************************************************************************/
uint8_t *Virtual_Com_Port_GetConfigDescriptor(uint16_t Length)
{
	return Standard_GetDescriptorData(Length, &Config_Descriptor);
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetStringDescriptor
* Description    : Gets the string descriptors according to the needed index
* Input          : Length.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
uint8_t *Virtual_Com_Port_GetStringDescriptor(uint16_t Length)
{
	uint8_t wValue0 = pInformation->USBwValue0;
	if (wValue0 > 7)
	{
		return NULL;
	}
	else
	{
		return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
	}
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_Get_Interface_Setting.
* Description    : test the interface and the alternate setting according to the
*                  supported one.
* Input1         : uint8_t: Interface : interface number.
* Input2         : uint8_t: AlternateSetting : Alternate Setting number.
* Output         : None.
* Return         : The address of the string descriptors.
*******************************************************************************/
RESULT Virtual_Com_Port_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
	if (AlternateSetting > 0)
	{
		return USB_UNSUPPORT;
	}
	else if (Interface > 7)
	{
		return USB_UNSUPPORT;
	}
	return USB_SUCCESS;
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_GetLineCoding.
* Description    : send the linecoding structure to the PC host.
* Input          : Length.
* Output         : None.
* Return         : Inecoding structure base address.
*******************************************************************************/
uint8_t *Virtual_Com_Port_GetLineCoding1(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = sizeof(USB_USART1_Data.lineCoding);
		return NULL;
	}
	return (uint8_t *)&(USB_USART1_Data.lineCoding);
}

uint8_t *Virtual_Com_Port_GetLineCoding2(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = sizeof(USB_USART2_Data.lineCoding);
		return NULL;
	}
	return (uint8_t *)&(USB_USART2_Data.lineCoding);
}

uint8_t *Virtual_Com_Port_GetLineCoding3(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = sizeof(USB_USART3_Data.lineCoding);
		return NULL;
	}
	return (uint8_t *)&(USB_USART3_Data.lineCoding);
}

/*******************************************************************************
* Function Name  : Virtual_Com_Port_SetLineCoding.
* Description    : Set the linecoding structure fields.
* Input          : Length.
* Output         : None.
* Return         : Linecoding structure base address.
*******************************************************************************/
uint8_t *Virtual_Com_Port_SetLineCoding1(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = sizeof(USB_USART1_Data.lineCoding);
		return NULL;
	}
	return (uint8_t *)&(USB_USART1_Data.lineCoding);
}

uint8_t *Virtual_Com_Port_SetLineCoding2(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = sizeof(USB_USART2_Data.lineCoding);
		return NULL;
	}
	return (uint8_t *)&(USB_USART2_Data.lineCoding);
}

uint8_t *Virtual_Com_Port_SetLineCoding3(uint16_t Length)
{
	if (Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = sizeof(USB_USART3_Data.lineCoding);
		return NULL;
	}
	return (uint8_t *)&(USB_USART3_Data.lineCoding);
}

