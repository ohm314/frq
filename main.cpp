/*
 *
 * Copyright 2023 Omar Awile
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

#include <iomanip>
#include <iostream>

#include "frq.hpp"


string_lines split_lines(const std::string& txt) {
    std::size_t pos = 0, ppos = 0;
    std::size_t lineno = 1;
    string_lines lines;
    while ((pos = txt.find('\n', ppos)) != std::string::npos) {
        lines.emplace_back(lineno++, txt.substr(ppos, pos - ppos));
        ppos = pos + 1;
    }
    if (ppos < txt.length()) {
        lines.emplace_back(lineno, txt.substr(ppos));
    }
    
    return lines;
}


std::vector<std::size_t> unique_lines(const string_lines& lines) {
    std::vector<std::size_t> uls;
    for (std::size_t i = 0; i < lines.size(); ++i) {
        bool unique = true;
        for (std::size_t j = i+1; j < lines.size(); ++j) {
            if (lines[i].second == lines[j].second) {
                unique = false;
                break;
            }
        }
        if (unique) {
            uls.push_back(i);
        }

    }
    return uls;
}

void print_txt(const string_lines& lines) {
    for (std::size_t i = 0; i < lines.size(); ++i) {
        std::cout << std::setw(2) << std::setfill('0') << lines[i].first << ": " << lines[i].second << std::endl;
    }
}


/*
int main(int argc, char** argv) {
    auto s1 = R"code(A
B
C
A
B
B
A)code";
    auto s2 = R"code(C
B
A
B
A
C)code";
    
    auto l1 = split_lines(s1);
    print_txt(l1);
    std::cout << "=========" << std::endl;
    auto l2 = split_lines(s2);
    print_txt(l2);
    std::cout << "=========" << std::endl;
    Myers myers = Myers(l1, l2);

    for (auto& edit : myers.diff()) {
      std::cout << edit << std::endl;
    }
    return 0;
}
*/

int main(int argc, char** argv) {
    auto s1 = R"code(#include <stdio.h>

int foo(int a) {
  return 2 *a ;
}

int bar(int x) {
  return x - 3;
}

int main(int argc, char **argv)
{
    foo(bar(4));
    return 0;
})code";

    auto s2 = R"code(#include <stdio.h>

int foo(int a) {
  return 2 *a ;
}

int bar(int x) {
  return x - 3;
}

float baz(float r) {
  return r*r;
}

int main(int argc, char **argv)
{
    foo(bar(4));
    baz(2.0);
    return 0;

})code";

    auto l1 = split_lines(s1);
    print_txt(l1);
    std::cout << "=========" << std::endl;
    auto l2 = split_lines(s2);
    print_txt(l2);
    std::cout << "=========" << std::endl;
    Myers myers = Myers(l1, l2);

    for (auto& edit : myers.diff()) {
      std::cout << edit << std::endl;
    }
    
    std::cout << "=========" << std::endl;
    
    auto s3 = R"code(void Chunk_copy(Chunk *src, size_t src_start, Chunk *dst, size_t dst_start, size_t n)
{
    if (!Chunk_bounds_check(src, src_start, n)) return;
    if (!Chunk_bounds_check(dst, dst_start, n)) return;

    memcpy(dst->data + dst_start, src->data + src_start, n);
}

int Chunk_bounds_check(Chunk *chunk, size_t start, size_t n)
{
    if (chunk == NULL) return 0;

    return start <= chunk->length && n <= chunk->length - start;
})code";

    auto s4 = R"code(int Chunk_bounds_check(Chunk *chunk, size_t start, size_t n)
{
    if (chunk == NULL) return 0;

    return start <= chunk->length && n <= chunk->length - start;
}

void Chunk_copy(Chunk *src, size_t src_start, Chunk *dst, size_t dst_start, size_t n)
{
    if (!Chunk_bounds_check(src, src_start, n)) return;
    if (!Chunk_bounds_check(dst, dst_start, n)) return;

    memcpy(dst->data + dst_start, src->data + src_start, n);
})code";
    auto l3 = split_lines(s3);
    print_txt(l3);
    std::cout << "=========" << std::endl;
    auto l4 = split_lines(s4);
    print_txt(l4);
    std::cout << "=========" << std::endl;
    Myers myers2 = Myers(l3, l4);

    for (auto& edit : myers2.diff()) {
      std::cout << edit << std::endl;
    }
   
    return 0;
}

