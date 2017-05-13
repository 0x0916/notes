#include <sys/prctl.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

int main()
{
	int nnp = prctl(PR_GET_NO_NEW_PRIVS, 0, 0, 0, 0);
	if (nnp == -EINVAL) {
		printf("Failed!\n");
		return 1;
	}
	
	printf("nnp was %d\n", nnp);
	
	if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0) != 0) {
		printf("Failed!\n");
		return 1;
	}
	
	nnp = prctl(PR_GET_NO_NEW_PRIVS, 0, 0, 0, 0);
	if (nnp == -EINVAL) {
		printf("Failed!\n");
		return 1;
	}

	printf("nnp is %d\n", nnp);

	printf("here goes...\n");
	execlp("bash", "bash", NULL);
	printf("Failed to exec bash\n");
	return 1;
}
