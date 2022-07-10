/*
 * service.c
 *
 *  Created on: Jul 10, 2022
 *      Author: sobro
 */
#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_aci.h"
#include "service.h"

uint8_t SERVICE_UUID[16] = {0x51,0x9D,0xDB,0x0B,0x40,0xAD,0x07,0x55,0x03,0x0A,0x3C,0x33,0x74,0x06,0x03,0x2F};
uint8_t CHAR_UUID[16]    = {0x51,0x9D,0xDB,0x0B,0x40,0xAD,0x07,0x55,0x03,0x0A,0x3C,0x33,0x75,0x06,0x03,0x2F};


uint16_t my_service_handle,my_char_handle;
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
	ret = aci_gatt_add_char(my_service_handle, UUID_TYPE_128, &char_uuid,1, CHAR_PROP_NOTIFY, ATTR_PERMISSION_NONE, 0, 0, 0, &my_char_handle);


	return ret;

}
