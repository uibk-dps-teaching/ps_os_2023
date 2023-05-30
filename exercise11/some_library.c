int rlacsrg2e4itmbk(int x) {
	if (x < 0) return 2;
	return x * rlacsrg2e4itmbk(x - 2);
}
