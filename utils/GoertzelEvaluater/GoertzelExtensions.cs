using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace GoertzelEvaluater
{
    public static class GoertzelExtensions
    {
        public static void PrintStructDefinitions(this IEnumerable<GoertzelFrequenciesBlock> freqs, TextWriter writer)
        {
            int block = 0;
            writer.WriteLine("#include \"GoertzelStructs.h\"\n");
            foreach (var freq in freqs)
            {
                
                writer.WriteLine("GoertzelFrequency const block{0}Freqs[] = \n{{", block);
                foreach (var goertzelFrequency in freq.Frequencies)
                {
                    writer.WriteLine("\t{{ {0}, {1} }},",
                                            goertzelFrequency.Frequency.ToString().Replace(',', '.'),
                                            goertzelFrequency.Coefficient.ToString().Replace(',', '.')
                                     );
                }
                writer.WriteLine("};");

                writer.WriteLine("double const block{0}filterValues[] = \n{{",block);
                foreach (var filterValue in freq.FilterValues)
                {
                    writer.WriteLine("\t{0},",filterValue.ToString().Replace(",","."));    
                }
                writer.WriteLine("};");

                writer.WriteLine("GoertzelFrequeciesBlock const block{0} = {{ {1}, {2}, {3}, {4},block{0}filterValues,block{0}Freqs }};",
                                    block,
                                    freq.Fs,
                                    freq.N,
                                    Program.FS / freq.Fs,
                                    freq.Frequencies.Length
                                );

                block++;
                writer.Flush();
            }

            writer.WriteLine("GoertzelFrequeciesBlock const blocks[] = {");

            for (int i = 0; i < block; ++i)
            {
                writer.WriteLine("\t &block{0} ,", i);
            }
            writer.WriteLine("};");
            writer.WriteLine("GoertzelFrequeciesBlock** goertzelBlocks = blocks;");
        }


        public static void GenerateFiltersValues(this IEnumerable<GoertzelFrequenciesBlock> blocks)
        {
            foreach (var block in blocks)
            {
                FillBlockFilter(block);
            }
        }


        private static void FillBlockFilter(GoertzelFrequenciesBlock block)
        {
            var smallerFrequency = block.Frequencies.First().Frequency ;
            var biggestFrequency = block.Frequencies.Last().Frequency ;
            var bandwidth = biggestFrequency - smallerFrequency;
            /*/
            var percentage = bandwidth*0.1;
            smallerFrequency -= percentage/2;
            biggestFrequency += percentage/2;
            //*/
            var smallerFrequencyW0 = GetW0(smallerFrequency, Program.FS);

            var biggestFrequencyW0 = GetW0(biggestFrequency, Program.FS);

            
            //Console.WriteLine("\nFilter values for band pass filter between {0} and {1} with fc:",smallerFrequency,biggestFrequency);
            //Console.WriteLine("\nFilter values for {0} ", smallerFrequency);
            GetFilterValues(smallerFrequencyW0, biggestFrequencyW0, block.FilterValues);







        }

        private static double GetW0(double freq, int fs)
        {
            return (2 * Math.PI * freq) / fs;
        }

        private static double LowPassFilter(double w0, int idx)
        {
            return (w0/Math.PI)*sinc((w0/Math.PI)*idx);
        }

        private static double sinc(double value)
        {
            if (value == 0)
                return 1;
            if (value == 1 || value == 2)
                return 0;
            return Math.Sin(Math.PI * value)/(Math.PI * value);
        }
        
        private static double Hanning(int idx, int nrOfPoints)
        {
            return 0.5 * (1 + Math.Cos((2 * Math.PI * idx) / (nrOfPoints-1)));
        }

        private static double Hamming(int idx, int nrOfPoints)
        {
            return 0.54 + 0.46*Math.Cos((2*Math.PI*idx)/(nrOfPoints));
        }
        private static void GetFilterValues(double w0, double w1, List<double> filter)
        {
            for (int i = -(Program.FILTER_POINTS / 2) + 1; i < Program.FILTER_POINTS / 2; ++i)
            {
                var hi = (LowPassFilter(w1, i) - LowPassFilter(w0, i)) * Hanning(i, Program.NR_OF_POINTS);
                Console.WriteLine("h({0}) = {1}", i, hi);
                filter.Add(hi);
            }
        }


    }
}
