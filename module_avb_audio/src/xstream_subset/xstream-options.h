
// ====================================================================================================================
// Configurations for all modules in the XSTREAM library
// ====================================================================================================================

#ifndef XSTREAM_OPTIONS
#define XSTREAM_OPTIONS

// ====================================================================================================================
// XPORTS Protocol Options for Each I2S Instance
// ====================================================================================================================

#define XPORTS_I2S__OPTIMIZATION_MODE__SPEED       0
#define XPORTS_I2S__OPTIMIZATION_MODE__SIZE        1

#define XPORTS_I2S__DATA_LOOPBACK_MODE__OFF        0  // Data is not looped back
#define XPORTS_I2S__DATA_LOOPBACK_MODE__ON         1  // DAC data is looped back to ADC data (ADC sampled values are overwritten)

#define XPORTS_I2S__DATA_FORMAT_MODE__EXTERNAL     0  // Perform DAC and ADC sample data rendering/parsing in application
#define XPORTS_I2S__DATA_FORMAT_MODE__INTERNAL     1  // Perform DAC and ADC sample data rendering/parsing in codec module

#define XPORTS_I2S__MASTER_CLOCK_SOURCE__EXTERNAL  0  // Use external MCLK signal (MCLK pin is an input)
#define XPORTS_I2S__MASTER_CLOCK_SOURCE__INTERNAL  1  // Generate MCLK from internal oscillator (MCLK pin in an output)

#define XPORTS_I2S__BUS_CLOCKING_MODE__MASTER      0  // Generate BCLK and WCLK signals (outputs)
#define XPORTS_I2S__BUS_CLOCKING_MODE__SLAVE       1  // Sample BCLK and WCLK signals (inputs)

#define XPORTS_I2S__MCLK_BCLK_RATIO__ONLY_1        1  // MCLK frequency / BCLK frequency = 1.0
#define XPORTS_I2S__MCLK_BCLK_RATIO__ONLY_2        2  // MCLK frequency / BCLK frequency = 2.0
#define XPORTS_I2S__MCLK_BCLK_RATIO__ONLY_4        4  // MCLK frequency / BCLK frequency = 4.0
#define XPORTS_I2S__MCLK_BCLK_RATIO__ONLY_8        8  // MCLK frequency / BCLK frequency = 8.0
#define XPORTS_I2S__MCLK_BCLK_RATIO__ONLY_16      16  // MCLK frequency / BCLK frequency = 16.0
#define XPORTS_I2S__MCLK_BCLK_RATIO__ONLY_32      32  // MCLK frequency / BCLK frequency = 32.0
#define XPORTS_I2S__MCLK_BCLK_RATIO__1_TO_32      (32+16+8+4+2+1)  // (MCLK frequency / BCLK frequency) can be 1.0 or 2.0 ... or 32.0
#define XPORTS_I2S__MCLK_BCLK_RATIO__2_TO_32      (32+16+8+4+2)    // (MCLK frequency / BCLK frequency) can be 2.0 ... or 32.0

// FS Base @ BitsPerSample                              // 48K0 @ 32/16,  44K1 @ 32/16,     48K0 @ 24,     44K1 @ 24
#define XPORTS_I2S__MCLK_FREQ_MULTIPLIER__X6       1  // 49152000 Mhz,  45158400 Mhz,  36864000 Mhz,  33868800 Mhz
#define XPORTS_I2S__MCLK_FREQ_MULTIPLIER__X5       2  // 24576000 Mhz,  22579200 Mhz,  18432000 Mhz,  16934400 Mhz
#define XPORTS_I2S__MCLK_FREQ_MULTIPLIER__X4       4  // 12288000 Mhz,  11289600 Mhz,   9216000 Mhz,   8467200 Mhz
#define XPORTS_I2S__MCLK_FREQ_MULTIPLIER__X3       8  //  6144000 Mhz,   5644800 Mhz,   4608000 Mhz,   4233600 Mhz
#define XPORTS_I2S__MCLK_FREQ_MULTIPLIER__X2      16  //  3072000 Mhz,   2822400 Mhz,   2304000 Mhz,   2116800 Mhz
#define XPORTS_I2S__MCLK_FREQ_MULTIPLIER__X1      32  //  1536000 Mhz,   1411200 Mhz,   1152000 Mhz,   1058400 Mhz

#define XPORTS_I2S__SLOT_SIZE__16_BIT             16  // 16 BCLK's per ADC/DAC sample, contains one 16-bit sample word
#define XPORTS_I2S__SLOT_SIZE__24_BIT             24  // 24 BCLK's per ADC/DAC sample, contains one 16/24-bit sample word
#define XPORTS_I2S__SLOT_SIZE__32_BIT             32  // 32 BCLK's per ADC/DAC sample, contains one 16/24/32-bit sample word

#define XPORTS_I2S__SLOT_COUNT__2_PER_CYCLE        2  // 2 slots - 2 samples per audio cycle (a.k.a stereo)
#define XPORTS_I2S__SLOT_COUNT__4_PER_CYCLE        4  // 4 slots - 4 samples per audio cycle (TDM I4S)
#define XPORTS_I2S__SLOT_COUNT__6_PER_CYCLE        4  // 6 slots - 6 samples per audio cycle (TDM I6S)
#define XPORTS_I2S__SLOT_COUNT__8_PER_CYCLE        8  // 8 slots - 8 samples per audio cycle (TDM I8S)
#define XPORTS_I2S__SLOT_COUNT__16_PER_CYCLE      16  // 16 slots - 16 samples per audio cycle (TDM I16S)
#define XPORTS_I2S__SLOT_COUNT__32_PER_CYCLE      32  // 32 slots - 32 samples per audio cycle (TDM I32S)

#define XPORTS_I2S__SYNC_WORD_FORMAT__PCM_STYLE    0  // Left=High/Right=Low word-clock formatting on WCLK/LRCLK line
#define XPORTS_I2S__SYNC_WORD_FORMAT__I2S_STYLE    1  // Left=Low/Right=High (1 bit-clock skew left) word-clock formatting on WCLK/LRCLK line
#define XPORTS_I2S__SYNC_WORD_FORMAT__TDM_STYLE    2  // TDM sync pulse (1 bit-clock skew left) per audio cycle on WCLK/LRCLK line

#define XPORTS_I2S__WORD_SIZE__16_BIT             16  // 16-bit sample word size, padded with zero's to slot size
#define XPORTS_I2S__WORD_SIZE__24_BIT             24  // 24-bit sample word size, padded with zero's to slot size
#define XPORTS_I2S__WORD_SIZE__32_BIT             32  // 32-bit sample word size, padded with zero's to slot size

#define XPORTS_I2S__WORD_FORMAT__LEFT_JUST         0  // Left justfied sample word, justified to left side of slot word
#define XPORTS_I2S__WORD_FORMAT__RIGHT_JUST        1  // Right justified sample word, justified to right side of slot word

#define XPORTS_I2S__DATA_WIRE_COUNT__0             0  // No data wire for PCM/I2S bus (doesn't use a port)
#define XPORTS_I2S__DATA_WIRE_COUNT__1             1  // One data wire for PCM/I2S bus (uses one 1-bit port)
#define XPORTS_I2S__DATA_WIRE_COUNT__2             2  // Two data wires for PCM/I2S bus (uses two 1-bit ports)
#define XPORTS_I2S__DATA_WIRE_COUNT__3             3  // Three data wires for PCM/I2S bus (uses three 1-bit ports)
#define XPORTS_I2S__DATA_WIRE_COUNT__4             4  // Four data wires for PCM/I2S bus (uses a one 4-bit port or four 1-bit ports)

// ====================================================================================================================
// XPORTS Protocol Options for Each I2C Instance
// ====================================================================================================================

#define XPORTS_I2C__OPTIMIZATION_MODE__SPEED       0
#define XPORTS_I2C__OPTIMIZATION_MODE__SIZE        1

#define XPORTS_I2C__DATA_LOOPBACK_MODE__OFF        0  // Data is not looped back
#define XPORTS_I2C__DATA_LOOPBACK_MODE__ON         1  // DAC data is looped back to ADC data (ADC sampled values are overwritten)

#define XPORTS_I2C__BUS_CLOCKING_MODE__MASTER      0  // Generate BCLK and WCLK signals (outputs)
#define XPORTS_I2C__BUS_CLOCKING_MODE__SLAVE       1  // Sample BCLK and WCLK signals (inputs)

#define XPORTS_I2C__DATA_BIT_RATE__CUSTOM          0
#define XPORTS_I2C__DATA_BIT_RATE__100_KBPS        1
#define XPORTS_I2C__DATA_BIT_RATE__400_KBPS        2

#define XPORTS_I2C__RESTART_CONDITIONS__DISABLED   0
#define XPORTS_I2C__RESTART_CONDITIONS__ENABLED    1

#define XPORTS_I2C__CLOCK_STRETCHING__DISABLED     0
#define XPORTS_I2C__CLOCK_STRETCHING__ENABLED      1

#define XPORTS_I2C__COLLISION_DETECTION__DISABLED  0
#define XPORTS_I2C__COLLISION_DETECTION__ENABLED   1

// ====================================================================================================================
// XDEVICE Protocol Options for Each CODEC Instance
// ====================================================================================================================

#define XDEVICE_CODEC__CS4270_I2C_ADDRESS_1  0b01001100
#define XDEVICE_CODEC__CS4270_I2C_ADDRESS_2  0b01001101
#define XDEVICE_CODEC__CS4270_I2C_ADDRESS_3  0b01001110
#define XDEVICE_CODEC__CS4270_I2C_ADDRESS_4  0b01001111

// ====================================================================================================================
// XDEVICE Protocol Options for Each RADIO Instance
// ====================================================================================================================

// ====================================================================================================================
// XAUDIO Instance Assignments
// ====================================================================================================================

#define XIO_AUDIO__INTERFACE_STANDARD        1
#define XIO_AUDIO__INTERFACE_UAC2            2

#define XIO_AUDIO__OUTPUT_CHANNEL_COUNT_2    2
#define XIO_AUDIO__OUTPUT_CHANNEL_COUNT_32   32

#define XIO_AUDIO__INPUT_CHANNEL_COUNT_2     2
#define XIO_AUDIO__INPUT_CHANNEL_COUNT_32    32

// ====================================================================================================================
// ====================================================================================================================

#endif
