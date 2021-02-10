#ifndef FIRFILTER_H
#define FIRFILTER_H


class FirFilter
{
public:
    FirFilter(unsigned int array_size);
    double* filter(double *array_to_filter);
private:
    unsigned int filter_order=4;
    double *coeficients;
    double *array_filtered;
    unsigned int m_array_size;
};

#endif // FIRFILTER_H
