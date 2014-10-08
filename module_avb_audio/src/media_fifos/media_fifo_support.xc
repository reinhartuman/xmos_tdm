/*
 * @ModuleName Media FIFO support
 * @Description: Interface tasks to speedup commuication between fifos and audio interface
 *
 *
 */

#include <xs1.h>
#include <xclib.h>
#include <print.h>
#include <stdlib.h>
#include <syscall.h>
#include <xscope.h>
#include "media_fifo.h"
//#include "simple_printf.h"
#include "xscope.h"

/**
 *  \brief Interface task between ififo/ofifo and an audio interface
 *
 *  Especially useful for TDM (4 or 8 channels per dataline)
 *  Where this task is essential to meet the timing
 *  by getting a subset of samples for each sub-slot
 */

//#ifdef TDM_WIRE_LOOPBACK
int channel_lut[16] = {6,7,0,1,2,3,4,5,14,15,8,9,10,11,12,13};
//#endif

void media_input_output_fifo_support_upto_16ch(streaming chanend samples_out,
			streaming chanend c_samples_from_adc,
            media_output_fifo_t ?output_fifos[],
            media_input_fifo_t ?input_fifos[]
            )
  {
	 unsigned int active_fifos;
 	 while (1) {
 		 unsigned int size;
 		 unsigned timestamp;
 		 samples_out :> timestamp;
 		 active_fifos = media_input_fifo_enable_req_state();

 		 // channels 0..AVB_AUDIO_IF_SAMPLES_PER_PERIOD on sdata_out[0],
 		 // channels AVB_AUDIO_IF_SAMPLES_PER_PERIOD..(2*AVB_AUDIO_IF_SAMPLES_PER_PERIOD-1) on sdata_out[1]
 		 // etc, etc
 		 // The same assignment on in
#pragma loop unroll
 		 for (int i=0;i<AVB_AUDIO_IF_SAMPLES_PER_PERIOD;i++) {
		     unsigned sample;
		     int fifo_idx;
 			 for(int j=0; j<AVB_NUM_AUDIO_SDATA_OUT; j++) {
                 fifo_idx = (i+j*AVB_AUDIO_IF_SAMPLES_PER_PERIOD);
 				 sample = media_output_fifo_pull_sample(output_fifos[fifo_idx],
 						 timestamp);
 				 samples_out <: sample;
 				 if(fifo_idx == 0) {
 				     //xscope_int(0, sext(24,sample));
 				 }
 			 }
 			 for(int j=0; j<AVB_NUM_AUDIO_SDATA_IN; j++) {
 				c_samples_from_adc :> sample;

 				fifo_idx = (i+j*AVB_AUDIO_IF_SAMPLES_PER_PERIOD);
//#ifdef TDM_WIRE_LOOPBACK
                // Wire loopback requires rotation of the input channel alignment (there is an implicit 32 bit delay due to the shiftreg)
 				fifo_idx = channel_lut[fifo_idx];
//#endif
 				if(fifo_idx == 0) {
                    //xscope_int(1, sext(24,sample));
 				}
 				if (active_fifos & (1 << fifo_idx)) {
 					media_input_fifo_push_sample(input_fifos[fifo_idx], sample, timestamp);
 				} else {
 					media_input_fifo_flush(input_fifos[fifo_idx]);
 				}
 			 }
 		 }
 	 }
  }
