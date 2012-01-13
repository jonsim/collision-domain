#ifndef SMOOTHER_H
#define SMOOTHER_H

#include <vector>

template <class T>
class Smoother
{
private:

  //this holds the history
  std::vector<T>  m_History;

  int           m_iNextUpdateSlot;

  //an example of the 'zero' value of the type to be smoothed. This
  //would be something like Ogre::Vector3(0, 0, 0)
  T             m_ZeroValue;

public:

  //to instantiate a Smoother pass it the number of samples you want
  //to use in the smoothing, and an exampe of a 'zero' type
  Smoother(int SampleSize, T ZeroValue):m_History(SampleSize, ZeroValue),
                                        m_ZeroValue(ZeroValue),
                                        m_iNextUpdateSlot(0)
  {}

  //each time you want to get a new average, feed it the most recent value
  //and this method will return an average over the last SampleSize updates
  T Update(const T& MostRecentValue)
  {
    //overwrite the oldest value with the newest
    m_History[m_iNextUpdateSlot++] = MostRecentValue;

    //make sure m_iNextUpdateSlot wraps around.
    if (m_iNextUpdateSlot == m_History.size())
    	m_iNextUpdateSlot = 0;

    //now to calculate the average of the history list
    T sum = m_ZeroValue;

    typename std::vector<T>::const_iterator it;;

    for (it = m_History.begin(); it != m_History.end(); ++it)
    {
      sum += *it;
    }

    return sum / (double)m_History.size();
  }
};


#endif
