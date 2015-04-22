void f(){
	pushi(ebp);
	move(esp, ebp);
	addi(-12, esp);
	print_string("I got ");
	loadi(ind(ebp, 4), eax);
	print_int(eax);
	print_string(" ");
	loadi(ind(ebp, 8), ebx);
	print_int(ebx);
	print_string(" ");
	loadi(ind(ebp, 12), ecx);
	print_int(ecx);
	print_string("\n");
	loadi(ind(ebp, 4), eax);
	storei(eax, ind(ebp, 16));
	j(Lf_iii);
Lf_iii:
	move(ebp, esp);
	loadi(ind(ebp), ebp);
	popi(1);
	return;
}

void main(){
	pushi(ebp);
	move(esp, ebp);
	addi(-4, esp);
	move(4, eax);
	storei(eax, ind(ebp, -4));
	pushi(0);
	addi(-12, esp);
	move(3, eax);
	storei(eax, ind(ebp, -4));
	storei(eax, ind(esp));
	addi(4, esp);
	loadi(ind(ebp, -4), eax);
	storei(eax, ind(esp));
	addi(4, esp);
	move(5, eax);
	storei(eax, ind(ebp, -4));
	storei(eax, ind(esp));
	addi(4, esp);
	addi(-12, esp);
	f();
	popi(1);
	popi(1);
	popi(1);
	loadi(ind(esp), eax);
	popi(1);
	move(0, eax);
	j(Lmain_);
Lmain_:
	move(ebp, esp);
	loadi(ind(ebp), ebp);
	popi(1);
	return;
}

