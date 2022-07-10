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



uint16_t my_service_handle;
tBleStatus add_simple_service(void)
{
	tBleStatus ret;
	Service_UUID_t service_uuid;

	BLUENRG_memcpy(service_uuid.Service_UUID_128,SERVICE_UUID,16);

	/*Add service*/

	/*tBleStatus aci_gatt_add_service(uint8_t Service_UUID_Type,
                                Service_UUID_t *Service_UUID,
                                uint8_t Service_Type,
                                uint8_t Max_Attribute_Records,
                                uint16_t *Service_Handle);*/

	ret = aci_gatt_add_service(UUID_TYPE_128,&service_uuid,PRIMARY_SERVICE,7,&my_service_handle);

	return ret;

}
