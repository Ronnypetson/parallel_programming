#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void producer_consumer(int *buffer, int size, int *vec, int n);
void seq_producer_consumer(int *buffer, int size, int *vec, int n);

int num_t;

int main(int argc, char* argv[]){
	int n, b_len, i;
	int* vec, *buff;
	scanf("%d %d %d",&num_t,&n,&b_len);
	vec = (int*)malloc(sizeof(int)*n);
	buff = (int*)malloc(sizeof(int)*b_len);
	for(i = 0; i < n; i++){
		scanf("%d",&vec[i]);
	}
	double t0 = omp_get_wtime();
	producer_consumer(buff,b_len,vec,n);
	printf("%lf\n",omp_get_wtime()-t0);
}

void producer_consumer(int *buffer, int size, int *vec, int n) {
	int i, j;
	long long unsigned int sum = 0;
#	pragma omp parallel num_threads(num_t) \
		default(none) shared(n,size,buffer,vec,sum) private(i,j)
	for(i=0;i<n;i++) {
		if(i % 2 == 0) {	// PRODUTOR
#			pragma omp for
			for(j=0;j<size;j++) {
				buffer[j] = vec[i] + j*vec[i+1];
			}
		}
		else {	// CONSUMIDOR
#			pragma omp for reduction(+: sum)
			for(j=0;j<size;j++) {
				sum += buffer[j];
			}
		}
	}
	printf("%llu\n",sum);
}

void seq_producer_consumer(int *buffer, int size, int *vec, int n) {
	int i, j;
	long long unsigned int sum = 0;

	for(i=0;i<n;i++) {
		if(i % 2 == 0) {	// PRODUTOR
			for(j=0;j<size;j++) {
				buffer[j] = vec[i] + j*vec[i+1];
			}
		}
		else {	// CONSUMIDOR
			for(j=0;j<size;j++) {
				sum += buffer[j];
			}
		}
	}
	printf("%llu\n",sum);
}

