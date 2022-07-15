#pragma once
// MESSAGE Controller PACKING

#define MAVLINK_MSG_ID_Controller 1


typedef struct __mavlink_controller_t {
 int16_t left_x; /*<  左摇杆x，0为原点，向右为正*/
 int16_t left_y; /*<  左摇杆y，0为原点，向上为正*/
 int16_t right_x; /*<  右摇杆x，0为原点，向右为正*/
 int16_t right_y; /*<  右摇杆y，0为原点，向上为正*/
 uint8_t buttons; /*<  按钮，次序：76543210*/
} mavlink_controller_t;

#define MAVLINK_MSG_ID_Controller_LEN 9
#define MAVLINK_MSG_ID_Controller_MIN_LEN 9
#define MAVLINK_MSG_ID_1_LEN 9
#define MAVLINK_MSG_ID_1_MIN_LEN 9

#define MAVLINK_MSG_ID_Controller_CRC 189
#define MAVLINK_MSG_ID_1_CRC 189



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_Controller { \
    1, \
    "Controller", \
    5, \
    {  { "left_x", NULL, MAVLINK_TYPE_INT16_T, 0, 0, offsetof(mavlink_controller_t, left_x) }, \
         { "left_y", NULL, MAVLINK_TYPE_INT16_T, 0, 2, offsetof(mavlink_controller_t, left_y) }, \
         { "right_x", NULL, MAVLINK_TYPE_INT16_T, 0, 4, offsetof(mavlink_controller_t, right_x) }, \
         { "right_y", NULL, MAVLINK_TYPE_INT16_T, 0, 6, offsetof(mavlink_controller_t, right_y) }, \
         { "buttons", NULL, MAVLINK_TYPE_UINT8_T, 0, 8, offsetof(mavlink_controller_t, buttons) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_Controller { \
    "Controller", \
    5, \
    {  { "left_x", NULL, MAVLINK_TYPE_INT16_T, 0, 0, offsetof(mavlink_controller_t, left_x) }, \
         { "left_y", NULL, MAVLINK_TYPE_INT16_T, 0, 2, offsetof(mavlink_controller_t, left_y) }, \
         { "right_x", NULL, MAVLINK_TYPE_INT16_T, 0, 4, offsetof(mavlink_controller_t, right_x) }, \
         { "right_y", NULL, MAVLINK_TYPE_INT16_T, 0, 6, offsetof(mavlink_controller_t, right_y) }, \
         { "buttons", NULL, MAVLINK_TYPE_UINT8_T, 0, 8, offsetof(mavlink_controller_t, buttons) }, \
         } \
}
#endif

/**
 * @brief Pack a controller message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param left_x  左摇杆x，0为原点，向右为正
 * @param left_y  左摇杆y，0为原点，向上为正
 * @param right_x  右摇杆x，0为原点，向右为正
 * @param right_y  右摇杆y，0为原点，向上为正
 * @param buttons  按钮，次序：76543210
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_controller_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               int16_t left_x, int16_t left_y, int16_t right_x, int16_t right_y, uint8_t buttons)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_Controller_LEN];
    _mav_put_int16_t(buf, 0, left_x);
    _mav_put_int16_t(buf, 2, left_y);
    _mav_put_int16_t(buf, 4, right_x);
    _mav_put_int16_t(buf, 6, right_y);
    _mav_put_uint8_t(buf, 8, buttons);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_Controller_LEN);
#else
    mavlink_controller_t packet;
    packet.left_x = left_x;
    packet.left_y = left_y;
    packet.right_x = right_x;
    packet.right_y = right_y;
    packet.buttons = buttons;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_Controller_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_Controller;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_Controller_MIN_LEN, MAVLINK_MSG_ID_Controller_LEN, MAVLINK_MSG_ID_Controller_CRC);
}

/**
 * @brief Pack a controller message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param left_x  左摇杆x，0为原点，向右为正
 * @param left_y  左摇杆y，0为原点，向上为正
 * @param right_x  右摇杆x，0为原点，向右为正
 * @param right_y  右摇杆y，0为原点，向上为正
 * @param buttons  按钮，次序：76543210
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_controller_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   int16_t left_x,int16_t left_y,int16_t right_x,int16_t right_y,uint8_t buttons)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_Controller_LEN];
    _mav_put_int16_t(buf, 0, left_x);
    _mav_put_int16_t(buf, 2, left_y);
    _mav_put_int16_t(buf, 4, right_x);
    _mav_put_int16_t(buf, 6, right_y);
    _mav_put_uint8_t(buf, 8, buttons);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_Controller_LEN);
#else
    mavlink_controller_t packet;
    packet.left_x = left_x;
    packet.left_y = left_y;
    packet.right_x = right_x;
    packet.right_y = right_y;
    packet.buttons = buttons;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_Controller_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_Controller;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_Controller_MIN_LEN, MAVLINK_MSG_ID_Controller_LEN, MAVLINK_MSG_ID_Controller_CRC);
}

/**
 * @brief Encode a controller struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param controller C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_controller_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_controller_t* controller)
{
    return mavlink_msg_controller_pack(system_id, component_id, msg, controller->left_x, controller->left_y, controller->right_x, controller->right_y, controller->buttons);
}

/**
 * @brief Encode a controller struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param controller C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_controller_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_controller_t* controller)
{
    return mavlink_msg_controller_pack_chan(system_id, component_id, chan, msg, controller->left_x, controller->left_y, controller->right_x, controller->right_y, controller->buttons);
}

/**
 * @brief Send a controller message
 * @param chan MAVLink channel to send the message
 *
 * @param left_x  左摇杆x，0为原点，向右为正
 * @param left_y  左摇杆y，0为原点，向上为正
 * @param right_x  右摇杆x，0为原点，向右为正
 * @param right_y  右摇杆y，0为原点，向上为正
 * @param buttons  按钮，次序：76543210
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_controller_send(mavlink_channel_t chan, int16_t left_x, int16_t left_y, int16_t right_x, int16_t right_y, uint8_t buttons)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_Controller_LEN];
    _mav_put_int16_t(buf, 0, left_x);
    _mav_put_int16_t(buf, 2, left_y);
    _mav_put_int16_t(buf, 4, right_x);
    _mav_put_int16_t(buf, 6, right_y);
    _mav_put_uint8_t(buf, 8, buttons);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_Controller, buf, MAVLINK_MSG_ID_Controller_MIN_LEN, MAVLINK_MSG_ID_Controller_LEN, MAVLINK_MSG_ID_Controller_CRC);
#else
    mavlink_controller_t packet;
    packet.left_x = left_x;
    packet.left_y = left_y;
    packet.right_x = right_x;
    packet.right_y = right_y;
    packet.buttons = buttons;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_Controller, (const char *)&packet, MAVLINK_MSG_ID_Controller_MIN_LEN, MAVLINK_MSG_ID_Controller_LEN, MAVLINK_MSG_ID_Controller_CRC);
#endif
}

/**
 * @brief Send a controller message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_controller_send_struct(mavlink_channel_t chan, const mavlink_controller_t* controller)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_controller_send(chan, controller->left_x, controller->left_y, controller->right_x, controller->right_y, controller->buttons);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_Controller, (const char *)controller, MAVLINK_MSG_ID_Controller_MIN_LEN, MAVLINK_MSG_ID_Controller_LEN, MAVLINK_MSG_ID_Controller_CRC);
#endif
}

#if MAVLINK_MSG_ID_Controller_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_controller_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  int16_t left_x, int16_t left_y, int16_t right_x, int16_t right_y, uint8_t buttons)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_int16_t(buf, 0, left_x);
    _mav_put_int16_t(buf, 2, left_y);
    _mav_put_int16_t(buf, 4, right_x);
    _mav_put_int16_t(buf, 6, right_y);
    _mav_put_uint8_t(buf, 8, buttons);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_Controller, buf, MAVLINK_MSG_ID_Controller_MIN_LEN, MAVLINK_MSG_ID_Controller_LEN, MAVLINK_MSG_ID_Controller_CRC);
#else
    mavlink_controller_t *packet = (mavlink_controller_t *)msgbuf;
    packet->left_x = left_x;
    packet->left_y = left_y;
    packet->right_x = right_x;
    packet->right_y = right_y;
    packet->buttons = buttons;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_Controller, (const char *)packet, MAVLINK_MSG_ID_Controller_MIN_LEN, MAVLINK_MSG_ID_Controller_LEN, MAVLINK_MSG_ID_Controller_CRC);
#endif
}
#endif

#endif

// MESSAGE Controller UNPACKING


/**
 * @brief Get field left_x from controller message
 *
 * @return  左摇杆x，0为原点，向右为正
 */
static inline int16_t mavlink_msg_controller_get_left_x(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int16_t(msg,  0);
}

/**
 * @brief Get field left_y from controller message
 *
 * @return  左摇杆y，0为原点，向上为正
 */
static inline int16_t mavlink_msg_controller_get_left_y(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int16_t(msg,  2);
}

/**
 * @brief Get field right_x from controller message
 *
 * @return  右摇杆x，0为原点，向右为正
 */
static inline int16_t mavlink_msg_controller_get_right_x(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int16_t(msg,  4);
}

/**
 * @brief Get field right_y from controller message
 *
 * @return  右摇杆y，0为原点，向上为正
 */
static inline int16_t mavlink_msg_controller_get_right_y(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int16_t(msg,  6);
}

/**
 * @brief Get field buttons from controller message
 *
 * @return  按钮，次序：76543210
 */
static inline uint8_t mavlink_msg_controller_get_buttons(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  8);
}

/**
 * @brief Decode a controller message into a struct
 *
 * @param msg The message to decode
 * @param controller C-struct to decode the message contents into
 */
static inline void mavlink_msg_controller_decode(const mavlink_message_t* msg, mavlink_controller_t* controller)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    controller->left_x = mavlink_msg_controller_get_left_x(msg);
    controller->left_y = mavlink_msg_controller_get_left_y(msg);
    controller->right_x = mavlink_msg_controller_get_right_x(msg);
    controller->right_y = mavlink_msg_controller_get_right_y(msg);
    controller->buttons = mavlink_msg_controller_get_buttons(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_Controller_LEN? msg->len : MAVLINK_MSG_ID_Controller_LEN;
        memset(controller, 0, MAVLINK_MSG_ID_Controller_LEN);
    memcpy(controller, _MAV_PAYLOAD(msg), len);
#endif
}
