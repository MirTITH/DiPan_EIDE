#pragma once
// MESSAGE Upper PACKING

#define MAVLINK_MSG_ID_Upper 2


typedef struct __mavlink_upper_t {
 float lift; /*<  升降架伺服，向上为正*/
 float claw_OC_DJI; /*<  爪子张闭（大疆电机），正为张开*/
 float claw_spin; /*<  爪子旋转，正为顺时针（角速度方向向前）*/
 int32_t claw_OC_L; /*<  爪子左舵机 PWM 脉冲宽度*/
 int32_t claw_OC_R; /*<  爪子右舵机 PWM 脉冲宽度*/
 uint8_t servo_type; /*<  伺服类型（0：位置；1：速度）位置从低到高: lift, claw_OC_DJI, claw_spin*/
 uint8_t vice_lift; /*<  副升降，1为升*/
} mavlink_upper_t;

#define MAVLINK_MSG_ID_Upper_LEN 22
#define MAVLINK_MSG_ID_Upper_MIN_LEN 22
#define MAVLINK_MSG_ID_2_LEN 22
#define MAVLINK_MSG_ID_2_MIN_LEN 22

#define MAVLINK_MSG_ID_Upper_CRC 192
#define MAVLINK_MSG_ID_2_CRC 192



#if MAVLINK_COMMAND_24BIT
#define MAVLINK_MESSAGE_INFO_Upper { \
    2, \
    "Upper", \
    7, \
    {  { "servo_type", NULL, MAVLINK_TYPE_UINT8_T, 0, 20, offsetof(mavlink_upper_t, servo_type) }, \
         { "lift", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_upper_t, lift) }, \
         { "claw_OC_DJI", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_upper_t, claw_OC_DJI) }, \
         { "claw_spin", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_upper_t, claw_spin) }, \
         { "claw_OC_L", NULL, MAVLINK_TYPE_INT32_T, 0, 12, offsetof(mavlink_upper_t, claw_OC_L) }, \
         { "claw_OC_R", NULL, MAVLINK_TYPE_INT32_T, 0, 16, offsetof(mavlink_upper_t, claw_OC_R) }, \
         { "vice_lift", NULL, MAVLINK_TYPE_UINT8_T, 0, 21, offsetof(mavlink_upper_t, vice_lift) }, \
         } \
}
#else
#define MAVLINK_MESSAGE_INFO_Upper { \
    "Upper", \
    7, \
    {  { "servo_type", NULL, MAVLINK_TYPE_UINT8_T, 0, 20, offsetof(mavlink_upper_t, servo_type) }, \
         { "lift", NULL, MAVLINK_TYPE_FLOAT, 0, 0, offsetof(mavlink_upper_t, lift) }, \
         { "claw_OC_DJI", NULL, MAVLINK_TYPE_FLOAT, 0, 4, offsetof(mavlink_upper_t, claw_OC_DJI) }, \
         { "claw_spin", NULL, MAVLINK_TYPE_FLOAT, 0, 8, offsetof(mavlink_upper_t, claw_spin) }, \
         { "claw_OC_L", NULL, MAVLINK_TYPE_INT32_T, 0, 12, offsetof(mavlink_upper_t, claw_OC_L) }, \
         { "claw_OC_R", NULL, MAVLINK_TYPE_INT32_T, 0, 16, offsetof(mavlink_upper_t, claw_OC_R) }, \
         { "vice_lift", NULL, MAVLINK_TYPE_UINT8_T, 0, 21, offsetof(mavlink_upper_t, vice_lift) }, \
         } \
}
#endif

/**
 * @brief Pack a upper message
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 *
 * @param servo_type  伺服类型（0：位置；1：速度）位置从低到高: lift, claw_OC_DJI, claw_spin
 * @param lift  升降架伺服，向上为正
 * @param claw_OC_DJI  爪子张闭（大疆电机），正为张开
 * @param claw_spin  爪子旋转，正为顺时针（角速度方向向前）
 * @param claw_OC_L  爪子左舵机 PWM 脉冲宽度
 * @param claw_OC_R  爪子右舵机 PWM 脉冲宽度
 * @param vice_lift  副升降，1为升
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_upper_pack(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg,
                               uint8_t servo_type, float lift, float claw_OC_DJI, float claw_spin, int32_t claw_OC_L, int32_t claw_OC_R, uint8_t vice_lift)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_Upper_LEN];
    _mav_put_float(buf, 0, lift);
    _mav_put_float(buf, 4, claw_OC_DJI);
    _mav_put_float(buf, 8, claw_spin);
    _mav_put_int32_t(buf, 12, claw_OC_L);
    _mav_put_int32_t(buf, 16, claw_OC_R);
    _mav_put_uint8_t(buf, 20, servo_type);
    _mav_put_uint8_t(buf, 21, vice_lift);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_Upper_LEN);
#else
    mavlink_upper_t packet;
    packet.lift = lift;
    packet.claw_OC_DJI = claw_OC_DJI;
    packet.claw_spin = claw_spin;
    packet.claw_OC_L = claw_OC_L;
    packet.claw_OC_R = claw_OC_R;
    packet.servo_type = servo_type;
    packet.vice_lift = vice_lift;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_Upper_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_Upper;
    return mavlink_finalize_message(msg, system_id, component_id, MAVLINK_MSG_ID_Upper_MIN_LEN, MAVLINK_MSG_ID_Upper_LEN, MAVLINK_MSG_ID_Upper_CRC);
}

/**
 * @brief Pack a upper message on a channel
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param servo_type  伺服类型（0：位置；1：速度）位置从低到高: lift, claw_OC_DJI, claw_spin
 * @param lift  升降架伺服，向上为正
 * @param claw_OC_DJI  爪子张闭（大疆电机），正为张开
 * @param claw_spin  爪子旋转，正为顺时针（角速度方向向前）
 * @param claw_OC_L  爪子左舵机 PWM 脉冲宽度
 * @param claw_OC_R  爪子右舵机 PWM 脉冲宽度
 * @param vice_lift  副升降，1为升
 * @return length of the message in bytes (excluding serial stream start sign)
 */
static inline uint16_t mavlink_msg_upper_pack_chan(uint8_t system_id, uint8_t component_id, uint8_t chan,
                               mavlink_message_t* msg,
                                   uint8_t servo_type,float lift,float claw_OC_DJI,float claw_spin,int32_t claw_OC_L,int32_t claw_OC_R,uint8_t vice_lift)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_Upper_LEN];
    _mav_put_float(buf, 0, lift);
    _mav_put_float(buf, 4, claw_OC_DJI);
    _mav_put_float(buf, 8, claw_spin);
    _mav_put_int32_t(buf, 12, claw_OC_L);
    _mav_put_int32_t(buf, 16, claw_OC_R);
    _mav_put_uint8_t(buf, 20, servo_type);
    _mav_put_uint8_t(buf, 21, vice_lift);

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), buf, MAVLINK_MSG_ID_Upper_LEN);
#else
    mavlink_upper_t packet;
    packet.lift = lift;
    packet.claw_OC_DJI = claw_OC_DJI;
    packet.claw_spin = claw_spin;
    packet.claw_OC_L = claw_OC_L;
    packet.claw_OC_R = claw_OC_R;
    packet.servo_type = servo_type;
    packet.vice_lift = vice_lift;

        memcpy(_MAV_PAYLOAD_NON_CONST(msg), &packet, MAVLINK_MSG_ID_Upper_LEN);
#endif

    msg->msgid = MAVLINK_MSG_ID_Upper;
    return mavlink_finalize_message_chan(msg, system_id, component_id, chan, MAVLINK_MSG_ID_Upper_MIN_LEN, MAVLINK_MSG_ID_Upper_LEN, MAVLINK_MSG_ID_Upper_CRC);
}

/**
 * @brief Encode a upper struct
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param msg The MAVLink message to compress the data into
 * @param upper C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_upper_encode(uint8_t system_id, uint8_t component_id, mavlink_message_t* msg, const mavlink_upper_t* upper)
{
    return mavlink_msg_upper_pack(system_id, component_id, msg, upper->servo_type, upper->lift, upper->claw_OC_DJI, upper->claw_spin, upper->claw_OC_L, upper->claw_OC_R, upper->vice_lift);
}

/**
 * @brief Encode a upper struct on a channel
 *
 * @param system_id ID of this system
 * @param component_id ID of this component (e.g. 200 for IMU)
 * @param chan The MAVLink channel this message will be sent over
 * @param msg The MAVLink message to compress the data into
 * @param upper C-struct to read the message contents from
 */
static inline uint16_t mavlink_msg_upper_encode_chan(uint8_t system_id, uint8_t component_id, uint8_t chan, mavlink_message_t* msg, const mavlink_upper_t* upper)
{
    return mavlink_msg_upper_pack_chan(system_id, component_id, chan, msg, upper->servo_type, upper->lift, upper->claw_OC_DJI, upper->claw_spin, upper->claw_OC_L, upper->claw_OC_R, upper->vice_lift);
}

/**
 * @brief Send a upper message
 * @param chan MAVLink channel to send the message
 *
 * @param servo_type  伺服类型（0：位置；1：速度）位置从低到高: lift, claw_OC_DJI, claw_spin
 * @param lift  升降架伺服，向上为正
 * @param claw_OC_DJI  爪子张闭（大疆电机），正为张开
 * @param claw_spin  爪子旋转，正为顺时针（角速度方向向前）
 * @param claw_OC_L  爪子左舵机 PWM 脉冲宽度
 * @param claw_OC_R  爪子右舵机 PWM 脉冲宽度
 * @param vice_lift  副升降，1为升
 */
#ifdef MAVLINK_USE_CONVENIENCE_FUNCTIONS

static inline void mavlink_msg_upper_send(mavlink_channel_t chan, uint8_t servo_type, float lift, float claw_OC_DJI, float claw_spin, int32_t claw_OC_L, int32_t claw_OC_R, uint8_t vice_lift)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char buf[MAVLINK_MSG_ID_Upper_LEN];
    _mav_put_float(buf, 0, lift);
    _mav_put_float(buf, 4, claw_OC_DJI);
    _mav_put_float(buf, 8, claw_spin);
    _mav_put_int32_t(buf, 12, claw_OC_L);
    _mav_put_int32_t(buf, 16, claw_OC_R);
    _mav_put_uint8_t(buf, 20, servo_type);
    _mav_put_uint8_t(buf, 21, vice_lift);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_Upper, buf, MAVLINK_MSG_ID_Upper_MIN_LEN, MAVLINK_MSG_ID_Upper_LEN, MAVLINK_MSG_ID_Upper_CRC);
#else
    mavlink_upper_t packet;
    packet.lift = lift;
    packet.claw_OC_DJI = claw_OC_DJI;
    packet.claw_spin = claw_spin;
    packet.claw_OC_L = claw_OC_L;
    packet.claw_OC_R = claw_OC_R;
    packet.servo_type = servo_type;
    packet.vice_lift = vice_lift;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_Upper, (const char *)&packet, MAVLINK_MSG_ID_Upper_MIN_LEN, MAVLINK_MSG_ID_Upper_LEN, MAVLINK_MSG_ID_Upper_CRC);
#endif
}

/**
 * @brief Send a upper message
 * @param chan MAVLink channel to send the message
 * @param struct The MAVLink struct to serialize
 */
static inline void mavlink_msg_upper_send_struct(mavlink_channel_t chan, const mavlink_upper_t* upper)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    mavlink_msg_upper_send(chan, upper->servo_type, upper->lift, upper->claw_OC_DJI, upper->claw_spin, upper->claw_OC_L, upper->claw_OC_R, upper->vice_lift);
#else
    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_Upper, (const char *)upper, MAVLINK_MSG_ID_Upper_MIN_LEN, MAVLINK_MSG_ID_Upper_LEN, MAVLINK_MSG_ID_Upper_CRC);
#endif
}

#if MAVLINK_MSG_ID_Upper_LEN <= MAVLINK_MAX_PAYLOAD_LEN
/*
  This variant of _send() can be used to save stack space by re-using
  memory from the receive buffer.  The caller provides a
  mavlink_message_t which is the size of a full mavlink message. This
  is usually the receive buffer for the channel, and allows a reply to an
  incoming message with minimum stack space usage.
 */
static inline void mavlink_msg_upper_send_buf(mavlink_message_t *msgbuf, mavlink_channel_t chan,  uint8_t servo_type, float lift, float claw_OC_DJI, float claw_spin, int32_t claw_OC_L, int32_t claw_OC_R, uint8_t vice_lift)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    char *buf = (char *)msgbuf;
    _mav_put_float(buf, 0, lift);
    _mav_put_float(buf, 4, claw_OC_DJI);
    _mav_put_float(buf, 8, claw_spin);
    _mav_put_int32_t(buf, 12, claw_OC_L);
    _mav_put_int32_t(buf, 16, claw_OC_R);
    _mav_put_uint8_t(buf, 20, servo_type);
    _mav_put_uint8_t(buf, 21, vice_lift);

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_Upper, buf, MAVLINK_MSG_ID_Upper_MIN_LEN, MAVLINK_MSG_ID_Upper_LEN, MAVLINK_MSG_ID_Upper_CRC);
#else
    mavlink_upper_t *packet = (mavlink_upper_t *)msgbuf;
    packet->lift = lift;
    packet->claw_OC_DJI = claw_OC_DJI;
    packet->claw_spin = claw_spin;
    packet->claw_OC_L = claw_OC_L;
    packet->claw_OC_R = claw_OC_R;
    packet->servo_type = servo_type;
    packet->vice_lift = vice_lift;

    _mav_finalize_message_chan_send(chan, MAVLINK_MSG_ID_Upper, (const char *)packet, MAVLINK_MSG_ID_Upper_MIN_LEN, MAVLINK_MSG_ID_Upper_LEN, MAVLINK_MSG_ID_Upper_CRC);
#endif
}
#endif

#endif

// MESSAGE Upper UNPACKING


/**
 * @brief Get field servo_type from upper message
 *
 * @return  伺服类型（0：位置；1：速度）位置从低到高: lift, claw_OC_DJI, claw_spin
 */
static inline uint8_t mavlink_msg_upper_get_servo_type(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  20);
}

/**
 * @brief Get field lift from upper message
 *
 * @return  升降架伺服，向上为正
 */
static inline float mavlink_msg_upper_get_lift(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  0);
}

/**
 * @brief Get field claw_OC_DJI from upper message
 *
 * @return  爪子张闭（大疆电机），正为张开
 */
static inline float mavlink_msg_upper_get_claw_OC_DJI(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  4);
}

/**
 * @brief Get field claw_spin from upper message
 *
 * @return  爪子旋转，正为顺时针（角速度方向向前）
 */
static inline float mavlink_msg_upper_get_claw_spin(const mavlink_message_t* msg)
{
    return _MAV_RETURN_float(msg,  8);
}

/**
 * @brief Get field claw_OC_L from upper message
 *
 * @return  爪子左舵机 PWM 脉冲宽度
 */
static inline int32_t mavlink_msg_upper_get_claw_OC_L(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  12);
}

/**
 * @brief Get field claw_OC_R from upper message
 *
 * @return  爪子右舵机 PWM 脉冲宽度
 */
static inline int32_t mavlink_msg_upper_get_claw_OC_R(const mavlink_message_t* msg)
{
    return _MAV_RETURN_int32_t(msg,  16);
}

/**
 * @brief Get field vice_lift from upper message
 *
 * @return  副升降，1为升
 */
static inline uint8_t mavlink_msg_upper_get_vice_lift(const mavlink_message_t* msg)
{
    return _MAV_RETURN_uint8_t(msg,  21);
}

/**
 * @brief Decode a upper message into a struct
 *
 * @param msg The message to decode
 * @param upper C-struct to decode the message contents into
 */
static inline void mavlink_msg_upper_decode(const mavlink_message_t* msg, mavlink_upper_t* upper)
{
#if MAVLINK_NEED_BYTE_SWAP || !MAVLINK_ALIGNED_FIELDS
    upper->lift = mavlink_msg_upper_get_lift(msg);
    upper->claw_OC_DJI = mavlink_msg_upper_get_claw_OC_DJI(msg);
    upper->claw_spin = mavlink_msg_upper_get_claw_spin(msg);
    upper->claw_OC_L = mavlink_msg_upper_get_claw_OC_L(msg);
    upper->claw_OC_R = mavlink_msg_upper_get_claw_OC_R(msg);
    upper->servo_type = mavlink_msg_upper_get_servo_type(msg);
    upper->vice_lift = mavlink_msg_upper_get_vice_lift(msg);
#else
        uint8_t len = msg->len < MAVLINK_MSG_ID_Upper_LEN? msg->len : MAVLINK_MSG_ID_Upper_LEN;
        memset(upper, 0, MAVLINK_MSG_ID_Upper_LEN);
    memcpy(upper, _MAV_PAYLOAD(msg), len);
#endif
}
