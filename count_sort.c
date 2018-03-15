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
	for(i = 0; i < len; i++){
		count = 0;
		for(j = 0; j < len; j++){
			if(*(vec+i) > *(vec+j))
				count++;
		}
		if(*(vec+i) != 0.0){
			while(*(res+count) == *(vec+i)) count++;
			*(res+count) = *(vec+i);
		}
	}
	for(i = 0; i < len; i++){
		printf("%f ",*(res+i));
	}
	printf("\n");
}

