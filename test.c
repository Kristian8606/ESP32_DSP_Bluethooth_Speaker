https://esp32.com/viewtopic.php?f=20&t=13532&start=10

short DspBuf[4096];
audio_element_info_t Dsp_info = { 0 };
static esp_err_t Dsp_open(audio_element_handle_t self)
	{
	audio_element_getinfo(self, &Dsp_info);	
	return ESP_OK;
	}

static esp_err_t Dsp_close(audio_element_handle_t self)
	{
	return ESP_OK;
	}

audio_element_err_t Dsp_read(audio_element_handle_t el, char *buf, int len, unsigned int wait_time, void *ctx)
	{
	return (audio_element_err_t)len;
	}

audio_element_err_t Dsp_write(audio_element_handle_t el, char *buf, int len, unsigned int wait_time, void *ctx)
	{
	return (audio_element_err_t)len;
	}


static audio_element_err_t Dsp_process(audio_element_handle_t self, char *inbuf, int len)
	{
	audio_element_input(self, (char *)DspBuf, len);
	int ret = audio_element_output(self, (char *)DspBuf, len);
	return (audio_element_err_t)ret;
	}
static esp_err_t Dsp_destroy(audio_element_handle_t self)
	{
	return ESP_OK;
	}

audio_element_cfg_t DspCfg;// = DEFAULT_AUDIO_ELEMENT_CONFIG();
	memset(&DspCfg, 0, sizeof(audio_element_cfg_t));
	DspCfg.destroy = Dsp_destroy;
	DspCfg.process = Dsp_process;
	DspCfg.read = Dsp_read; // why are these needed if they are not called?
	DspCfg.write = Dsp_write; // why are these needed if they are not called?
	DspCfg.open = Dsp_open;
	DspCfg.close = Dsp_close;
	DspCfg.buffer_len = (1024);
	DspCfg.tag = "Dsp";
	DspCfg.task_stack = (2 * 1024);
	DspCfg.task_prio = (5);
	DspCfg.task_core = (0); // core 1 has FPU issues lol
	DspCfg.out_rb_size = (8 * 1024);

	DspProcessor = audio_element_init(&DspCfg);
	audio_pipeline_register(pipeline, DspProcessor, "DspProcessor");

//и след това го пъхнете в аудио тръбопровода:

const char * boomer[]{ "mp3Decoder","DspProcessor", "i2s" };
audio_pipeline_relink(pipeline, boomer, 3);
audio_pipeline_set_listener(pipeline, evt);

//////////////////////////////////////////////
///////////////////////////////////////////////



// should match bit rate in pipeline?
int16_t DspBuf[4096];

float coeffs_lpf[5];
float w_lpf[5] = {0,0};
#define NNN 4096
float FloatDspBuf[NNN];
float FloatDspBufB[NNN];
const float *pFloatDspBuf = FloatDspBuf;


// ********** PROCESS the buffer with DSP IIR !!

static audio_element_err_t Dsp_process(audio_element_handle_t self, char *inbuf, int len)
{


audio_element_input(self, (char *)DspBuf, len);

// ************* DSP Process DspBuf here ********************************



// Calculate iir filter coefficients ( instead of preset )

// generate low pass filter

float freq = 4000;
float qFactor = 200;

esp_err_t rety = ESP_OK;

rety = dsps_biquad_gen_lpf_f32(coeffs_lpf, freq, qFactor); // low pass
// rety = dsps_biquad_gen_lpf_f32(coeffs_hpf, freq, qFactor); // high pass

if (rety != ESP_OK) { ESP_LOGE(TAG, "Operation error dsps_biquad_gen_lpf_f32 = %i", rety);
return rety; }


// convert 16bit audio smaples to float ****************
for ( int i = 0; i < len; i++ )
{
// do this properly with ESP-DSP maths ?
FloatDspBuf = ((float)DspBuf) / (float)32768;
}

// DSP IIR biquad process
esp_err_t rett = ESP_OK;
rett = dsps_biquad_f32(pFloatDspBuf,FloatDspBufB,len,coeffs_lpf,w_lpf);

if (rett != ESP_OK) { ESP_LOGE(TAG, "Operation error = %i", rett);
return rett; }

// convert float audio samples back into 16bit audio samples for pipeline
for ( int j = 0; j < len; j++ )
{
// do this properly with ESP-DSP maths ?
FloatDspBufB[j] = FloatDspBufB[j] * 32768 ;
if( FloatDspBufB[j] > 32767 ) FloatDspBufB[j] = 32767;
if( FloatDspBufB[j] < -32768 ) FloatDspBufB[j] = -32768;
DspBuf[j] = (int16_t)FloatDspBufB[j]; // cast back
}

/// ************* END DSP Process ********************************

// DspBuf samples back into pipeline

int ret = audio_element_output(self, (char *)DspBuf, len);
return (audio_element_err_t)ret;
}


