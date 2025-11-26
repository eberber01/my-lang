int work(){
    int num1 = 10;
    int num2 = 0;

    for(int i =0; i != 10; i = i + 1){
       num1 = num1 + 1; 
       num2 = num2 + 1; 
        if(num2 == 5)
            return 10;
    }
        
    return 2;
}


int main(){
    int hundred = 100;
    int num;
    int ret = 10;
    int more_num;

    work();
    return ret;
}