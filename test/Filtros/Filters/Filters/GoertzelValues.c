#include "GoertzelStructs.h"

GoertzelFrequency const block0Freqs[] =
{
	{ 27.5, 1.61803398874989 },
	{ 29.1352, 1.57299463233049 },
	{ 30.8677, 1.52288145852962 },
	{ 32.7032, 1.46718785998182 },
	{ 34.6478, 1.40537146919054 },
	{ 36.7081, 1.33685359251794 },
	{ 38.8909, 1.26103400535143 },
	{ 41.2034, 1.17729258187869 },
	{ 43.6535, 1.08498864045419 },
	{ 46.2493, 0.983493701829211 },
	{ 48.9994, 0.87220068003704 },
	{ 51.9131, 0.750541011507307 },
	{ 55, 0.618033988749895 },
	{ 58.2705, 0.474307674113196 },
	{ 61.7354, 0.319167936733305 },
};
GoertzelFrequeciesBlock const block0 = { 275, 169, 32, 15,block0Freqs };
GoertzelFrequency const block1Freqs[] =
{
	{ 65.4064, 1.46718785998182 },
	{ 69.2957, 1.40536984355741 },
	{ 73.4162, 1.33685359251794 },
	{ 77.7817, 1.2610357787521 },
	{ 82.4069, 1.17729073487114 },
	{ 87.3071, 1.08498672109009 },
	{ 92.4986, 0.983493701829211 },
	{ 97.9989, 0.872198623954223 },
	{ 103.826, 0.750545247127781 },
	{ 110, 0.618033988749895 },
	{ 116.541, 0.474307674113196 },
	{ 123.471, 0.319163425705377 },
	{ 130.813, 0.152635660216125 },
	{ 138.591, -0.0249264643508869 },
	{ 146.832, -0.21281338488139 },
};
GoertzelFrequeciesBlock const block1 = { 550, 150, 16, 15,block1Freqs };
GoertzelFrequency const block2Freqs[] =
{
	{ 155.563, 1.2610393255485 },
	{ 164.814, 1.17728888786206 },
	{ 174.614, 1.08498864045419 },
	{ 184.997, 0.983495691286584 },
	{ 195.998, 0.872196567870268 },
	{ 207.652, 0.750545247127781 },
	{ 220, 0.618033988749895 },
	{ 233.082, 0.474307674113196 },
	{ 246.942, 0.319163425705377 },
	{ 261.626, 0.152635660216125 },
	{ 277.183, -0.0249378874364762 },
	{ 293.665, -0.212824743993789 },
	{ 311.127, -0.40979100102747 },
	{ 329.628, -0.613990874516521 },
	{ 349.228, -0.822799650085379 },
};
GoertzelFrequeciesBlock const block2 = { 1100, 126, 8, 15,block2Freqs };
GoertzelFrequency const block3Freqs[] =
{
	{ 369.994, 0.983495691286584 },
	{ 391.995, 0.872201708078022 },
	{ 415.305, 0.750539952601576 },
	{ 440, 0.618033988749895 },
	{ 466.164, 0.474307674113196 },
	{ 493.883, 0.319169064490026 },
	{ 523.251, 0.152641355543374 },
	{ 554.365, -0.0249321758937832 },
	{ 587.33, -0.212824743993789 },
	{ 622.254, -0.40979100102747 },
	{ 659.255, -0.613985438353575 },
	{ 698.456, -0.822799650085379 },
	{ 739.989, -1.03274111677182 },
	{ 783.991, -1.23926866371561 },
	{ 830.609, -1.43668580578722 },
};
GoertzelFrequeciesBlock const block3 = { 2200, 106, 4, 15,block3Freqs };
GoertzelFrequency const block4Freqs[] =
{
	{ 880, 1.61803398874989 },
	{ 932.328, 1.57299322125469 },
	{ 987.767, 1.52288090312343 },
	{ 1046.5, 1.46719018905845 },
	{ 1108.73, 1.40537106278243 },
	{ 1174.66, 1.3368527428278 },
	{ 1244.51, 1.26103267529985 },
	{ 1318.51, 1.17729119662317 },
	{ 1396.91, 1.08499103965731 },
	{ 1479.98, 0.983490717640743 },
	{ 1567.98, 0.872201708078022 },
	{ 1661.22, 0.750539952601576 },
	{ 1760, 0.618033988749895 },
	{ 1864.66, 0.474302125075554 },
	{ 1975.53, 0.319171883881374 },
};
GoertzelFrequeciesBlock const block4 = { 8800, 179, 1, 15,block4Freqs };
GoertzelFrequency const block5Freqs[] =
{
	{ 2093, 0.152647050869377 },
	{ 2217.46, -0.0249321758937832 },
	{ 2349.32, -0.212824743993789 },
	{ 2489.02, -0.409796591826102 },
	{ 2637.02, -0.613985438353575 },
	{ 2793.83, -0.822807459405852 },
	{ 2959.96, -1.0327460083145 },
	{ 3135.96, -1.23926418042578 },
	{ 3322.44, -1.43668977954882 },
	{ 3520, -1.61803398874989 },
	{ 3729.31, -1.77503091430041 },
	{ 3951.07, -1.89813380784295 },
	{ 4186.01, -1.97670105124705 },
};
GoertzelFrequeciesBlock const block5 = { 8800, 75, 1, 13,block5Freqs };
GoertzelFrequeciesBlock const blocks[] = {
	&block0 ,
	&block1 ,
	&block2 ,
	&block3 ,
	&block4 ,
	&block5 ,
};
GoertzelFrequeciesBlock** goertzelBlocks = blocks;
