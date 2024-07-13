#include <app_priv.h>

extern uint8_t fs;



struct iir_filt {
   float in_z1;
   float in_z2;
   float out_z1;
   float out_z2;
   float a0[4];
   float a1[4];
   float a2[4];
   float b1[4];
   float b2[4];  
};
/*

//bass-speaker, 30Hz Hipass
static struct iir_filt conf_45_hp = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = { 0.9917041170065917, 0.9958434200204267 , 0.9969821645939272, 0.9972270234220063 },
	.a1 = { -1.9834082340131833, -1.9916868400408534, -1.9939643291878544,  -1.9944540468440126 },
	.a2 = { 0.9917041170065917, 0.9958434200204267, 0.9969821645939272, 0.9972270234220063 },
	.b1 = { -1.9833394124635135, -1.9916695631391037, -1.993955222010733, -1.9944463575772033 },
	.b2 = { 0.9834770555628534, 0.991704116942603, 0.9939734363649758, 0.9944617361108218 },
	
}; 	





//bass-speaker, 2500Hz Lowpass
static struct iir_filt conf_2k5_lp = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = { 0.13993146179027674, 0.044278036805267616, 0.025175362450974036, 0.021620113635254866 },
	.a1 = { 0.2798629235805535, 0.08855607361053523, 0.05035072490194807, 0.04324022727050973 },
	.a2 = { 0.13993146179027674, 0.044278036805267616, 0.025175362450974036, 0.021620113635254866 } ,
	.b1 = { -0.699698900564656, -1.3228374096880198, -1.50365042037159, -1.5430779694435248 },
	.b2 = { 0.259424747725763, 0.4999495569090904, 0.6043518701754859, 	0.6295584239845442}
	
}; 
*/	

//bass-speaker, 150Hz Lowpass///////////////////////
static struct iir_filt conf_150Hz_lp = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = { 0.0008325330343748603, 0.00021242182071631172, 0.00011247970644584286, 0.00009505983285468104 },
	.a1 = { 0.0016650660687497205, 0.00042484364143262343, 0.0002249594128916857, 0.00019011966570936208 },
	.a2 = { 0.0008325330343748603, 0.00021242182071631172, 0.00011247970644584286, 0.00009505983285468104 } ,
	.b1 = { -1.9167296538189462, -1.9583477764103403, -1.9697741272631184, -1.9722296503999952 },
	.b2 = { 0.9200597859564457, 0.9591974636932055, 0.9702240460889018, 	0.9726098897314139}
	
}; 	

//bass-speaker, eq 60Hz +5db, q=2.0, bass fun factor :-)
static struct iir_filt conf_60_eq = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = { 1.0045571784556593, 1.0022854380342285, 1.00165972312044, 1.0015251377673262 },
	.a1 = { -1.9877372051598552, -1.9939885650528055, -1.995661969908465, -1.9960191810845285 },
	.a2 = { 0.9837319156746053, 0.991841509165585, 0.994075168233407, 0.9945556071485654 },
	.b1 = { -1.9877372051598552, -1.9939885650528055, -1.995661969908465, -1.9960191810845285 },
	.b2 = { 0.9882890941302647, 0.9941269471998133, 0.9957348913538471, 0.9960807449158914 }
	
}; 	

//tweeter 250 Hz Hipass
static struct iir_filt conf_250_hp = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = {0.9329230232468055,0.7056967416492775,0.9751244546803259,0.9771222901052568,},
	.a1 = {-1.865846046493611,-1.411393483298555,-1.9502489093606519,-1.9542445802105135,},
	.a2 = {0.9329230232468055,0.7056967416492775,0.9751244546803259,0.9771222901052568,},
	.b1 = {-1.8613429252631009,-1.32283740968802,-1.9496302031626966,-1.9537212766076175,},
	.b2 = {0.8703491677241215,0.4999495569090903,0.9508676155586074,0.9547678838134095,}
	 
};

//tweeter 100 Hz Lolself
static struct iir_filt conf_100_l_self = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = {0.9727615373228193,0.9862793521926011,0.9900242515869623,0.9908309133725992,},
	.a1 = {-1.891513075659393,-1.945173404803105,-1.960101041631071,-1.9633200693530277,},
	.a2 = {0.920210951522782,0.9592690755875621,0.9702757509529032,0.9726573727853123,},
	.b1 = {-1.8893377679813477,-1.9446144196101878,-1.9598044832632138,-1.9630693361743101,},
	.b2 = {0.8951477965236466,0.9461074129730804,0.9605965609077227,0.9637390193366289,}
	 
};

static struct iir_filt conf_2000hz_noch = {
	//index0 = 16k, index1 = 32k, index 2 = 44k1, index 3 = 48k
	.a0 = {  0.6666331051985078, 0.7870056430245259, 0.8341636066681521, 0.8452797115980594},
	.a1 = { -0.9427615784986199, -1.4541968111224874, -1.6010522977285138, -1.6329550077414818},
	.a2 = { 0.6666331051985078, 0.7870056430245259, 0.8341636066681521, 0.8452797115980594},
	.b1 = { -0.9427615784986199, -1.4541968111224874, -1.6010522977285138, -1.6329550077414818},
	.b2 = { 0.3332662103970156, 0.574011286049052, 0.6683272133363043, 0.690559423196119}
	
}; 	

static float process_iir (float inSampleF, struct iir_filt * config) {
	float outSampleF =
	(* config).a0[fs] * inSampleF
	+ (* config).a1[fs] * (* config).in_z1
	+ (* config).a2[fs] * (* config).in_z2
	- (* config).b1[fs] * (* config).out_z1
	- (* config).b2[fs] * (* config).out_z2;
	(* config).in_z2 = (* config).in_z1;
	(* config).in_z1 = inSampleF;
	(* config).out_z2 = (* config).out_z1;
	(* config).out_z1 = outSampleF;
	return outSampleF;
}

static void process_data (uint8_t * data, size_t item_size) {
	
	int16_t * samples = (int16_t *) data;
	int16_t * outsample = (int16_t *) data;
	
	for (int i=0; i<item_size; i=i+4) {
		//restore input samples and make monosum
		float insample = (float) *samples;
		samples++;
		insample += *samples;
		samples++;
		//monosum now available in insample
		
		//process bass speaker
		 //lowsample = process_iir(insample, &conf_45_hp);
		//lowsample = process_iir(lowsample, &conf_2k5_lp);
		float lowsample = process_iir(insample, &conf_150Hz_lp);
		//lowsample = process_iir(lowsample, &conf_120Hz_lp);
		lowsample = process_iir(lowsample, &conf_60_eq);
		//process tweeter
		float highsample = process_iir(insample, &conf_250_hp);
		
		highsample = process_iir(highsample, &conf_100_l_self);
		//highsample = process_iir(highsample, &conf_180_hp);
		highsample = process_iir(highsample, &conf_2000hz_noch);
		
		
		
		//restore two outputsamples lowsample & highsample to outputbuffer
		*outsample = (int16_t) lowsample;
		outsample++;
		*outsample = (int16_t) highsample;
		outsample++;		
	
	}

}

static void set_sample_rate (uint8_t samplerate) {
	fs=samplerate;
	 
}

