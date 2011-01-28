/* ============================================================================
 * Copyright (c) 2010, Michael A. Jackson (BlueQuartz Software)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of Michael A. Jackson nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

#ifndef _STATSGEN_H_
#define _STATSGEN_H_

#if defined (_MSC_VER)
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#endif

#include <math.h>

#include "AIM/Common/Constants.h"

/**
 * @class StatsGen StatsGen.h AIM/StatsGenerator/StatsGen.h
 * @author Michael A. Groeber US Air Force Research Laboratory
 * @date Dec 16, 2010
 * @version 1.0
 */
class StatsGen
{
  public:
    StatsGen();
    virtual ~StatsGen();


    template<typename T>
    int GenBeta(double alpha, double beta, T &x, T &y, int size)
    {
      int err=0;
      double value, gammapq, gammap, gammaq, betain, betaout;
      x.resize(size);
      y.resize(size);
      value = alpha;
      gammap = gamma(value);
      value = beta;
      gammaq = gamma(value);
      value = alpha+beta;
      gammapq = gamma(value);
      for(int i=0;i<size;i++)
      {
        betain = (i*(1.0/double(size)))+((1.0/double(size))/2.0);
        betaout = (gammapq/(gammap*gammaq))*pow(betain,(alpha-1))*pow((1-betain),(beta-1));
        x[i] = betain;
        y[i] = betaout*(1.0/double(size));
        if(betaout < 0) err = 1;
      }
      return err;
    }


    template<typename T>
    int GenODF(T weights, T sigmas, T &x001, T &y001,  T &x011, T &y011, T &x111, T &y111, int size)
    {
      int err=0;
      double value, gammapq, gammap, gammaq, betain, betaout;
      x001.resize(size*3);
      y001.resize(size*3);
      x011.resize(size*6);
      y011.resize(size*6);
      x111.resize(size*4);
      y111.resize(size*4);
      value = alpha;
      gammap = gamma(value);
      value = beta;
      gammaq = gamma(value);
      value = alpha+beta;
      gammapq = gamma(value);
      for(int i=0;i<size;i++)
      {
        betain = (i*(1.0/double(size)))+((1.0/double(size))/2.0);
        betaout = (gammapq/(gammap*gammaq))*pow(betain,(alpha-1))*pow((1-betain),(beta-1));
        x[i] = betain;
        y[i] = betaout*(1.0/double(size));
        if(betaout < 0) err = 1;
      }
      return err;
    }


	template<typename T>
    int GenLogNormal(double avg, double stdDev, T &x, T &y, int size)
    {
      int err = 0;
      double lognormin, lognormout, max, min;
      double s2 = pow(stdDev, 2);
      double root2pi = pow((2.0 * 3.1415926535897), 0.5);
      x.resize(size);
      y.resize(size);
      min = exp(avg - (5 * stdDev));
      max = exp(avg + (5 * stdDev));
      for (int i = 0; i < size; i++)
      {
        lognormin = (i * ((max - min) / double(size))) + (((max - min) / double(size)) / 2.0) + min;
        lognormout = (1.0 / (lognormin * stdDev * root2pi)) * exp(-((log(lognormin) - avg) * (log(lognormin) - avg)) / (2 * s2));
        x[i] = lognormin;
        y[i] = lognormout * ((max - min) / double(size));
        if (lognormout < 0) err = 1;
      }
      return err;
    }


    template<typename T>
    int GenPowerLaw(double alpha, double k, double beta, T &x, T &y, int size)
    {
      int err = 0;
      double in, out, max, min;
      x.resize(size);
      y.resize(size);
      min = 0;
      max = 5;
      for (int i = 0; i < size; i++)
      {
        in = (i * ((max - min) / double(size))) + (((max - min) / double(size)) / 2.0) + min;
        out = alpha*pow(in,k) + beta;
        x[i] = in;
        y[i] = out;
        if (out < 0) err = 1;
      }
      return err;
    }


    /**
     * @brief Calculates the number of bins using 64 bit floating point values
     * @param mu
     * @param sigma
     * @param cutoff
     * @param binstep
     * @return
     */
    int computeNumberOfBins(double mu, double sigma, double cutoff, double binstep, double &max, double &min);

    /**
     * @brief Calculates the number of bins using 32 bit floating point values
     * @param mu
     * @param sigma
     * @param cutoff
     * @param binstep
     * @return
     */
    int computeNumberOfBins(float mu, float sigma, float cutoff, float binstep, float &max, float &min);


    /**
     * @brief Generates the CutOff values
     * @param mu
     * @param sigma
     * @param cutoff
     * @param x
     * @param y
     * @param yMax
     * @param numsizebins
     * @param binsizes
     * @return
     */
    template<typename J, typename T>
    int GenCutOff(J mu, J sigma, J cutoff, J binstep,
                  T &x, T &y, J yMax,
                  int &numsizebins, T &binsizes)
    {
      J max, min;
      numsizebins = computeNumberOfBins(mu, sigma, cutoff, binstep, max, min);
      int err = 0;
      x.resize(2);
      y.resize(2);

      x[0] = min;
      x[1] = max;

      y[0] = 0.0;
      y[1] = yMax;

      binsizes.resize(numsizebins);
      for (int i = 0; i < numsizebins; i++)
      {
        binsizes[i] = min + (i*binstep);
      }
      return err;
    }

    /**
     * @brief
     * @param value
     * @return
     */
    double gamma(double value);


  private:
    StatsGen(const StatsGen&); // Copy Constructor Not Implemented
    void operator=(const StatsGen&); // Operator '=' Not Implemented
};


#endif /* _STATSGEN_H_ */
