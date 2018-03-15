#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

double Trap(double a, double b, int n);

int main(int argc, char* argv[]){
//	double global_result = 0.0;
//	double a, b;
//	int n;
	int thread_count;	
	thread_count = strtol(argv[1],NULL,10);

//	printf("Enter a, b, and n\n");
//	scanf("%lf %lf %d",&a,&b,&n);
//#	pragma omp parallel num_threads(thread_count) \
//		reduction(+: global_result)
//	global_result += Trap(a,b,n);
	
//	printf("With n = %d trapezoids, our estimate\n",n);
//	printf("of the integral from %f to %f = %.14e\n", a, b, global_result);

	double sum = 0.0, factor;
	int k, n = 500;
#	pragma omp parallel for num_threads(thread_count) \
		default(none) reduction(+:sum) private(k,factor) \
		shared(n)
	for(k = 0; k < n; k++){
		if(k%2 == 0)
			factor = 1.0;
		else
			factor = -1.0;
		sum += factor/(2*k + 1);
	}
	printf("%lf\n",4*sum);
	return 0;
}

double Trap(double a, double b, int n){
	double h, x, my_result;
	double local_a, local_b;
	int i, local_n;
	int my_rank = omp_get_thread_num();
	int thread_count = omp_get_num_threads();

	h = (b-a)/n;
	local_n = n/thread_count;
	local_a = a + my_rank*local_n*h;
	local_b = local_a + local_n*h;
	my_result = (local_a*local_a + local_b*local_b)/2.0;
#	pragma omp parallel for num_threads(thread_count) \
		reduction(+: my_result)
	for(i = 1; i <= n-1; i++){
		x = a + i*h;
		my_result += x*x;
	}
	my_result = my_result*h;
	return my_result;
}

