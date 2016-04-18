#ifndef TACS_ELEMENT_ALGEBRA_H
#define TACS_ELEMENT_ALGEBRA_H

/*
  A header file with lots of useful linear algebra. Note that this is
  designed to be included in .c/.cpp files directly.

  Copyright (c) 2015 Graeme Kennedy. All rights reserved. 
*/

/*
  Compute y <- A*x

  input:
  A:   the 3x3 input matrix in row-major order
  x:   the input 3-vector

  output:
  y:   the resulting vector 
*/
static inline void matMult( const TacsScalar A[],
			    const TacsScalar x[],
			    TacsScalar y[] ){
  y[0] = A[0]*x[0] + A[1]*x[1] + A[2]*x[2];
  y[1] = A[3]*x[0] + A[4]*x[1] + A[5]*x[2];
  y[2] = A[6]*x[0] + A[7]*x[1] + A[8]*x[2];
}

/*
  Compute C = A*B

  input:
  A:   the first 3x3 input matrix in row-major order
  B:   the second 3x3 input matrix in row-major order

  output:
  C:   the resulting matrix 
*/
static inline void matMatMult( const TacsScalar A[],
			       const TacsScalar B[],
			       TacsScalar C[] ){
  C[0] = A[0]*B[0] + A[1]*B[3] + A[2]*B[6];
  C[3] = A[3]*B[0] + A[4]*B[3] + A[5]*B[6];
  C[6] = A[6]*B[0] + A[7]*B[3] + A[8]*B[6];

  C[1] = A[0]*B[1] + A[1]*B[4] + A[2]*B[7];
  C[4] = A[3]*B[1] + A[4]*B[4] + A[5]*B[7];
  C[7] = A[6]*B[1] + A[7]*B[4] + A[8]*B[7];

  C[2] = A[0]*B[2] + A[1]*B[5] + A[2]*B[8];
  C[5] = A[3]*B[2] + A[4]*B[5] + A[5]*B[8];
  C[8] = A[6]*B[2] + A[7]*B[5] + A[8]*B[8];
}

/*
  Compute C = A*B^{T}

  input:
  A:   the first 3x3 input matrix in row-major order
  B:   the second 3x3 input matrix in row-major order

  output:
  C:   the resulting matrix 
*/
static inline void matMatTransMult( const TacsScalar A[],
				    const TacsScalar B[],
				    TacsScalar C[] ){
  C[0] = A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
  C[3] = A[3]*B[0] + A[4]*B[1] + A[5]*B[2];
  C[6] = A[6]*B[0] + A[7]*B[1] + A[8]*B[2];

  C[1] = A[0]*B[3] + A[1]*B[4] + A[2]*B[5];
  C[4] = A[3]*B[3] + A[4]*B[4] + A[5]*B[5];
  C[7] = A[6]*B[3] + A[7]*B[4] + A[8]*B[5];

  C[2] = A[0]*B[6] + A[1]*B[7] + A[2]*B[8];
  C[5] = A[3]*B[6] + A[4]*B[7] + A[5]*B[8];
  C[8] = A[6]*B[6] + A[7]*B[7] + A[8]*B[8];
}

/*
  Compute C = A^{T}*B

  input:
  A:   the first 3x3 input matrix in row-major order
  B:   the second 3x3 input matrix in row-major order

  output:
  C:   the resulting matrix 
*/
static inline void matTransMatMult( const TacsScalar A[],
				    const TacsScalar B[],
				    TacsScalar C[] ){
  C[0] = A[0]*B[0] + A[3]*B[3] + A[6]*B[6];
  C[1] = A[0]*B[1] + A[3]*B[4] + A[6]*B[7];
  C[2] = A[0]*B[2] + A[3]*B[5] + A[6]*B[8];

  C[3] = A[1]*B[0] + A[4]*B[3] + A[7]*B[6];
  C[4] = A[1]*B[1] + A[4]*B[4] + A[7]*B[7];
  C[5] = A[1]*B[2] + A[4]*B[5] + A[7]*B[8];

  C[6] = A[2]*B[0] + A[5]*B[3] + A[8]*B[6];
  C[7] = A[2]*B[1] + A[5]*B[4] + A[8]*B[7];
  C[8] = A[2]*B[2] + A[5]*B[5] + A[8]*B[8];
}

/*
  Compute C += A*B

  input:
  A:   the first 3x3 input matrix in row-major order
  B:   the second 3x3 input matrix in row-major order

  output:
  C:   the resulting matrix 
*/
static inline void matMatMultAdd( const TacsScalar A[],
				  const TacsScalar B[],
				  TacsScalar C[] ){
  C[0] += A[0]*B[0] + A[1]*B[3] + A[2]*B[6];
  C[3] += A[3]*B[0] + A[4]*B[3] + A[5]*B[6];
  C[6] += A[6]*B[0] + A[7]*B[3] + A[8]*B[6];

  C[1] += A[0]*B[1] + A[1]*B[4] + A[2]*B[7];
  C[4] += A[3]*B[1] + A[4]*B[4] + A[5]*B[7];
  C[7] += A[6]*B[1] + A[7]*B[4] + A[8]*B[7];

  C[2] += A[0]*B[2] + A[1]*B[5] + A[2]*B[8];
  C[5] += A[3]*B[2] + A[4]*B[5] + A[5]*B[8];
  C[8] += A[6]*B[2] + A[7]*B[5] + A[8]*B[8];
}

/*
  Compute C += A*B^{T}

  input:
  A:   the first 3x3 input matrix in row-major order
  B:   the second 3x3 input matrix in row-major order

  output:
  C:   the resulting matrix 
*/
static inline void matMatTransMultAdd( const TacsScalar A[],
				       const TacsScalar B[],
				       TacsScalar C[] ){
  C[0] += A[0]*B[0] + A[1]*B[1] + A[2]*B[2];
  C[3] += A[3]*B[0] + A[4]*B[1] + A[5]*B[2];
  C[6] += A[6]*B[0] + A[7]*B[1] + A[8]*B[2];

  C[1] += A[0]*B[3] + A[1]*B[4] + A[2]*B[5];
  C[4] += A[3]*B[3] + A[4]*B[4] + A[5]*B[5];
  C[7] += A[6]*B[3] + A[7]*B[4] + A[8]*B[5];

  C[2] += A[0]*B[6] + A[1]*B[7] + A[2]*B[8];
  C[5] += A[3]*B[6] + A[4]*B[7] + A[5]*B[8];
  C[8] += A[6]*B[6] + A[7]*B[7] + A[8]*B[8];
}

/*
  Compute y <- A^{T}*x

  input:
  A:   the 3x3 input matrix in row-major order
  x:   the input 3-vector

  output:
  y:   the resulting vector 
*/
static inline void matMultTrans( const TacsScalar A[],
				 const TacsScalar x[],
				 TacsScalar y[] ){
  y[0] = A[0]*x[0] + A[3]*x[1] + A[6]*x[2];
  y[1] = A[1]*x[0] + A[4]*x[1] + A[7]*x[2];
  y[2] = A[2]*x[0] + A[5]*x[1] + A[8]*x[2];
}

/*
  Compute y <- y + A*x

  input:
  A:   the 3x3 input matrix in row-major order
  x:   the input 3-vector

  in/out:
  y:   the resulting vector 
*/
static inline void matMultAdd( const TacsScalar A[],
			       const TacsScalar x[],
			       TacsScalar y[] ){
  y[0] += A[0]*x[0] + A[1]*x[1] + A[2]*x[2];
  y[1] += A[3]*x[0] + A[4]*x[1] + A[5]*x[2];
  y[2] += A[6]*x[0] + A[7]*x[1] + A[8]*x[2];
}

/*
  Compute y <- y + A*x

  input:
  A:   the 3x3 input matrix in row-major order
  x:   the input 3-vector

  in/out:
  y:   the resulting vector 
*/
static inline void matMultTransAdd( const TacsScalar A[],
				    const TacsScalar x[],
				    TacsScalar y[] ){
  y[0] += A[0]*x[0] + A[3]*x[1] + A[6]*x[2];
  y[1] += A[1]*x[0] + A[4]*x[1] + A[7]*x[2];
  y[2] += A[2]*x[0] + A[5]*x[1] + A[8]*x[2];
}

/*
  Compute y <- y + A*x

  input:
  A:   the 3x3 input matrix in row-major order
  x:   the input 3-vector

  in/out:
  y:   the resulting vector 
*/
static inline void matSymmMult( const TacsScalar A[],
				const TacsScalar x[],
				TacsScalar y[] ){
  y[0] = A[0]*x[0] + A[1]*x[1] + A[2]*x[2];
  y[1] = A[1]*x[0] + A[3]*x[1] + A[4]*x[2];
  y[2] = A[2]*x[0] + A[4]*x[1] + A[5]*x[2];
}

/*
  Compute y <- y + A*x

  input:
  A:   the 3x3 input matrix in row-major order
  x:   the input 3-vector

  in/out:
  y:   the resulting vector 
*/
static inline void matSymmMultAdd( const TacsScalar A[],
				   const TacsScalar x[],
				   TacsScalar y[] ){
  y[0] += A[0]*x[0] + A[1]*x[1] + A[2]*x[2];
  y[1] += A[1]*x[0] + A[3]*x[1] + A[4]*x[2];
  y[2] += A[2]*x[0] + A[4]*x[1] + A[5]*x[2];
}

/*
  Compute the cross-product 

  out = a*(x cross y)

  input:
  a:    the scalar multiplier
  x:    the first input 3-vector
  y:    the second input 3-vector

  output:
  out:  the resulting vector
*/
static inline void crossProduct( const TacsScalar a,
				 const TacsScalar x[],
				 const TacsScalar y[],
				 TacsScalar out[] ){
  out[0] = a*(x[1]*y[2] - x[2]*y[1]);
  out[1] = a*(x[2]*y[0] - x[0]*y[2]);
  out[2] = a*(x[0]*y[1] - x[1]*y[0]);
}

/*
  Compute the cross-product and add it to the output vector

  out = a*(x cross y)

  input:
  a:    the scalar multiplier
  x:    the first input 3-vector
  y:    the second input 3-vector

  output:
  out:  the resulting vector
*/
static inline void crossProductAdd( const TacsScalar a,
				    const TacsScalar x[],
				    const TacsScalar y[],
				    TacsScalar out[] ){
  out[0] += a*(x[1]*y[2] - x[2]*y[1]);
  out[1] += a*(x[2]*y[0] - x[0]*y[2]);
  out[2] += a*(x[0]*y[1] - x[1]*y[0]);
}

/*
  Scale the vector by the given scalar

  input
  a:   the scalar 
  x:   the vector
*/
static inline void vecScale( const TacsScalar a,
			     TacsScalar x[] ){
  x[0] *= a;
  x[1] *= a;
  x[2] *= a;
}

/*
  Compute the dot-product of two vectors

  return = x^{T}*y

  input:
  x:   the first vector
  y:   the second vector
  
  returns: the dot product
*/
static inline TacsScalar vecDot( const TacsScalar x[],
				 const TacsScalar y[] ){
  return (x[0]*y[0] + x[1]*y[1] + x[2]*y[2]);
}

/*
  Add the product using an AXPY operation

  y <- y + alpha*x

  input:
  a:    the alpha scalar
  x:    the input vector

  in/out:
  y:    the result
*/
static inline void vecAxpy( const TacsScalar a,
			    const TacsScalar x[],
			    TacsScalar y[] ){
  y[0] += a*x[0];
  y[1] += a*x[1];
  y[2] += a*x[2];
}

/*
  Compute the derivative of x/||x||_{2} w.r.t. x using x and the norm
  of x.

  This code computes a 3x3 matrix that takes the form:

  d(x/||x||_{2})/dx = (I*||x||^2 + x*x^{T})/||x||^3
*/
static inline void vecNormDeriv( TacsScalar nrm,
				 const TacsScalar x[],
				 TacsScalar D[] ){
  TacsScalar s = 1.0/(nrm*nrm*nrm);
  TacsScalar t = nrm*nrm;
 
  D[0] = s*(t - x[0]*x[0]);
  D[1] =-s*x[0]*x[1];
  D[2] =-s*x[0]*x[2];

  D[3] =-s*x[1]*x[0];
  D[4] = s*(t - x[1]*x[1]);
  D[5] =-s*x[1]*x[2];

  D[6] =-s*x[2]*x[0];
  D[7] =-s*x[2]*x[1];
  D[8] = s*(t - x[2]*x[2]);
}

/*
  Add a 3x3 block-matrix to a larger block matrix

  D[:,:] += a*A

  input:
  a:    the scalar
  A:    the 3x3 block matrix
  ldr:  the leading row-dimension of the D matrix
  
  in/out:
  D: the block matrix with in row-major order
*/
static inline void addBlockMat( const TacsScalar a,
				const TacsScalar A[],
				const int ldr, TacsScalar D[] ){
  D[0] += a*A[0];
  D[1] += a*A[1];
  D[2] += a*A[2];

  D += ldr;
  D[0] += a*A[3];
  D[1] += a*A[4];
  D[2] += a*A[5];

  D += ldr;
  D[0] += a*A[6];
  D[1] += a*A[7];
  D[2] += a*A[8];
}

/*
  Add the transpose of a 3x3 block-matrix to a larger block matrix

  D[:,:] += a*A^{T}

  input:
  a:    the scalar
  A:    the 3x3 block matrix
  ldr:  the leading row-dimension of the D matrix
  
  in/out:
  D:    the block matrix with in row-major order
*/
static inline void addBlockMatTrans( const TacsScalar a,
				     const TacsScalar A[],
				     const int ldr, TacsScalar D[] ){
  D[0] += a*A[0];
  D[1] += a*A[3];
  D[2] += a*A[6];

  D += ldr;
  D[0] += a*A[1];
  D[1] += a*A[4];
  D[2] += a*A[7];

  D += ldr;
  D[0] += a*A[2];
  D[1] += a*A[5];
  D[2] += a*A[8];
}

/*
  Add a vector to the matrix 

  D[:,:] += a*A

  input:
  a:    the scalar
  A:    the 3-vector to add to the matrix
  ldr:  the leading row-dimension of the D matrix
  
  in/out:
  D:    the block matrix with in row-major order
*/
static inline void addVecMat( const TacsScalar a,
			      const TacsScalar A[],
			      const int ldr, 
			      TacsScalar D[] ){
  D[0] += a*A[0]; D += ldr;
  D[0] += a*A[1]; D += ldr;
  D[0] += a*A[2];
}

/*
  Add a 3x3 block-matrix to a larger block matrix

  D[:,:] += a*A

  input:
  a:    the scalar
  A:    the 3x3 block matrix
  ldr:  the leading row-dimension of the D matrix
  
  in/out:
  D: the block matrix with in row-major order
*/
static inline void addBlockSymmMat( const TacsScalar a,
				    const TacsScalar A[],
				    const int ldr, TacsScalar D[] ){
  D[0] += a*A[0];
  D[1] += a*A[1];
  D[2] += a*A[2];

  D += ldr;
  D[0] += a*A[1];
  D[1] += a*A[3];
  D[2] += a*A[4];

  D += ldr;
  D[0] += a*A[2];
  D[1] += a*A[4];
  D[2] += a*A[5];
}

/*
  Add a scalar multiple of the identity matrix to the block matrix

  input:
  a:    the scalar
  ldr:  the leading row-dimension of the D matrix
 
  in/out:
  D:    the block matrix in row-major order
*/
static inline void addBlockIdent( const TacsScalar a,
				  const int ldr, TacsScalar D[] ){
  D[0] += a;

  D += ldr;
  D[1] += a;

  D += ldr;
  D[2] += a;
}

/*
  Add a 3x3 scalar skew-symmetric matrix to the D matrix

  input:
  a:    the scalar
  x:    the vector 
  ldr:  the leading row-dimension of the D matrix
 
  in/out:
  D:    the block matrix in row-major order
*/
static inline void addBlockSkew( const TacsScalar a,
				 const TacsScalar x[],
				 const int ldr, TacsScalar D[] ){
  D[1] -= a*x[2];
  D[2] += a*x[1];

  D += ldr;
  D[0] += a*x[2];
  D[2] -= a*x[0];

  D += ldr;
  D[0] -= a*x[1];
  D[1] += a*x[0];
}

/*
  Add the product of two skew-symm matrices to the D matrix

  D[:,:] += a*x^{x}*y^{x} = a*(y*x^{T} - I*x^{T}*y)

  input:
  a:    the scalar
  x:    the first vector 
  y:    the second vectorr
  ldr:  the leading row-dimension of the D matrix
 
  in/out:
  D:    the block matrix in row-major order
*/
static inline void addBlockSkewSkew( const TacsScalar a,
				     const TacsScalar x[],
				     const TacsScalar y[],
				     const int ldr, TacsScalar D[] ){
  D[0] -= a*(x[1]*y[1] + x[2]*y[2]);
  D[1] += a*y[0]*x[1];
  D[2] += a*y[0]*x[2];

  D += ldr;
  D[0] += a*y[1]*x[0];
  D[1] -= a*(x[0]*y[0] + x[2]*y[2]);
  D[2] += a*y[1]*x[2];

  D += ldr;
  D[0] += a*y[2]*x[0];
  D[1] += a*y[2]*x[1];
  D[2] -= a*(x[0]*y[0] + x[1]*y[1]);
}

/*
  Compute the determinant of a 3x3 matrix

  input:
  A:        a 3x3 matrix in row-major order

  returns:  the determinant of A
*/
static inline TacsScalar det3x3( const TacsScalar A[] ){
  return (A[8]*(A[0]*A[4] - A[3]*A[1]) - 
	  A[7]*(A[0]*A[5] - A[3]*A[2]) + 
	  A[6]*(A[1]*A[5] - A[2]*A[4]));
}

/*
  Compute the inverse of a 3x3 matrix

  input:
  A:          a 3x3 matrix in row major order
  Ainv:       the inverse of the 3x3 matrix
  
  returns:    the determinant of A
*/
static inline TacsScalar inv3x3( const TacsScalar A[],
				 TacsScalar Ainv[] ){
  TacsScalar det = (A[8]*(A[0]*A[4] - A[3]*A[1]) - 
		    A[7]*(A[0]*A[5] - A[3]*A[2]) + 
		    A[6]*(A[1]*A[5] - A[2]*A[4]));
  TacsScalar detinv = 1.0/det;

  Ainv[0] = (A[4]*A[8] - A[5]*A[7])*detinv;
  Ainv[1] =-(A[1]*A[8] - A[2]*A[7])*detinv;
  Ainv[2] = (A[1]*A[5] - A[2]*A[4])*detinv;
    
  Ainv[3] =-(A[3]*A[8] - A[5]*A[6])*detinv;
  Ainv[4] = (A[0]*A[8] - A[2]*A[6])*detinv;
  Ainv[5] =-(A[0]*A[5] - A[2]*A[3])*detinv;
    
  Ainv[6] = (A[3]*A[7] - A[4]*A[6])*detinv;
  Ainv[7] =-(A[0]*A[7] - A[1]*A[6])*detinv;
  Ainv[8] = (A[0]*A[4] - A[1]*A[3])*detinv;
    
  return det;
}

/*
  Compute the inner product with a 3x3 matrix:

  return:  x^{T}*A*y

  input:
  A:   a 3x3 matrix in row-major order
  x:   a 3-vector
  y:   a 3-vector
*/
static inline TacsScalar mat3x3Inner( const TacsScalar A[],
				      const TacsScalar x[],
				      const TacsScalar y[] ){
  return (x[0]*(A[0]*y[0] + A[1]*y[1] + A[2]*y[2]) +
	  x[1]*(A[3]*y[0] + A[4]*y[1] + A[5]*y[2]) +
	  x[2]*(A[6]*y[0] + A[7]*y[1] + A[8]*y[2]));
}

/*
  Given the quaternion parameters, compute the rotation matrix.

  input:
  eta:    the quaternion scalar
  eps:    the quaternion 3-vector

  output:
  C:      the rotation matrix
*/
static inline void computeRotationMat( const TacsScalar eta,
                                       const TacsScalar eps[],
                                       TacsScalar C[] ){
  C[0] = 1.0 - 2.0*(eps[1]*eps[1] + eps[2]*eps[2]);
  C[1] = 2.0*(eps[0]*eps[1] + eps[2]*eta);
  C[2] = 2.0*(eps[0]*eps[2] - eps[1]*eta);
  
  C[3] = 2.0*(eps[1]*eps[0] - eps[2]*eta);
  C[4] = 1.0 - 2.0*(eps[0]*eps[0] + eps[2]*eps[2]);
  C[5] = 2.0*(eps[1]*eps[2] + eps[0]*eta);
  
  C[6] = 2.0*(eps[2]*eps[0] + eps[1]*eta);
  C[7] = 2.0*(eps[2]*eps[1] - eps[0]*eta);
  C[8] = 1.0 - 2.0*(eps[0]*eps[0] + eps[1]*eps[1]);
}

/*
  Compute the product of the 3x4 rotation rate matrix with the
  given components of the quaternion vector. 

  y <- S(eta, eps)*x 
  y <- -2*eps*xeta + 2*(eta*I - eps^{x})*xeps

  input: 
  eta:   the quaternion scalar
  eps:   the quaternion vector
  xeta:  the x component of the scalar
  xeps:  the x components of the vector
  
  output:
  y:     3-vector containing the result
*/
static inline void computeSRateProduct( const TacsScalar eta,
                                        const TacsScalar eps[],
                                        const TacsScalar xeta,
                                        const TacsScalar xeps[],
                                        TacsScalar y[] ){
  crossProduct(-2.0, eps, xeps, y);
  vecAxpy(2.0*eta, xeps, y);
  vecAxpy(-2.0*xeta, eps, y);
}

/*
  Add the product of the 3x4 rotation rate matrix with the given
  components of the quaternion vector.

  y <- y + a*S(eta, eps)*x 
  y <- y - 2*a*eps*xeta + 2*a*(eta*I - eps^{x})*xeps

  input: 
  eta:   the quaternion scalar
  eps:   the quaternion vector
  xeta:  the x component of the scalar
  xeps:  the x components of the vector
  
  output:
  y:     3-vector containing the result
*/
static inline void addSRateProduct( const TacsScalar eta,
                                    const TacsScalar eps[],
                                    const TacsScalar xeta,
                                    const TacsScalar xeps[],
                                    TacsScalar y[] ){
  crossProductAdd(-2.0, eps, xeps, y);
  vecAxpy(2.0*eta, xeps, y);
  vecAxpy(-2.0*xeta, eps, y);
}

/*
  Add the product of the transpose of the 3x4 rotation rate matrix
  with the given components of x.

  y <- y + a*S(eta, eps)^{T}*x
  
  [ yeta ] += a*S^{T}*x = [      2*a*eps^{T}*x      ]
  [ yeps ] +=             [ 2*a*(eta*I + eps^{x})*x ] 
  

  input: 
  a:     the scalar input
  eta:   the quaternion scalar
  eps:   the quaternion vector
  x:     the 3-vector input
  
  output:
  yeps:  the scalar component of the output
  yeta:  the 3-vector component of the output
*/
static inline void addSRateTransProduct( const TacsScalar a,
                                         const TacsScalar eta,
                                         const TacsScalar eps[],
                                         const TacsScalar x[],
                                         TacsScalar *yeta,
                                         TacsScalar yeps[] ){
  *yeta -= 2.0*a*vecDot(eps, x);
  crossProductAdd(2.0*a, eps, x, yeps);
  vecAxpy(2.0*a, x, yeps);
}

/*
  Compute the 3x4 rotation rate matrix that takes the quaternion rates
  and returns the angular velocity:

  S = 2[ -eps | (eta*I - eps^{x}) ]

  input: 
  eta:   the quaternion scalar
  eps:   the quaternion vector
  
  output:
  S:     the 3x4 rate matrix
*/
static inline void computeSRateMat( const TacsScalar eta,
                                    const TacsScalar eps[],
                                    TacsScalar S[] ){
  S[0] = -2.0*eps[0];
  S[1] = 2.0*eta;
  S[2] = 2.0*eps[2];
  S[3] = -2.0*eps[1];

  S[4] = -2.0*eps[1];
  S[5] = -2.0*eps[2];
  S[6] = 2.0*eta;
  S[7] = 2.0*eps[0];

  S[8] = -2.0*eps[2];
  S[9] = 2.0*eps[1];
  S[10] = -2.0*eps[0];
  S[11] = 2.0*eta;
}

/*
  Add the 4x4 matrix from the derivative of the transpose of the
  angular rate S matrix

  d(S^{T}*v)/dq = 
  [ 0 | -v^{T} ]
  [ v | -v^{x} ]

  input:
  a:      a scalar multiplier on the contribution
  v:      the input vector
  ldd:    the leading dimension of the Jacobian matrix

  output:
  D:      the Jacobian matrix to which the the contribution is added
*/
static inline void addSRateMatTransDeriv( const TacsScalar a,
                                          const TacsScalar v[],
                                          TacsScalar D[],
                                          const int ldd ){
  const TacsScalar b = 2.0*a;
  D[1] -= b*v[0];
  D[2] -= b*v[1];
  D[3] -= b*v[2];
  D += ldd;

  D[0] += b*v[0];
  D[2] += b*v[2];
  D[3] -= b*v[1];
  D += ldd;

  D[0] += b*v[1];
  D[1] -= b*v[2];
  D[3] += b*v[0];
  D += ldd;

  D[0] += b*v[2];
  D[1] += b*v[1];
  D[2] -= b*v[0];
}

/*
  Compute: D += a*A^{T}*B where a is a scalar, and A and B are 3x4
  matrices stored in column-major order.

  input: 
  a:    the scalar multiple
  A:    3x4 matrix in row-major order
  B:    3x4 matrix in row-major order
  ldd:  the leading row dimension of the Jacobian matrix D

  output:
  D:    the result is added to this matrix D += a*A^{T}*B
*/
static inline void add3x4Product( const TacsScalar a,
				  const TacsScalar A[],
				  const TacsScalar B[],
				  TacsScalar D[],
				  const int ldd ){
  D[0] += a*(A[0]*B[0] + A[4]*B[4] + A[8]*B[8]);
  D[1] += a*(A[0]*B[1] + A[4]*B[5] + A[8]*B[9]);
  D[2] += a*(A[0]*B[2] + A[4]*B[6] + A[8]*B[10]);
  D[3] += a*(A[0]*B[3] + A[4]*B[7] + A[8]*B[11]);
  D += ldd;

  D[0] += a*(A[1]*B[0] + A[5]*B[4] + A[9]*B[8]);
  D[1] += a*(A[1]*B[1] + A[5]*B[5] + A[9]*B[9]);
  D[2] += a*(A[1]*B[2] + A[5]*B[6] + A[9]*B[10]);
  D[3] += a*(A[1]*B[3] + A[5]*B[7] + A[9]*B[11]);
  D += ldd;

  D[0] += a*(A[2]*B[0] + A[6]*B[4] + A[10]*B[8]);
  D[1] += a*(A[2]*B[1] + A[6]*B[5] + A[10]*B[9]);
  D[2] += a*(A[2]*B[2] + A[6]*B[6] + A[10]*B[10]);
  D[3] += a*(A[2]*B[3] + A[6]*B[7] + A[10]*B[11]);
  D += ldd;

  D[0] += a*(A[3]*B[0] + A[7]*B[4] + A[11]*B[8]);
  D[1] += a*(A[3]*B[1] + A[7]*B[5] + A[11]*B[9]);
  D[2] += a*(A[3]*B[2] + A[7]*B[6] + A[11]*B[10]);
  D[3] += a*(A[3]*B[3] + A[7]*B[7] + A[11]*B[11]);
}

/*
  Compute the second derivative of the product of the transpose of a
  quaternion-parametrized rotation matrix with a vector, i.e.:

  d^2/dq^2(C^{T}*v)

  where v is a constant vector. Note that the second derivative of the
  rotation matrix is a constant and so this code only depends on the
  input vector v.

  The order of the derivatives is as follows:
  d^2(C^{T}*v)/(d(eta)d(epsilon_{i})

  d^2(C^{T}*v)/(d(epsilon_{i})d(epsilon_{j})

  for (i,j) = (0,0), (0,1), (0,2), (1,1), (1,2), (2,2)

  The result is stored in a 9x3 array in row-major order.

  input:
  v:    the constant vector in the multiplication C^{T}*v

  output:
  dv    the second derivatives of C^{T}*v w.r.t q
*/
static inline void computeQtr2ndDeriv( const TacsScalar v[],
				       TacsScalar dv[] ){
  // Derivatives of eta and eps
  dv[0] = 0.0;
  dv[1] = -2.0*v[2];
  dv[2] = 2.0*v[1];
  dv += 3;

  dv[0] = 2.0*v[2];
  dv[1] = 0.0;
  dv[2] = -2.0*v[0];
  dv += 3;

  dv[0] = -2.0*v[1];
  dv[1] = 2.0*v[0];
  dv[2] = 0.0;
  dv += 3;

  // Second derivatives w.r.t eps
  // C,11
  dv[0] = 0.0;
  dv[1] = -4.0*v[1];
  dv[2] = -4.0*v[2];
  dv += 3;

  // C,12
  dv[0] = 2.0*v[1];
  dv[1] = 2.0*v[0];
  dv[2] = 0.0;
  dv += 3;

  // C,13
  dv[0] = 2.0*v[2];
  dv[1] = 0.0;
  dv[2] = 2.0*v[0];
  dv += 3;

  // C,22
  dv[0] = -4.0*v[0];
  dv[1] = 0.0;
  dv[2] = -4.0*v[2];
  dv += 3;

  // C,23
  dv[0] = 0.0;
  dv[1] = 2.0*v[2];
  dv[2] = 2.0*v[1];
  dv += 3;

  // C,33
  dv[0] = -4.0*v[0];
  dv[1] = -4.0*v[1];
  dv[2] = 0.0;
  dv += 3;
}

#endif // TACS_ALGEBRA_H