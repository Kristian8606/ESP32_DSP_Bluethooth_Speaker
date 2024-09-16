#include <math.h>
#include <stdio.h>
//uint8_t fs = 44100;
#define SAMPLE_RATE (44100)

//#define M_PI 3.14159265358979323846
/*
Filter Settings file

Room EQ V5.31.3
Dated: Sep 14, 2024 2:15:35 PM

Notes:a

Equaliser: Generic
L+R Sep 14
Filter  1: ON  PK       Fc   85.30 Hz  Gain  -3.30 dB  Q  5.000
Filter  2: ON  PK       Fc   101.0 Hz  Gain  -7.30 dB  Q  4.998
Filter  3: ON  PK       Fc   143.5 Hz  Gain  -9.20 dB  Q  3.252
Filter  4: ON  PK       Fc   230.0 Hz  Gain  -5.50 dB  Q  4.999
Filter  5: ON  PK       Fc   305.0 Hz  Gain   3.00 dB  Q  1.133
Filter  6: ON  PK       Fc   328.0 Hz  Gain  -1.90 dB  Q  4.999
Filter  7: ON  PK       Fc   486.0 Hz  Gain  -3.20 dB  Q  3.592
Filter  8: ON  PK       Fc   626.0 Hz  Gain  -2.90 dB  Q  4.237
Filter  9: ON  PK       Fc    1451 Hz  Gain  -6.10 dB  Q  1.006
Filter 10: ON  PK       Fc    2885 Hz  Gain  -6.60 dB  Q  1.529
Filter 11: ON  PK       Fc    4607 Hz  Gain  -7.80 dB  Q  1.385
Filter 12: ON  PK       Fc    7652 Hz  Gain  -5.70 dB  Q  1.342


*/
enum {
    LP = 0,
    HP,
    BP,
    Notch,
    PK,
    LS,
    HS
};


int type_filters[] = { LP
                      ,LP
                      ,HP
                      ,HP
                      
                      ,PK
                      ,LS
                      ,PK
                      ,PK
                      ,PK
                      ,HS
                      ,PK
                     
};
double Hz[] = {150.0
              ,150.0
              ,200.0
              ,200.0
              
              ,96.80
              ,117.0
              ,212.0
			  , 2577
			  , 8668
			  , 8882
	          ,14204
	        
};

double dB[] = { 0.0
              , 0.0
              , 0.0
              , 0.0
              
              ,-11.00
              ,  6.00
              , -4.80
              , -5.30
              , -9.70
              ,  7.40
              , -3.60
             
};

double Qd[] = { 0.54119610
              , 1.3065630
              , 0.54119610
              , 1.3065630
              
              , 1.738
              , 0.0
              , 4.988
              , 1.185
              , 1.067
              , 0.0
              , 1.662
             
};
/*
int type_filters[] = { PK
                      ,PK
                      ,LS
                      ,PK
                      ,PK
                      ,PK
                      ,PK
                      ,PK
                      ,PK
                      ,PK
                      ,PK
                                         
};
double Hz[] = {  48.90
              ,  71.20
              ,  87.10
              ,  92.10
              ,  118.5
              ,  147.5
              ,  165.0
              ,  342.0
              ,   1338
              ,  10159
              ,  37
             
};

double dB[] = {  -5.80
              , -11.00
              ,   5.60
              ,  -6.90
              ,  -5.80
              ,  -5.70
              ,  -6.00
              ,  -4.50
              ,  -3.70
              ,  -4.20
              ,  12
             
};

double Qd[] = {   8.522
              ,   5.412
              ,   0.0
              ,   6.204
              ,  13.465
               , 18.259
               , 15.018
               ,  3.622
               ,  1.000
               ,  1.038
               ,  10
             
};
*/
static struct iir_filt {
	float in_z1_st;
    float in_z2_st;
    float out_z1_st;
    float out_z2_st;
    float in_z1;
    float in_z2;
    float out_z1;
    float out_z2;
    float a0;
    float a1;
    float a2;
    float b1;
    float b2;
    char* type;
};

void bq_print_info(struct iir_filt* bq){
	printf("A0: %13.16f\n",bq->a0);
	printf("A1: %13.16f\n",bq->a1);
	printf("A2: %13.16f\n",bq->a2);
	printf("B1: %13.16f\n",bq->b1);
	printf("B2: %13.16f\n",bq->b2);
	printf("TYPE: %s\n",bq->type);
	printf("\n");
}
int length = (sizeof(Hz) / sizeof(Hz[0]));

struct iir_filt iir_coeff[11];

 static void calcBiquad(int type, double Fc, double peakGain,  double Q, struct iir_filt* config) {

  //   double a0 = 1.0;
   //  double a1 = 0.0, a2 = 0.0, b1 = 0.0, b2 = 0.0;
     
		 config->in_z1 = 0.0;
		 config->in_z2 = 0.0;
		 config->out_z1 = 0.0;
		 config->out_z2 = 0.0;
		 
		 config->in_z1_st = 0.0;
		 config->in_z2_st = 0.0;
		 config->out_z1_st = 0.0;
		 config->out_z2_st = 0.0;



     double norm;
     double V = pow(10, fabs(peakGain) / 20.0);
     double K = tan(M_PI * Fc);
     switch (type) {
     case LP:
         norm = 1 / (1 + K / Q + K * K);
         config->a0 = K * K * norm;
         config->a1 = 2 * config->a0;
         config->a2 = config->a0;
         config->b1 = 2 * (K * K - 1) * norm;
         config->b2 = (1 - K / Q + K * K) * norm;
         config->type = "LOWPASS";
         break;

     case HP:
         norm = 1 / (1 + K / Q + K * K);
        config->a0 = 1 * norm;
        config->a1 = -2 * config->a0;
        config->a2 = config->a0;
        config->b1 = 2 * (K * K - 1) * norm;
        config->b2 = (1 - K / Q + K * K) * norm;
         config->type = "HIGHPASS";
         break;

     case BP:
         norm = 1 / (1 + K / Q + K * K);
        config->a0 = K / Q * norm;
        config->a1 = 0;
        config->a2 = - config->a0;
        config->b1 = 2 * (K * K - 1) * norm;
        config->b2 = (1 - K / Q + K * K) * norm;
         config->type = "BANDPASS";
         break;

     case Notch:
         norm = 1 / (1 + K / Q + K * K);
         config->a0 = (1 + K * K) * norm;
         config->a1 = 2 * (K * K - 1) * norm;
         config->a2 = config->a0;
         config->b1 = config->a1;
         config->b2 = (1 - K / Q + K * K) * norm;
         config->type = "NOTCH";
         break;

     case PK:
         if (peakGain >= 0) {    // boost
             norm = 1 / (1 + 1 / Q * K + K * K);
             config->a0 = (1 + V / Q * K + K * K) * norm;
             config->a1 = 2 * (K * K - 1) * norm;
             config->a2 = (1 - V / Q * K + K * K) * norm;
             config->b1 = config->a1;
             config->b2 = (1 - 1 / Q * K + K * K) * norm;
         }
         else {    // cut
             norm = 1 / (1 + V / Q * K + K * K);
             config->a0 = (1 + 1 / Q * K + K * K) * norm;
             config->a1 = 2 * (K * K - 1) * norm;
             config->a2 = (1 - 1 / Q * K + K * K) * norm;
             config->b1 = config->a1;
             config->b2 = (1 - V / Q * K + K * K) * norm;
         }
         config->type = "PEAK";
         break;
     case LS:
         if (peakGain >= 0) {    // boost
             norm = 1 / (1 + sqrt(2) * K + K * K);
            config->a0 = (1 + sqrt(2 * V) * K + V * K * K) * norm;
            config->a1 = 2 * (V * K * K - 1) * norm;
            config->a2 = (1 - sqrt(2 * V) * K + V * K * K) * norm;
            config->b1 = 2 * (K * K - 1) * norm;
            config->b2 = (1 - sqrt(2) * K + K * K) * norm;
         }
         else {    // cut
             norm = 1 / (1 + sqrt(2 * V) * K + V * K * K);
            config->a0 = (1 + sqrt(2) * K + K * K) * norm;
            config->a1 = 2 * (K * K - 1) * norm;
            config->a2 = (1 - sqrt(2) * K + K * K) * norm;
            config->b1 = 2 * (V * K * K - 1) * norm;
            config->b2 = (1 - sqrt(2 * V) * K + V * K * K) * norm;
         }
         config->type = "LOWSHELF";
         break;
     case HS:
         if (peakGain >= 0) {    // boost
             norm = 1 / (1 + sqrt(2) * K + K * K);
             config->a0 = (V + sqrt(2 * V) * K + K * K) * norm;
             config->a1 = 2 * (K * K - V) * norm;
             config->a2 = (V - sqrt(2 * V) * K + K * K) * norm;
             config->b1 = 2 * (K * K - 1) * norm;
             config->b2 = (1 - sqrt(2) * K + K * K) * norm;
         }
         else {    // cut
             norm = 1 / (V + sqrt(2 * V) * K + K * K);
            config->a0 = (1 + sqrt(2) * K + K * K) * norm;
            config->a1 = 2 * (K * K - 1) * norm;
            config->a2 = (1 - sqrt(2) * K + K * K) * norm;
            config->b1 = 2 * (K * K - V) * norm;
            config->b2 = (V - sqrt(2 * V) * K + K * K) * norm;
         }
         config->type = "HIGHSHELF";
         break;
     }
 }

 static float process_iir_mono(float inSampleF, struct iir_filt* config) {
     float outSampleF =
	config->a0 * inSampleF
	+ config->a1 * config->in_z1
	+ config->a2 * config->in_z2
	- config->b1 * config->out_z1
	- config->b2 * config->out_z2;
	config->in_z2 = config->in_z1;
	config->in_z1 = inSampleF;
	config->out_z2 = config->out_z1;
	config->out_z1 = outSampleF;
	return outSampleF;
 }
 static float process_iir_ch_1 (float inSampleF, struct iir_filt * config) {
         
		 
	  float outSampleF =
	config->a0 * inSampleF
	+ config->a1 * config->in_z1
	+ config->a2 * config->in_z2
	- config->b1 * config->out_z1
	- config->b2 * config->out_z2;
	config->in_z2 = config->in_z1;
	config->in_z1 = inSampleF;
	config->out_z2 = config->out_z1;
	config->out_z1 = outSampleF;
	return outSampleF;
}
 static float process_iir_ch_2 (float inSampleF, struct iir_filt * config) {
 		
	  float outSampleF =
	config->a0 * inSampleF
	+ config->a1 * config->in_z1_st
	+ config->a2 * config->in_z2_st
	- config->b1 * config->out_z1_st
	- config->b2 * config->out_z2_st;
	config->in_z2_st = config->in_z1_st;
	config->in_z1_st = inSampleF;
	config->out_z2_st = config->out_z1_st;
	config->out_z1_st = outSampleF;
	return outSampleF;
}

 static void create_biquad() {
      
     for (int i = 0; i < length; i++){
       //  printf("type iir filter %d  %fHz, dB %f, Q %f\n", type_filters[i], Hz[i], dB[i], Qd[i]);
         calcBiquad(type_filters[i], Hz[i] /SAMPLE_RATE, dB[i], Qd[i], &iir_coeff[i]);
         bq_print_info(&iir_coeff[i]);
     }
   
 }
 
 static void process_data_mono(uint8_t * data, size_t item_size) {
	
	int16_t * samples = (int16_t *) data;
	int16_t * outsample = (int16_t *) data;
	
	for (int i=0; i<item_size; i=i+4) {
		//restore input samples and make monosum
		float insample = (float) *samples;
		samples++;
		insample += *samples;
		insample = insample / 2;
		samples++;
		
		//monosum now available in insample
		
		//process bass speaker
		 
		float lowsample = process_iir_mono(insample, &iir_coeff[0]);
		lowsample = process_iir_mono(lowsample, &iir_coeff[1]);
		
		lowsample = process_iir_mono(lowsample, &iir_coeff[4]);
	    lowsample = process_iir_mono(lowsample, &iir_coeff[5]);
	    lowsample = process_iir_mono(lowsample, &iir_coeff[6]);
	 	
	 

		//process tweeter lowpass
		float highsample = process_iir_mono(insample, &iir_coeff[2]);
		highsample = process_iir_mono(highsample, &iir_coeff[3]);
		highsample = process_iir_mono(highsample, &iir_coeff[7]);
	    highsample = process_iir_mono(highsample, &iir_coeff[8]);
        highsample = process_iir_mono(highsample, &iir_coeff[9]);
        highsample = process_iir_mono(highsample, &iir_coeff[10]);
		//restore two outputsamples lowsample & highsample to outputbuffer
		*outsample = (int16_t) lowsample; // *0.9;
		outsample++;
		*outsample = (int16_t) highsample * 0.5;
		outsample++;		
		
	
	}

}

static void process_data_stereo(uint8_t * data, size_t item_size) {
	
	int16_t * samples = (int16_t *) data;
	int16_t * outsample = (int16_t *) data;
	
	for (int i=0; i<item_size; i=i+4) {
	
		//restore input samples and make monosum
		float insample = (float) *samples;
		samples++;
		float insample2 = (float) *samples;
		samples++;
		
		
		float sample_ch_1 = process_iir_ch_1(insample, &iir_coeff[0]);
			  sample_ch_1 = process_iir_ch_1(sample_ch_1, &iir_coeff[1]);
			  sample_ch_1 = process_iir_ch_1(sample_ch_1, &iir_coeff[2]);
			  sample_ch_1 = process_iir_ch_1(sample_ch_1, &iir_coeff[3]);
			  sample_ch_1 = process_iir_ch_1(sample_ch_1, &iir_coeff[4]);
			  sample_ch_1 = process_iir_ch_1(sample_ch_1, &iir_coeff[5]);
			  sample_ch_1 = process_iir_ch_1(sample_ch_1, &iir_coeff[6]);
			  sample_ch_1 = process_iir_ch_1(sample_ch_1, &iir_coeff[7]);
			  sample_ch_1 = process_iir_ch_1(sample_ch_1, &iir_coeff[8]);
			  sample_ch_1 = process_iir_ch_1(sample_ch_1, &iir_coeff[9]);
			  sample_ch_1 = process_iir_ch_1(sample_ch_1, &iir_coeff[10]);
			  

		
		float sample_ch_2 = process_iir_ch_2(insample2, &iir_coeff[0]);
		      sample_ch_2 = process_iir_ch_2(sample_ch_2, &iir_coeff[1]);
		      sample_ch_2 = process_iir_ch_2(sample_ch_2, &iir_coeff[2]);
		      sample_ch_2 = process_iir_ch_2(sample_ch_2, &iir_coeff[3]);
		      sample_ch_2 = process_iir_ch_2(sample_ch_2, &iir_coeff[4]);
		      sample_ch_2 = process_iir_ch_2(sample_ch_2, &iir_coeff[5]);
		      sample_ch_2 = process_iir_ch_2(sample_ch_2, &iir_coeff[6]);
		      sample_ch_2 = process_iir_ch_2(sample_ch_2, &iir_coeff[7]);
		      sample_ch_2 = process_iir_ch_2(sample_ch_2, &iir_coeff[8]);
		      sample_ch_2 = process_iir_ch_2(sample_ch_2, &iir_coeff[9]);
		      sample_ch_2 = process_iir_ch_2(sample_ch_2, &iir_coeff[10]);
		      

		*outsample = (int16_t) sample_ch_1;// *0.9;
		outsample++;
		*outsample = (int16_t) sample_ch_2;// *0.9;
		outsample++;	
	
	}

}

static void set_sample_rate (uint8_t samplerate) {
	 
}

/*
static float process(float in) {
    double out = in * a0 + z1;
    z1 = in * a1 + z2 - b1 * out;
    z2 = in * a2 - b2 * out;
    printf("process %f\n",out);
    return out;
}

*/