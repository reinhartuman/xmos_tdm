/*
 * @ModuleName IEC 61883-6/AVB1722 Audio over 1722 AVB Transport.
 * @Description: Audio Codec TDM Ctl module.
 *
 */
 
#ifndef __tdm_h__
#define __tdm_h__

#include "avb_conf.h"
#include "media_fifo.h"
#include <xclib.h>
//#include <simple_printf.h>
#include <assert.h>
#include <xscope.h>
#include <xports-i2s.h>

#define AVB_SINGLE_CORE_FIFO_SUPPORT 1

#define AVB_AUDIO_IF_SW_LOOPBACK 0
#define LLVM_COMPILER_UNROLL_WORKAROUND 1

#ifndef CLOCKS_PER_CHANNEL
#define CLOCKS_PER_CHANNEL 32
#endif

#ifndef TDM_NUM_CHANNELS
#define TDM_NUM_CHANNELS 8
#endif

typedef struct i2s_ports_t {
  clock mclk;
  clock bclk;
  in port p_mclk;
  out buffered port:32 p_bclk;
  out buffered port:32 p_lrclk;
} i2s_ports_t;

// Each sample will be right aligned, with left 32-RESOLUTION bits zeroed out
// This exactly matches the 1722 packet format
#define RESOLUTION 24

#ifdef GEN_TEST_SIGNAL
extern unsigned samples[];
#endif

#ifdef CHECK_TEST_SIGNAL
extern unsigned prev_rx_samples[];
#endif

#ifdef CHECK_HW_LOOPBACK
extern unsigned hw_loopback_samples[];
#endif

#define XSTREAME_AUDIO_IF 1


void tdm_master_multi_configure_ports(const clock mclk,
        clock bclk,
        out buffered port:32 p_bclk,
        out buffered port:32 p_wclk,
        out buffered port:32 ?p_dout[],
        int num_out,
        in buffered port:32 ?p_din[],
        int num_in);

/** Input and output audio data using TDM format with the XCore acting 
 as master.

 This function implements a thread that can handle up to 8 channels on
 per wire. It inputs and outpus 24-bit data.

 The function will take input from the TDM interface and put the samples 
 directly into shared memory media input FIFOs. The output samples are 
 received over a channel. Every two word clock periods (i.e. once a
 sample) a timestamp is sent from this thread over the channel 
 and num_out samples are taken from the channel.

 This function can handle up to 8in and 8out at 48KHz. 

 TODO: correct params list
  \param b_mck      clock block that clocks the system clock of the codec
  \param p_mck      the input system clock port
  \param p_bck      clock block that clocks the bit clock; configured 
                    within the i2s_master function
  \param p_bck      the port to output the bit clock to
  \param p_wck      the port to output the word clock to
  \param p_dout     array of ports to output data to
  \param p_din      array of ports to input data from
  \param num_channels     number of input and output ports
  \param c_samples_to_dac  chanend connector to a listener component
  \param input_fifos           a map from the inputs to local talker streams.
                               The channels of the inputs are interleaved,
							   for example, if you have two input ports, the map
                               {0,1,0,1} would map to the two stereo local
                               talker streams 0 and 1.
  \param media_ctl the media control channel
  \param clk_ctl_index the index of the clk_ctl channel array that
                       controls the master clock fo the codec
 */
#pragma unsafe arrays
static inline void tdm_master_multi_upto_32(
		xports_i2s__context_t &audio_if,
        int num_chan_out,
        int num_chan_in,
        int master_to_word_clock_ratio,
        streaming chanend ?c_samples_to_dac,
#if XSTREAME_SHARED_MEM_IF
        chanend ?c_sync,
#else
        streaming chanend ?c_samples_from_adc,
#endif
        media_input_fifo_t ?input_fifos[]
        )  // Both in and out
{
    unsigned timestamp;
    timer tmr;
    unsigned int active_fifos;

#ifdef CHECK_TEST_SIGNAL
    unsigned check_active=0;
#endif

#if XSTREAME_SHARED_MEM_IF
    unsigned dac_buffer[AVB_NUM_MEDIA_OUTPUTS];
    unsigned adc_buffer[2][AVB_NUM_MEDIA_INPUTS]; // double_buffer

	unsigned int dac_buffer_address;
	unsigned int adc_buffer_address[2]; //double buffer
	unsigned int adc_buffer_idx = 0;  //

	// init pointers
	asm("mov %0, %1":"=r"(dac_buffer_address):"r"(dac_buffer));
	asm("mov %0, %1":"=r"(adc_buffer_address[0]):"r"(adc_buffer[0]));
	asm("mov %0, %1":"=r"(adc_buffer_address[1]):"r"(adc_buffer[1]));
#endif


	xports_i2s__initialize(audio_if);
#ifdef UART_COMMS
	uart_tdm_multiplier <: audio_if.mclk_bclk_ratio; // Send the calculated BCLK_MCLK ratio through to the UART
#endif

#if 0 // not sure why this is needed
    if(num_chan_out>0) {
        c_samples_to_dac <: 0;
        for (int n=0;n<num_chan_out;n++) {
            int x;
            c_samples_to_dac :> x;
        }
    }
#endif

    while (1) {
        active_fifos = media_input_fifo_enable_req_state();
        tmr :> timestamp;

        //if(num_chan_out>0) {
          c_samples_to_dac <: timestamp;

#if XSTREAME_SHARED_MEM_IF
          // get all samples
#pragma loop unroll
#ifdef LLVM_COMPILER_UNROLL_WORKAROUND
          for(int i=0; i<AVB_NUM_MEDIA_OUTPUTS; i++) {
#else
          for(int i=0; i<num_chan_out; i++) {
#endif
        		  c_samples_to_dac :> dac_buffer[i];
          }
#endif

#if XSTREAME_SHARED_MEM_IF
		xports_i2s__transfer(audio_if, dac_buffer_address, adc_buffer_address[adc_buffer_idx]);
#else
#if !AVB_SINGLE_CORE_FIFO_SUPPORT
		c_samples_from_adc <: timestamp; //signal next block of samples
#endif
		xports_i2s__transfer_chan(audio_if, c_samples_to_dac, c_samples_from_adc);
#endif

		// Notify the input fifo interface core.
		// Send buffere address and timestamp
#if XSTREAME_SHARED_MEM_IF
		master {
#if AVB_AUDIO_IF_SW_LOOPBACK
		c_sync <: dac_buffer_address;
#else
		c_sync <: adc_buffer_address[adc_buffer_idx];
#endif
		c_sync <: timestamp;
		};
		// switch double buffer
		adc_buffer_idx = 1-adc_buffer_idx;
#endif

		//simple_printf("Sample Period Finished at timestamp %d\n",timestamp);
        media_input_fifo_update_enable_ind_state(active_fifos, 0xFFFFFFFF);
    }
}


#pragma unsafe arrays
static inline void tdm_master_multi(xports_i2s__context_t &audio_if,
                              int num_in,
                              int num_out,
                              int master_to_word_clock_ratio,
                              media_input_fifo_t ?input_fifos[],
                              media_output_fifo_t ?output_fifos[],
                              chanend media_ctl,
                              int clk_ctl_index
                              )
{
    media_ctl_register(media_ctl,
                       input_fifos, num_in,
                       output_fifos, num_out,
                       clk_ctl_index);
    /*media_ctl_register(media_ctl,
                     num_in, input_fifos,
                     num_out, output_fifos,
                     clk_ctl_index);*/

#if XSTREME_DOES_PORT_INIT
  tdm_master_multi_configure_ports(ports.mclk,
                             ports.bclk,
                             ports.p_bclk,
                             ports.p_lrclk,
                             p_dout,
                             num_out>>1,
                             p_din,
                             num_in>>1);
#endif

  {
    streaming chan c_samples_to_dac;
#if XSTREAME_SHARED_MEM_IF
    chan c_sync;
#else
    streaming chan c_samples_from_adc;
#endif
    par {

#if AVB_SINGLE_CORE_FIFO_SUPPORT
    	media_input_output_fifo_support_upto_16ch(c_samples_to_dac, c_samples_from_adc,
    			output_fifos, input_fifos);
#else
      if (num_out > 0)
      {
    	  media_output_fifo_to_xc_channel(c_samples_to_dac,
                                               output_fifos,
                                               num_out);
      }

#if XSTREAME_SHARED_MEM_IF
      media_input_fifo_stuffer(c_sync, input_fifos, num_in);
#else
      media_input_fifo_stuffer_chan(c_samples_from_adc, input_fifos, num_in);
#endif
#endif

      tdm_master_multi_upto_32(
    		  audio_if,
    		  num_out,
    		  num_in,
    		  master_to_word_clock_ratio,
    		  c_samples_to_dac,
#if XSTREAME_SHARED_MEM_IF
              c_sync,
#else
              c_samples_from_adc,
#endif
    		  null
    		  );
    }
  }

}


// loop back inputs to outputs - for testing
void tdm_loopback(clock b_mck, in port p_mck, out port p_bck, 
                  out buffered port:4 p_wck,
                  in buffered port:32 p_din, out buffered port:32 p_dout);


#endif
