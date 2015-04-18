void main(){
	pushi(ebp);
	move(esp, ebp);
	move(0, eax);
	move(-4, ebx);
	move(1, ecx);
	muli(-4,ecx);
	addi(ecx,ebx);
	addi(ebp, ebx);
	storei(eax,ind(ebx));
	return;
}

