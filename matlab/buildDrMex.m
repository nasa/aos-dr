disp('Warning, you must enable C99 mode to compile this mex-file, ');
disp('for gcc you can open the mex configuration file ');
disp('(e.g. mex_C_glnxa64.xml) and in both CFLAGS sections, replace ');
disp('-ansi with -std=c99.');
mex -I../fsw/src dr_mex.c ../fsw/src/dr_process_d_matrix.c -output dr_process_d_matrix.mexglx
