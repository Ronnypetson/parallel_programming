#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main(int argc, char* argv[]){
	int num_t, len, i, j, count;
	float* vec, *res;
	scanf("%d %d",&num_t,&len);
	vec = (float*)malloc(sizeof(float)*len);
	res = (float*)malloc(sizeof(float)*len);
	for(i = 0; i < len; i++){
		scanf("%f",vec+i);
		*(res+i) = 0.0;
	}
	double t0 = omp_get_wtime();
#	pragma omp parallel for num_threads(num_t) \
		default(none) private(i, j, count) shared(len, vec, res)
	for(i = 0; i < len; i++){
		count = 0;
		for(j = 0; j < len; j++){
			if(*(vec+i) > *(vec+j))
				count++;
		}
		*(res+count) = *(vec+i);
	}
	for(j = 1; j < len; j++){
		if(*(res+j) == 0.0)
			*(res+j) = *(res+j-1);
	}
	for(i = 0; i < len; i++){
		printf("%.2f ",*(res+i));
	}
	free(vec);
	free(res);
	printf("\n%lf\n",omp_get_wtime()-t0);
}

