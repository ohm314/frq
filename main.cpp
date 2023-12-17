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
#include <string>
#include <iostream>
#include <utility>
#include <array>
#include <deque>
#include <vector>

using line = std::pair<std::size_t, std::string>;
using string_lines = std::vector<line>;

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


class Myers {

  public:
  
  struct Edit {
    enum etype {ins, del, eql};
    etype edit;
    const line* old_line = nullptr;
    const line* new_line = nullptr;
    Edit(etype e, const line* o, const line* n) : edit(e), old_line(o), new_line(n) {};

    friend std::ostream& operator<<(std::ostream& out, Edit&);
  };
  
  Myers(string_lines& _a, string_lines& _b) : a(_a), b(_b) {
    max = a.size() + b.size();
  }

  std::deque<Edit> diff() {
    auto edits = std::deque<Edit>();
    auto x = a.size();
    auto y = b.size();

    auto trace = shortest_edit();
    std::size_t d = trace.size();

    for (auto v_it = trace.rbegin(); v_it != trace.rend(); v_it++, d--) {
      
      auto v = *v_it;
      int k = x - y;
      int prev_k;
      if ((k == -d) || ((k != d) && (v[idx(k - 1)] < v[idx(k + 1)]))) {
          prev_k = k + 1;
      } else {
          prev_k = k - 1;
      }
      std::size_t prev_x = v[idx(prev_k)];
      std::size_t prev_y = prev_x - prev_k;

      while ((x > prev_x) && (y > prev_y)) {
        edits.emplace_front(Edit::etype::eql, &a[x - 1], &b[y - 1]);
        x--; y--;
      }
      if (x == prev_x) {
        edits.emplace_front(Edit::etype::ins, nullptr, &b[y - 1]);
      } else if (y == prev_y) {
        edits.emplace_front(Edit::etype::del, &a[x - 1], nullptr);
      }
      x = prev_x;
      y = prev_y;
    }
    return edits;
  }
  
  private:

  struct TraceTuple {
    std::size_t prev_x = 0;
    std::size_t prev_y = 0;
    std::size_t x = 0;
    std::size_t y = 0;
  };
  
  string_lines& a, b;
  std::size_t max;

  inline std::size_t idx(int i) {
    return (i + max) % max;
  }

  std::vector<std::vector<int>> shortest_edit() {
    auto n = a.size();
    auto m = b.size();
    int x, y;

    auto trace = std::vector<std::vector<int>>();
    auto v = std::vector<int>(2 * max + 1, 0);
    for (int d = 0; d <= max; ++d) {
      trace.push_back(v);
      for (int k = -d; k <= d; k += 2) {
        if ((k == -d) || (k != d && (v[idx(k - 1)] < v[idx(k + 1)]))) {
            x = v[idx(k + 1)];
        } else {
            x = v[idx(k - 1)] + 1;
        }
        y = x - k;
        while ((x < n) && (y < m) && (a[x].second == b[y].second)) {
          x++; y++;
        }
        v[idx(k)] = x;

        if ((x >= n) && (y >= m)) {
          trace.push_back(v);
          return trace;
        }
      }
    }
    return trace;
  }



};

std::ostream& operator<<(std::ostream& out, Myers::Edit& edit) {


  if (edit.edit == Myers::Edit::etype::ins)
    out << '+' << " " << " " << " " << edit.new_line->first << " " << edit.new_line->second;
  else if (edit.edit == Myers::Edit::etype::del)
    out << '-' << " " << edit.old_line->first << " " << " " << " " << edit.old_line->second;
  else
    out << ' ' << " " << edit.old_line->first << " " << edit.new_line->first << " " << edit.old_line->second;

  return out;
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

