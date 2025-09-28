#include <unistd.h>

# define FIZZ() (write(1,"fizz",4))
# define BUZZ() (write(1,"buzz",4))
# define FIZZBUZZ() (FIZZ(), BUZZ())
# define NEW_LINE() (write(1,"\n",1))
# define PUT_NBR(n) ft_putnbr_macro(n)

// Helper for number printing
static inline void ft_putnbr_macro(int n)
{
	char c;
	if (n > 9)
		ft_putnbr_macro(n / 10);
	c = (n % 10) + '0';
	write(1, &c, 1);	
}

// WRAPPERS OF MACROS

void 	ft_putnbr(int n)	{PUT_NBR(n); NEW_LINE();}
void 	ft_fizz(int n) 		{(void) n; FIZZ(); NEW_LINE();}
void	ft_buzz(int n) 		{(void)n; BUZZ(); NEW_LINE();}
void 	ft_fizzbuzz(int n)	{(void)n; FIZZBUZZ(); NEW_LINE();}
void	fizz_buzz(void);

typedef void (*state_handler)(int);

//totally set
int main(void)
{
		fizz_buzz();
}

void fizz_buzz(void)
{
	state_handler state_handlers[4] = 
	{
		ft_putnbr,
		ft_fizz,
		ft_buzz,
		ft_fizzbuzz
	};
	int i = 1;
	int state;
	while(i <= 100)
	{
		state = 0;
		if (i % 3 == 0)
			state |= 1;
		if (i % 5 == 0)
			state |= 2;
		state_handlers[state](i);
		i++;
	}
}

