void main(){
	pushi(ebp);
	move(esp, ebp);
	move(0, eax);
	move(1, ebx);
	muli(168,ebx);
	addi(ebx,eax);
	move(2, ebx);
	muli(28,ebx);
	addi(ebx,eax);
	move(3, ebx);
	muli(4,ebx);
	addi(ebx,eax);
	addf(ebp, eax);
	loadf(ind(eax), eax);
	storef(eax,ind(ebp, -840));
	return;
}

