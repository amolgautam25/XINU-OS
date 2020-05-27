	.text
	.global zfunction

zfunction:
	
	pushl %ebp
	movl %esp,%ebp
	
	movl 8(%ebp),%eax
	andl $0xFF803FFF,%eax
	sall $0x04,%eax
	popl %ebp
	
	ret