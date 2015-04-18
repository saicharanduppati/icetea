void main(){
	pushi(ebp);
	move(esp, ebp);
	move(7, eax);
	storei(eax, ind(ebp, -4));
L0:
	move(3, eax);
	loadi(ind(ebp, -4), ebx);
	cmpi(ebx, eax);
	jg(L1);
	move(0, ebx);
	j(L2);
L1:
	move(1, ebx);
L2:
	cmpi(0,ebx);
	je(L3);
	move(1, eax);
	loadi(ind(ebp, -4), ecx);
	muli(-1,eax);
	addi(eax, ecx);
	storei(ecx, ind(ebp, -4));
	move(1, eax);
	loadi(ind(ebp, -4), ebx);
	muli(-1,eax);
	addi(eax, ebx);
	storei(ebx, ind(ebp, -4));
	j(L0);
L3:
	move(0, eax);
	storei(eax, ind(ebp, 12));
	j(Lmain_ii);
Lmain_ii:
	move(ebp, esp);
	loadi(ind(ebp), ebp);
	popi(1);
	return;
}

