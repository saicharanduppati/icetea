void main(){
	pushi(ebp);
	move(esp, ebp);
	loadi(ind(ebp, -4), eax);
	move(5, ebx);
	addi(eax, ebx);
	loadi(ind(ebp, -4), eax);
	addi(eax, ebx);
	loadi(ind(ebp, -4), eax);
	addi(eax, ebx);
	loadi(ind(ebp, -4), eax);
	addi(eax, ebx);
	loadi(ind(ebp, -4), eax);
	addi(eax, ebx);
	loadi(ind(ebp, -4), eax);
	addi(eax, ebx);
	loadi(ind(ebp, -4), eax);
	addi(eax, ebx);
	loadi(ind(ebp, -4), eax);
	addi(eax, ebx);
	return;
}
