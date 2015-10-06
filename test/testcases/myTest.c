int lol(int a)
{	
    if(a>0)
	return -1;
    else a=a*7;
    return a;
}
int main(int argc, char* argv[])
{
    lol(argc);
    return 0;
}
