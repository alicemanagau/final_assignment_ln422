//binary
#include <stdio.h>
int main()
{
	int tableau[]={3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52}; //tableau de 50 éléments
	int n = sizeof(tableau) / sizeof(tableau[0]);
	int x=60;
	int low=0;
	int high=n-1;
	int check=-1;
	int m,milieu;
	
	while (low <= high) 
	{
		int milieu = low + (high - low) / 2;
		if (tableau[milieu] == x)
			//milieu=milieu;
			m = milieu;
			check =0;

		if (tableau[milieu] < x)
			low = milieu + 1;

		else
			high = milieu - 1;
	}
		
		
	if ( check == -1)
		printf("Task 4 : La valeur n'est pas dans le tableau \n");
		
	else
		printf("Task 4 : la valeur est à l'occurence  %d \n", m);
		
	
	
};