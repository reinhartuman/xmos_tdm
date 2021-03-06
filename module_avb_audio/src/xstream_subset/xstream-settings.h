#include "xstream-options.h"
#include <avb_conf.h>

#define XSTREAME_SHARED_MEM_IF 0

#define XDEVICE_CODE__CS4270_I2C_ADDRESS    XDEVICE_CODEC__CS4270_I2C_ADDRESS_1

#define XPORTS_I2S__ADC_DATA_WIRE_COUNT     AVB_NUM_AUDIO_SDATA_IN
// Don't do this in unless XSTREAME_SHARED_MEM_IF: #define XPORTS_I2S__ADC_DATA_WIRE_COUNT     1
#define XPORTS_I2S__ADC_WORD_FORMAT         XPORTS_I2S__WORD_FORMAT__LEFT_JUST
#define XPORTS_I2S__ADC_WORD_SIZE           XPORTS_I2S__WORD_SIZE__32_BIT
#define XPORTS_I2S__BUS_CLOCKING_MODE       XPORTS_I2S__BUS_CLOCKING_MODE__MASTER
#define XPORTS_I2S__DAC_DATA_WIRE_COUNT     AVB_NUM_AUDIO_SDATA_OUT
// Don't do this in unless XSTREAME_SHARED_MEM_IF:#define XPORTS_I2S__DAC_DATA_WIRE_COUNT     1
#define XPORTS_I2S__DAC_WORD_FORMAT         XPORTS_I2S__WORD_FORMAT__LEFT_JUST
#define XPORTS_I2S__DAC_WORD_SIZE           XPORTS_I2S__WORD_SIZE__32_BIT
#define XPORTS_I2S__DATA_FORMAT_MODE        XPORTS_I2S__DATA_FORMAT_MODE__EXTERNAL

#define XPORTS_I2S__DATA_LOOPBACK_MODE      XPORTS_I2S__DATA_LOOPBACK_MODE__OFF

#define XPORTS_I2S__MASTER_CLOCK_SOURCE     XPORTS_I2S__MASTER_CLOCK_SOURCE__EXTERNAL
#define XPORTS_I2S__MCLK_BCLK_RATIO         XPORTS_I2S__MCLK_BCLK_RATIO__ONLY_2
#define XPORTS_I2S__MCLK_FREQ_MULTIPLIER    XPORTS_I2S__MCLK_FREQ_MULTIPLIER__X6
#define XPORTS_I2S__OPTIMIZATION_MODE       XPORTS_I2S__OPTIMIZATION_MODE__SPEED
#define XPORTS_I2S__SLOT_COUNT              AVB_AUDIO_IF_SAMPLES_PER_PERIOD
#define XPORTS_I2S__SLOT_SIZE               XPORTS_I2S__SLOT_SIZE__32_BIT
#define XPORTS_I2S__SYNC_WORD_FORMAT        XPORTS_I2S__SYNC_WORD_FORMAT__TDM_STYLE
