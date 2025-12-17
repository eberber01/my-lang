int sumdown(int n, int add) {
    if (n == 0) return 0;
    return add + sumdown(n - 1, add);
}

int main() {
    return sumdown(5, 3);   // 3 added 5 times => 15
}
