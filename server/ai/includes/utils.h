#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <cassert>
#include <iomanip>
#include <OgreVector3.h>

using namespace std;
using namespace Ogre;

enum game_types{
  agent_type  = 1 << 1,
  static_type = 1 << 2,
  camera_type = 1 << 3,
  wall_type   = 1 << 4,
  floor_type  = 1 << 5,
  debug_type  = 1 << 6
};


//Constants
const int MaxInt = (numeric_limits<int>::max)();
const double MaxDouble = (numeric_limits<double>::max)();
const double MinDouble = (numeric_limits<double>::min)();
const float MaxFloat   = (numeric_limits<float>::max)();
const float MinFloat   = (numeric_limits<float>::min)();

const double Pi        = atan(1.0) *4.0;
const double TwoPi     = Pi * 2;
const double HalfPi    = Pi/2;
const double QuarterPi = Pi/4;

/**@brief Truncates a vector to a maximum value */
inline Vector3 Truncate(Ogre::Vector3 vec, double max)
{
    Vector3 retVec = Ogre::Vector3::ZERO;
    if(vec.length() > max)
    {
        retVec = vec.normalisedCopy();
        retVec *= max;
        return retVec;
    }

    return vec;
}

//returns trye if the value is a NaN
template <typename T>
inline bool isNaN(T val)
{
    return val != val;
}

//Returns true if the third parameter is in the range described by the first two
inline bool InRange(double start, double end, double val)
{
      if(start < end)
      {
          if( (val > start) && (val < end) )
              return true;
          else
              return false;
      }
      else
      {
          if( (val < start) && (val > end) )
              return true;
          else
              return false;
      }
}

//Returns the bigger of 2 values
template <class T>
T Maximum(const T& v1, const T& v2)
{
    return v1 > v2 ? v1: v2;
}

//Returns an integer between x and y
inline int RandomInteger(int x, int y)
{
    assert(y>=x && "<RandInt>: y is less than x");
    return rand()%(y-x+1)+x;
}

//Returns a random double between 0 and 1
inline double RandomFloat(void)
{
  return ((rand())/(RAND_MAX+1.0));
}

//Returns a random double between x and y
inline double RandomFloatInRange(float x, float y)
{
  return x + RandomFloat()*(y-x);
}

//Returns a random boolean value
inline bool RandBool()
{
    if(RandomFloat() > 0.5)
        return true;

    return false;
}

//Returns a random double in the range -1 < n < 1
inline double RandomClamped()
{
    return RandomFloat() - RandomFloat();
}

//Returns a random number with a normal distribution.
//See method at http://www.taygeta.com/random/gaussian.html
inline double RandGaussian(double mean = 0.0, double standard_deviation = 1.0)
{
  double x1,x2,w,y1;
  static double y2;
  static int use_last = 0;

  if(use_last)
  {
    y1 = y2;
    use_last = 0;
  }
  else
  {
    do
    {
      x1 = 2.0 * RandomFloat() - 1.0;
      x2 = 2.0 * RandomFloat() - 1.0;
      w = x1  * x1 + x2 * x2;
    }while(w >= 1.0);
    w = sqrt( (-2.0 * log(w)) / w);
    y1 = x1 * w;
    y2 = x2 * w;
    use_last = 1;
  }
  return (mean + y1 * standard_deviation);
}

//Sigmoid....WTF?
inline double Sigmoid(double input, double response = 1.0)
{
  return (1.0 / (1.0+exp(-input/response)));
}

//Return the maximum of three values (adapted for 3d)
template <class T>
inline T MaxOf3(const T&a, const T&b, const T&c)
{
  if(a > b && a > c) return a;
  if(b > a && b > c) return b;
  return c;
}

//Returns the maximum of two values
template <class T>
inline T MaxOf(const T& a, const T& b)
{
  if(a > b) return a;
  return b;
}

//Returns the minimum of two values
template <class T>
inline T MinOf(const T& a, const T& b)
{
  if(a < b) return a;
  return b;
}

//Clamps the first argument between the second two
template <class T, class U, class V>
inline void Clamp(T& arg, const U& minVal, const V& maxVal)
{
  assert( ((double)minVal < (double)maxVal) && "<Clamp>MaxVal < MinVal!");
  if(arg < (T)minVal)
  {
    arg = (T)minVal;
  }
  if(arg > (T)maxVal)
  {
    arg = (T)maxVal;
  }
}

//Rounds a double up or down depending on its value
inline int Rounded(double val)
{
  int integral    = (int)val;
  double mantissa = val - integral;

  if(mantissa < 0.5)
  {
    return integral;
  }
  else
  {
    return integral + 1;
  }
}

//Rounds a double up or down depending on wheter its mantissa is higher or lower than offset
inline int RoundUnderOffset(double val, double offset)
{
  int integral = (int)val;
  double mantissa = val - integral;

  if(mantissa < offset)
  {
    return integral;
  }
  else
  {
    return integral + 1;
  }
}

//Returns the average value from a vector
template <class T>
inline double Average(const std::vector<T>& v)
{
  double average = 0.0;
  for(unsigned int i = 0; i < v.size(); ++i)
  {
    average += (double)v[i];
  }
  return average / (double)v.size();
}

//Returns the SD of a vector
inline double StandardDeviation(const std::vector<double>& v)
{
  double sd      = 0.0;
  double average = Average(v);
  for(unsigned int i = 0; i < v.size(); ++i)
  {
    sd += (v[i] - average)*(v[i] - average);
  }
  sd = sd / v.size();
  return sqrt(sd);
}

#endif
