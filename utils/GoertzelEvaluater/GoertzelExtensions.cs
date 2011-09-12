using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace GoertzelEvaluater
{
    public static class GoertzelExtensions
    {
       
        static readonly string[] EnglishNotation =    new[]{  "C", "C#",     "D","D#",      "E", "F", "F#",      "G",  "G#",      "A", "A#",     "B"  };
        static readonly string[] PortugueseNotation = new[] { "DO","DO#/REb","RE","RE#/MIb","MI","FA","FA#/SOLb","SOL","SOL#/LAb","LA","LA#/SIb","SI" };


        const int LOWEST_FREQUENCY_NOTATION_POSITION = 9;
        const int MINIMUM_SCALE = 1;
        

        private static IEnumerable<Tuple<string,string>> GetNotations()
        {
            var next = LOWEST_FREQUENCY_NOTATION_POSITION;
            var currScale = MINIMUM_SCALE;
            while (true)
            {
                for (int i = next; i < EnglishNotation.Length; ++i)
                {
                    yield return new Tuple<string, string>(string.Format("{0}({1})",EnglishNotation[i],currScale),
                                                           string.Format("{0}({1})",PortugueseNotation[i],currScale));
                }
                next = 0;
                currScale++;
            }
        }

        public static void PrintStructDefinitions(this IEnumerable<GoertzelFrequenciesBlock> freqs, TextWriter writer)
        {
            int block = 0;
            var notations = GetNotations().GetEnumerator();

            writer.WriteLine("#include \"Goertzel.h\"\n");
            
            writer.WriteLine("#define GOERTZEL_NR_OF_FREQUENCIES ({0})",freqs.Sum(bl=>bl.Frequencies.Length));
            writer.WriteLine("#define GOERTZEL_NR_OF_BLOCKS ({0})",freqs.Count());
            writer.WriteLine("#define GOERTZEL_FREQUENCY_MAX_N ({0})",freqs.Max(bl=>bl.N));
            writer.WriteLine("#define GOERTZEL_QUEUE_MAX_BLOCKS (40*5)");
            writer.WriteLine("#define GOERTZEL_CONTROLLER_BUFFER_SIZE (((GOERTZEL_FREQUENCY_MAX_N + (sizeof(double)/sizeof(short))) * GOERTZEL_QUEUE_MAX_BLOCKS))");
            writer.WriteLine("#define GOERTZEL_CONTROLLER_FS ({0})", Program.FS);
            writer.WriteLine("#define GOERTZEL_CONTROLLER_SAMPLES_TYPE {0}", "short");

            writer.WriteLine();
            foreach (var freq in freqs)
            {
               
                writer.WriteLine("SECTION(\".internalmem\") GoertzelFrequency block{0}Freqs[] = \n{{", block);
                foreach (var goertzelFrequency in freq.Frequencies)
                {
                    notations.MoveNext();
                    writer.WriteLine("\t{{ {0}, {1}, \"{2}\", \"{3}\" }},",
                                            goertzelFrequency.Frequency.ToString().Replace(',', '.'),
                                            goertzelFrequency.Coefficient.ToString().Replace(',', '.'),
                                            notations.Current.Item1,
                                            notations.Current.Item2

                                     );
                    
                }
                writer.WriteLine("};");

                writer.WriteLine("SECTION(\".internalmem\") double block{0}filterValues[] = \n{{", block);
                foreach (var filterValue in freq.FilterValues)
                {
                    writer.WriteLine("\t{0},",filterValue.ToString().Replace(",","."));    
                }
                writer.WriteLine("};");

                writer.WriteLine("SECTION(\".internalmem\") GoertzelFrequeciesBlock block{0} = {{ {1}, {2}, {3}, {4},(double*)block{0}filterValues,(GoertzelFrequency*)block{0}Freqs }};",
                                    block,
                                    freq.Fs,
                                    freq.N,
                                    Program.FS / freq.Fs,
                                    freq.Frequencies.Length
                                );

                block++;
                writer.Flush();
            }

            writer.WriteLine("SECTION(\".internalmem\") GoertzelFrequeciesBlock blocks[] = {");

            for (int i = 0; i < block; ++i)
            {
                writer.WriteLine("\t block{0} ,", i);
            }
            writer.WriteLine("};");
            writer.WriteLine("GoertzelFrequeciesBlock* goertzelBlocks = (GoertzelFrequeciesBlock*)blocks;");
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
            
            var realSmallerFrequency = block.Frequencies.Min().Frequency ;
            var realBiggestFrequency = block.Frequencies.Max().Frequency ;
            
            /*/
            var percentage = bandwidth*0.1;
            smallerFrequency -= percentage/2;
            biggestFrequency += percentage/2;
            //*/

            var realBandwidth = realBiggestFrequency - realSmallerFrequency;

            var bandwidthGain = realBandwidth*10/100;

            //var smallerFrequency = realSmallerFrequency + bandwidthGain;
            //var biggestFrequency = realBiggestFrequency - bandwidthGain;
            
            var smallerFrequency = realSmallerFrequency;
            var biggestFrequency = realBiggestFrequency;

            var smallerFrequencyW0 = GetW0(smallerFrequency, Program.FS) ;

            var biggestFrequencyW0 = GetW0(biggestFrequency, Program.FS) ;

            var bandwidth = biggestFrequency - smallerFrequency;

            //Console.WriteLine("\nFilter values for band pass filter between {0} and {1} with fc:",smallerFrequency,biggestFrequency);
            //Console.WriteLine("\nFilter values for {0} ", smallerFrequency);
            GetFilterValues(smallerFrequencyW0, biggestFrequencyW0, block.FilterValues);

            var maxValue = GetFilterMaxValue((bandwidth / 2) + smallerFrequency, block.FilterValues.ToArray(), Program.FS);
            
            List<double> d = new List<double>();

            foreach (var filterValue in block.FilterValues)
            {
                d.Add(filterValue*(1/maxValue));
            }

            block.FilterValues = d;

            //Console.WriteLine(GetFilterMaxValue((bandwidth / 2) + smallerFrequency, block.FilterValues.ToArray(), Program.FS));




        }

        private static double GetFilterMaxValue(double middlePoint, double[] filterCoeffs, int Fs)
        {
            var w = (2*Math.PI * middlePoint)/Fs;
            var accumulator = Complex.Zero;
            for (int k = 0; k < filterCoeffs.Length; k++)
            {
               
                accumulator += filterCoeffs[k]*new Complex(Math.Cos(w*k),-Math.Sin(w*k));
            }

            return Complex.Abs(accumulator);
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
                var hi = (LowPassFilter(w1, i) - LowPassFilter(w0, i)) * Hamming(i, Program.NR_OF_POINTS);
              
                filter.Add(hi);
            }
        }

        public static void AssertEqualCoefficients(this IEnumerable<GoertzelFrequenciesBlock> blocks)
        {

            var x = new LinkedList<dynamic>();

            foreach (var block in blocks)
            {
                foreach (var frequency in block.Frequencies)
                {
                    foreach (var frequenciesBlock in blocks)
                    {
                        GoertzelFrequency frequency1 = frequency;
                        foreach (var goertzelFrequency in frequenciesBlock.Frequencies.Where(freq=>freq.Frequency != frequency1.Frequency))
                        {
                            if (goertzelFrequency.Coefficient == frequency1.Coefficient)
                            {
                                dynamic aux =
                                    new
                                        {
                                            Freq1 = goertzelFrequency.Frequency,
                                            Freq2 = frequency1.Frequency,
                                            K1 = goertzelFrequency.K,
                                            K2 = frequency1.K
                                        };
                                x.AddLast(aux);
                                /*
                                Console.WriteLine("Freq1:{0} & Freq2:{1} | K1:{2}, K2:{3} ",goertzelFrequency.Frequency,
                                                                                           frequency1.Frequency,
                                                                                           (int)goertzelFrequency.K, 
                                                                                           (int)frequency1.K);
                            */
                                 }
                        }
                    }
                }   
            }

            foreach (var match in x.OrderBy(freq => freq.Freq1))
            {
                Console.WriteLine("Freq1:{0}\t Freq2:{1} \t| K1:{2}\t K2:{3} ", match.Freq1,
                                                                                           match.Freq2,
                                                                                           (int)Math.Round(match.K1),
                                                                                           (int)Math.Round(match.K2));
            }
        }

    }
}
