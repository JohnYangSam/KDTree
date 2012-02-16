/**********************************************************
 * File: grid.h
 * Author: Keith Schwarz (htiek@cs.stanford.edu)
 *
 * Implementation of a two-dimensional grid class.  This is
 * based on the implementation from the course reader with a
 * minor fix that corrects a bug that only appears in
 * grid<bool>s.
 */

#ifndef Grid_Included
#define Grid_Included

#include <vector>
#include <algorithm>
using namespace std;

template <typename ElemType> class grid {
public:
	/* Constructors either create an empty grid or a grid of the
	 * specified size.
	 */
	grid();
	grid(size_t numRows, size_t numCols);

	/* Resizing functions. */
	void clear();
	void resize(size_t width, size_t height);

	/* Size queries. */
	size_t numRows() const;
	size_t numCols() const;
	bool empty() const;
	size_t size() const;

	/* Element access. */
	typename vector<ElemType>::reference getAt(size_t row, size_t col);
	typename vector<ElemType>::const_reference getAt(size_t row, size_t col) const;

	/* Iterator definitions. */
	typedef typename vector<ElemType>::iterator iterator;
	typedef typename vector<ElemType>::const_iterator const_iterator;

	/* Container iteration. */
	iterator begin();
	iterator end();
	const_iterator begin() const;
	const_iterator end() const;

	/* Row iteration. */
	iterator row_begin(size_t row);
	iterator row_end(size_t row);
	const_iterator row_begin(size_t row) const;
	const_iterator row_end(size_t row) const;

	/* Proxy objects for operator[] */
	class MutableReference;
	class ImmutableReference;

	/* Element selection functions. */
	MutableReference operator[](size_t row);
	ImmutableReference operator[](size_t row) const;

	/* Relational operators. */
	bool operator < (const grid& other) const;
	bool operator <= (const grid& other) const;
	bool operator == (const grid& other) const;
	bool operator != (const grid& other) const;
	bool operator >= (const grid& other) const;
	bool operator > (const grid& other) const;

private:
	vector<ElemType> elems;
	size_t rows;
	size_t cols;
};

/* MutableReference and ImmutableReference implementations call back into the
 * grid that created them to index correctly.
 */
template <typename ElemType> class grid<ElemType>::MutableReference {
public:
	friend class grid;
	typename vector<ElemType>::reference operator[](size_t col);
private:
	MutableReference(grid* source, size_t row);
	grid * ref;
	size_t row;
};
template <typename ElemType> class grid<ElemType>::ImmutableReference {
public:
	friend class grid;
	typename vector<ElemType>::const_reference operator[](size_t col) const;
private:
	ImmutableReference(const grid* source, size_t row);
	const grid * ref;
	size_t row;
};

/* Constructs a new, empty grid. */
template <typename ElemType> grid<ElemType>::grid() : rows(0), cols(0) {
  // Handled in initializer list
}

/* Constructs a grid of the specified size. */
template <typename ElemType>
grid<ElemType>::grid(size_t rows, size_t cols) :
	elems(rows * cols), rows(rows), cols(cols) {
  // Handled in initializer list
}

/* Empties the grid. */
template <typename ElemType> void grid<ElemType>::clear() {
	elems.clear();
}

/* Discards all existing content and redimensions the grid. */
template <typename ElemType> void grid<ElemType>::resize(size_t rows, size_t cols) {
	elems.assign(rows * cols, ElemType());
	this->rows = rows;
	this->cols = cols;
}

/* Size query functions. */
template <typename ElemType> size_t grid<ElemType>::numRows() const {
	return rows;
}

template <typename ElemType> size_t grid<ElemType>::numCols() const {
	return cols;
}

/* The grid is empty if the size is zero. */
template <typename ElemType> bool grid<ElemType>::empty() const {
	return size() == 0;
}

/* The size of the grid is the product of the number of rows and columns. */
template <typename ElemType> size_t grid<ElemType>::size() const {
	return rows * cols;
}

/* Accessor member functions use the row-major order index conversions to access
 * elements.
 */
template <typename ElemType> typename vector<ElemType>::reference grid<ElemType>::getAt(size_t row, size_t col) {
	return elems[col + row * cols];
}

template <typename ElemType>
typename vector<ElemType>::const_reference grid<ElemType>::getAt(size_t row, size_t col) const {
	return elems[col + row * cols];
}

/* Iterator support, including const overloads. */
template <typename ElemType>
typename grid<ElemType>::iterator grid<ElemType>::begin() {
	return elems.begin();
}

template <typename ElemType>
typename grid<ElemType>::const_iterator grid<ElemType>::begin() const {
	return elems.begin();
}

template <typename ElemType>
typename grid<ElemType>::iterator grid<ElemType>::end() {
	return elems.end();
}

template <typename ElemType>
typename grid<ElemType>::const_iterator grid<ElemType>::end() const {
	return elems.end();
}

/* Row iteration uses the row-major order formula to select the proper elements. */
template <typename ElemType>
typename grid<ElemType>::iterator grid<ElemType>::row_begin(size_t row) {
	return elems.begin() + row * cols;
}

template <typename ElemType>
typename grid<ElemType>::const_iterator grid<ElemType>::row_begin(size_t row) const {
	return elems.begin() + row * cols;
}

template <typename ElemType>
typename grid<ElemType>::iterator grid<ElemType>::row_end(size_t row) {
	return row_begin(row) + cols;
}

template <typename ElemType>
typename grid<ElemType>::const_iterator grid<ElemType>::row_end(size_t row) const {
	return row_begin(row) + cols;
}

/* Implementation of the MutableReference and ImmutableReference classes. */
template <typename ElemType>
grid<ElemType>::MutableReference::MutableReference(grid* source, size_t row) :
	ref(source), row(row) {
  // Handled in initializer list
}

template <typename ElemType>
grid<ElemType>::ImmutableReference::ImmutableReference(const grid* source, size_t row) :
	ref(source), row(row) {
  // Handled in initializer list
}

/* operator[] calls back into the original grid. */
template <typename ElemType>
typename vector<ElemType>::reference grid<ElemType>::MutableReference::operator [](size_t col) {
	return ref->getAt(row, col);
}
template <typename ElemType>
typename vector<ElemType>::const_reference grid<ElemType>::ImmutableReference::operator [](size_t col) const {
	return ref->getAt(row, col);
}
/* operator[] implementations create and return (Im)MutableReferences. */
template <typename ElemType>
typename grid<ElemType>::MutableReference grid<ElemType>::operator[](size_t row) {
	return MutableReference(this, row);
}
template <typename ElemType>
typename grid<ElemType>::ImmutableReference grid<ElemType>::operator[](size_t row) const {
	return ImmutableReference(this, row);
}

/* operator< performs a lexicographical comparison of two grids. */
template <typename ElemType>
bool grid<ElemType>::operator <(const grid& other) const {
	/* Lexicographically compare by dimensions. */
	if(rows != other.rows)
		return rows < other.rows;
	if(cols != other.cols)
		return cols < other.cols;

	/* Perform a lexicographical comparison. */
	return lexicographical_compare(begin(), end(), other.begin(), other.end());
}

/* Other relational operators defined in terms of operator<. */
template <typename ElemType>
bool grid<ElemType>::operator >=(const grid& other) const {
	return !(*this < other);
}
template <typename ElemType>
bool grid<ElemType>::operator ==(const grid& other) const {
	return !(*this < other) && !(other < *this);
}
template <typename ElemType>
bool grid<ElemType>::operator !=(const grid& other) const {
	return (*this < other) || (other < *this);
}
template <typename ElemType>
bool grid<ElemType>::operator >(const grid& other) const {
	return other < *this;
}
template <typename ElemType>
bool grid<ElemType>::operator <= (const grid& other) const {
	return !(other < *this);
}

#endif

