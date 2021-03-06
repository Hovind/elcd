#include <stdio.h>
#include <limits.h> /* CHAR_BIT */

static unsigned int polynomial256 = 0x11b;

unsigned int
ord(unsigned int a)
{
	unsigned int i;
	for (i = 0; a; ++i)
		a = a >> 1;
	return i;
}

void
swap(unsigned int *a, unsigned int *b)
{
	if (a != b) {
		*a ^= *b;
		*b ^= *a;
		*a ^= *b;
	}
}
	
void
print256(unsigned int a)
{
	int i;
	int first = 1;

	if (a) {
		for (i = CHAR_BIT - 1; i >= 0; --i) {
			if (a & (1 << i)) {
				if (first) 
					first = 0;
				else
					printf("+");	
				
				if (i)
					printf("x^%u", i);
				else
					printf("%u", 1);
			}
		}
	} else {
		printf("%u", a);
	}
}

void
print_poly(unsigned int a)
{
	int i;
	int first;
	unsigned int mask;

	if (a) {
		first = 1;
		mask = 1 << CHAR_BIT*sizeof(a) - 1;
		for (i = CHAR_BIT*sizeof(a) - 1; a; --i, a = a << 1) {
			if (a & mask) {
				if (first) 
					first = 0;
				else
					printf("+");	
					
				if (i)
					printf("x^%u", i);
				else
					printf("%u", 1);
			}
		}
	} else {
		printf("%d", 0);
	}
}

unsigned int
add256(unsigned int a, unsigned int b)
{
	return a ^ b;
}

unsigned int
div_euclidean(unsigned int a, unsigned int b, unsigned int *r)
{
	int factor;
	int i;
	int j;
	unsigned int ret = 0;
	
	j = ord(b);
	
	while ((i = ord(a)) >= j) {
		factor = i - j;
		ret |= 1 << factor;
		a = add256(a, b << factor);
	}
	*r = a;
	return ret;
}

unsigned int
mul(unsigned int a, unsigned int b)
{
	int i;
	unsigned int ret = 0;
	for (i = 0; a; ++i, a = a >> 1) {
		if (a & 0x1)
			ret = add256(ret, b << i);
	}
	return ret;	
}

unsigned int
euclid(unsigned int a, unsigned int b, unsigned int *u, unsigned int *v)
{
	unsigned int r_prev = a;
	unsigned int r      = b;
	unsigned int s_prev = 1;
	unsigned int s      = 0;
	unsigned int t_prev = 0;
	unsigned int t      = 1;
	unsigned int q;

	while (r) {
		q = div_euclidean(r_prev, r, &r_prev);
		swap(&r, &r_prev);

		s_prev = add256(s_prev, mul(q, s));
		swap(&s, &s_prev);

		t_prev = add256(t_prev, mul(q, t));
		swap(&t, &t_prev);
	}
	*u = s_prev;
	*v = t_prev;
	return r_prev;
}

unsigned int
mul256(unsigned int a, unsigned int b)
{
	unsigned int r;
	div_euclidean(mul(a, b), polynomial256, &r);
	return r;
}

unsigned int
inv256(unsigned int a)
{
	unsigned int u;
	unsigned int v;
	euclid(polynomial256, a, &u, &v);
	return v;
}

unsigned int
div256(unsigned int a, unsigned int b)
{
	return mul256(a, inv256(b));
}

/* int
main()
{
	unsigned int oct;
	unsigned int d;
	unsigned int q;
	unsigned int u;
	unsigned int v;
	
	d = 0x53;
	printf("gcd(");
	print_poly(polynomial256);
	printf(", ");
	print_poly(d);
	printf(") = ");
	q = euclid(polynomial256, d, &u, &v);
	print_poly(q);
	printf("\n");
	print_poly(u);
	printf(" * ");
	print_poly(polynomial256);
	printf(" + ");
	print_poly(v);
	printf(" * ");
	print_poly(d);
	printf(" = ");
	print_poly(q);
	
	printf("\n\n");
	
	printf("inv(");
	print_poly(d);
	printf(") = ");
	oct = inv256(d);
	print_poly(oct);
	printf("\n\n");
	
	return 0;
} */
