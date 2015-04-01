void main()
{
	pushi(0); //To make space in stack for return val
	pushi(6); // argument to fact
	fact(); // invoking fact
	loadi(ind(esp,I), eax);
	print_int(eax); // Special method to print contents of registers
	print_char('\n');
	popi(2); // pop the stack after func call
	return;
}


void fact()
{
	pushi(ebp);
	move(esp,ebp); // Setting dynamic link
	loadi(ind(ebp, I), eax);
	cmpi(0, eax);
	jne(l1);
	storei(1, ind(ebp, 2*I)); // Save the return value in stack
	j(e);

l1:	loadi(ind(ebp, I), eax); // load n to eax
	pushi(eax); //Save register
	loadi(ind(ebp, I), eax); // load n to eax
	addi(-1, eax); // compute (n-1)
	pushi(0); //To make space in stack
	pushi(eax); // argument to fact
	fact(); // call fact
	loadi(ind(esp, I), ebx); // receives the return value
	popi(2); // pop the stack after func call
	loadi(ind(esp), eax); //Load saved register
	popi(1); // pop the stack after loading saved regs
	muli(eax, ebx); // result is in ebx
	storei(ebx, ind(ebp, 2*I)); // Save the return value in stack

e:	loadi(ind(ebp), ebp); // restoring dynamic link
	popi(1); //pop stack
	return; //return
}











