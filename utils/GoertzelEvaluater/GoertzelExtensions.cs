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

                writer.WriteLine("GoertzelFrequeciesBlock const block{0} = {{ {1}, {2}, {3}, {4},block{0}Freqs }};",
                                    block,
                                    freq.Fs,
                                    freq.N,
                                    Program.FS / freq.Fs,
                                    freq.Frequencies.Length
                                );

                block++;
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
            var filterMiddlePoint = (smallerFrequency + biggestFrequency) / 2;
            var neededBandwidth = (biggestFrequency - smallerFrequency);
            var originBandwidth = neededBandwidth*2;

            var w0 = 2*Math.PI*originBandwidth/Program.FS;
            Console.WriteLine("Filter values for band pass filter between {0} and {1} with fc: {2}",smallerFrequency,biggestFrequency,filterMiddlePoint);
            Func<int, double> hx = idx => (w0/Math.PI)*sinc(w0*idx/Math.PI) * 2*Math.Cos(2 * Math.PI * (filterMiddlePoint / Program.FS) * idx );

                
            for(int i = -(Program.FILTER_POINTS / 2) +1; i < Program.FILTER_POINTS/2; ++i)
            {
                var hi = hx(i);
                Console.WriteLine("h({0}) = {1}",i,hi);
                block.FilterValues.Add(hi);
            }



        }

        private static double sinc(double value)
        {
            if (value == 0)
                return 1;
            if (value == 1 || value == 2)
                return 0;
            return Math.Sin(value)/value;
        }
    }
}
