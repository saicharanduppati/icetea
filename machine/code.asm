void f(){
	pushi(ebp);
	move(esp, ebp);
	addi(-4, esp);
	loadi(ind(ebp, 4), eax);
	storei(eax, ind(ebp, -4));
	loadi(ind(ebp, -4), eax);
	storei(eax, ind(ebp, 8));
	j(Lf_i);
Lf_i:
	move(ebp, esp);
	loadi(ind(ebp), ebp);
	popi(1);
	return;
}

void main(){
	pushi(ebp);
	move(esp, ebp);
	addi(-4, esp);
	move(10, eax);
	storei(eax, ind(ebp, -4));
	pushi(0);
	loadi(ind(ebp, -4), eax);
	pushi(eax);
	f();
	popi(1);
	loadi(ind(esp), eax);
	popi(1);
	print_string("the value of b is ");
	loadi(ind(ebp, -4), eax);
	print_int(eax);
	print_string("\n");
	move(0, eax);
	j(Lmain_);
Lmain_:
	move(ebp, esp);
	loadi(ind(ebp), ebp);
	popi(1);
	return;
}

