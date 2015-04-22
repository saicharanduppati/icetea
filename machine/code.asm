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
	addi(-12, esp);
	move(4, eax);
	storei(eax, ind(ebp, -4));
	loadi(ind(ebp, -4), eax);
	addi(1,eax);
	storei(eax,ind(ebp, -4));
	storei(eax, ind(ebp, -8));
	pushi(0);
	addi(-12, esp);
	loadi(ind(ebp, -4), eax);
	storei(eax, ind(esp));
	addi(4, esp);
	loadi(ind(ebp, -4), eax);
	addi(1,eax);
	storei(eax,ind(ebp, -4));
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
	storei(eax, ind(ebp, -4));
	print_string("value of b is ");
	move(0, eax);
	storei(eax, ind(ebp, -8));
	print_int(eax);
	print_string("\n");
	loadi(ind(ebp, -8), eax);
	cmpi(0, eax);
	je(L0);
	move(0, eax);
	j(L1);
L0:
	move(1, eax);
L1:
	storei(eax, ind(ebp, -12));
	print_string("value of c is ");
	loadi(ind(ebp, -12), eax);
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

