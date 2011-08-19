/* ***************************************************************
   CifArray.h: Basic array class template header file.
  
        Adapted and modified from array.h 
           Practical Data Structures in C++
           Bryan Flamig, Azarona Software/John Wiley & Sons, Inc
 * ***************************************************************/

#include "range.h"
#ifndef H_ARRAY
#define H_ARRAY

#define INLINE

template <class TYPE>
class CifArray {
 protected:
  TYPE *data;   // Pointer to actual data
  unsigned len; // Logical length.

 public:
  CifArray(TYPE *m, unsigned n);
  virtual ~CifArray() {};
  virtual void CopyN(const TYPE *s, unsigned int n);
  void Copy(const CifArray<TYPE> &s) { CopyN(s.data, s.len);}
  CifArray<TYPE> &operator=(const CifArray<TYPE> &s);

#ifndef NO_RANGE_CHECK
  unsigned CheckIndex(unsigned i) const;
#endif

  TYPE &operator[](unsigned i);
  const TYPE &operator[](unsigned i) const; 
  unsigned Length() const { return len; } 
  virtual unsigned DimLength() const { return len; }
  TYPE *Data() { return data;} 
  const TYPE *Data() const { return data;}
};

template <class TYPE>
INLINE CifArray<TYPE>::CifArray(TYPE *m, unsigned n)
: data(m), len(n) 
// ---------------------------------------------------------------
//  This constructor wraps an array object around the
//  data pointed to by m, of length n.
// ---------------------------------------------------------------
{ 
}
template <class TYPE>
INLINE CifArray<TYPE> &CifArray<TYPE>::operator=(const CifArray<TYPE> &s)
// ---------------------------------------------------------------
//  This constructor wraps an array object around the
//  data pointed to by m, of length n.
// ---------------------------------------------------------------
{ 
  if (this != &s) Copy(s); 
  return *this; 
}
template <class TYPE>
INLINE  TYPE &CifArray<TYPE>::operator[](unsigned i)
// ---------------------------------------------------------------
//  This constructor wraps an array object around the
//  data pointed to by m, of length n.
// ---------------------------------------------------------------
{ 
  return  data[Check(i)]; 
}

template <class TYPE>
INLINE  const TYPE &CifArray<TYPE>::operator[](unsigned i) const
// ---------------------------------------------------------------
//  This constructor wraps an array object around the
//  data pointed to by m, of length n.
// ---------------------------------------------------------------
{ 
  return  data[Check(i)]; 
}

#undef INLINE

#ifdef INCL_TEMPLATE_SRC
#include "CifArray.C"
#endif

#endif

