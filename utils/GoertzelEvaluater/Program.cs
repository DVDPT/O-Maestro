﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace GoertzelEvaluater
{
    class Program
    {
        private const int FS = 8800;
        private const int MAX_N = 200;
        private const int MIN_N = 200;

        static void Main(string[] args)
        {
            var ge = new GoertzelEvaluater(FS,MAX_N,MIN_N);
            GoertzelFrequenciesBlock[] freqs = ge.Evaluate(notes);
            //
            foreach (GoertzelFrequenciesBlock goertzelFrequency in freqs)
            {
                Console.WriteLine(goertzelFrequency);
            }
            //*/
            //PrintFreqs(freqs,Console.Out);

        }

        private static void PrintFreqs(GoertzelFrequenciesBlock[] freqs, TextWriter writer )
        {
            int block = 0;
            foreach (var freq in freqs)
            {

                writer.WriteLine("GoertzelFrequency const block{0}Freqs[] = \n{{", block);
                foreach (var goertzelFrequency in freq.Frequencies)
                {
                    writer.WriteLine("\t{{ {0}, {1} }},",
                                            goertzelFrequency.Frequency.ToString().Replace(',','.'),
                                            goertzelFrequency.Coefficient.ToString().Replace(',','.')
                                     );
                }
                writer.WriteLine("};");

                writer.WriteLine("GoertzelFrequeciesBlock const block{0} = {{ {1}, {2}, {3}, {4},block{0}Freqs }};",
                                    block,
                                    freq.Fs,
                                    freq.N,
                                    FS / freq.Fs,
                                    freq.Frequencies.Length
                                );

                block++;
            }

            writer.WriteLine("GoertzelFrequeciesBlock const blocks[] = {");
            
            for (int i = 0; i < block; ++i)
            {
                writer.WriteLine("\t &block{0} ,",i);
            }
            writer.WriteLine("};");
            writer.WriteLine("GoertzelFrequeciesBlock** goertzelBlocks = blocks;");
        }


        private static double[] notes = {
                                     27.5, //
                                     29.1352,
                                     30.8677,
                                     32.7032,
                                     34.6478,
                                     36.7081,
                                     38.8909,
                                     41.2034,
                                     43.6535,
                                     46.2493,
                                     48.9994,
                                     51.9131,
                                     55.0000,
                                     58.2705,
                                     61.7354,
                                     65.4064,
                                     69.2957,
                                     73.4162,
                                     77.7817,
                                     82.4069,
                                     87.3071,
                                     92.4986,
                                     97.9989,
                                     103.826,
                                     110.000,
                                     116.541,
                                     123.471,
                                     130.813,
                                     138.591,
                                     146.832,
                                     155.563,
                                     164.814,
                                     174.614,
                                     184.997,
                                     195.998,
                                     207.652,
                                     220.000,
                                     233.082,
                                     246.942,
                                     261.626,
                                     277.183,
                                     293.665,
                                     311.127,
                                     349.228,
                                     329.628,
                                     369.994,
                                     391.995,
                                     415.305,
                                     440.000,
                                     466.164,
                                     493.883,
                                     523.251,
                                     554.365,
                                     587.330,
                                     622.254,
                                     659.255,
                                     698.456,
                                     739.989,
                                     783.991,
                                     830.609,
                                     880.000,
                                     932.328,
                                     987.767,
                                     1046.50,
                                     1108.73,
                                     1174.66,
                                     1244.51,
                                     1318.51,
                                     1396.91,
                                     1479.98,
                                     1567.98,
                                     1661.22,
                                     1760.00,
                                     1864.66,
                                     1975.53,
                                     2093.00,
                                     2217.46,
                                     2349.32,
                                     2489.02,
                                     2637.02,
                                     2793.83,
                                     2959.96,
                                     3135.96,
                                     3322.44,
                                     3520.00,
                                     3729.31,
                                     3951.07,
                                     4186.01,
                                 };



    }
}
