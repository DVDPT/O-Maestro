#pragma once

typedef struct _GoertzelFrequency
{
	int frequency;	///< the target frequency.
	double coefficient;	///< the frequency pre-calculated coefficient.
}GoertzelFrequency;


typedef struct _GoertzelFrequeciesBlock
{
	int blockFs;	///< The Sampling Frequency that all "frequencies" should be captured.
	int blockN;	///< The size of the block needed to process the "frequencies".
	int blockFsDivFs; ///< Fs / blockFs
	int blockNrOfFrequencies;	///< The number of frequencies present in "frequencies".
	GoertzelFrequency* frequencies;
}GoertzelFrequeciesBlock;

