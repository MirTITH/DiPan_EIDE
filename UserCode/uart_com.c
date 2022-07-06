/**
 * @file uart_com.c
 * @author TITH (1023515576@qq.com)
 * @brief 串口数据包通讯
 * @version 0.1
 * @date 2022-07-04
 *
 * @copyright Copyright (c) 2022
 *
 */

#include "uart_com.h"
#include "usart.h"
#include "cmsis_os.h"
#include "crcLib.h"
#include "crc.h"
#include <stdbool.h>
#include <string.h>

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

#define FRAME_HEAD_DEFINE \
	{                     \
		0xAA, 0x93        \
	}
#define FRAME_TAIL_DEFINE \
	{                     \
		0xBB, 0x00        \
	}

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

/**
 * @brief 检查数据包校验是否通过
 *
 * @param packet
 * @return true 通过
 * @return false 不通过
 */
bool DataPacket_Check(Data_Packet_t *packet)
{
	return HAL_CRC_Calculate(&hcrc, (uint32_t *)&(packet->effective_data), (DATA_REGION_SIZE + DATA_CRC_REGION_SIZE) / 4) == 0;
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
	packet->_DATA_CRC = HAL_CRC_Calculate(&hcrc, (uint32_t *)&(packet->effective_data), DATA_REGION_SIZE / 4);
}

/**
 * @brief 解包数据包，当校验通过时会更新 rx_dest_data
 *
 * @param packet
 * @return int 0：成功；1：校验失败；2：rx_dest_data 无效；3：ID 不匹配
 */
int DataPacket_Unpack(Data_Packet_t *packet)
{
	if (UC_Var.rx_dest_data != NULL)
	{
		if (memcmp(packet, UC_Var.rx_exp_info, INFO_REGION_SIZE) != 0)
			return 3;

		if (DataPacket_Check(packet) == true)
		{
			memcpy(UC_Var.rx_dest_data, &packet->effective_data, sizeof(UC_Data_t));
			return 0;
		}
		else
		{
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
	HAL_UART_Transmit(huart, (uint8_t *)frame, sizeof(*frame), portMAX_DELAY);
}

/**
 * @brief 串口通讯接收初始化（只发送不需要执行此函数）
 *
 * @param huart 接收数据的串口句柄（传入 NULL 表示去初始化，即不再接收）
 * @param data_to_receive 接收到的数据会写到这里（去初始化时传 NULL）
 */
void UC_Rcv_Init(uint8_t ID, UART_HandleTypeDef *huart, UC_Data_t *data_to_receive)
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

/**
 * @brief 发送数据
 *
 * @param data_to_send 要发送的数据
 */
void UC_Send(uint8_t ID, UART_HandleTypeDef *huart, UC_Data_t *data_to_send)
{
	DataPacket_Pack(ID, &UC_Var.tx_buffer.data, data_to_send);
	// UD_printf("crc check: %d\n", DataPacket_Check(&pkg_to_send));
	Frame_Send(huart, &UC_Var.tx_buffer);
}

/**
 * @brief 串口接收回调函数
 *
 */
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
				UC_Var.rx_head_aligned = false;
				memset(UC_Var.rx_buffer.frame_head, 0, sizeof(UC_Var.rx_buffer.frame_head));
				HAL_UART_Receive_IT(UC_Var.rx_huart, ((uint8_t *)&UC_Var.rx_buffer.frame_head) + sizeof(FRAME_HEAD) - 1, 1);
			}
		}
		else
		{
			if (Frame_CheckHead(&UC_Var.rx_buffer))
			{
				UC_Var.rx_head_aligned = true;
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

void test()
{
	// UC_Data_t test_data = {
	// 	.test_int8 = 0x22,
	// 	.test_int82 = 0x33};
	// UD_printf("pkg size: %d ", sizeof(Data_Packet_t));
	// UD_printf("data size: %d\n", sizeof(UC_Data_t));

	// UC_Send(1, &huart6, &test_data);

	// UD_printf("pos:  ");

	// for (int i = 0; i < sizeof(pkg_to_send); i++)
	// {
	// 	UD_printf("%3d ", i + 1);
	// }
	// UD_printf("\ndata: ");
	// for (int i = 0; i < sizeof(pkg_to_send); i++)
	// {
	// 	UD_printf("%3x ", ((uint8_t*)&pkg_to_send)[i]);
	// }
	// UD_printf("\n");
}
