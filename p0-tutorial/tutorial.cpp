// Program to count unique numbers in input file
#include "tutorial.h"

using namespace std;

int main(int argc, char **argv)
{
    (void)argc;
    (void)argv;
#ifdef __APPLE__
    xcode_redirect(argc, argv);
#endif

    vector<int> numbers;
    int a;
    while (cin >> a)
    {
        numbers.push_back(a);
    }

    sort(numbers.begin(), numbers.end());

    /*
        Unique occrences in a sorted array are never preceded by 
        a matching value
    */
    int unique_count = numbers.size() == 0 ? 0 : 1;
    for (size_t i = 1; i < numbers.size(); ++i)
    {
        if(numbers[i] != numbers[i - 1]){
            ++unique_count;
        }
    }

    cout << "Found " << unique_count << " unique numbers." << endl; 

    return 0;
}