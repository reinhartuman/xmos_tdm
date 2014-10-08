#ifndef XPORTS_I2S
#define XPORTS_I2S

#include "xstream-types.h"
#include "xstream-options.h"
#include "xstream-settings.h"

// ====================================================================================================================
// Driver Overview
// ====================================================================================================================

// This driver supports most combininatons of sampling frequency, slot counts and sizes, word sizes and formmating, and
// ADC/DAC wire counts ...
//
// Slot Count:     Supports 2, 4, 6, 8, 16, and 32 words per audio cycle.  A slot count of two is standard two-word (left
//                 and right) stereo PCM/I2S.
//
// Slot Size:      Supports 16, 24 and 32 bit-clock cycles per sample word slot.
//
// Word Size:      Supports 16, 24 and 32-bit signed 2's compliment sample words.
//
// Frame Sync:     Supports PCM left/right clock (aligned to sample word), I2S left/right clock (aligned with sample word
//                 minus one bit-clock cycle), and TDM frame-sync pulse.
//
// Word Format:    Supports left-justified MSB-first and right-justified MSB-first sample word alignment.
//
// Sampleing Rate: Supports single (44100 KHz, 48000 KHz), double (88200 KHz, 96000 KHz), quad (176400 KHz, 192000 KHz),
//                 and octave (352800 KHz, 384000 KHz) sampling rates.
//
// Master Clock:   Supports internaly generated MCLK (derived from the XS1 system clock) or an external MCLK.
////
// Most combininatons of sampling frequency, slot counts and sizes, word sizes and formmating, and ADC/DAC wire counts
// are supported.  Unsupported combinations are detected at compile-time (via checking of the preprocessor #defines used
// to define the configration).  Invalid combinitations are either due to either the lack of implementation (not done yet)
// or to the innability of the XS1 core to meet timing/performance requirements.
//
// Valid slot count and slot size combininations (slot count / slot size): 2/16 2/32 4/16 4/24 4/32 6/16 6/32
//                                                                         8/16 8/24 8/32 16/16 16/24 16/32
//                                                                         32/16 32/24 32/32
//
// Valid slot size and word size comibinations (slot size / word size): 32/32 32/24 32/16 24/24 24/16 16/16
//
// Valid sample frequency, port width, slot size and DAC/ADC wire count combinations:
//
//    Sample Freq   Port Width   Slot Size   DAC/ADC Wires
//    single        1            16          1/2/3/4
//    single        1            24          1/2/3/4
//    single        1            32          1/2/3/4
//    double        1            16          1/2/3/4
//    double        1            24          1/2/3
//    double        1            32          1/2/3/4
//    quad          1            16          1/2/3/4
//    quad          1            24          1/2
//    quad          1            32          1/2/3/4
//    octave        1            16          1/2/3/4
//    octave        1            24          1
//    octave        1            32          1/2/3/4

// ====================================================================================================================
// Driver Instance Context
// ====================================================================================================================

typedef struct
{
	clock                clock_mclk;
	clock                clock_bclk;

	#if XPORTS_I2S__MASTER_CLOCK_SOURCE == XPORTS_I2S__MASTER_CLOCK_SOURCE__EXTERNAL
	in port              pin_mclk;
	#elif XPORTS_I2S__MASTER_CLOCK_SOURCE == XPORTS_I2S__MASTER_CLOCK_SOURCE__INTERNAL
	out port             pin_mclk;
	#endif

	#if XPORTS_I2S__BUS_CLOCKING_MODE == XPORTS_I2S__BUS_CLOCKING_MODE__MASTER
	out buffered port:32 pin_bclk;
	out buffered port:32 pin_wclk;
	#elif XPORTS_I2S__BUS_CLOCKING_MODE == XPORTS_I2S__BUS_CLOCKING_MODE__SLAVE
	in buffered port:32  pin_bclk;
	in buffered port:32  pin_wclk;
	#endif

	#if XPORTS_I2S__DAC_DATA_WIRE_COUNT > 0
	out buffered port:32 pin_dac[XPORTS_I2S__DAC_DATA_WIRE_COUNT];
	#endif

	#if XPORTS_I2S__ADC_DATA_WIRE_COUNT > 0
	in  buffered port:32 pin_adc[XPORTS_I2S__ADC_DATA_WIRE_COUNT];
	#endif

	unsigned int first_pass;
	unsigned int mclk_bclk_ratio;
}
xports_i2s__context_t;

// ====================================================================================================================
// Driver Public Interface
// ====================================================================================================================

// Should be performed before any other API function call.

void xports_i2s__initialize( xports_i2s__context_t& self );

// To be called to when MCLK or the sampling frequency need to change.

void xports_i2s__configure ( xports_i2s__context_t& self, unsigned int mclk_frequency, unsigned int sample_rate );

// Pack sample data into byte stream approrpriate for DAC output

#if XPORTS_I2S__DATA_FORMAT_MODE == XPORTS_I2S__DATA_FORMAT_MODE__EXTERNAL

	void xports_i2s__pack  ( xports_i2s__context_t& self, unsigned int buffer_address,  unsigned int samples_address );

#endif

// Perform sample data transfer (one sample for each PCM/I2S left/right or TDM slot) for one audio cycle

void xports_i2s__transfer  ( xports_i2s__context_t& self, unsigned int dac_buffer_address, unsigned int adc_buffer_address );

void xports_i2s__transfer_chan( xports_i2s__context_t& self, streaming chanend c_dac_samples, streaming chanend c_adc_samples );

// Unpack sample data from byte stream read from ADC input

#if XPORTS_I2S__DATA_FORMAT_MODE == XPORTS_I2S__DATA_FORMAT_MODE__EXTERNAL

	void xports_i2s__unpack( xports_i2s__context_t& self, unsigned int samples_address, unsigned int buffer_address  );

#endif

// ====================================================================================================================
// ====================================================================================================================

#endif
