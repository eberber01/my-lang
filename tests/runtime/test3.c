
int two(){
    return 2;
}

int one(){
    return 1;
}

int mult(int num1, int num2){
    return num1 * num2;
}

int add(int num1, int num2){
    return num1 + num2;
}

int main(){
    return mult(add(one(), one()), mult(two(), two() + one()));
}