buildDrMex
dmatrix = [ 0 0 1 0 ; 0 0 0 1 ; 1 0 0 0 ];
  dmatrix = logical(dmatrix)
test_results = [ 1 0 0 1 ];
  test_results = uint8(test_results)

[failure_modes] = dr_process_d_matrix(dmatrix, test_results)
