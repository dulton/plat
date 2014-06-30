#ifndef PTZDEFINES_H
#define PTZDEFINES_H
#include <QObject>
#include <QMetaType>
typedef enum _PTZ_CMD{
    PTZ_ZOOM_FOCUS           = 0x0204,
    PTZ_ZOOM_IN_STOP         = 0x0303,
    PTZ_ZOOM_IN_START        = 0x0304,
    PTZ_ZOOM_OUT_STOP        = 0x0301,
    PTZ_ZOOM_OUT_START       = 0x0302,
    PTZ_UP_STOP              = 0x0401,
    PTZ_UP_START             = 0x0402,
    PTZ_DOWN_STOP            = 0x0403,
    PTZ_DOWN_START           = 0x0404,
    PTZ_RIGHT_STOP           = 0x0501,
    PTZ_RIGHT_START          = 0x0502,
    PTZ_LEFT_STOP            = 0x0503,
    PTZ_LEFT_START           = 0x0504,
    PTZ_PRE_POS_SAVE         = 0x0601,
    PTZ_PRE_POS_ROLL         = 0x0602,
    PTZ_PRE_POS_DEL          = 0x0603,
    PTZ_UP_LEFT_STOP         = 0x0701,
    PTZ_UP_LEFT_START        = 0x0702,
    PTZ_DOWN_LEFT_STOP       = 0x0703,
    PTZ_DOWN_LEFT_START      = 0x0704,
    PTZ_UP_RIGHT_STOP        = 0x0801,
    PTZ_UP_RIGHT_START       = 0x0802,
    PTZ_DOWN_RIGHT_STOP      = 0x0803,
    PTZ_DOWN_RIGHT_START     = 0x0804,
    PTZ_STOP_ALL             = 0x0901,
    PTZ_W_WIPER_ON           = 0x0a01,
    PTZ_W_WIPER_OFF          = 0x0a02,
    PTZ_LIGHT_ON             = 0x0b01,
    PTZ_LIGHT_OFF            = 0x0b02,
    PTZ_WARM_ON              = 0x0c01,
    PTZ_WARM_OFF             = 0x0c02,
    PTZ_INFRARED_ON          = 0x0d01,
    PTZ_INFRARED_OFF         = 0x0d02,
    PTZ_LINE_SCAN_START      = 0x0e01,
    PTZ_LINE_SCAN_STOP       = 0x0e02,
    PTZ_CRUISE_START         = 0x0f01,
    PTZ_CRUISE_STOP          = 0x0f02,
    PTZ_PRE_POS_CRUISE_START = 0x1001,
    PTZ_PRE_POS_CRUISE_STOP  = 0x1002,
    PTZ_LOCK                 = 0x1101,
    PTZ_UNLOCK               = 0x1102,
    PTZ_CMD_NONE             = 0xffff
} PTZ_CMD;
typedef enum _SPEED {
    SPEED1 = 0x1,
    SPEED2 = 0x2,
    SPEED3 = 0x3,
    SPEED4 = 0x4,
    SPEED5 = 0x5,
    SPEED6 = 0x6,
    SPEED7 = 0x7,
    SPEED8 = 0x8,
    SPEED9 = 0x9,
    SPEED_NONE = 0xf
} SPEED;

typedef enum _CMD_TYPE {
    CMD_DI,     /*for up down left right etc...*/
    CMD_ZOOM,
    CMD_CTL,    /*for warm w_wiper lights infrared lock etc...*/
    CMD_NONE
}CMD_TYPE;

/*
Q_DECLARE_METATYPE(SPEED)
Q_DECLARE_METATYPE(PTZ_CMD)
Q_DECLARE_METATYPE(CMD_TYPE)
*/

#endif // PTZDEFINES_H
