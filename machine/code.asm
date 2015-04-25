void main(){
	pushi(ebp);
	move(esp, ebp);
	addi(-80, esp);
	print_string("i am good\n");
	move(0, eax);
	move(1, ebx);
	addi(eax, ebx);
	move(1, eax);
	addi(eax, ebx);
	move(0, eax);
	move(0, ecx);
	addi(eax, ecx);
	move(-4, eax);
	muli(-16,ecx);
	addi(ecx,eax);
	muli(-4,ebx);
	addi(ebx,eax);
	move(1, ebx);
	move(1, ecx);
	addi(ebx, ecx);
	addi(ebp, eax);
	storei(ecx,ind(eax));
	print_string("i am good\n");
	print_string("value of a[0][2] is ");
	move(0, eax);
	move(-4, ebx);
	muli(-16,eax);
	addi(eax,ebx);
	move(2, eax);
	muli(-4,eax);
	addi(eax,ebx);
	addi(ebp, ebx);
	loadi(ind(ebx), ebx);
	print_int(ebx);
	print_string("\n");
	move(0, eax);
	j(Lmain_);
Lmain_:
	move(ebp, esp);
	loadi(ind(ebp), ebp);
	popi(1);
	return;
}

