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

#include <deque>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

using line = std::pair<std::size_t, std::string>;
using string_lines = std::vector<line>;

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

class Patience {

  struct Slice {
    std::pair<std::size_t, std::size_t> a; // left text slice, low and high line numbers
    std::pair<std::size_t, std::size_t> b; // right text slice, low and high line numbers
  };

public:
  
  Patience(string_lines& _a, string_lines& _b) : a(_a), b(_b) {};

private:
  string_lines& a;
  string_lines& b;

};
