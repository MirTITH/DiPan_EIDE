/**
 * @file uart_com.c
 * @author TITH (1023515576@qq.com)
 * @brief 串口数据包通讯
 * @version 0.2
 * @date 2022-07-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "uart_com.h"
#include "usart.h"
#include "cmsis_os.h"
#include "crc.h"
#include <stdbool.h>
#include <string.h>

#ifdef UC_DEBUG
#include "uart_device.h"
#endif // UC_DEBUG

#define RESERVEDATA_SIZE (sizeof(UC_Data_t) % 4)

// 数据包
typedef struct __attribute__((packed))
{
	// 信息区
	uint8_t _ID;	 // ID
	uint8_t _ID_CRC; // ID 校验值

	// 数据区
	UC_Data_t effective_data;				// 有效数据区
	uint8_t _RESERVEDATA[RESERVEDATA_SIZE]; // 保留数据区，用于将要发送的数据补全到 32 bit 的整数倍（CRC32要求数据为32的整数倍）

	// 数据校验区
	uint32_t _DATA_CRC; // 数据区校验值
} Data_Packet_t;

#define INFO_REGION_SIZE (sizeof(((Data_Packet_t *)0)->_ID) + sizeof(((Data_Packet_t *)0)->_ID_CRC)) // 信息区大小
#define DATA_REGION_SIZE (sizeof(UC_Data_t) + RESERVEDATA_SIZE)										 // 数据区大小
#define DATA_CRC_REGION_SIZE sizeof(((Data_Packet_t *)0)->_DATA_CRC)								 // 数据校验区大小

#define FRAME_HEAD_DEFINE {0xAA, 0x93}
#define FRAME_TAIL_DEFINE {0xBB, 0x00}

const uint8_t FRAME_HEAD[] = FRAME_HEAD_DEFINE; // 帧头
const uint8_t FRAME_TAIL[] = FRAME_TAIL_DEFINE; // 帧尾

// 数据帧
typedef struct __attribute__((packed))
{
	uint8_t frame_head[sizeof(FRAME_HEAD)];
	Data_Packet_t data;
	uint8_t frame_tail[sizeof(FRAME_TAIL)];
} Frame_t;

// 数据帧收发相关的变量
struct
{
	Frame_t tx_buffer;

	UART_HandleTypeDef *rx_huart;		   // 接收数据的串口
	UC_Data_t *rx_dest_data;			   // 接收数据目的地
	Frame_t rx_buffer;					   // 接收缓冲
	uint8_t rx_exp_info[INFO_REGION_SIZE]; // 期望接收的信息区
	bool rx_head_aligned;				   // 接收帧头是否对齐
} UC_Var = {
	.tx_buffer.frame_head = FRAME_HEAD_DEFINE,
	.tx_buffer.frame_tail = FRAME_TAIL_DEFINE};


UC_Stat_t UC_debug_data = {0}; // debug 时用的数据

uint8_t crc8(uint8_t *data, uint16_t length);

/**
 * @brief 检查数据包校验是否通过
 *
 * @param packet
 * @return true 通过
 * @return false 不通过
 */
bool DataPacket_Check(Data_Packet_t *packet)
{
	return HAL_CRC_Calculate(&hcrc, (uint32_t *)(((uint8_t *)packet) + INFO_REGION_SIZE), (DATA_REGION_SIZE + DATA_CRC_REGION_SIZE) / 4) == 0;
}

/**
 * @brief 数据包打包并发送
 *
 * @param packet
 * @param data_to_pack
 */
void DataPacket_Pack(uint8_t ID, Data_Packet_t *packet, UC_Data_t *data_to_pack)
{
	packet->_ID = ID;
	packet->effective_data = *data_to_pack;

	// 计算校验值
	packet->_ID_CRC = crc8((uint8_t *)packet, sizeof(packet->_ID));
	packet->_DATA_CRC = HAL_CRC_Calculate(&hcrc, (uint32_t *)(((uint8_t *)packet) + INFO_REGION_SIZE), DATA_REGION_SIZE / 4);
}

/**
 * @brief 解包数据包，当校验通过时会更新 rx_dest_data
 *
 * @param packet
 * @return int 0：成功；1：校验失败；2：rx_dest_data 无效；3：ID 不匹配
 */
int DataPacket_Unpack(Data_Packet_t *packet)
{
	UC_debug_data.rx_pkt_total++;
	if (UC_Var.rx_dest_data != NULL)
	{
		if (memcmp(packet, UC_Var.rx_exp_info, INFO_REGION_SIZE) != 0)
		{
			UC_debug_data.rx_pkt_id_invalid++;
			return 3;
		}

		if (DataPacket_Check(packet) == true)
		{
			UC_debug_data.rx_pkt_data_valid++;
			memcpy(UC_Var.rx_dest_data, &packet->effective_data, sizeof(UC_Data_t));
			return 0;
		}
		else
		{
			UC_debug_data.rx_pkt_data_invalid++;
			return 1;
		}
	}
	else
	{
		return 2;
	}
}

bool Frame_CheckHead(Frame_t *frame)
{
	if (memcmp(&(frame->frame_head), FRAME_HEAD, sizeof(FRAME_HEAD)) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Frame_CheckTail(Frame_t *frame)
{
	if (memcmp(&(frame->frame_tail), FRAME_TAIL, sizeof(FRAME_TAIL)) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

/**
 * @brief 将帧头数据前移一位
 *
 * @param frame
 */
void Frame_MoveForwardHead(Frame_t *frame)
{
	for (int i = 0; i < sizeof(FRAME_HEAD) - 1; i++)
	{
		frame->frame_head[i] = frame->frame_head[i + 1];
	}
}

void Frame_Send(UART_HandleTypeDef *huart, Frame_t *frame)
{
	HAL_UART_Transmit(huart, (uint8_t *)frame, sizeof(Frame_t), portMAX_DELAY);
}

void UC_Receive_Start(uint8_t ID, UART_HandleTypeDef *huart, UC_Data_t *data_to_receive)
{
	if (huart != NULL)
	{
		UC_Var.rx_huart = huart;
		UC_Var.rx_dest_data = data_to_receive;
		UC_Var.rx_head_aligned = false;
		UC_Var.rx_exp_info[0] = ID;
		UC_Var.rx_exp_info[1] = crc8(&ID, 1);
		HAL_UART_Receive_IT(huart, ((uint8_t *)&UC_Var.rx_buffer.frame_head) + sizeof(FRAME_HEAD) - 1, 1);
	}
	else if (UC_Var.rx_huart != NULL)
	{
		HAL_UART_AbortReceive(UC_Var.rx_huart);
		UC_Var.rx_huart = NULL;
		UC_Var.rx_head_aligned = false;
		UC_Var.rx_dest_data = NULL;
		memset(&UC_Var.rx_exp_info, 0, sizeof(UC_Var.rx_exp_info));
	}
}

void UC_Send(uint8_t ID, UART_HandleTypeDef *huart, UC_Data_t *data_to_send)
{
	DataPacket_Pack(ID, &UC_Var.tx_buffer.data, data_to_send);
	Frame_Send(huart, &UC_Var.tx_buffer);
}

void UC_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (UC_Var.rx_huart == NULL)
		return;

	if (huart->Instance == UC_Var.rx_huart->Instance)
	{
		if (UC_Var.rx_head_aligned)
		{
			if (Frame_CheckHead(&UC_Var.rx_buffer) && Frame_CheckTail(&UC_Var.rx_buffer))
			{
				DataPacket_Unpack(&UC_Var.rx_buffer.data);
				HAL_UART_Receive_IT(UC_Var.rx_huart, (uint8_t *)&UC_Var.rx_buffer, sizeof(Frame_t));
			}
			else
			{
				UC_debug_data.rx_frame_loss++;
				UC_Var.rx_head_aligned = false;
				memset(UC_Var.rx_buffer.frame_head, 0, sizeof(UC_Var.rx_buffer.frame_head));
				HAL_UART_Receive_IT(UC_Var.rx_huart, ((uint8_t *)&UC_Var.rx_buffer.frame_head) + sizeof(FRAME_HEAD) - 1, 1);
			}
		}
		else
		{
			UC_debug_data.rx_frame_loss_sync++;
			if (Frame_CheckHead(&UC_Var.rx_buffer))
			{
				UC_Var.rx_head_aligned = true;
				UC_debug_data.rx_frame_resync++;
				HAL_UART_Receive_IT(UC_Var.rx_huart, (uint8_t *)&(UC_Var.rx_buffer.data), sizeof(Frame_t) - sizeof(FRAME_HEAD));
			}
			else
			{
				Frame_MoveForwardHead(&UC_Var.rx_buffer);
				HAL_UART_Receive_IT(UC_Var.rx_huart, ((uint8_t *)&UC_Var.rx_buffer.frame_head) + sizeof(FRAME_HEAD) - 1, 1);
			}
		}
	}
}

/**
 * Name:    CRC-8               x8+x2+x+1
 * Poly:    0x07
 * Init:    0x00
 * Refin:   False
 * Refout:  False
 * Xorout:  0x00
 * Note:
 */
uint8_t crc8(uint8_t *data, uint16_t length)
{
    uint8_t i;
    uint8_t crc = 0;        // Initial value
    while(length--)
    {
        crc ^= *data++;        // crc ^= *data; data++;
        for ( i = 0; i < 8; i++ )
        {
            if ( crc & 0x80 )
                crc = (crc << 1) ^ 0x07;
            else
                crc <<= 1;
        }
    }
    return crc;
}

#ifdef UC_DEBUG
void UC_print_debug_data()
{
	UD_printf("frame: ");
	UD_printf("loss:%5d  ", UC_debug_data.rx_frame_loss);
	UD_printf("out of sync:%5d  ", UC_debug_data.rx_frame_loss_sync);
	UD_printf("resync:%4d\n", UC_debug_data.rx_frame_resync);
	UD_printf("pkg: ");
	UD_printf("total:%5d  ", UC_debug_data.rx_pkt_total);
	UD_printf("invalid id:%5d  ", UC_debug_data.rx_pkt_id_invalid);
	UD_printf("valid data:%5d  ", UC_debug_data.rx_pkt_data_valid);
	UD_printf("invalid data:%5d  ", UC_debug_data.rx_pkt_data_invalid);
	UD_printf("data loss rate:%.2f%%\n", (float)UC_debug_data.rx_pkt_data_invalid * 100 / (UC_debug_data.rx_pkt_data_invalid + UC_debug_data.rx_pkt_data_valid));
	UD_printf("\n");
}
#else
void UC_print_debug_data(){}
#endif
