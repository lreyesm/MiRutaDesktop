#include "firfilter.h"

FirFilter::FirFilter(unsigned int array_size)
{

    m_array_size = array_size;
    array_filtered = new double[array_size + filter_order];
    coeficients = new double[filter_order];
}

double *FirFilter::filter(double *array_to_filter){
    for(unsigned int j = 0;j < m_array_size + filter_order; j++)
    {
      array_filtered[j] = 0;
      for(unsigned int i = 0;i < filter_order;i++)
      {
        array_filtered[j] += array_to_filter[i] * coeficients[j-i];
      }
    }
    return array_filtered;
}
