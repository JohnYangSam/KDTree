/******************************************************************************
 * File: Point.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * A class representing a point in N-dimensional space.  Unlike the other class
 * templates you've seen before, Point is parameterized over an integer rather
 * than a type.  This allows the compiler to verify that the type is being used
 * correctly.
 */
#ifndef POINT_INCLUDED
#define POINT_INCLUDED

#include <cmath>


template <size_t N>
class Point {
public:
    /**
     * double& operator[] (size_t index);
     * double operator[] (size_t index);
     * Usage: myPoint[3] = 137;
     * -------------------------------------------------------------------------
     * Queries or retrieves the value of the point at a particular point.  The
     * index is assumed to be in-range.
     */
    double& operator[] (size_t index);
    double operator[] (size_t index) const;

    /**
     * size_t size() const;
     * Usage: for (size_t i = 0; i < myPoint.size(); ++i)
     * -------------------------------------------------------------------------
     * Returns N, the dimension of the point.
     */
    size_t size() const;

    /**
     * Type: iterator
     * Type: const_iterator
     * ------------------------------------------------------------------------
     * Types representing iterators that can traverse and optionally modify the
     * elements of the Point.
     */
    typedef double* iterator;
    typedef const double* const_iterator;

    /**
     * iterator begin();
     * iterator end();
     * const_iterator begin();
     * const_iterator end();
     * Usage: for (Point<3>::iterator itr = myPoint.begin(); itr != myPoint.end(); ++itr)
     * ------------------------------------------------------------------------
     * Returns iterators delineating the full range of elements in the Point.
     */
    iterator begin();
    iterator end();
    
    const_iterator begin() const;
    const_iterator end() const;

private:
    /*
     * The point's actual coordinates are stored in an array.
     */
    double mPoints[N];
};

/*
 * double Distance(const Point<N>& one, const Point<N>& two);
 * Usage: double d = Distance(one, two);
 * ----------------------------------------------------------------------------
 * Returns the Euclidean distance between two points.
 */
template <size_t N>
double Distance(const Point<N>& one, const Point<N>& two);

/*
 * bool operator==(const Point<N>& one, const Point<N>& two);
 * bool operator!=(const Point<N>& one, const Point<N>& two);
 * Usage: if (one == two)
 * ----------------------------------------------------------------------------
 * Returns whether two points are equal or not equal.
 */
template <size_t N>
bool operator==(const Point<N>& one, const Point<N>& two);

template <size_t N>
bool operator!=(const Point<N>& one, const Point<N>& two);


////////////////////////////////////////
// Point class implementation details //
////////////////////////////////////////

#include <algorithm>

/*
 * Element access operators just read the array at the specified index.
 */
template <size_t N>
double& Point<N>::operator[] (size_t index) {
    return mPoints[index];
}

template <size_t N>
double Point<N>::operator[] (size_t index) const {
    return mPoints[index];
}

/*
 * size just returns the template parameter.
 */
template <size_t N>
size_t Point<N>::size() const {
    return N;
}

/*
 * Iterators span the range the same way that our Vector iterators do.
 */
template <size_t N>
typename Point<N>::iterator Point<N>::begin() {
    return mPoints;
}

template <size_t N>
typename Point<N>::const_iterator Point<N>::begin() const {
    return mPoints;
}

template <size_t N>
typename Point<N>::iterator Point<N>::end() {
    return begin() + size();
}

template <size_t N>
typename Point<N>::const_iterator Point<N>::end() const {
    return begin() + size();
}

/*
 * Computing the distance uses the standard distance formula: the square root of
 * the sum of the squares of the differences between matching components.
 */
template <size_t N>
double Distance(const Point<N>& one, const Point<N>& two) {
    double result = 0.0;
    for (size_t i = 0; i < N; ++i)
        result += (one[i] - two[i]) * (one[i] - two[i]);
    
    return sqrt(result);
}

/*
 * Equality is implemented using the equal algorithm, which takes in two ranges and
 * reports whether they contain equal values.
 */
template <size_t N>
bool operator==(const Point<N>& one, const Point<N>& two) {
    return std::equal(one.begin(), one.end(), two.begin());
}

template <size_t N>
bool operator!=(const Point<N>& one, const Point<N>& two) {
    return !(one == two);
}

#endif // POINT_INCLUDED
