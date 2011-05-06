
typedef struct _WavHeader
{							// Size(bytes)	Description		Contents.							
	char	RIFF[4];		// 4  RIFF     block		      "RIFF"	
	int		FLEN;			// 4  FileSize
	char	WAVE[4];		// 4                          "WAVE"
	char	FMT[4];  		// 4	FormatBlock             "fmt"
	int		blen;			// 4	Length of block 0x10 - length of format block
	short	FTAG;			// 2	Format Tag 1 - MS PCM
	short	CH;				// 2	Channels ( 1-mono, 2-stereo )
	int		fs1;			// 4	Samples/s
	int		byte_sec;		// 4	Bytes/s
	short	byte_blk;		// 2	Bytes/block
	short	bit_samp;		// 2	Bits/sample 
	char	data[4];		// 4	"data"
	int 	D_leng;			// 4	Length of block FileSize - HeaderSize 	
							// Total = 44 
}WavHeader;
typedef WavHeader* WavHeader_t;  
