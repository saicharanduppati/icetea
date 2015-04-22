void f(){
	pushi(ebp);
	move(esp, ebp);
	addi(-4, esp);
	loadi(ind(ebp, 4), eax);
	storei(eax, ind(ebp, -4));
	loadi(ind(ebp, -4), eax);
	storei(eax, ind(ebp, 12));
	j(Lf_ii);
Lf_ii:
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
	print_string("the value of f(b) is ");
	pushi(0);
	addi(-8, esp);
	loadi(ind(ebp, -4), eax);
	storei(eax, ind(esp));
	addi(4, esp);
	storei(7, ind(esp));
	addi(4, esp);
	addi(-8, esp);
	f();
	popi(1);
	popi(1);
	loadi(ind(esp), eax);
	popi(1);
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

