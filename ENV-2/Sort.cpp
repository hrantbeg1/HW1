#include "Sort.h"
void bubble(vector<Complex>& arr){
    size_t n = arr.size();
    bool s;
    for (size_t i = 0; i < n - 1; ++i){
        s = false;
        for (size_t j = 0; j < n - i - 1; ++j){
            if (arr[j].abs() > arr[j + 1].abs()){
                swap(arr[j], arr[j + 1]);
                s = true;
            }
        }
        if (s == false){
            break;
        }
    }
}
