/*
 * service.c
 *
 *  Created on: Jul 10, 2022
 *      Author: sobro
 */
#include <stdio.h>
#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_aci.h"
#include "service.h"

uint8_t SERVICE_UUID[16] = {0x51,0x9D,0xDB,0x0B,0x40,0xAD,0x07,0x55,0x03,0x0A,0x3C,0x33,0x74,0x06,0x03,0x2F};
uint8_t CHAR_UUID[16]    = {0x51,0x9D,0xDB,0x0B,0x40,0xAD,0x07,0x55,0x03,0x0A,0x3C,0x33,0x75,0x06,0x03,0x2F};


uint16_t my_service_handle,my_char_handle;

uint8_t connected = FALSE;
uint8_t set_connectable = TRUE;
uint16_t connection_handle = FALSE;
uint8_t notification_enabled = FALSE;

tBleStatus add_simple_service(void)
{
	tBleStatus ret;
	Service_UUID_t service_uuid;
	Char_UUID_t char_uuid;

	BLUENRG_memcpy(service_uuid.Service_UUID_128,SERVICE_UUID,16);

	/*Add service*/

	/*tBleStatus aci_gatt_add_service(uint8_t Service_UUID_Type,
                                Service_UUID_t *Service_UUID,
                                uint8_t Service_Type,
                                uint8_t Max_Attribute_Records,
                                uint16_t *Service_Handle);*/

	ret = aci_gatt_add_service(UUID_TYPE_128,&service_uuid,PRIMARY_SERVICE,7,&my_service_handle);

	BLUENRG_memcpy(char_uuid.Char_UUID_128,CHAR_UUID,16);

	/* Add Characteristic */
	/* tBleStatus aci_gatt_add_char(uint16_t Service_Handle,
                             uint8_t Char_UUID_Type,
                             Char_UUID_t *Char_UUID,
                             uint16_t Char_Value_Length,
                             uint8_t Char_Properties,
                             uint8_t Security_Permissions,
                             uint8_t GATT_Evt_Mask,
                             uint8_t Enc_Key_Size,
                             uint8_t Is_Variable,
                             uint16_t *Char_Handle); */
	ret = aci_gatt_add_char(my_service_handle, UUID_TYPE_128, &char_uuid, 2, CHAR_PROP_READ, ATTR_PERMISSION_NONE, GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP, 0, 0, &my_char_handle);


	return ret;

}

void update_data(uint16_t new_data)
{
	tBleStatus ret;

	/* Update characteristic value */
	/* tBleStatus aci_gatt_update_char_value(uint16_t Service_Handle,
                                      uint16_t Char_Handle,
                                      uint8_t Val_Offset,
                                      uint8_t Char_Value_Length,
                                      uint8_t Char_Value[]); */
	ret = aci_gatt_update_char_value(my_service_handle,my_char_handle, 0, 2, (uint8_t *)&new_data);

	if(ret != BLE_STATUS_SUCCESS)
	{
		printf("aci_gatt_update_char_value : FAILED !! \n\r");
	}


}
/*old deprecated version of callbacks*/
/*void GAP_ConnectionComplete_CB(uint8_t addr[6],uint16_t handle)
{
	connected = TRUE;
	connection_handle = handle;

	printf("Connection complete...\n\r");
}

void GAP_DisconnectionComplete_CB(void)
{
	printf("Disconnection complete...\n\r");
}*/

void Read_Request_CB(uint16_t handle)
{
	int data;
	data = 450; /* Sent as 0xC201 the bytes are reversed, 0x01C2 = 450 */

	update_data(data);

	if(connection_handle != FALSE)
	{
		aci_gatt_allow_read(connection_handle);
	}

}

void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
									uint16_t Attribute_Handle,
									uint16_t Offset)
{
	Read_Request_CB(Attribute_Handle);
}

void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)
{
	connected = TRUE;
	connection_handle = Connection_Handle;
}

void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
	connected = FALSE;
	set_connectable = TRUE;
	connection_handle = FALSE;

	printf("Disconnected\n\r");

}

void APP_UserEvtRx(void *pData)
{
	uint32_t i;

	hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;

	/* Process event packet*/
	if(hci_pckt->type == HCI_EVENT_PKT)
	{
		/* Get data from packet */
		hci_event_pckt *event_pckt = (hci_event_pckt*)hci_pckt->data;

		/* Process meta data event */
		if(event_pckt->evt == EVT_LE_META_EVENT)
		{
			/* Get meta data */
			evt_le_meta_event *evt = (void *) event_pckt->data;

			/* Process each meta data event */
			for(i = 0; i < (sizeof(hci_le_meta_events_table)/sizeof(hci_le_meta_events_table_type)); i++)
			{
				if(evt->subevent == hci_le_meta_events_table[i].evt_code)
				{
					hci_le_meta_events_table[i].process((void *)evt->data);
				}

			}
		}

		/* Process vendor event */
		else if(event_pckt->evt == EVT_VENDOR)
		{
			/* gGet data from packet */
			evt_blue_aci *blue_evt = (void*)event_pckt->data;

			/* Process each vendor event*/
			for(i = 0; i < (sizeof(hci_vendor_specific_events_table)/sizeof(hci_vendor_specific_events_table_type)); i++)
			{
				if(blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code)
				{
					hci_vendor_specific_events_table[i].process((void *)blue_evt->data);
				}
			}

		}

		/* Process other events */
		else
		{
			for(i = 0; i < (sizeof(hci_events_table)/sizeof(hci_events_table_type)); i++)
			{
				if(event_pckt->evt == hci_events_table[i].evt_code)
				{
					hci_events_table[i].process((void *)event_pckt->data);
				}
			}
		}

	}
}


