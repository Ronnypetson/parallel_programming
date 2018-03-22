/*
Ronnypetson Souza da Silva
RA: 211848
*/

#include <stdio.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

/* funcao que calcula o minimo valor em um vetor */
double min_val(double * vet,int nval) {
	int i;
	double min;

	min = FLT_MAX;

	for(i=0;i<nval;i++) {
		if(vet[i] < min)
			min =  vet[i];
	}
	
	return min;
}

/* funcao que calcula o maximo valor em um vetor */
double max_val(double * vet, int nval) {
	int i;
	double max;

	max = FLT_MIN;

	for(i=0;i<nval;i++) {
		if(vet[i] > max)
			max =  vet[i];
	}
	
	return max;
}

/* conta quantos valores no vetor estao entre o minimo e o maximo passados como parametros */
int * count(double min, double max, int * vet, int nbins, double h, double * val, int nval, int id) {
	int i, j, count;
	double min_t, max_t;

	for(j=0;j<nbins;j++) {
		count = 0;
		min_t = min + j*h;
		max_t = min + (j+1)*h;
		for(i=0;i<nval;i++) {
			if( (val[i] <= max_t && val[i] > min_t)
			|| (id == 0 && j == 0 && val[i] <= max_t) ) {
				count++;
			}
		}

		vet[j] = count; //
	}

	return vet;
}

double h, *val, max, min;
int n, nval, i, *vet, size;

void* t_count(void* par){
	long thread = (long)par;
	int t_nbins, t_resto;
	t_nbins = n/size;
	t_resto = n%size;
	//
	int i, j, count;
	double min_t, max_t;
	double min_th = min + thread*t_nbins*h;
	int* vet_ = vet + thread*t_nbins;
	int num_bins = t_nbins + t_resto*(thread == size-1);

	for(j=0; j < num_bins; j++) {
		count = 0;
		min_t = min_th + j*h;
		max_t = min_th + (j+1)*h;
		for(i=0;i<nval;i++) {
			if( (val[i] <= max_t && val[i] > min_t)
			|| (thread == 0 && j == 0 && val[i] <= max_t) ) {
				count++;
			}
		}

		vet_[j] = count; //
	}

	return NULL;
}

int main(int argc, char * argv[]) {
	long unsigned int duracao;
	long thread;
	struct timeval start, end;

	scanf("%d",&size);

	/* entrada do numero de dados */
	scanf("%d",&nval);
	/* numero de barras do histograma a serem calculadas */
	scanf("%d",&n);

	/* vetor com os dados */
	val = (double *)malloc(nval*sizeof(double));
	vet = (int *)malloc(n*sizeof(int));

	/* entrada dos dados */
	for(i=0;i<nval;i++) {
		scanf("%lf",&val[i]);
	}

	/* calcula o minimo e o maximo valores inteiros */
	min = floor(min_val(val,nval));
	max = ceil(max_val(val,nval));

	/* calcula o tamanho de cada barra */
	h = (max - min)/n;

	//
	pthread_t* thread_handles;
	thread_handles = (pthread_t*)malloc(size*sizeof(pthread_t));
	//
	gettimeofday(&start, NULL);
	/* chama a funcao */
	for(thread = 0; thread < size; thread++){
		/* A última thread fica com o resto dos bins */
		pthread_create(&thread_handles[thread],NULL,t_count,(void*)thread);
	}
	//
	for(thread = 0; thread < size; thread++)
		pthread_join(thread_handles[thread],NULL);
	//
	gettimeofday(&end, NULL);
	//

	duracao = ((end.tv_sec * 1000000 + end.tv_usec) - \
	(start.tv_sec * 1000000 + start.tv_usec));

	printf("%.2lf",min);	
	for(i=1;i<=n;i++) {
		printf(" %.2lf",min + h*i);
	}
	printf("\n");

	/* imprime o histograma calculado */	
	printf("%d",vet[0]);
	for(i=1;i<n;i++) {
		printf(" %d",vet[i]);
	}
	printf("\n");
	
	/* imprime o tempo de duracao do calculo */
	printf("%lu\n",duracao);

	free(vet);
	free(val);
	free(thread_handles);

	return 0;
}

/*
Speedup e Eficiência
//
		Num threads			1		2		4		8		16

arq1.in	Speedup			  1.0	 1.50	 1.82	 1.34	  0.97
		Eficiência		  1.0	 0.38	 0.46	 0.34	  0.24

arq2.in	Speedup			  1.0	 1.65	 2.60	 3.10	  2.78
		Eficiência		  1.0	 0.41	 0.65	 0.78	  0.70

arq3.in	Speedup			  1.0	 1.81	 2.91	 3.19	  3.48
		Eficiência		  1.0	 0.45	 0.73	 0.80	  0.87

//

Um fator importante a se considerar nesses testes é o número de núcleos da máquina, que é 4 (1 socket com 4 cores):

$lscpu
Architecture:        x86_64
CPU op-mode(s):      32-bit, 64-bit
Byte Order:          Little Endian
CPU(s):              4
On-line CPU(s) list: 0-3
Thread(s) per core:  1
Core(s) per socket:  4
Socket(s):           1
...

Também observa-se que cada core pode executar 1 thread por vez. Assim sendo, é natural esperar que as execuções com até pelo menos 4 threads tenham aumento de speedup.

No arquivo de entrada arq1.in, devido ao número de bins ser relativamente pequeno (100), a partir de 8 threads o paralelismo introduzido não compensa o overhead do número de threads.

Já no arquivo arq2.in, o mesmo efeito ocorre, mas a partir de 16 threads, visto que nesse caso o número de bins para cada thread é maior.

Pode-se verificar a partir de informações de perfilamento que a maior parte do tempo é gasta na execução dos métodos das threads e não nas demais partes do programa:

Each sample counts as 0.01 seconds.
  %   cumulative   self              self     total           
 time   seconds   seconds    calls  Ts/call  Ts/call  name    
100.51      0.06     0.06                             t_count
  0.00      0.06     0.00        1     0.00     0.00  max_val
  0.00      0.06     0.00        1     0.00     0.00  min_val

Isso indica que realmente as quedas de speedup são devidas ao exceço de threads, pois praticamente todo o tempo de execução é ocupado pelo método t_count, que é o método que as threads executam.

//
Ao medir-se o percentual paralelizável do programa como definido no grupo da disciplina -

"O percentual do programa serial que foi (ou será) paralelizado é exatamente o que a definição diz:
T(par)/T(total), onde:
T(par) é o tempo gasto pelo programa **sequencial** na parte do código que se irá paralelizar.
T(total) é o tempo total gasto pelo programa **sequencial** como um todo.

-Guido" -

Foi obtido o valor 343819/376817 = 0.912, o que indica que, em termos de tempo de execução, quase todo o programa pode ser paralelizado.

*/

