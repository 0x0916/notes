#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


#define MAX_ALLOCS 1000000

static int get_int(const char *arg)
{
	long res;
	char *endptr;

	if (arg == NULL || *arg == '\0')
		return -1;

	res = strtol(arg, &endptr, 10);
	if (*endptr != '\0')
		return -1;

	return (int)res;
}
int main(int argc, char **argv)
{
	char *ptr[MAX_ALLOCS];
	int free_step, free_min, free_max, block_size, num_allocs, j;

	printf("\n");

	if (argc < 3 || strcmp(argv[1], "--help") == 0) {
		fprintf(stderr, "%s num-allocs block-size [step [min [max]]]\n", argv[0]);
		exit(-1);
	}

	num_allocs = get_int(argv[1]);

	block_size = get_int(argv[2]);
	free_step = (argc > 3) ? get_int(argv[3]) : 1;
	free_min = (argc > 4) ? get_int(argv[4]) : 1;
	free_max = (argc > 5) ? get_int(argv[5]) : num_allocs;

	if (free_max > num_allocs) {
		fprintf(stderr, "free-max > mun-allocs\n");
		exit(-1);
	}

	printf("Initial program break:          %10p\n", sbrk(0));
	printf("Allocating %d*%d bytes\n", num_allocs, block_size);

	for (j = 0; j < num_allocs; j++) {
		ptr[j] = malloc(block_size);
		printf("ptr[%d]:                %10p\n", j, ptr[j]);
		if (ptr[j] == NULL) {
			fprintf(stderr, "malloc\n");
			exit(-1);
		}
	}

	printf("Program break is now:           %10p\n", sbrk(0));

	printf("Free blocks from %d to %d in steps of %d\n", free_min, free_max, free_step);

	for (j = free_max -1; j < free_max; j+= free_step)		free(ptr[j]);

	printf("After free(), program break is: %10p\n", sbrk(0));
	exit(0);
}
