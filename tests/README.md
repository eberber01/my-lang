# Tests

## Notes

When adding new tests you must run ```cmake -B build``` for cmake to recognize them.

## Runtime Tests

Tests currently check exit status of program that is returned in a0 register. 
On lLinux, the exit status is restricted to 8 bits (0 - 255). 
For validating values greater than 255, use  ```expected exit status % 256``` in the .expected file.
