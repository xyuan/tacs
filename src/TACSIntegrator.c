#include <math.h>
#include "TACSIntegrator.h"
/*
  Base class constructor for integration schemes. This base class
  contains common methods and variables pertaining to the integration
  schemes used in TACS.

  Input:
  tInit: the initial time
  tfinal: the final time
  num_steps_per_sec: the number of steps to take for each second
*/
TacsIntegrator::TacsIntegrator( TACSAssembler * _tacs,
                                double _tinit, double _tfinal, 
                                int _num_steps_per_sec ){
  // copy over the input parameters
  tacs = _tacs;
  tacs->incref();

  tinit = _tinit;
  tfinal = _tfinal;
  num_steps_per_sec = _num_steps_per_sec;

  // compute the step size
  h = 1.0/double(num_steps_per_sec);

  // compute the total number of time steps
  num_time_steps = int(double(num_steps_per_sec)*(tfinal-tinit)) + 1;

  // Default print level
  print_level = 1;

  //------------------------------------------------------------------//
  //                     Time history of states                       //
  //------------------------------------------------------------------//

  // state variables that store the entire time history
  q = new BVec*[num_time_steps];
  qdot = new BVec*[num_time_steps];
  qddot = new BVec*[num_time_steps];

  // create state vectors for TACS during each timestep
  for ( int k = 0; k < num_time_steps; k++ ) {
    q[k] = tacs->createVec(); 
    q[k]->incref(); 

    qdot[k] = tacs->createVec(); 
    qdot[k]->incref(); 

    qddot[k] = tacs->createVec(); 
    qddot[k]->incref(); 
  }

  // Get the number of state variables and store into the class variable
  q[0]->getSize(&num_state_vars);
  
  // store time
  time = new double[num_time_steps];
  memset(time, 0, num_time_steps*sizeof(double));

  //------------------------------------------------------------------//
  //                     Newton's method                              //
  //------------------------------------------------------------------//

  // Frequency of Jacobian recomputation during nonlinear solve
  jac_comp_freq = 1;

  // Default parameters for Newton solve
  max_newton_iters = 25;
  atol = 1.0e-12;
  rtol = 1.0e-6;

  // Create vector for storing the residual at each Newton iteration
  res = tacs->createVec();
  res->incref();
  
  // Create a vector for storing the Newton updates
  update = tacs->createVec();
  update->incref();
  
  // Create a matrix for storing the Jacobian
  D = tacs->createFEMat();
  D->incref();

  // Associate the maxtrix with FEMatrix
  mat = D;
  mat->incref();
    
  // Allocate the factorization
  int lev = 4500; double fill = 10.0; int reorder_schur = 1;
  pc = new PcScMat(D, lev, fill, reorder_schur);
  pc->incref();
    
  // The Krylov subspace method (KSM) associated with the solver
  int gmres_iters = 10, num_restarts = 0, is_flexible = 0;
  ksm = new GMRES(D, pc, gmres_iters, num_restarts, is_flexible);
  ksm->incref();
  
  ksm->setTolerances(rtol, atol);

  //------------------------------------------------------------------//
  //                     Adjoint parameters                           //
  //------------------------------------------------------------------//
  
  func = NULL;
  num_func = 0;
}

/*
  Destructor
*/
TacsIntegrator::~TacsIntegrator(){
  // Dereference TACS
  tacs->decref();

  // Dereference position, velocity and acceleration states
  for ( int k = 0; k < num_time_steps; k++ ) {
    q[k]->decref();
    qdot[k]->decref();
    qddot[k]->decref();
  }

  // Dereference Newton's method objects
  res->decref();
  update->decref();
  D->decref();
  mat->decref();
  pc->decref();
  ksm->decref();
  
  delete [] time;
  delete [] q;
  delete [] qdot;
  delete [] qddot;

  if ( num_func > 0 ) {
    // Dereference the objective/constraint functions
    for ( int j = 0; j < num_func; j++ ) {
      func[j]->decref();
    }
    delete [] func;
  }  
}

/*

  Drives the residual R(t,q,qdot,qddot) to zero using Newton's method
  with KSM solver

  Input: 
  The guessed (initial) state variable values q, qdot, qddot are
  supplied
  
  Output: q, qdot, qddot updated iteratively until the corresponding
  residual R = 0
  
  alpha: multiplier for derivative of Residual wrt to q
  beta : multiplier for derivative of Residual wrt to qdot
  gamma: multiplier for derivative of Residual wrt to qddot
*/
void TacsIntegrator::newtonSolve( double alpha, double beta, double gamma,
                                  double t, BVec *q, BVec *qdot, 
                                  BVec *qddot ){
  // Initialize the norms
  TacsScalar init_norm = 0.0;
  TacsScalar norm = 0.0;

  if (print_level > 0){
    fprintf(stdout, "%12s %8s %12s %12s %12s %12s\n",
            "time", "Newton", "tcpu", "|R|", "|R|/|R0|", "delta");
  }
  
  double delta = gamma;
  double t0 = MPI_Wtime();

  // Iterate until max iters or R <= tol
  for ( int n = 0; n < max_newton_iters; n++ ){
    // Set the supplied initial input states into TACS
    tacs->setVariables(q);
    tacs->setDotVariables(qdot);
    tacs->setDDotVariables(qddot);

    // Assemble the Jacobian matrix once in five newton iterations
    if (n % jac_comp_freq == 0){
      if (n > 0){
        double frac = 10.0*RealPart(norm/(init_norm + rtol));
        if (frac < 1.0){
          delta = frac*gamma;
        }
        else {
          delta = gamma;
        }
      }
      tacs->assembleJacobian(res, mat, alpha, beta, gamma, NORMAL);
    } 
    else {
      tacs->assembleRes(res);
    }    

    // Compute the L2-norm of the residual
    norm = res->norm();
    
    // Record the residual norm at the first Newton iteration
    if (n == 0){
      init_norm = norm;
    }

    // Write a summary
    if(print_level > 0) {
      if (n == 0){
        fprintf(stdout, "%12.5e %8d %12.5e %12.5e %12.5e %12.5e\n",
                t, n, MPI_Wtime()-t0, norm, norm/init_norm, delta);
      }
      else {
        fprintf(stdout, "%12s %8d %12.5e %12.5e %12.5e %12.5e\n",
                " ", n, MPI_Wtime()-t0, norm, norm/init_norm, delta);
      }
    }
           
    // Check if the norm of the residuals is a NaN
    if (norm != norm){ 
      fprintf(stderr,
              "Newton iteration %d, failed with NaN residual norm\n", n);
      break;
    }
    
    // Check if the Newton convergence tolerance is satisfied
    if (norm < rtol*init_norm || norm < atol){
      break;
    }

    // Factor the preconditioner
    if (n % jac_comp_freq == 0){
      pc->factor();
    }
    
    // Solve for update using KSM
    ksm->solve(res, update);

    // Update the state variables using the solution
    qddot->axpy(-gamma, update);
    qdot->axpy(-beta, update);
    q->axpy(-alpha, update);

    // Check whether the Newton iteration was successful
    if (n == max_newton_iters && norm >= rtol*init_norm){
      fprintf(stderr,"Newton iteration failed to converge in %d iters\n", n);
      break;
    }
  }
}

/*
  Creates an f5 file for each time step and writes the data
*/
void TacsIntegrator::writeSolutionToF5(){

}

/*
  Function that writes time, q, qdot, qddot to file
*/
void TacsIntegrator::writeSolution( const char *filename ){
  // Temporary variables to access the states at each time
  TacsScalar *qvals, *qdotvals, *qddotvals;

  // Open a new file
  FILE *fp = fopen(filename, "w");
 
  for ( int k = 0; k < num_time_steps; k++ ){    
    // Copy over the state values from BVec
    q[k]->getArray(&qvals);
    qdot[k]->getArray(&qdotvals);
    qddot[k]->getArray(&qddotvals);
  
    // Write the time
    fprintf(fp, "%e ", time[k]);

    // Write the states (q, qdot, qddot) to file
    for ( int j = 0; j < num_state_vars; j++ ){
      fprintf(fp, "%e %e %e ", RealPart(qvals[j]), 
              RealPart(qdotvals[j]), RealPart(qddotvals[j]));
    }
    fprintf(fp, "\n");
  }

  // Close the file
  fclose(fp);
}

/*
  Constructor for BDF Integration scheme

  Input:
  tinit: the initial time
  tfinal: the final time
  num_steps_per_sec: the number of steps to take for each second
*/
TacsBDFIntegrator:: TacsBDFIntegrator( TACSAssembler * _tacs, 
                                       double _tinit, double _tfinal, 
                                       int _num_steps_per_sec, 
                                       int _max_bdf_order ):
TacsIntegrator(_tacs, _tinit,  _tfinal,  _num_steps_per_sec){		
  // copy over the variables
  max_bdf_order = _max_bdf_order;

  // Truncate the maximum order to 3rd order
  max_bdf_order = (max_bdf_order <= 3 ? 
		   max_bdf_order : 3);

  // Number of first and second order BDF coefficients
  nbdf = 0;
  nbddf = 0;
    
  // Adjoint variables
  psi = new BVec*[num_time_steps];

  // Create state vectors for TACS during each timestep
  for (int k = 0; k < num_time_steps; k++) {
    psi[k] = tacs->createVec(); 
    psi[k]->incref(); 
  }
}

/*
  Destructor for TACSBDFIntegrator
*/
TacsBDFIntegrator::~TacsBDFIntegrator(){
  // Delete the adjoint variables
  for (int k = 0; k < num_time_steps; k++) {
    psi[k]->decref();
  }
  delete [] psi;
}

/*
  Integration logic of BDF. Use this function to march in time. The
  solution over time is set into the class variables q, qdot and qddot
  and time.
*/
void TacsBDFIntegrator::integrate(){
  current_time_step = 0;

  // Initial condition
  tacs->getInitConditions(q[0], qdot[0]);

  for ( int k = 1; k < num_time_steps; k++ ){
    current_time_step++;

    // Approximate states and their derivatives using BDF formula
    approxStates(q, qdot, qddot);
    
    // Determine the coefficients for Jacobian Assembly
    double gamma = bddf_coeff[0]/h/h;
    double beta = bdf_coeff[0]/h;
    double alpha = 1.0;

    // Solve the nonlinear system of equations
    newtonSolve(alpha, beta, gamma, time[k], q[k], qdot[k], qddot[k]);
    
    // Advance time (states are already advanced at the end of Newton solve)
    time[k] = time[k-1] + h;
  }
}

/*
  Solves for the adjoint variables psi[k] marching backwards in time.
*/
void TacsBDFIntegrator::adjointSolve(){
  for ( int k = num_time_steps-1; k >= 1; k-- ){
    current_time_step = k;
   
    // Get the BDF coefficients at this time step
    get2ndBDFCoeff(k, bdf_coeff, &nbdf, bddf_coeff, &nbddf, max_bdf_order);

    // Determine the linearization coefficients for Jacobian Assembly
    double gamma = bddf_coeff[0]/h/h;
    double beta = bdf_coeff[0]/h;
    double alpha = 1.0;

    // Set the states variables into TACS
    tacs->setVariables(q[k]);
    tacs->setDotVariables(qdot[k]);
    tacs->setDDotVariables(qddot[k]);
    
    // Setup the linear system for adjoint solve
    tacs->assembleJacobian(NULL, mat, alpha, beta, gamma, TRANSPOSE);
    
    // Perform LU factorization of the jacobian matrix
    pc->factor();
   
    // Solve for the adjoint variables at the k-th step [mat]^T psi[k] = res^T
    // Assemble the RHS for the current function
    setupAdjointRHS(res, 0);

    // Apply the factorization for each RHS
    pc->applyFactor(res, psi[k]);

    TacsScalar *resvals;
    res->getArray(&resvals);

    printf("\nrhs = ");
    for ( int j = 0; j < num_state_vars; j++ ){
      printf(" %e ", resvals[j]);
    }
    printf("\n");

    printf("\n q = ");
    q[k]->getArray(&resvals);
    for ( int j = 0; j < num_state_vars; j++ ){
      printf(" %e ", resvals[j]);
    }
    printf("\n");
    exit(-1);    
  }

  // Compute the total derivative
  
  // Print Lagrange multipliers
  if ( print_level > 1 ){
    TacsScalar *psivals;
    for ( int k = 0; k < num_time_steps; k++ ){    
      psi[k]->getArray(&psivals);
      printf("\n");
      for ( int j = 0; j < num_state_vars; j++ ){
	printf(" %e ", psivals[j]);
      }
      printf("\n");
    }
  }
}

/*
  Assembles the right hand side of the adjoint equation.
  
  input:
  res  : the right hand side vector in the linear system
  func_num: the index of the current objective function

  output:
  res: right hand side vector for the adjoint linear system
*/
void TacsBDFIntegrator::setupAdjointRHS( BVec *res, int func_num ){
  int k = current_time_step;
  
  // Zero the RHS vector each time
  res->zeroEntries();
     
  // Add the contribution from the j-th objective function df/dq. 
  if ( k == num_time_steps - 1 ){
    // Set the states variables into TACS
    tacs->setVariables(q[k]);
    tacs->setDotVariables(qdot[k]);
    tacs->setDDotVariables(qddot[k]);
    printf("Evaluating SV Sens \n");
    tacs->evalSVSens(func[func_num], res);
  }

  // Add contribution from the first derivative terms d{R}d{qdot}
  double scale;
  for ( int i = 1; i < nbdf; i++ ){
    scale = bdf_coeff[i]/h;
  
    // Set the states variables into TACS
    tacs->setVariables(q[k-i]);
    tacs->setDotVariables(qdot[k-i]);
    tacs->setDDotVariables(qddot[k-i]);
  
    tacs->addJacobianVecProduct(scale, 0.0, 1.0, 0.0, psi[k-i], res, TRANSPOSE);
  }
 
  // Add contribution from the second derivative terms d{R}d{qddot}
  for ( int i = 1; i < nbddf; i++ ){
    scale = bddf_coeff[i]/h/h;
    
    // Set the states variables into TACS
    tacs->setVariables(q[k-i]);
    tacs->setDotVariables(qdot[k-i]);
    tacs->setDDotVariables(qddot[k-i]);
  
    tacs->addJacobianVecProduct(scale, 0.0, 0.0, 1.0, psi[k-i], res, TRANSPOSE);
  }

  // Negative the RHS
  res->scale(-1.0);
}

/*
  Approximate states (q, qdot, qddot) at the current time step using
  the BDF coefficients and previous time step values of the states q,
  qdot and qddot.
  
  Input:
  pointers to the global states q, qdot, qddot
*/
void TacsBDFIntegrator::approxStates( BVec **q, BVec **qdot, BVec **qddot ){
  int k = current_time_step;
  
  // get the BDF coefficients
  get2ndBDFCoeff(k, bdf_coeff, &nbdf, bddf_coeff, &nbddf, max_bdf_order);
  
  // copy over the q values from previous time step into the current q
  q[k]->copyValues(q[k-1]);

  // approximate qdot using BDF formula
  for ( int i = 0; i < nbdf; i++ ){
    double scale = bdf_coeff[i]/h;
    qdot[k]->axpy(scale, q[k-i]);
  }

  // approximate qddot using BDF formula
  for ( int i = 0; i < nbddf; i++ ){
    double scale = bddf_coeff[i]/h/h;
    qddot[k]->axpy(scale, q[k-i]);
  }

  // If required, add the contribution to the second derivative
  // from the initial values of the first derivatives
  if (k == nbdf-1){
    double scale = bdf_coeff[nbdf-1]/h;
    qddot[k]->axpy(scale, qdot[k-1]);
  }
}

/*
  This code computes the BDF coefficients for first and second
  derivative approximations.
  
  input:
  k:         the integration time step
  max_order: the maximum order to use
  
  output:
  bdf:    the first derivative approximation
  nbdf:   the number first derivative of coefficients
  bddf:   the second derivative approximation
  nbddf:  the number second derivative of coefficients
*/
void TacsBDFIntegrator::get2ndBDFCoeff( const int k, 
					double bdf[], int *nbdf,
					double bddf[], int *nbddf,
					const int max_order ){
  // Construct the second-order BDF scheme
  memset(bddf, 0, (2*max_order+1)*sizeof(double));
  memset(bdf, 0, (max_order+1)*sizeof(double));

  // For the first time step, set the first coefficient to 1.0, 
  // but set the number of coefficients to zero
  if (k == 0){
    bdf[0] = 1.0;
    *nbdf = 0;
    *nbddf = 0;
    return;
  }

  // Get the first-order coefficients - one greater than the maximum
  // order of coefficients
  int order = (k < max_order ? k : max_order);
  *nbdf = getBDFCoeff(bdf, order)+1;
  *nbddf = 2*(*nbdf)-1;
  if (*nbddf > k+1){
    *nbddf = k+1;
  }

  // Now, compute the second-order coefficients
  for ( int j = 0; j < *nbdf && (k - j > 0); j++ ){
    // order >= 1 always
    int order = (k-j < max_order ? k-j : max_order);
    double bdf2[5];
    int len = getBDFCoeff(bdf2, order)+1;

    for ( int i = 0; i < len; i++ ){
      bddf[j + i] += bdf[j]*bdf2[i];
    }
  }
}

/*
  Get the first-order BDF coefficients of order <= 3 

  input:
  order:  order of the backwards-difference coefficients

  output: 
  bdf:    the backwards difference coefficients
*/ 
int TacsBDFIntegrator::getBDFCoeff( double bdf[], int order ){
  if (order <= 1){
    bdf[0] = 1.0;
    bdf[1] = -1.0;
    return 1;
  }
  else if (order == 2){
    bdf[0] =  1.5;
    bdf[1] = -2.0;
    bdf[2] =  0.5;
    return 2;
  }
  else if (order >= 3){
    bdf[0] =  35.0/24.0;
    bdf[1] = -15.0/8.0;
    bdf[2] =  3.0/8.0;
    bdf[3] =  1.0/24.0;
    return 3;
  }
  return 0;
}

/*
  Constructor for TacsDIRKIntegrator

  Input:

  num_stages:        the number of Runge-Kutta stages
  tinit:             the initial time
  tfinal:            the final time
  num_steps_per_sec: the number of steps to take for each second
  max_newton_iters:  the max number of Newton iterations
*/
TacsDIRKIntegrator::TacsDIRKIntegrator( TACSAssembler * _tacs, 
                                        double _tinit, double _tfinal, 
                                        int _num_steps_per_sec,
                                        int _num_stages ): 
TacsIntegrator(_tacs, _tinit, _tfinal, _num_steps_per_sec){   
  // copy over the variables
  num_stages = _num_stages;
  
  // allocate space for stage state variables
  qS = new BVec*[num_stages*num_time_steps];
  qdotS = new BVec*[num_stages*num_time_steps];
  qddotS = new BVec*[num_stages*num_time_steps];

  // store stage time
  tS = new double[num_stages*num_time_steps];
  memset(tS, 0, num_stages*num_time_steps*sizeof(double));
  
  // create state vectors for TACS during each timestep
  for ( int k = 0; k < num_stages*num_time_steps; k++ ){
    qS[k] = tacs->createVec(); 
    qS[k]->incref(); 
    
    qdotS[k] = tacs->createVec(); 
    qdotS[k]->incref(); 

    qddotS[k] = tacs->createVec(); 
    qddotS[k]->incref(); 
  }
  
  // Allocate space for Butcher tableau
  A = new double[num_stages*(num_stages+1)/2];
  B = new double[num_stages];
  C = new double[num_stages];
  
  // Set the Butcher tableau entries to zero
  memset(A, 0, num_stages*(num_stages+1)/2*sizeof(double));
  memset(B, 0, num_stages*sizeof(double));
  memset(C, 0, num_stages*sizeof(double));

  // Add entries into the Butcher tableau
  setupButcherTableau();

  // Allocate space for adjoint variables
  psi = new BVec*[num_time_steps*num_stages];

  // Create adjoint vectors
  for (int k = 0; k < num_time_steps*num_stages; k++) {
    psi[k] = tacs->createVec(); 
    psi[k]->incref(); 
  }
}

/*
  Destructor for TacsDIRKIntegrator
*/
TacsDIRKIntegrator::~TacsDIRKIntegrator(){
  // Cleanup Butcher's Tableau
  delete [] A;
  delete [] B;
  delete [] C;

  // Cleanup stage stages
  for ( int i = 0; i < num_stages*num_time_steps; i++ ){
    qS[i]->decref();
    qdotS[i]->decref();
    qddotS[i]->decref();
  }

  // cleanup stage time
  delete [] tS;

  delete [] qS;
  delete [] qdotS;
  delete [] qddotS;

  // Delete the adjoint variables
  for (int k = 0; k < num_time_steps*num_stages; k++) {
    psi[k]->decref();
  }
  delete [] psi;
}

/*
  Function that puts the entries into Butcher tableau
*/
void TacsDIRKIntegrator::setupButcherTableau(){
  if (num_stages == 1){
    // Implicit mid-point rule (A-stable)
    A[0] = 0.5;
    B[0] = 1.0;
    C[0] = 0.5;
    order = 2;
  } 
  else if (num_stages == 2){
    // Crouzeix formula (A-stable)
    double tmp = 1.0/(2.0*sqrt(3.0));

    A[0] = 0.5 + tmp;
    A[1] = -1.0/sqrt(3.0);
    A[2] = A[0];

    B[0] = 0.5;
    B[1] = 0.5;

    C[0] = 0.5 + tmp;
    C[1] = 0.5 - tmp;

    order = 3;     
  } 
  else if (num_stages == 3){
    // Crouzeix formula (A-stable)
    double alpha = 2.0*cos(M_PI/18.0)/sqrt(3.0);
    
    A[0] = (1.0 + alpha)*0.5;
    A[1] = -0.5*alpha;
    A[2] = A[0];
    A[3] = 1.0 + alpha;
    A[4] = -(1.0 + 2.0*alpha);
    A[5] = A[0];    

    B[0] = 1.0/(6.0*alpha*alpha);
    B[1] = 1.0 - 1.0/(3.0*alpha*alpha);
    B[2] = B[0];

    C[0] = 0.5*(1.0+alpha);
    C[1] = 0.5;
    C[2] = 0.5*(1.0-alpha);

    order = 4;
  }
  else {
    fprintf(stderr, "ERROR: Invalid number of stages %d\n", num_stages);
    num_stages = 1;
    setupButcherTableau();
  }

  // check for the consistency of butcher tableau entries
  checkButcherTableau();
}

/*
  Function that checks the consistency of Butcher tableau values
*/
void TacsDIRKIntegrator::checkButcherTableau(){
  double tmp;

  // Check #1: sum(A(i,:)) = C(i)  
  int idx = -1;
  for ( int i = 0; i < num_stages; i++ ){
    tmp = 0.0;
    for ( int j = 0; j <= i; j++ ){
      idx++;
      tmp += A[idx];
    }

    // Check the difference
    if (fabs(C[i] - tmp) >= 1.0e-6) {
      fprintf(stderr, "WARNING: Sum A[%d,:] != C[%d] i.e. %f != %f \n", 
              i, i, C[i], tmp);
    }  
  }
  
  // Check #2: sum(B) = 1.0
  tmp = 0.0;
  for ( int i = 0; i < num_stages; i++ ){
    tmp += B[i];
  }
  if (fabs(1.0 - tmp) >= 1.0e-6) {
    fprintf(stderr, "WARNING: Sum B != 1.0 \n");
  }
}

/*
  Solves for the adjoint variables psi[k,i] marching backwards in time
  and stage
*/
void TacsDIRKIntegrator::adjointSolve(){
  for ( int k = num_time_steps-1; k >= 1; k-- ){
    for ( int i = num_stages-1; i >= 0; i-- ){
      current_time_step = k;
      current_stage = i;
      
      // Find the offset
      int toffset = (k-1)*num_stages;

      // Determine the coefficients for linearizing the Residual
      double gamma = 1.0;
      double beta  = h*A[0]; 
      double alpha = h*A[0]*h*A[0];

      // Set the stage states variables into TACS
      tacs->setVariables(qS[toffset+i]);
      tacs->setDotVariables(qdotS[toffset+i]);
      tacs->setDDotVariables(qddotS[toffset+i]);
      
      // Setup the linear system for adjoint solve (gamma + delta?)
      tacs->assembleJacobian(NULL, mat, alpha, beta, gamma, TRANSPOSE);
      
      // Perform LU factorization of the jacobian matrix
      pc->factor();
      
      // Solve for the adjoint variables at the k-th step [mat]^T psi[k] = res^T
      // Assemble the RHS for the current function
      setupAdjointRHS(&res[0], 0);
	
      // Apply the factorization for each RHS
      pc->applyFactor(&res[0], psi[toffset+i]);
    }
  }
}

/*
  Assembles the right hand side of the adjoint equation.
  
  input:
  res  : the right hand side vector in the linear system
  func_num: the index of the current objective function

  output:
  res: right hand side vector for the adjoint linear system
*/
void TacsDIRKIntegrator::setupAdjointRHS( BVec *res, int func_num ){
  int k = current_time_step;
  int i = current_stage;

  int toffset = (k-1)*num_stages;
  
  // Zero the RHS vector each time
  res->zeroEntries();
  
  // Add the contribution from the objective function df/dq at
  // the last time step
  double scale;
  if ( k == num_time_steps - 1 ){
    for ( int j = i; j < num_stages; j++ ){
      double weight = B[j];

      // Part 1 (Add FUNCTIONAL contribution from stages of CURRENT time step)
      int idx1 = getIdx(j);
      scale = 0.0;
      for ( int p = i; p <= j; p++ ){
	int idx2 = getIdx(p);
	scale += weight*A[idx1+i]*A[idx2+i];
      }

      // Set the required states into TACS before calling the
      // derivative evaluation
      tacs->setVariables(qS[toffset+j]);
      tacs->setDotVariables(qdotS[toffset+j]);
      tacs->setDDotVariables(qddotS[toffset+j]);

      // Evalute the sensitivity
      tacs->evalSVSens(func[func_num], res);
      res->scale(scale);
      
      // Part 2 (Add FUNCTIONAL contribution from stages of the NEXT time step)
      // Note: skipped at the last time step as we don't have any future states
      if ( k < num_time_steps - 1 ){ 
	double scale1 = 0.0;
	for ( int p = i; p < num_stages; p++ ){
	  int idx2 = getIdx(p);
	  scale1 += weight*A[idx2+i]*B[p];
	}

	double scale2 = 0.0;
	int idx = getIdx(j);
	for ( int p = 1; p <= j; p++ ){
	  scale2 += weight*B[i]*A[idx+p];
	}

	scale = scale1 + scale2;

	// Set the required states into TACS before calling the
	// derivative evaluation

	int off = k*num_stages; // Points to the next time step

	tacs->setVariables(qS[off+j]);
	tacs->setDotVariables(qdotS[off+j]);
	tacs->setDDotVariables(qddotS[off+j]);

	tacs->evalSVSens(func[func_num], res);
	res->scale(scale);

	// Part 3: Add inter-stage contributions from the same time step
	// 'scale' is same as previously computed 'scale'
	tacs->addJacobianVecProduct(scale, 1.0, 0.0, 0.0, psi[off+j], res, TRANSPOSE); 

	scale = weight*B[i]/h;
	tacs->addJacobianVecProduct(scale, 0.0, 1.0, 0.0, psi[off+j], res, TRANSPOSE);
      } 
    }  
  } 

  // Part 4: Add contributions from stages of CURRENT time step
  for ( int j = i+1; j < num_stages; j++ ){
    double weight = B[j];

    // Set the required states into TACS before calling the
    // derivative evaluation
    tacs->setVariables(qS[toffset+j]);
    tacs->setDotVariables(qdotS[toffset+j]);
    tacs->setDDotVariables(qddotS[toffset+j]);

    int idx1 = getIdx(j);
    scale = weight*A[idx1+i]/h;
    tacs->addJacobianVecProduct(scale, 0.0, 1.0, 0.0, psi[toffset+j], res, TRANSPOSE);

    scale = 0.0;
    for ( int p = i; p <= j; p++ ){
      int idx2 = getIdx(p);
      scale += weight*A[idx1+i]*A[idx2+i];
    }
    tacs->addJacobianVecProduct(scale, 1.0, 0.0, 0.0, psi[toffset+j], res, TRANSPOSE);

  }

  // Negate the RHS
  res->scale(-1.0);
}

/*
  Integration logic of DIRK. Use this function to march in time. The
  solution over time is set into the class variables q, qdot and qddot
  and time.
*/
void TacsDIRKIntegrator::integrate(){
  current_time_step = 0;
  
  // Initial condition
  tacs->getInitConditions(q[0], qdot[0]);

  for ( int k = 1; k < num_time_steps; k++ ){
    current_time_step++;
       
    // Compute the stage states qS, qdotS, qddotS based on the DIRK formula
    computeStageValues();
    
    // Advance the global state to the next time
    timeMarch(time, q, qdot, qddot);
  }
}

/*
  Function that computes the stage values at each time step. This
  function uses the global states and time at the previous time-step
  and sets the stage states tS, qS, qdotS and qdotS.
*/
void TacsDIRKIntegrator::computeStageValues(){

  int k = current_time_step;
  int toffset = (k-1)*num_stages;

  for ( int i = 0; i < num_stages; i++ ){
    // Compute the stage time
    tS[toffset+i] = time[k-1] + C[i]*h;

    // Initial guess for qddotS
    if (i == 0){
      qddotS[toffset+i]->copyValues(qddot[k-1]);
    }
    else {
      qddotS[toffset+i]->copyValues(qddotS[toffset+i-1]);
    }

    // Compute qdotS
    int idx = getIdx(i);
    for ( int j = 0; j <= i; j++ ){
      qdotS[toffset+i]->axpy(h*A[idx+j], qddotS[toffset+j]);
    }
    qdotS[toffset+i]->axpy(1.0, qdot[k-1]);

    // Compute qS
    idx = getIdx(i);
    for ( int j = 0; j <= i; j++ ){
      qS[toffset+i]->axpy(h*A[idx+j], qdotS[toffset+j]);
    }
    qS[toffset+i]->axpy(1.0, q[k-1]);
    
    // Determine the coefficients for linearizing the Residual
    double gamma = 1.0;
    double beta  = h*A[0]; 
    double alpha = h*A[0]*h*A[0];

    // Solve the nonlinear system of stage equations
    newtonSolve(alpha, beta, gamma, tS[toffset+i], qS[toffset+i], qdotS[toffset+i], qddotS[toffset+i]);
  }
}

/*
  Start index of the Butcher Tableau A for the supplied stage
*/
int TacsDIRKIntegrator::getIdx( int stageNum ){
  return stageNum*(stageNum+1)/2;
}

/*
  Function that advances the global states q, qdot, qddot and time to
  next time step
  
  Input:
  The pointers to the global state variables q, qdot, qddot
  
  Output:
  Updated global state variables q, qdot, qddot at the time step
*/
void TacsDIRKIntegrator::timeMarch( double *time, 
				    BVec **q, BVec **qdot, BVec **qddot ){
  int k = current_time_step;
  int toffset = (k-1)*num_stages;
  
  // advance the time
  time[k] = time[k-1] + h;
  
  // advance the position state
  for ( int j = 0; j < num_stages; j++ ){
    q[k]->axpy(h*B[j], qdotS[toffset+j]);
  }
  q[k]->axpy(1.0, q[k-1]);

  // advance the velocity state
  for ( int j = 0; j < num_stages; j++ ){
    qdot[k]->axpy(h*B[j], qddotS[toffset+j]);
  }
  qdot[k]->axpy(1.0, qdot[k-1]);

  // advance the acceleration state
  for ( int j = 0; j < num_stages; j++ ){
    qddot[k]->axpy(B[j], qddotS[toffset+j]);
  }
}



