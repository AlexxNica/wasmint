/*
 * Copyright 2015 WebAssembly Community Group
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <iostream>
#include <cstdlib>
#include <climits>
#include <cstdint>
#include "PiDigits.h"

void swap(char *a, char *b)
{
    char tmp = *a;
    *a = *b;
    *b = tmp;
}

void quicksort(char *begin, char *end)
{
    char *ptr;
    char *split;
    if (end - begin <= 1)
        return;
    ptr = begin;
    split = begin + 1;
    while (++ptr <= end) {
        if (*ptr < *begin) {
            swap(ptr, split);
            ++split;
        }
    }
    swap(begin, split - 1);
    quicksort(begin, split - 1);
    quicksort(split, end);
}

int main()
{
    char str[] = WASMINT_PI_DIGITS_STR;

    /*std::qsort(str, sizeof str, sizeof(char), [](const void* a, const void* b)
    {
        char arg1 = *static_cast<const char*>(a);
        char arg2 = *static_cast<const char*>(b);

        if(arg1 < arg2) return -1;
        if(arg1 > arg2) return 1;
        return 0;
    }); */
    quicksort(str, str + sizeof(str));

    char smallestValue = -128;
    for (std::size_t i = 0; i < sizeof str; i++) {
        char value = str[i];
        if (value < smallestValue)
            return 1;
        smallestValue = value;
    }
    return 0;
}
