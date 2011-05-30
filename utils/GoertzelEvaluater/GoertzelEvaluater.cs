using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;

namespace GoertzelEvaluater
{
    public class GoertzelFrequency
    {
        public double Frequency;
        public double Coefficient;
        public double K;
        public double DiffFromLast = 0;


    }

    public class GoertzelFrequenciesBlock
    {

        public void AddFrequency(GoertzelFrequency f)
        {
            _freqs.Add(f);
            NrFrequencys++;
        }

        
        public int Fs;
        public int NrFrequencys;
        public int N;
        public List<double> _lowFilterValues = new List<double>();
        public List<double> _highFilterValues = new List<double>();
        
        
        private List<GoertzelFrequency> _freqs = new List<GoertzelFrequency>();

        public List<double> LowFilterValues { get { return _lowFilterValues; } }
        public List<double> HighFilterValues { get { return _highFilterValues; } }
        public GoertzelFrequency[] Frequencies { get { return _freqs.ToArray(); } }

        public override string ToString()
        {
            var sb =new StringBuilder();
            sb.Append(string.Format("\n\n FS:{0}\t| N:{1}\t| Freqs:{2} | Delta:{3}\n",Fs,N,_freqs.Count,Fs/N));
            foreach (GoertzelFrequency goertzelFrequency in _freqs)
            {
                sb.Append(string.Format("Freq: {0}\t| Coeff:{1} \t |K:{2} \t \t | Diff:{3}\n",
                    goertzelFrequency.Frequency,
                    goertzelFrequency.Coefficient,
                    goertzelFrequency.K,
                    goertzelFrequency.DiffFromLast
                    ));
            }
            return sb.ToString();
        }
    }

    public class GoertzelEvaluater
    {
        private const int MINIMUM_FREQUENCIES_IN_SAMPLE = 1;
        private const int MAXIMUM_FREQUENCIES_IN_SAMPLE = 15;

        public struct FrequencyDiff
        {
            public double Frequency;
            public double DiffFromLast;
        }

        private readonly int _minN;
        private readonly int _maxN;
        private readonly int _fs;
        
        public GoertzelEvaluater(int fs, int maxN, int minN)
        {
            _fs = fs;
            _minN = minN;
            _maxN = maxN;
        }

        private static FrequencyDiff[] CalculateDiff(double[] freqs)
        {
            return freqs.Select((freq, idx) =>
            {
                if (idx == 0)
                    return new FrequencyDiff { Frequency = freq };

                return new FrequencyDiff
                {
                    Frequency = freq,
                    DiffFromLast = freq - freqs[idx - 1]
                };
            }).ToArray();
        }



        public GoertzelFrequenciesBlock[] Evaluate(double[] freqs)
        {
            freqs = freqs.OrderBy(d => d).ToArray();
            var diffs = CalculateDiff(freqs);
            var generatedFrequencies =  GenerateGoertzelFrequencies(diffs);

            CalculateCoefficientFromFrequencies(generatedFrequencies);
            return generatedFrequencies;
        }

        private void CalculateCoefficientFromFrequencies(GoertzelFrequenciesBlock[] generatedFrequenciesBlock)
        {
            foreach (GoertzelFrequenciesBlock goertzelFrequency in generatedFrequenciesBlock)
            {
               
                foreach (GoertzelFrequency freq in goertzelFrequency.Frequencies)
                {
                    freq.Coefficient = (2 * Math.Cos(2 * Math.PI * freq.Frequency / (double)goertzelFrequency.Fs)) ;
                    freq.K = goertzelFrequency.N * freq.Frequency / goertzelFrequency.Fs;
                }
            }

            
        }

        private GoertzelFrequenciesBlock[] GenerateGoertzelFrequencies(FrequencyDiff[] diffs)
        {
            var ret = new List<GoertzelFrequenciesBlock>();
            var currN = _minN;
            var currFs = _fs;
            var lastGoodN = 0;
            var lastGoodFs = 0;
            double delta;

            var exception = false;
            var gf = new GoertzelFrequenciesBlock();
            gf.AddFrequency(new GoertzelFrequency{Frequency = diffs[0].Frequency});

            for (var i = 1; i < diffs.Length; i++)
            {
                CalculateDelta(currFs,currN,out delta);
                //if (delta < diffs[i].DiffFromLast)
                //{
                //    lastGoodFs = currFs;
                //    lastGoodN = currN;
                //}

                if (delta < diffs[i].DiffFromLast /*&& exception*/)
                {
                    gf.AddFrequency(new GoertzelFrequency{Frequency = diffs[i].Frequency,DiffFromLast = diffs[i].DiffFromLast});

                    if (gf.NrFrequencys == MAXIMUM_FREQUENCIES_IN_SAMPLE)
                    {

                        gf.Fs = currFs;
                        gf.N = currN;
                        ret.Add(gf);
                        gf = new GoertzelFrequenciesBlock();
                        currN = _minN;
                        currFs = _fs;
                        exception = false;

                    }
                }
                else
                {
                    try
                    {
                        GetAnotherConfiguration(ref currN, ref currFs, diffs[i]);
                    }
                    catch (InvalidOperationException)
                    {
                        //exception = true;
                        //currN = lastGoodN;
                        //currFs = lastGoodFs;

                    }
                        i = i - 1;
                }
      
            }
            if (gf.NrFrequencys != 0)
            { 
                ret.Add(gf);
                gf.Fs = currFs;
                gf.N = currN;

            }
             
            return ret.ToArray();
        }

        private void GetAnotherConfiguration(ref int currN, ref int fs, FrequencyDiff freq)
        {
            currN++;
            
            if (currN >= _maxN)
            {
                currN = _minN;
                var divisor = (_fs/fs) + 1;
                while (_fs % divisor != 0)
                    divisor++;
                fs = _fs/divisor;

                if (((double)fs / 2) < freq.Frequency)
                    throw new InvalidOperationException("Nyquist not fulfilled");
            }
        }


        private static void CalculateDelta(int fs, int n, out double delta)
        {
            delta = (double)fs/n;

        }
    }
}