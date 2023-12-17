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
    for (const auto& e : backtrack()) {
      //std::cout << "DBG: (" << e.prev_x << ", " << e.prev_y << ") -> (" << e.x << ", " << e.y << ")" << std::endl;
      auto& a_line = a[e.x - 1];
      auto& b_line = b[e.y - 1];

      if (e.prev_x == e.x)
        edits.emplace_front(Edit::etype::ins, nullptr, &b_line);
      else if (e.prev_y == e.y)
        edits.emplace_front(Edit::etype::del, &a_line, nullptr);
      else
        edits.emplace_front(Edit::etype::eql, &a_line, &b_line);
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

  std::vector<TraceTuple> backtrack() {
    auto x = a.size();
    auto y = b.size();

    auto trace = shortest_edit();
    std::cout << "DBG: " << "done shortest_edit" << std::endl;
    std::size_t d = trace.size();

    auto edits = std::vector<TraceTuple>();
    for (auto v_it = trace.rbegin(); v_it != trace.rend(); v_it++, d--) {
      
      auto v = *v_it;
      int k = x - y;
      std::cout << "DBG: d=" << d << " x=" << x << " y=" << y << " k=" << k << std::endl;
      int prev_k;
      if ((k == -d) || ((k != d) && (v[idx(k - 1)] < v[idx(k + 1)]))) {
          std::cout << "DBG: v[k-1],v[k+1]: " <<v[idx(k - 1)] << ", " << v[idx(k + 1)] << std::endl;
          prev_k = k + 1;
      } else {
          std::cout << "DBG: v[k-1],v[k+1]: " <<v[idx(k - 1)] << ", " << v[idx(k + 1)] << std::endl;
          prev_k = k - 1;
      }
      std::size_t prev_x = v[idx(prev_k)];
      std::size_t prev_y = prev_x - prev_k;
      std::cout << "DBG: " << "prev_k=" << prev_k 
        << " prev_x=" << prev_x
        << " prev_y=" << prev_y
        << std::endl;

      while ((x > prev_x) && (y > prev_y)) {
        edits.emplace_back(TraceTuple{x - 1, y - 1, x, y});
        x--; y--;
      }
      if (d > 0) {
        edits.emplace_back(TraceTuple{prev_x, prev_y, x, y});
      }
      x = prev_x;
      y = prev_y;
    }
    return edits;
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

    auto s2 = R"code(#include <stdlib.h>
#include <stdio.h>

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
    std::cout << "=========" << std::endl;

    for (auto& edit : myers.diff()) {
      std::cout << edit << std::endl;
    }
   
    return 0;
}

