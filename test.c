int main()
{
	int a[3][3] = { {1, 2, 3}, {4, 5, 6}, {7, 8, 9} };
	int b[3][2] = { {35, 36}, {37, 38}, {39, 40} };
	int i;
	i = -44;
	print(i);
	for(i = 0; i < 10; i = i+1)
	{
		if(i < 3)
		{
			a[0][i] = b[i][0];
		}
		else
		{
			print(i);
		}
	}

	printmat(a);

	float x;
	x = 8.5;

	print(x);
	
	i = 0;
	
	while(i < 8)
	{
		i = i + 5;
	}
	
	print(i);
}
