#ifndef MICRO_EPM_COMMANDS_H
#define MICRO_EPM_COMMANDS_H
// Copyright (c) Qualcomm Technologies, Inc. and/or its subsidiaries.
// SPDX-License-Identifier: BSD-3-Clause

/*
	Author: Michael Simpson {msimpson@qti.qualcomm.com};
*/

#include <qglobal.h>

/**********************************************
 *					  COMMANDS				 *
 **********************************************/

const quint8 NULL_CMD							{0x00};
const quint8 HELLO_CMD							{0x01};
const quint8 HELLO_RSP							{0x02};
const quint8 ENTER_BOOTLOADER_CMD				{0x03};
const quint8 ENTER_BOOTLOADER_RSP				{0x04};
const quint8 ENABLE_DISABLE_CHANNEL_CMD			{0x05};
const quint8 ENABLE_DISABLE_CHANNEL_RSP			{0x06};
const quint8 SET_AVERAGING_CMD					{0x07};
const quint8 SET_AVERAGING_RSP					{0x08};
const quint8 GET_LAST_CHANNEL_MEASUREMENT_CMD 	{0x09};
const quint8 GET_LAST_CHANNEL_MEASUREMENT_RSP 	{0x0A};
const quint8 GET_BUFFERED_DATA_CMD				{0x0B};
const quint8 GET_BUFFERED_DATA_RSP				{0x0C};
const quint8 GET_GPIO_VALUE_CMD					{0x0D};
const quint8 GET_GPIO_VALUE_RSP					{0x0E};
const quint8 SET_GPIO_VALUE_CMD					{0x0F};
const quint8 SET_GPIO_VALUE_RSP					{0x10};
const quint8 GET_SYSTEM_TIMESTAMP_CMD			{0x11};
const quint8 GET_SYSTEM_TIMESTAMP_RSP			{0x12};
const quint8 SET_SYSTEM_TIMESTAMP_CMD			{0x13};
const quint8 SET_SYSTEM_TIMESTAMP_RSP			{0x14};
const quint8 SET_CHANNEL_TYPES_CMD				{0x15};
const quint8 SET_CHANNEL_TYPES_RSP				{0x16};
const quint8 LOOPBACK_REQUEST					{0x17};
const quint8 LOOPBACK_RESPONSE					{0x18};
const quint8 GET_AVERAGED_BUFFERED_DATA_CMD		{0x19};
const quint8 GET_AVERAGED_BUFFERED_DATA_RSP		{0x1A};
const quint8 SET_CHANNEL_SWITCH_DELAY_CMD	 	{0x1B};
const quint8 SET_CHANNEL_SWITCH_DELAY_RSP		{0x1C};
const quint8 CLEAR_BUFFER_CMD					{0x1D};
const quint8 CLEAR_BUFFER_RSP					{0x1E};
const quint8 SET_VDAC_REFERENCE_VOLTAGE_CMD		{0x1F};
const quint8 SET_VDAC_REFERENCE_VOLTAGE_RSP		{0x20};
const quint8 ENABLE_DISABLE_FAST_MODE_CMD	 	{0x21};
const quint8 ENABLE_DISABLE_FAST_MODE_RSP	 	{0x22};
const quint8 EEPROM_READ_CMD					{0x23};
const quint8 EEPROM_READ_RSP					{0x24};
const quint8 EEPROM_WRITE_CMD					{0x25};
const quint8 EEPROM_WRITE_RSP					{0x26};
const quint8 MEMORY_READ_CMD					{0x27};
const quint8 MEMORY_READ_RSP					{0x28};
const quint8 MEMORY_WRITE_CMD					{0x29};
const quint8 MEMORY_WRITE_RSP					{0x2A};
const quint8 RECALIBRATE_CMD					{0x2B};
const quint8 RECALIBRATE_RSP					{0x2C};
const quint8 GET_CALIBRATION_DATA_CMD			{0x2D};
const quint8 GET_CALIBRATION_DATA_RSP			{0x2E};
const quint8 GET_DELTA_SIGMA_GAIN_CMD			{0x2F};
const quint8 GET_DELTA_SIGMA_GAIN_RSP			{0x30};
const quint8 ADJUST_DELTA_SIGMA_GAIN_CMD		{0x31};
const quint8 ADJUST_DELTA_SIGMA_GAIN_RSP		{0x32};
const quint8 SELECT_DELTA_SIGMA_CONFIG_CMD		{0x33};
const quint8 SELECT_DELTA_SIGMA_CONFIG_RSP		{0x34};
const quint8 PAUSE_ADC_CONVERSIONS_CMD			{0x35};
const quint8 PAUSE_ADC_CONVERSIONS_RSP			{0x36};
const quint8 UNPAUSE_ADC_CONVERSIONS_CMD		{0x37};
const quint8 UNPAUSE_ADC_CONVERSIONS_RSP		{0x38};
const quint8 SET_GPIO_DIRECTION_CMD				{0x39};
const quint8 SET_GPIO_DIRECTION_RSP				{0x3A};
const quint8 SOFTWARE_RESET_CMD					{0x3B};
const quint8 SOFTWARE_RESET_RSP					{0x0C};
const quint8 SET_GPIO_DRIVE_CMD					{0x0D};
const quint8 SET_GPIO_DRIVE_RSP					{0x0E};
const quint8 GET_GPIO_DRIVE_CMD					{0x0F};
const quint8 GET_GPIO_DRIVE_RSP					{0x40};
const quint8 ENTER_SECURE_MODE_CMD				{0x41};
const quint8 ENTER_SECURE_MODE_RSP				{0x42};
const quint8 GOODBYE_CMD						{0x43};
const quint8 GOODBYE_RSP						{0x44};
const quint8 GET_DELTA_SIGMA_OFFSET_CMD			{0x45};
const quint8 GET_DELTA_SIGMA_OFFSET_RSP			{0x46};
const quint8 SET_DELTA_SIGMA_OFFSET_CMD			{0x47};
const quint8 SET_DELTA_SIGMA_OFFSET_RSP			{0x48};
const quint8 GET_LATEST_AVERAGING_PACKET_CMD	{0x49};
const quint8 GET_LATEST_AVERAGING_PACKET_RSP	{0x4A};
const quint8 MEMORY_READ_BYTES_CMD				{0x4B};
const quint8 MEMORY_READ_BYTES_RSP				{0x4C};
const quint8 MEMORY_WRITE_BYTES_CMD				{0x4D};
const quint8 MEMORY_WRITE_BYTES_RSP				{0x4E};
const quint8 SET_GPIO_BUFFER_STATUS_CMD			{0x4F};
const quint8 SET_GPIO_BUFFER_STATUS_RSP			{0x50};
const quint8 GET_GPIO_BUFFER_CMD				{0x51};
const quint8 GET_GPIO_BUFFER_RSP				{0x52};
const quint8 GET_16BIT_AVERAGED_BUFFERED_CMD	{0x53};
const quint8 GET_16BIT_AVERAGED_BUFFERED_RSP	{0x54};
const quint8 SET_CONVERSION_TIME_CMD			{0x55};
const quint8 SET_CONVERSION_TIME_RSP			{0x56};
const quint8 SET_ADC_MODE_CMD					{0x57};
const quint8 SET_ADC_MODE_RSP					{0x58};
const quint8 SET_SET_PERIOD_CMD					{0x59};
const quint8 SET_SET_PERIOD_RSP					{0x5A};
const quint8 APPLY_SETTINGS_CMD					{0x5B};
const quint8 APPLY_SETTINGS_RSP					{0x5C};
const quint8 INA231_REGISTER_READ_CMD			{0x5D};
const quint8 INA231_REGISTER_READ_RSP			{0x5E};
const quint8 INA231_REGISTER_WRITE_CMD			{0x5F};
const quint8 INA231_REGISTER_WRITE_RSP			{0x60};
const quint8 SET_DATA_RATE_GOVERNOR_CMD			{0x61};
const quint8 SET_DATA_RATE_GOVERNOR_RSP			{0x62};
const quint8 POWER_ON_TEST_CMD					{0x63};
const quint8 POWER_ON_TEST_RSP					{0x64};
const quint8 EPM_TAC_CMD						{0x65};
const quint8 EPM_TAC_RSP						{0x66};
const quint8 GET_EPM_ID_CMD						{0X67};
const quint8 GET_EPM_ID_RSP						{0X68};
const quint8 CTI_CONTROL_CMD					{0X69};
const quint8 CTI_CONTROL_RSP					{0X6A};

/**********************************************
 *				  PACKET LENGTHS					 *
 **********************************************/

const quint8 HELLO_CMD_LENGTH							{0x04};
const quint8 HELLO_RSP_LENGTH							{0x09};
const quint8 ENTER_BOOTLOADER_CMD_LENGTH				{0x09};
const quint8 ENTER_BOOTLOADER_RSP_LENGTH				{0x02};
const quint8 ENABLE_DISABLE_CHANNEL_CMD_LENGTH			{0x06};
const quint8 ENABLE_DISABLE_CHANNEL_RSP_LENGTH			{0x06};
const quint8 SET_AVERAGING_CMD_LENGTH					{0x12};
const quint8 SET_AVERAGING_RSP_LENGTH					{0x02};
const quint8 GET_LAST_CHANNEL_MEASUREMENT_CMD_LENGTH	{0x03};
const quint8 GET_LAST_CHANNEL_MEASUREMENT_RSP_LENGTH	{0x09};
const quint8 GET_BUFFERED_DATA_CMD_LENGTH				{0x01};
const quint8 GET_BUFFERED_DATA_RSP_LENGTH				{0x40};
const quint8 GET_GPIO_VALUE_CMD_LENGTH					{0x02};
const quint8 GET_GPIO_VALUE_RSP_LENGTH					{0x03};
const quint8 SET_GPIO_VALUE_CMD_LENGTH					{0x03};
const quint8 SET_GPIO_VALUE_RSP_LENGTH					{0x03};
const quint8 GET_SYSTEM_TIMESTAMP_CMD_LENGTH			{0x01};
const quint8 GET_SYSTEM_TIMESTAMP_RSP_LENGTH			{0x05};
const quint8 SET_SYSTEM_TIMESTAMP_CMD_LENGTH			{0x05};
const quint8 SET_SYSTEM_TIMESTAMP_RSP_LENGTH			{0x05};
const quint8 SET_CHANNEL_TYPES_CMD_LENGTH				{0x06};
const quint8 SET_CHANNEL_TYPES_RSP_LENGTH				{0x06};
const quint8 LOOPBACK_REQUEST_LENGTH					{0x40};
const quint8 LOOPBACK_RESPONSE_LENGTH					{0x40};
const quint8 GET_AVERAGED_BUFFERED_DATA_CMD_LENGTH		{0x01};
const quint8 GET_AVERAGED_BUFFERED_DATA_RSP_LENGTH		{0x40};
const quint8 SET_CHANNEL_SWITCH_DELAY_CMD_LENGTH		{0x06};
const quint8 SET_CHANNEL_SWITCH_DELAY_RSP_LENGTH		{0x06};
const quint8 CLEAR_BUFFER_CMD_LENGTH					{0x02};
const quint8 CLEAR_BUFFER_RSP_LENGTH					{0x02};
const quint8 SET_VDAC_REFERENCE_VOLTAGE_CMD_LENGTH		{0x06};
const quint8 SET_VDAC_REFERENCE_VOLTAGE_RSP_LENGTH		{0x06};
const quint8 ENABLE_DISABLE_FAST_MODE_CMD_LENGTH		{0x02};
const quint8 ENABLE_DISABLE_FAST_MODE_RSP_LENGTH		{0x02};
const quint8 EEPROM_READ_CMD_LENGTH						{0x02};
const quint8 EEPROM_READ_RSP_LENGTH						{0x13};
const quint8 EEPROM_WRITE_CMD_LENGTH					{0x12};
const quint8 EEPROM_WRITE_RSP_LENGTH					{0x13};
const quint8 MEMORY_READ_CMD_LENGTH						{0x06};
const quint8 MEMORY_READ_RSP_LENGTH						{0x0A};
const quint8 MEMORY_WRITE_CMD_LENGTH					{0x14};
const quint8 MEMORY_WRITE_RSP_LENGTH					{0x0A};
const quint8 RECALIBRATE_CMD_LENGTH						{0x02};
const quint8 RECALIBRATE_RSP_LENGTH						{0x02};
const quint8 GET_CALIBRATION_DATA_CMD_LENGTH			{0x06};
const quint8 GET_CALIBRATION_DATA_RSP_LENGTH			{0x15};
const quint8 GET_DELTA_SIGMA_GAIN_CMD_LENGTH			{0x01};
const quint8 GET_DELTA_SIGMA_GAIN_RSP_LENGTH			{0x03};
const quint8 ADJUST_DELTA_SIGMA_GAIN_CMD_LENGTH			{0x05};
const quint8 ADJUST_DELTA_SIGMA_GAIN_RSP_LENGTH			{0x04};
const quint8 SELECT_DELTA_SIGMA_CONFIG_CMD_LENGTH		{0x02};
const quint8 SELECT_DELTA_SIGMA_CONFIG_RSP_LENGTH		{0x02};
const quint8 PAUSE_ADC_CONVERSIONS_CMD_LENGTH			{0x01};
const quint8 PAUSE_ADC_CONVERSIONS_RSP_LENGTH			{0x01};
const quint8 UNPAUSE_ADC_CONVERSIONS_CMD_LENGTH			{0x01};
const quint8 UNPAUSE_ADC_CONVERSIONS_RSP_LENGTH			{0x01};
const quint8 SET_GPIO_DIRECTION_CMD_LENGTH				{0x03};
const quint8 SET_GPIO_DIRECTION_RSP_LENGTH				{0x03};
const quint8 SOFTWARE_RESET_CMD_LENGTH					{0x0E};
const quint8 SOFTWARE_RESET_RSP_LENGTH					{0x02};
const quint8 SET_GPIO_DRIVE_CMD_LENGTH					{0x03};
const quint8 SET_GPIO_DRIVE_RSP_LENGTH					{0x03};
const quint8 GET_GPIO_DRIVE_CMD_LENGTH					{0x02};
const quint8 GET_GPIO_DRIVE_RSP_LENGTH					{0x03};
const quint8 ENTER_SECURE_MODE_CMD_LENGTH				{0x0B};
const quint8 ENTER_SECURE_MODE_RSP_LENGTH				{0x02};
const quint8 GOODBYE_CMD_LENGTH							{0x02};
const quint8 GOODBYE_RSP_LENGTH							{0x02};
const quint8 GET_DELTA_SIGMA_OFFSET_CMD_LENGTH			{0x01};
const quint8 GET_DELTA_SIGMA_OFFSET_RSP_LENGTH			{0x05};
const quint8 SET_DELTA_SIGMA_OFFSET_CMD_LENGTH			{0x05};
const quint8 SET_DELTA_SIGMA_OFFSET_RSP_LENGTH			{0x05};
const quint8 GET_LATEST_AVERAGING_PACKET_CMD_LENGTH		{0x01};
const quint8 GET_LATEST_AVERAGING_PACKET_RSP_LENGTH		{0x40};
const quint8 MEMORY_READ_BYTES_CMD_LENGTH				{0x40};
const quint8 MEMORY_READ_BYTES_RSP_LENGTH				{0x40};
const quint8 MEMORY_WRITE_BYTES_CMD_LENGTH				{0x40};
const quint8 MEMORY_WRITE_BYTES_RSP_LENGTH				{0x40};
const quint8 SET_GPIO_BUFFER_STATUS_CMD_LENGTH			{0x02};
const quint8 SET_GPIO_BUFFER_STATUS_RSP_LENGTH			{0x02};
const quint8 GET_GPIO_BUFFER_CMD_LENGTH					{0x01};
const quint8 GET_GPIO_BUFFER_RSP_LENGTH					{0x07};
const quint8 GET_16BIT_AVERAGED_BUFFERED_DATA_CMD_LENGTH{0x01};
const quint8 GET_16BIT_AVERAGED_BUFFERED_DATA_RSP_LENGTH{0x40};
const quint8 SET_CONVERSION_TIME_CMD_LENGTH				{0x22};
const quint8 SET_CONVERSION_TIME_RSP_LENGTH				{0x02};
const quint8 SET_ADC_MODE_CMD_LENGTH					{0x02};
const quint8 SET_ADC_MODE_RSP_LENGTH					{0x02};
const quint8 SET_SET_PERIOD_CMD_LENGTH					{0x05};
const quint8 SET_SET_PERIOD_RSP_LENGTH					{0x02};
const quint8 APPLY_SETTINGS_CMD_LENGTH					{0x01};
const quint8 APPLY_SETTINGS_RSP_LENGTH					{0x02};
const quint8 INA231_REGISTER_READ_CMD_LENGTH			{0x04};
const quint8 INA231_REGISTER_READ_RSP_LENGTH			{0x04};
const quint8 INA231_REGISTER_WRITE_CMD_LENGTH			{0x06};
const quint8 INA231_REGISTER_WRITE_RSP_LENGTH			{0x02};
const quint8 SET_DATA_RATE_GOVERNOR_CMD_LENGTH			{0x05};
const quint8 SET_DATA_RATE_GOVERNOR_RSP_LENGTH			{0x02};
const quint8 POWER_ON_TEST_CMD_LENGTH					{0x0B};
const quint8 POWER_ON_TEST_RSP_LENGTH					{0x0A};
const quint8 EPM_TAC_CMD_LENGTH							{0x40};
const quint8 EPM_TAC_RSP_LENGTH							{0x40};
const quint8 GET_EPM_ID_CMD_LENGTH						{0x01};
const quint8 GET_EPM_ID_RSP_LENGTH						{0x03};
const quint8 CTI_CONTROL_CMD_LENGTH						{0x02};
const quint8 CTI_CONTROL_RSP_LENGTH						{0x02};

const quint8 EPM_PACKET_BUFFER_SUCCESS {0};
const quint8 EPM_PACKET_BUFFER_OVERFLOW {1};
const quint8 EPM_PACKET_BUFFER_EMPTY {2};

/**********************************************
 *				  Status mask bits				  *
 **********************************************/
const quint8 EPM_HELLO_STATUS_USB_MASK		{0x01};
const quint8 EPM_HELLO_STATUS_USB_SHIFT		{0x00};
const quint8 EPM_HELLO_STATUS_SPI_MASK		{0x02};
const quint8 EPM_HELLO_STATUS_SPI_SHIFT		{0x01};
const quint8 EPM_HELLO_STATUS_STATUS_MASK	{0xf0};
const quint8 EPM_HELLO_STATUS_STATUS_SHIFT	{0x04};

/**********************************************
 * Packet Size and Protocol version			  *
 **********************************************/
const quint8 MICRO_EPM_PROTOCOL_VERSION							{0x06};
const quint8 MICRO_EPM_LOWEST_COMPATIBLE_PROTOCOL_VERSION		{0x01};
const quint8 MICRO_EPM_LOWEST_COMPATIBLE_PROTOCOL_VERSION_SPMV3	{0x01};
const quint8 MICRO_EPM_LOWEST_COMPATIBLE_PROTOCOL_VERSION_SPMV4	{0x05};

#endif /* #ifndef MICRO_EPM_COMMANDS_H */
