

__kernel void hello_kernel(__global const int *a, __global const int *b, __global int *result) {
  int gid = get_global_id(0);
  if(a[gid] == 7) {
    result[gid] = 100;
  } else {
    result[gid] = a[gid] + b[gid];
  }
}