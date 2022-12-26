#include "NewsFleX.h"

#define BIG_SPACE 65535

char *increment_space(char *pa)
{
int a, i;
char result[BIG_SPACE];
char temp[BIG_SPACE];
char *ptr;
int rlen;
int rpos;
int carry;

/* argument check */
if(!pa) return(0);

strcpy(result, pa);
rlen = strlen(result) - 1;

i = 0;
carry = 1;
rpos = rlen;
while(1)
	{
	if( (rlen - i) < 0)
		{
		strcpy(temp, "1");
		result[rlen - i] = 48;
		strcat(temp, result);
		ptr = strsave(temp);
		return(ptr);
		}
	a = result[rlen - i] - 48;
	a += carry;
	if(a < 10)
		{
		result[rlen - i] = a + 48;
		break;
		}	
	else
		{
		result[rlen - i] = 48;
		carry = 1;
		}	
	i++;
	}

ptr = strsave(result);
return(ptr);
}/* end function increment_space */


char *substract_space(char *pbig, char *psmall)
{
char result[BIG_SPACE];
char *ptr;
int endbig, endsmall, endresult;
int a, i;
int bc, sc;
int carry;

/* argument check */
if(! pbig) return(0);
if(! psmall) return(0);

endbig = strlen(pbig) - 1;
endsmall = strlen(psmall) - 1;
if(endbig < endsmall)
	{
/*	fprintf(stdout, "substact_space(): length pbig < length psmall\n");*/
	return(0);
	}

strcpy(result, pbig);
endresult = strlen(result) - 1;

carry = 0;
for(i = 0; i <= endbig; i++)
	{
	bc = pbig[endbig - i] - 48;

	if( (endsmall - i) > - 1)
		{
		sc = psmall[endsmall - i] - 48;
		}
	else
		{
		sc = 0;
		}	

	a = bc - sc - carry;
	if(a < 0)
		{
		carry = 1;
		a += 10;
		}
	else
		{
		carry = 0;
		}

	result[endresult - i] = a + 48;
	}/* end for i */

if(carry)
	{
	/*fprintf(stdout, "substract_space(): negative result\n");*/
	return(0);
	}

ptr = strsave(result);
return(ptr);
}/* end function substract_space */


char *divide_space(char *pbig, char *psmall)
{
int i, k;
char divider[BIG_SPACE];
char divisor[BIG_SPACE];
char result[BIG_SPACE];
char rest[BIG_SPACE];
char stemp[12];
char *ptr;
int lenbig, lensmall;
int leading_zero;

/* argument check */
if(! pbig) return(0);
if(! psmall) return(0);
	
lenbig = strlen(pbig) - 1;
lensmall = strlen(psmall) - 1;
k = lensmall;
strcpy(divisor, psmall);
strcpy(result, "");
strcpy(divider, pbig);
divider[k + 1] = 0;
leading_zero = 1;
while(1)
	{
	strcpy(rest, divider);
	i = 0;
	while(1)
		{
		ptr = substract_space(rest, divisor);
		if(! ptr)
			{
			break;
			}
		strcpy(rest, ptr);
		free(ptr);
		i++;
		}	

	if( ! (leading_zero && (i == 0) ) )
		{
		sprintf(stemp, "%c", i + 48); 
		strcat(result, stemp);
		}
	if(i) leading_zero = 0;

/*	fprintf(stdout,\
	"i=%d divider=%s divisor=%s rest=%s k=%d result=%s\n",\
	i, divider, divisor, rest, k, result);
*/	
	strcpy(divider, rest);
	sprintf(stemp, "%c", pbig[k + 1]);
	strcat(divider, stemp);
	
	k++;
	if(k > lenbig) break;
	}

ptr = strsave(result);
return(ptr);
}/* end function divide_space */


char * add_space(char *pa, char *pb)
{
char pr[BIG_SPACE];
char *ptr;
int enda, endb, end, endpr;
int a, b, c, i;
int carry;

/* argument check */
if(! pa) return(0);
if(! pb) return(0);

enda = strlen(pa) - 1;
endb = strlen(pb) - 1;

strcpy(pr, "0");
if(enda > endb)
	{
	end = enda;
	strcat(pr, pa);
	}
else
	{
	end = endb;
	strcat(pr, pb);
	}
endpr = strlen(pr) - 1;

carry = 0;
for(i = 0; i < end; i++)
	{
	if( (enda - i) >= 0) a = pa[enda - i] - 48;
	else a = 0;
	
	if( (endb - i) >= 0) b = pb[endb - i] - 48;
	else b = 0;
	
	c = a + b;	

	c += carry;
	if(c > 9)
		{
		carry = c / 10;
		c = c % 10;
		}
	else
		{
		carry = 0;
		}

	pr[endpr - i] = c + 48;
	}/* end for i */
if(carry)
	{
	c = pr[endpr - i] - 48;
	c += carry;
	pr[endpr - i] = c + 48;
	}
ptr = strsave(pr);
return(ptr);
}/* end function add_space */


char *multiply_space(char *space, char *cd)
{
int a, c, i, j, k;
char bigspace[BIG_SPACE];
char totalspace[BIG_SPACE];
char *ptr;
int end;
int cend;
int carry;
extern char *strsave();

/* argument check */
if(! space) return(0);
if(! cd) return(0);

strcpy(totalspace, "");

cend = strlen(cd) - 1;
for(k = cend; k > - 1; k--)
	{
	strcpy(bigspace, "0");
	strcat(bigspace, space);
	end = strlen(bigspace) - 1;
	carry = 0;
	c = cd[k] - 48;
	for(i = end; i > - 1; i--)
		{
		a = bigspace[i] - 48;
		a *= c;
		a += carry;
		if(a > 9)
			{
			carry = a / 10;
			a = a % 10;
			}
		else
			{
			carry = 0;
			}
		bigspace[i] = a + 48;
		}
	for(j = k; j < cend; j++)
		{
		strcat(bigspace, "0");
		}

	ptr = (char *)add_space(totalspace, bigspace);
	if(! ptr) return(0);
	strcpy(totalspace, ptr);
	free(ptr);

/*fprintf(stdout, "multiply_space(): totalspace=%s\n", totalspace);*/

	}/* end for k */

ptr = strsave(totalspace);
return(ptr);
}/* end function multiply_space */


char *code(char *text, char *key1)
{
int c, i;
char space[BIG_SPACE];
char temp[80];
char *ptr1;

/* argument check */
if(! text) return(0);
if(! key1) return(0);

strcpy(space, "");
i=0;
while(1)
	{
	c = text[i];
	sprintf(temp, "%03d", c);
	if(c == 0) break;
	i++;
	strcat(space, temp);
	}

ptr1 = multiply_space(space, key1);
if(! ptr1) return(0);

return(ptr1);
}/* end function code */


char *decode(char *text, char *key1)
{
int a, c, i, j;
char result[BIG_SPACE];
char temp[80];
char *ptr1, *ptr2;
int mul;

/* argument check */
if(! text) return(0);
if(! key1) return(0);

ptr1 = divide_space(text, key1);
if(!ptr1) return(0);

/*fprintf(stdout, "decoded=%s\n", ptr1);*/

strcpy(result, "");
j = strlen(ptr1) - 1;
while(1)
	{
	mul = 1;
	c = 0;
	for(i = 0; i < 3; i++)
		{
		if( (j - i) < 0)
			{
			sprintf(temp, "%c", c);
			strcat(temp, result);
			strcpy(result, temp);
/*			fprintf(stdout, "end result=%s\n", result);*/

			return(strsave(result) );
/*			exit(1);*/
			}
		a = ptr1[j - i] - 48;
		c += a * mul;
		mul *= 10;
/*fprintf(stdout, "j=%d i=%d a=%d (%c) c=%d\n", j, i, a, a, c);*/
		}/* end for */
	sprintf(temp, "%c", c);
	strcat(temp, result);
	strcpy(result, temp);
	j -= 3;
	}/* end while */	

ptr2 = strsave(result);

/*fprintf(stdout, "decode(): returning=%s\n", ptr2);*/
return(ptr2);
}/* end function decode */


